# GNU Make cheet sheet

## Makefile general syntax

```
target: dependencies1 dependencies2
action1
action2
...
```

## Run target

```
make target
```

targetを指定しない場合は最初に定義したものを実行する

## Assignment

### Simple assignment (:=)
変数に値を代入

### Recursible assignment (=)
変数に値を代入
再評価された時のみ実行

### Conditional assignment (?=)
変数が値を保持していない時のみ値を代入

### Appending assignment (+=)
変数に値を追加(Javascriptなどの文字列演算と同じ)

### Using patterns and special variable (%)

#### -(minus)
actionのプレフィックスとして付与するとそのアクションがエラーを吐いた際に無視して処理を継続する。

#### @(at)
actionもしくはcommandにプレフィックスとして付けるとaction/commandがSTDOUTに出力する際に
自信のコマンドを排除して実行結果だけ出力することができる。

