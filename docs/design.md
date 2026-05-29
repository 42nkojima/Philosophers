# Philosophers Design Notes

## 課題の捉え方

この課題は、単にマルチスレッドを使って処理を並列化する課題ではない。
主題は、複数の実行主体が同時に進行する状況で、共有リソースと共有状態を安全に扱うことである。

`philo` の mandatory では、各 philosopher は別々の thread として動く。
一方で、fork、終了状態、食事時刻、食事回数、ログ出力などは複数 thread から参照される共有対象になる。
そのため、設計の中心は「どのデータをどの mutex で守るか」と「mutex をどの順序で取るか」になる。

この課題で特に意識することは次の通り。

- fork の二重取得を防ぐ
- data race を起こさない
- deadlock を起こさない
- 死亡判定を遅らせすぎない
- ログ出力を重ねない
- simulation 終了後に不要な状態ログを出さない

## 採用するデッドロック回避・公平化方針

### fork 予約（誤死亡対策）

`fork_reserved[]` を `state_mutex` で保護する。

食事の流れ:

1. `state_mutex` 下で両 fork が未予約なら、両方を予約して unlock
2. 取れなければ短い sleep 後に再試行（`time_sleep_ms`、終了フラグも確認）
3. 予約後にのみ fork mutex を触る（この時点ではまだ1本も握らない）
4. リソース階層順（小さい fork id 先）で両方 lock → ログ → `last_meal_time` 更新
5. 食事 → `eat_count` 更新 → fork unlock → 予約解除

これにより「1本だけ握ったまま2本目を待つ」状態を避ける。

### リソース階層（デッドロック対策）

予約取得後、実際の `pthread_mutex_lock` は常に小さい fork id から行う。

```c
first = min(left_fork, right_fork);
second = max(left_fork, right_fork);
```

循環待ちを作らない。予約と組み合わせて、no-death 定番テストの安定性を上げる。

### 補助: 起動遅延

奇数 `id` の philosopher は開始時に `time_to_eat / 2` だけ遅延（`stagger_start`）。予約の補助。

食事ループ全体:

1. 予約（上記）
2. 両 fork lock・ログ・`last_meal_time` 更新
3. `is eating` → `time_to_eat`
4. `eat_count` 更新 → fork unlock → 予約解除
5. `is sleeping` → `time_to_sleep` → `is thinking`
6. ループ先頭へ

philosopher のルーティンは eat → sleep → think の繰り返しである。
subject が要求する状態ログは `has taken a fork`、`is eating`、`is sleeping`、`is thinking`、`died` の5種類。
すべての状態遷移でログを出力する。

## 共有状態と mutex

fork は `pthread_mutex_t` の配列で管理する。
各 fork mutex は、その fork を同時に複数 philosopher が取らないために使う。

ログ出力には専用の print mutex を使う。
subject ではログが重なってはいけないため、状態メッセージの出力は必ずこの mutex で直列化する。
終了後に通常ログを出さないように、print 時には終了状態も確認する。
ただし、死亡ログは一度だけ必ず出せるように扱う。

食事時刻と食事回数は、monitor thread と philosopher thread の両方から触られる。
そのため、`last_meal_time` と `eat_count` は meal/state 用 mutex で保護する。
読み取りだけの場合でも、書き込み側と同じ mutex を通す。

終了フラグも共有状態である。
philosopher thread は routine の途中で終了フラグを確認し、monitor は死亡または全員の食事回数達成で終了フラグを立てる。
このフラグの読み書きも mutex で保護する。

mutex の役割は混ぜすぎない。
基本方針は次のように分ける。

- fork mutex: fork の占有を守る
- print mutex: ログ出力を直列化する
- meal/state mutex: `last_meal_time`、`eat_count`、`fork_reserved`、終了状態を守る

異種 mutex を同時に保持する場合のグローバルロック順序は次の通り。

```
fork mutex → meal/state mutex → print mutex
```

複数の mutex を取得する必要があるときは、必ずこの左から右の順序で取得する。
逆順での取得や、右側を保持した状態で左側を取得することは禁止する。
この順序をすべてのコードパスで守ることで、異種 mutex 間のデッドロックを防ぐ。

print 関数は次の順序で動作する。

1. meal/state mutex を取得して終了フラグを確認する
2. 終了済みなら meal/state mutex を解放して return する（通常ログを出さない）
3. 終了していなければ print mutex を取得する（meal/state → print の順序でグローバルロック順序に適合）
4. メッセージを出力する
5. print mutex を解放し、meal/state mutex を解放する

死亡ログだけは終了フラグに関係なく一度だけ出す。
この方式により、print mutex 単体で終了フラグを読むことによる data race を回避する。

終了検出時の fork 解放について:
philosopher が fork を保持した状態で終了を検出した場合、保持中の fork mutex をすべて unlock してから routine を終了する。
fork を保持したまま終了すると他の philosopher がデッドロックする。

終了確認のタイミングと対応:

1. fork 取得前（routine ループの先頭）: 終了済みならそのまま return する。
2. 1本目の fork 取得後、2本目の取得前: 1本目を unlock してから return する。
3. 両方の fork 取得後（食事中）: 食事完了後に両方 unlock して、次のループ先頭で終了確認する。

食事中（`time_to_eat` の sleep 中）は `precise_sleep` が終了フラグを確認するため、食事の途中でも早期に抜けられる。
その場合は両方の fork を unlock してから return する。

## 1人の場合の扱い

`number_of_philosophers == 1` は特別扱いする。

fork が 1 本しかないため、philosopher は 2 本目の fork を取れない。
通常の fork order 処理に流すと、同じ mutex を 2 回 lock しようとする危険がある。

