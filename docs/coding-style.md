# Coding Style

## malloc

戻り値にキャストを書かない。

```c
int *p = malloc(sizeof(*p) * n);
```

## sizeof

中身は型ではなく変数を書く。

```c
sizeof(*p)    // good
sizeof(int)   // avoid
```

## 出力

ログ出力には `write` を使う。`printf` は usage 表示のみ。
理由と詳細は `docs/design.md` の「出力方針」を参照。

## 変数名

スコープの広さに応じて名前の長さを変える。

- 狭いスコープ（ループ、短い関数内）: `i`, `n`, `buf`, `len`, `ret`
- 広いスコープ（構造体メンバ、モジュール横断）: `last_meal_time`, `eat_count`, `philo_count`
- 略語: 定着したもの（`ctx`, `cfg`, `msg`, `ts`, `ms`）は使う。独自略語は避ける。

## 関数名

`モジュール接頭辞_動詞` の形式にする。

```c
philo_eat();
monitor_check();
print_status();
time_get_ms();
```

内部ヘルパーは `static` にして接頭辞を省略可。

## 構造体

norminette 準拠の typedef 形式を使う。型名には `t_` 接頭辞。

```c
typedef struct s_philo
{
    int id;
    // ...
} t_philo;
```

## 定数・マクロ

`UPPER_SNAKE_CASE` にする。

```c
#define MAX_PHILO 200
#define USAGE_MSG "Usage: ./philo ..."
```

## ファイル構成

- 1ファイル1責務を意識する程度
- norminette の5関数制限があるので自然とモジュール分割される
- 言語機能としてカプセル化はできないので、それ以上の設計原則は追わない

## DRY

同じロジックを2箇所以上に書かない。繰り返しが出たら関数に切り出す。

## エラー処理

早期リターンで異常系を先に弾く。正常系をネストさせない。

返却値の規約:

- `int` 返却関数: 成功 `0`、失敗 `-1`
- ポインタ返却関数: 失敗 `NULL`

```c
if (!ptr)
    return (NULL);
// normal path continues here
```

システムコール（`gettimeofday`, `write`, `usleep` 等）は有効な引数を渡す限り失敗しない。
戻り値の防御チェックは不要。
