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

## 採用するデッドロック回避方針

mandatory では、デッドロック回避にリソース階層方式を採用する。

各 fork に配列 index として一意な id を与え、philosopher が 2 本の fork を取るときは必ず小さい fork id から lock する。

```c
first = min(left_fork, right_fork);
second = max(left_fork, right_fork);
```

食事時は次の順序にする。

1. `first` の fork mutex を lock する
2. `has taken a fork` を出力する
3. `second` の fork mutex を lock する
4. `has taken a fork` を出力する
5. `last_meal_time` を更新する
6. `is eating` を出力する
7. `time_to_eat` だけ待つ
8. `eat_count` を更新する
9. `second`、`first` の順に unlock する

この方式では、すべての thread が同じ順序規則に従って mutex を取得する。
待ち関係が小さい id から大きい id にしか進まないため、循環待ちを作れない。
その結果、deadlock を構造的に避けられる。

偶数・奇数 philosopher で fork 取得順を変える方式も使えるが、この実装ではリソース階層方式を選ぶ。
理由は、人数が偶数でも奇数でも説明が変わらず、deadlock しない根拠を評価時に説明しやすいためである。

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
- meal/state mutex: `last_meal_time`、`eat_count`、終了状態を守る

## 1人の場合の扱い

`number_of_philosophers == 1` は特別扱いする。

fork が 1 本しかないため、philosopher は 2 本目の fork を取れない。
通常の fork order 処理に流すと、同じ mutex を 2 回 lock しようとする危険がある。

1 人の場合は次の流れにする。

1. 唯一の fork を lock する
2. `has taken a fork` を出力する
3. `time_to_die` まで待つ
4. `died` を出力する
5. fork を unlock して終了する

## テスト方針

ユニットテストは、入力と出力がはっきりしている純粋ロジック寄りの関数に絞る。
並行処理そのものはタイミング依存のため、ユニットテストだけでは十分に検証できない。

ユニットテスト向きの対象は次の通り。

- 引数パース
- 数値変換、負数、空文字、overflow の検出
- config 初期化
- fork id の計算
- `min(left_fork, right_fork)` と `max(left_fork, right_fork)` による取得順序
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
可能なら ThreadSanitizer も使う。

基本シナリオは次の通り。

```sh
./philo 1 800 200 200
./philo 2 800 200 200
./philo 5 800 200 200
./philo 5 800 200 200 3
./philo 4 410 200 200
./philo 4 310 200 100
./philo 200 800 200 200
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

## 実装時に注意すること

`usleep` は指定時間ぴったりに復帰する保証がない。
死亡ログは実際の死亡から 10 ms 以内に出す必要があるため、長い `usleep` をそのまま使わず、短い間隔で終了状態や現在時刻を確認する `precise_sleep` を用意する。

`last_meal_time` は、食事開始時に更新する。
subject では、`time_to_die` は最後の食事開始、または simulation 開始からの経過時間で判定される。

monitor は全 philosopher を短い間隔で巡回し、死亡と全員の食事回数達成を確認する。
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