1 人の場合は次の流れにする。

1. 唯一の fork を lock する
2. `has taken a fork` を出力する
3. `precise_sleep` で待機する（終了フラグを短い間隔で確認）
4. monitor が死亡を検出し、終了フラグを立て、`died` を出力する
5. philosopher は終了フラグを検知し、fork を unlock して終了する

死亡の検出と `died` の出力は monitor に一本化する。
philosopher 自身が died を出力すると、monitor との二重出力やロック順序の問題が生じるため。

## テスト方針

ユニットテストは、入力と出力がはっきりしている純粋ロジック寄りの関数に絞る。
並行処理そのものはタイミング依存のため、ユニットテストだけでは十分に検証できない。

ユニットテスト向きの対象は次の通り。

- 引数パース
- 数値変換、負数、空文字、overflow の検出
- config 初期化
- fork id の計算
- `fork_reserved` による予約と、min/max による実 lock 順序
- timestamp 差分計算
- 死亡判定関数

fork order では、特に円環の境界を確認する。

```text
n = 5, philosopher 1: left=0, right=1 -> first=0, second=1
n = 5, philosopher 5: left=4, right=0 -> first=0, second=4
n = 2, philosopher 1: left=0, right=1 -> first=0, second=1
n = 2, philosopher 2: left=1, right=0 -> first=0, second=1
```

thread routine、mutex のタイミング、death log の遅延、data race、deadlock は実行テストで確認する。
ThreadSanitizer (`-fsanitize=thread`) で data race がないことを確認する。

基本シナリオは次の通り。

```sh
./philo 1 800 200 200       # 死亡する（fork 1本のみ）
./philo 2 800 200 200       # 死亡しない
./philo 5 800 200 200       # 死亡しない
./philo 5 800 200 200 3     # 各 philosopher が 3 回食べて停止する
./philo 4 410 200 200       # 死亡しない（time_to_die > eat + sleep）
./philo 4 310 200 100       # 死亡する（time_to_die < eat + sleep の余裕不足）
./philo 200 800 200 200     # 死亡しない、大人数での動作確認
```

異常系では、引数不足、余分な引数、ゼロ、負数、非数値、overflow を確認する。

```sh
./philo
./philo 0 800 200 200
./philo -1 800 200 200
./philo 5 abc 200 200
./philo 5 800 0 200
./philo 5 800 200
./philo 5 800 200 200 0
./philo 999999999999999999999 800 200 200
```

## 出力方針

ログ出力には `printf` ではなく `write` を使う。

`printf` は内部バッファリングがあり、flush タイミングが不定である。
mutex で出力を直列化していても、バッファの flush がスレッド間で混ざる可能性がある。
また、死亡直後にプロセスが終了するケースでバッファが flush されないまま消える恐れがある。

`write` はシステムコール一発で出力されるため、PIPE_BUF 以下であればアトミック性が保証される。
ログ 1 行は十分短いため、この条件を満たす。

具体的には自前の数値変換でバッファに整形し、`write(1, buf, len)` で出力する。
print mutex との組み合わせで、ログの混在と消失の両方を防ぐ。

`printf` は引数エラー時の usage 表示のみに限定する（シングルスレッド、正常系前なので問題なし）。

## 実装時に注意すること

philosopher の番号は 1 から `number_of_philosophers` までの 1-indexed で表示する。
内部配列は 0-indexed で管理し、ログ出力時に `id + 1` で変換する。

時刻取得には `gettimeofday` を使う（subject の許可関数にこれしかない）。
simulation 開始時のタイムスタンプとの差分をミリ秒で算出し、ログの timestamp とする。

philosopher thread は `pthread_create` で生成し、simulation 終了後に `pthread_join` で全スレッドを回収する。
monitor thread も同様に `pthread_join` で回収する。`pthread_detach` は使わない。
join で回収することでリソースリークを防ぎ、全スレッドの終了を main で確認してから exit する。

`usleep` は指定時間ぴったりに復帰する保証がない。
死亡ログは実際の死亡から 10 ms 以内に出す必要があるため、長い `usleep` をそのまま使わず、短い間隔で終了状態や現在時刻を確認する `precise_sleep` を用意する。
`precise_sleep` の確認間隔は 500μs〜1ms 程度とする。10ms 要件に対して十分短く、かつ CPU を過度に消費しない範囲である。

`last_meal_time` は、両 fork 取得直後（`is eating` 出力前）に更新する。
subject では、`time_to_die` は最後の食事開始、または simulation 開始からの経過時間で判定される。
各 philosopher の `last_meal_time` はスレッド生成前に simulation 開始時のタイムスタンプで初期化する。
スレッド生成前であればロック不要。初期化しないと、最初の食事前に monitor が誤って死亡判定する。

monitor は全 philosopher を巡回し、死亡と全員の食事回数達成を確認する。
各巡回の完了後に 1ms 以下のスリープを入れる。
N が大きい場合（例: 200人）でも、巡回処理自体 + スリープの合計が 10ms を超えないようにする。
priority queue などの複雑なデータ構造は使わない。
この課題では、データ構造の工夫よりも共有状態の保護と lock 順序の方が重要である。

実装は小さく動かしながら進める。

1. 引数パースと config
2. 時刻関数と sleep 関数
3. mutex 付きログ出力
4. 1 人ケース
5. 2 人ケース
6. 複数人ケース
7. monitor による死亡判定
8. optional eat count
9. sanitizer と長時間テスト

README は subject 上必須だが、この設計メモとは役割を分ける。
`README.md` は英語の提出用ドキュメントとして、概要、ビルド方法、実行方法、参考資料、AI 利用について後で整理する。
