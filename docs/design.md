# Philosophers Design Notes

実装の正本。`philo/` の mutex 分担・食事許可・ロック順序・テスト方針をまとめる。

## 課題の捉え方

主題は並列化ではなく、複数 thread が共有リソースと共有状態を安全に扱うこと。

mandatory `philo` では各 philosopher が別 thread になり、fork・終了状態・食事時刻・食事回数・ログが共有される。設計の中心は次の2点。

- どのデータをどの mutex で守るか
- mutex をどの順序で取るか

特に守ること:

- fork の二重取得を防ぐ
- data race を起こさない
- deadlock を起こさない
- 死亡判定と `died` 出力を遅らせすぎない（10 ms 以内）
- ログを重ねない
- simulation 終了後に不要な状態ログを出さない
- 「1 本だけ握ったまま長時間 2 本目を待つ」状態を避ける（誤死亡の主因）

## subject 上の制約

mandatory で使える同期は mutex のみ（`pthread_cond_t` は不可）。

```text
pthread_create / pthread_join
pthread_mutex_init / destroy / lock / unlock
```

fork は subject 上も mutex で守る。`fork_reserved[]` は予約用の論理状態であり、実 fork の排他は `forks[]` の mutex が担う。

## データ構造

### `t_table`

| メンバ | 役割 |
|--------|------|
| `state_mutex` | `finished`, `death_printed`, `fork_reserved[]`, `meal_turn`, `active_reservations`, 各 philo の `last_meal_ms` / `eat_count` |
| `print_mutex` | ログ 1 行の直列化 |
| `forks[]` | 各 fork の `pthread_mutex_t` |
| `fork_reserved[]` | 食事許可済みの論理予約（`state_mutex` 下でのみ読み書き） |
| `meal_turn` | 予約を許可する turn |
| `active_reservations` | 現在の turn で予約済みの食事数 |
| `finished` | simulation 終了 |
| `death_printed` | `died` を 1 回だけ出すためのフラグ |

### `t_philo`

| メンバ | 役割 |
|--------|------|
| `last_meal_ms` | 最後の食事開始時刻（死亡判定用） |
| `eat_count` | 食事回数（optional 5 引数用） |
| `left_fork_index` / `right_fork_index` | 円卓上の隣接 fork |

`enum` による philosopher 状態機械は採用していない。ログ 5 種と終了状態・食事回数の確認で足りる。

## デッドロック回避

### リソース階層

実 fork の `pthread_mutex_lock` は常に小さい fork index から（`philo_order_forks`）。

```c
first  = min(left_fork_index, right_fork_index);
second = max(left_fork_index, right_fork_index);
```

循環待ちを作らない。

### fork 予約

`fork_reserved[i] == true` は「この fork は予約済みで、これから物理 lock する」意味。

予約は `state_mutex` 下で両方まとめて行う。予約に成功するまで fork mutex は触らない。これで「1 本 lock 済み・2 本目待ち」の長時間ブロックを避ける。

予約は turn 制で許可する。偶数人数では 0-indexed 偶数グループと奇数グループを交互に進める。奇数人数では、最後の philosopher と 0 番 philosopher が隣接して同じ偶数グループになるため、最後の philosopher を第 3 グループとして扱う。

turn は固定の座席配置だけで決める。隣の philosopher の `last_meal_ms` や食べたい意思は見ない。

## 饥饿（starvation）対策

階層だけでは `5 800 200 200` などで隣が先に食べ続け、1 本目だけ取って `time_to_die` 内に食事開始できないケースが出る。スケジューリングの公平性問題。

### turn 制

失敗回数カウンタは使わない（OS スケジューラ依存で不安定）。

隣の `last_meal_ms` も優先度には使わない。philosopher 同士が互いの空腹度を知る設計に見えるため。

固定 turn で予約許可を切り替えることで、隣接 philosopher が同じ turn で同時に fork 競争へ入ることを避ける。

### 予約条件（`try_reserve_forks`）

`state_mutex` 下で、次をすべて満たすときだけ両 fork を予約する。

- simulation 未終了
- 自分の turn
- 両 fork が未予約

### 食事待ちループ（`philo_wait_fork_reservation`）

1. 予約を試行
2. 失敗なら `time_sleep_ms(table, 1)` で再試行（終了フラグと自分の食事回数も確認）
3. 成功したら fork mutex へ進む
4. 食事完了または失敗時に予約解除

`pthread_cond_t` が使えないためポーリング。再試行間隔は 1 ms（`time_sleep_ms` 内で 500 μs 刻みの `usleep`）。

### 起動遅延（補助）

0-indexed 奇数（表示上は偶数 id）の philosopher は routine 開始時に `time_to_eat / 2` ms 遅延（`stagger_start`）。予約競争の偏りを緩和する。

## 食事ループ（2 人以上）

`philo_routine` → `routine_multi`:

```text
stagger_start（奇数 index のみ）
loop while not finished:
  philo_meal_cycle
  must 回数未到達なら rest_phase（sleep → thinking ログ）
```

`philo_meal_cycle`:

1. `philo_order_forks` で first / second を決定
2. `philo_wait_fork_reservation`（上記）
3. `acquire_forks`: `forks[first]` lock → fork ログ → 終了なら解放して return
4. `forks[second]` lock → fork ログ → `philo_record_meal_start`（`last_meal_ms` 更新）
5. `eat_phase`: `is eating` ログ → `time_sleep_ms(time_to_eat)` → `eat_count++` → fork unlock → 予約解除

`last_meal_ms` は両 fork 取得と 2 回目の `has taken a fork` の後、`is eating` の前に更新する。subject の「最後の食事開始」基準に合わせる。

各 philo の `last_meal_ms` は thread 生成前に simulation 開始時刻で初期化する。未初期化だと最初の食事前に monitor が誤死亡判定する。

## mutex の役割分担

- **fork mutex**: 物理 fork の占有
- **state_mutex**: 終了フラグ、予約配列、turn、食事時刻・回数
- **print_mutex**: `write` による 1 行出力の直列化

### ログ（`print_status`）

1. `state_mutex` で `finished` を確認
2. 終了済みなら return（通常ログなし）
3. `print_mutex` で 1 行出力
4. 両 mutex を解放

死亡ログ（`print_death_locked`）は monitor が `state_mutex` 保持中に呼ぶ。`death_printed` で 1 回だけ。`finished` を立ててから `died` を出す。

fork を 1 本握った状態で `print_status` するパスがある（1 本目取得直後のログ）。意図的で、ログ直後に終了チェックして fork を解放する。

### monitor（`monitor_routine`）

別 thread で巡回:

1. 各 philo の死亡判定（`last_meal_ms` は `state_mutex` 下で読む）
2. optional: 全員 `eat_count >= must` なら `finished`
3. 1 ms sleep（`time_sleep_ms`）して繰り返し

`state_mutex` を長時間保持したまま sleep しない。死亡検出の遅延を抑える。

## 1 人の場合

`routine_one_philo`:

1. 唯一の fork を lock
2. `has taken a fork` を 1 回だけ出力
3. `time_sleep_ms(UINT_MAX)` で終了まで待機（500 μs 刻みで `finished` を確認）
4. monitor が死亡検出 → `died`（philosopher 側は出さない）
5. fork unlock して終了

2 本目の fork がないため、通常の予約・階層ルートに入れない。

## 時間まわり

- 時刻: `gettimeofday` → `time_now_ms()`
- 待機: `time_sleep_ms`（deadline まで 500 μs 刻みの `usleep`、`table_is_finished` を都度確認）
- monitor の巡回間隔: 1 ms
- 予約リトライ: 1 ms

長い `usleep` を一発で使わない。10 ms 以内の死亡ログ要件と早期終了のため。

## テスト方針

### ユニットテスト向き

- 引数パース、overflow、異常値
- `philo_order_forks` の境界（n=2,5 の円環端）
- 時刻差分
- print の終了後抑止（`finished` 時）

### 実行テスト（no-death を重点）

subject 推奨: philosopher ≤ 200、各時間引数 ≥ 60 ms。

```sh
./philo 1 800 200 200
./philo 5 800 200 200
./philo 5 800 200 200 7
./philo 4 410 200 200
./philo 4 310 200 100
./philo 2 800 200 200
./philo 2 310 200 100
./philo 3 610 200 200
./philo 7 800 200 200
./philo 10 800 200 200 5
./philo 200 800 200 200
```

期待:

- `1 800 200 200`: 食べず ~800 ms で死亡
- `5 800 200 200` / `4 410 200 200` 等: 誰も死なない（複数回実行で安定）
- `4 310 200 100`: 誰か死亡
- optional 付き: 規定回数食事後に停止、死亡なし

ログの整合:

- `is eating` の前に `has taken a fork` が 2 回
- 隣が同時に `is eating` しない
- `died` の後に状態ログなし
- 死亡期待ケースで `died` が期限から 10 ms 以内

ThreadSanitizer（`-fsanitize=thread`）で data race 確認。

### 異常系引数

```sh
./philo
./philo 0 800 200 200
./philo -1 800 200 200
./philo 5 abc 200 200
./philo 5 800 0 200
./philo 5 800 200
./philo 5 800 200 200 0
```

## 出力方針

ログは `write`（`print_write_line`）。`printf` は usage のみ。

`printf` のバッファは flush タイミングが不定で、mutex 直列化だけでは混在・消失のリスクがある。`write` は短い行ならアトミックに近い挙動になる。

詳細は `docs/coding-style.md` も参照。

## モジュール対応

| ファイル | 責務 |
|----------|------|
| `philo_reserve.c` | fork 順序、turn 制予約、待ちループ |
| `philo_meal.c` | fork 取得、食事、解放 |
| `philo_routine.c` | thread 本体、1 人/複数、stagger |
| `philo_state.c` | `last_meal_ms`, `eat_count` |
| `monitor.c` | 死亡・全員食事完了 |
| `print.c` | 状態ログ・死亡ログ |
| `time.c` | 時刻・割り込み可能 sleep |
| `table_state.c` | `finished` の読み書き |

## README との役割分担

`README.md` は提出用（英語、ビルド・実行・参考資料・AI 利用）。

このファイルは設計判断と実装の対応表。eval 前や bonus 着手時にコードと突き合わせて更新する。
