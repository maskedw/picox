# ヘッダファイル

## define インクルードガード

### 結論
ディレクトリ階層も含めたインクルードガード名を定義する。

### 理由

ヘッダファイル名だけをインクルードガードに使用しているのをよく見かけますが、これ
はやめた方がいいでしょう。

```cpp
    /* Foo.hというファイル名とする */
    #ifndef Foo_h_
    #define Foo_h_

    ...

    #endif /* Foo_h_ */
```

ファイル数が増えてくると、ファイル名が重複することは十分にあり得るからです。ファ
イル名だけのインクルードガードが深刻なバグの原因となり得ます。
ディレクトリ階層も含めたインクルードガードにすべきです。

```cpp
    /* bar/Foo.h */
    #ifndef bar_Foo_h_
    #define bar_Foo_h_

    ...

    #endif /* bar_Foo_h_ */

    /* baz/Foo.h */
    #ifndef baz_Foo_h_
    #define baz_Foo_h_

    ...

    #endif /* baz_Foo_h_ */
```

### 考察

また、インクルードガードは大抵全て大文字で定義されていますが(`BAZ_FOO_H_`)個人的
にはディレクトリ名やファイル名と一致した名称の方が好みです(`baz_Foo_h_`)。

マクロや定数は大文字というルールは多いですが、インクルードガードはそれらとは明確
に用途が違うので大文字にこだわる必要はないでしょう。

ファイルのリネームや移動を行った際にスクリプトによるインクルードの一括変換が簡単
にできた方がいいので、一貫性のある命名規則にしましょう。

## `#pragma once`の是非

### 結論
使用しない。

### 理由

使用する派の論旨は以下のようなものです。

+ インクルードガードより高速に処理できるのでコンパイル時間が短縮できる
+ 大抵のコンパイラでサポートされている

しかし実際の所、高速化の効果の程は微妙です。それより依存関係の最小化に注意を払っ
た方がよほど効果があると思います。また、大抵のコンパイラでといった主張には組込み
系のコンパイラは考慮されていません。
無効なpragmaは無視してくれれば、pragma + インクルードガードが最良ということにな
るのですが、エラー扱いになることが多いので、デメリットの方が多いと考えます。


## ヘッダファイルの依存関係

## 結論

前方宣言を多いに活用して、極力無用なヘッダファイルはインクルードしないようする。

### 理由
ヘッダファイルの依存関係が増えると少しのヘッダの修正が大量の再コンパイルを引き起
こします。小規模な開発であれば、この再コンパイル時間は許容できるかもしれません
が、キチンと依存関係が考慮されたヘッダファイルかどうかを見るのはそのプロジェクト
の品質を図る指針の一つであると思います。
プログラマが、前方宣言を理解しているかや依存関係の最小化に意識を払っているかは力
量を図る指針になるからです。

### 考察
とはいえ、常に最小限のヘッダファイルだけをインクルードするというのは中々面倒なも
のです。高級な言語では大抵不要なimportを検出する機能がありますがC/C++で不要な
includeを自動検出する一般的な方法はありません。
無用なヘッダファイルはインクルードしないというルールには反しますが、頻繁に使う
ヘッダ(特にC標準ライブラリ)はプロジェクト共通のヘッダファイルでまとめてincludeし
てしまっていいと思います。ただしC++のヘッダは巨大なので、プリコンパイルヘッダの
使用が前提でない限り共通ヘッダでのインクルードは控えた方がいいでしょう。

さらに列挙型やdefine定数も、別のヘッダが必要とするものは共通ヘッダに入れておいた
方が扱いやすいです。
あるファイルの列挙型やdefine定数をの定義を参照したいだけなのにインクルードファイ
ル間の依存関係を複雑にするのは得策ではないと考えます。

```cpp
    #ifndef project_core_h_
    #define project_core_h_

    #include <stddef.h>
    #include <stdint.h>
    #include <stdbool.h>
    #include <stdarg.h>
    #include <string.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <limits.h>

    enum Foo
    {
        FOO_XXX,
        FOO_YYY,
    };

    #define FOO_BAR     (10)

    #endif /* project_core_h_ */
```


### 問題点

+ そもそも前方宣言を理解しているプログラマが少ない <br>
     ...勉強してください！！前方宣言を理解することはC言語を理解するいい勉強にな
     ると思います。


## ソースファイル内のヘッダファイルインクルード順序

Foo.cというファイルに対応するFoo.hというヘッダがあるとします。この時、Foo.cで
Foo.hをインクルードすると思いますが、その他インクルードファイルとの順序をどうす
るかという問題です。

### 結論
ソースファイルに対応するヘッダファイルを初めにインクルードする。

### 理由
include順に制限があるヘッダファイルは良くないヘッダファイルです。例えばFoo.hのイ
ンクルードの前には必ずBar.hをインクルードしておかなければならないといった制限は
最悪です。すぐに順序関係が意味不明になって、コピペincludeが横行してしまうことに
なるでしょう。
しかし、記述ミスや関連するヘッダファイルの変更で意図せず順序関係のあるのあるヘッ
ダになってしまうことがあります。初めにソースに対応するヘッダをインクルードするこ
とでそのミスを検出できます。

```cpp
    /* Foo.c */
    /* 記述ミスでFoo.hの前にBar.hをインクルードしないといけないという順序関係が
     * 発生しているが、ソース内でたまたまBar.hを先にインクルードしているとそのミ
     * スに気づけない
     */
    #include "Bar.h"
    #include "Foo.h"
    #include "Baz.h"

    /* 初めにインクルードしておけばエラーになるので、ミスを検出できる。
     * 先頭以外のヘッダの順序は好きにすればいい。
     */
    #include "Foo.h"
    #include "Bar.h"
    #include "Baz.h"
```

------------------------------------------------------------

# コメント

## 日本語コメント

### 結論
日本語コメントには必ずマルチラインコメント`/* */`を使用する。

### 理由
ファイルエンコーディングがutf8なら恐らく問題ないはずですが、sjisだと結構なコンパ
イラで深刻な不具合の元になります。
エラーになるならまだいいですが、謎の命令が生成される等のいやらしい症状が発生する
ことがあります。`/* */`だと、コメント終端(`*/`)をコンパイラが正確に認識できま
す。sjisは末尾の日本語文字によっては'\n'の検出に失敗し、不正な解釈が行われること
があるのです。

## `/* */`か`//`か
どっちのコメント形式を使用すればよいのでしょうか？

### 結論
日本語じゃなければあまり気にしなくてよい

### 考察
一応シングルラインコメントが正式に規格化されているのはC99以降です。しかし、ま
あこれは流石にどんなコンパイラでも使用できると思います。
GitHub等で広く公開したいライブラリで、C89対応を謳うなら全て`/* */`で統一した方が
行儀がよいといえるでしょう。

## 関数のコメント

### 結論
少なくともグローバル関数にはDoxygenスタイルのコメントを書きましょう。
Doxygenの出力は対して役に立ちませんが、決まったスタイルで書くべきです。
Doxygenのディレクティブについては各自調べましょう。

### 理由
この関数がどういった入力を想定していて、どんな出力を返すのか、前提条件は何なの
か。それら全てを毎回ソースコードから確認するのはゲンナリしますね。
とはいえ、あまりにも冗長に書く必要はないと思います。

ここまで書かなくても

```cpp
    /** 矩形の幅を返します。
     *  @param self 矩形オブジェクト
     *  @return 矩形の幅
     *  @pre
     *  + self != NULL
     */
    int xrect_width(const XRect* self);
```

シンプルな関数ならこれで十分でしょう。このレベルの関数ならコメントなしでもいいく
らいですね。

```cpp
    /** 矩形の幅を返します。
     */
    int xrect_width(const XRect* self);
```

`[in], [out]`等のディレクティブは使用しなくていいです。constのありなしで、`[in],
[out]`は明白だからです。入出力兼用の引数の時には`[in, out]`と書いた方が親切です
ね。

## コメントをソースとヘッダのどちらに書くか

### 結論
ヘッダファイルに書く。

### 理由
ヘッダファイルにコメントを書くと、コメントを編集するだけでヘッダをインクルードし
ているソースの再コンパイルが必要になってしまう、というデメリットがあります。
しかし、このスタイルガイドではコンパイルに数時間かかるような超大規模プロジェクト
は想定していません。
それよりも、ヘッダファイルがそのまま関数ドキュメントにもなるということの方が魅力
を感じます。

## コメントの粒度

### 結論
やや過剰なくらい書く。

### 理由
よく実装がキレイであればコメントは最小限で構わないといわれます。
しかし、我々日本人にとって英文で書かれたコードはどれだけ洗練されていてもやはり読
みづらいものです。注意深く命名され、整理されたコードは、素の英文に近くなると思い
ますが、そもそも一般的な日本人プログラマにネイティブの英文が理解できる人がどれだ
けいるのでしょうか？

と考えると欧米プログラマに比べて、やや多めのコメントを意識することはよい習慣だと
思います。
とはいえ間違ったコメントや、無意味なコメントは負債になるということも理解しておき
ましょう。

```cpp
    /* 言うまでもないですがこういう無駄なコメントは書かないこと */
    int value = 0; /* 変数valueを0に初期化する */
```

どうせコメントを書くから変数名や関数名は適当でいいとは考えないでください。十分に
吟味した上で、さらに日本語のコメントをつけましょう。

------------------------------------------------------------

# 書式

コーディングスタイルや書式にはかなり好みがありますが、全員が同じスタイルを使うと
プロジェクトの見通しがよくなります。とはいえ書式のルールに合意してくれない人もい
るでしょう。慣れるまで時間がかかるルールもあるでしょう。それでもプロジェクトのコ
ントリビュータ全員が同じスタイルにしたがうことは重要です。これによって、全員の
コードが読みやすく理解しやすくなるためです。

## 行の長さ

### 結論
1行は長くても100文字までにしましょう。

### 理由
単純に長すぎる行は読みにくいからです。70~80文字を推奨しているスタイルが多いです
が、C/C++は冗長な記述が多くなりがちなので、70~80文字では厳しいと考えます。もう少
し緩和して100文字としました。

### 考察
では、100文字を超えた場合の行送りはどうすればいいでしょうか？以下に推奨案を掲載
します。

```cpp
    /* --------------------------------------------------
     * 関数コール
     */
    /* '('で2段インデントして引数ごとに改行 */
    long_name_function(
            arg1,
            arg2,
            arg3);

    /* '('で2段インデントして適当な引数ごとに改行 */
    long_name_function(
            arg1, arg2, arg3, arg4,
            arg5, arg6, arg7, arg8
            arg9);

    /* 第一引数に合わせて引数ごとに改行 */
    long_name_function(arg1,
                       arg2,
                       arg3);

    /* 適当な引数ごとに改行して、2行目以降は第一引数に揃える */
    long_name_function(arg1, arg2, arg3, arg4,
                       arg5, arg6, arg7, arg8,
                       arg9);

    /* --------------------------------------------------
     * if文等
     */

    /* 条件ごとに改行 */
    if (condition1 ||
        condition2 ||
        condition3)
    {
    }

    /* 100文字以内で改行、2行目以降は第１条件に揃える */
    if (condition1 || condition2 || condition3 || condition4
        condition5 || condition6 || condition7)
    {
    }

```

## タブ vs スペース

### 結論
タブは使用せず全てスペースを使う。

### 理由
タブはエディタの設定によって見た目が変わるからです。全てがタブならまだましです
が、タブとスペースが混在していると悲惨なことになります。
タブキーを押したときにはスペースが入力されるようエディタを設定しておきましょう。

## インデント幅

### 結論
インデントは空白4文字

### 理由
2文字は狭すぎ、8文字は広すぎなので4文字がちょうどいいでしょう。一般的なスタイル
でも4文字が大多数です。

## ポインタの`*`(アスタリスク)の位置

以下の3種類を見かけるがどれを採用するか？

```cpp
    /* 左寄せ*/
    char* p;
    /* 中央 */
    char * p;
    /* 右寄せ */
    char *p;
```

### 結論
左寄せを採用する。

### 理由
一番自然に読めるから。

### 考察
右寄せ派の主張として、以下のようなケースで左寄せには一貫性がないというものがあり
ます。

```cpp
    /* 左寄せで1度に復数のポインタ変数を宣言すると、見た身に一貫性がない */
    int* p1, * p2;
    /* これはint*とintの宣言 */
    int* p, i;
    /* 右寄せだと自然 */
    int *p1, *p2;
```

しかし、そもそも1行で復数の変数宣言はあまりやるべきではないというスタンスです。
複数行にわけて書くことに何のデメリットもないので、普段使いの自然さを重視していま
す。


## ブラケット(`{}`)の位置

### 結論
全てのブラケットの前で改行します。

### 理由
正直いって一貫性があれば何でもいいと思います。ブラケットの前での改行は個人的な趣
味です。

### 考察

**ブラケット後改行 VS ブラケット前改行**

```cpp
    /* ブラケット後 */
    if (condition1) {
        ...
        ...
    } else if (condition2) {
        ...
        ...
    } else {
        ...
    }

    for (int i = 0; i < 10; i++) {
        ...
        ...
    }

    while (condition)  {
        ...
        ...
    }

    do {
        ...
        ...
    } while (condition);

    switch (condition) {
        case 0:
            break;
        case 1:
            break;
        case 2: {
            int x;
            break;
        }
    }
```

```cpp
    /* ブラケット前 */
    if (condition)
    {
        ...
        ...
    }
    else if (condition2)
    {
        ...
        ...
    }
    else
    {
        ...
    }

    for (int i = 0; i < 10; i++)
    {
        ...
        ...
    }

    while (condition)
    {
        ...
        ...
    }

    do
    {
        ...
        ...
    } while (condition);

    switch (condition)
    {
        case 0:
            break;
        case 1:
            break;
        case 2:
        {
            int x;
            break;
        }
    }
```

個人的にはブラケット後の方が、見た目は洗練されていると思います。しかしelse ifの
条件式を書こうとすると、密度が高すぎて見づらく感じます。
対してブラケット前は見やすいですが、ifブロックが1ステートメントの時、密度が低す
ぎて間抜けな印象に見えるのと`do...while`が相当ダサいです。どっちもどっちですが、
見た目の不格好さより読みやすさを優先してブラケット前を選定しました。

## ハンガリアン記法の是非
ハンガリアン記法とはint8_t変数をi8xxxとしたり構造体変数や形名をStXXXとしたりする
記法のことです。もはや絶滅危惧種の記法ですが、組込み世界では未だにちょくちょく見
かけます。

### 結論
ハンガリアン記法は使用しない。ただし、グローバル変数を示す`g_`は使用する。

### 理由
1つだけを使用するならまだいいとして、復数組み合わせた場合、すぐに謎の暗号とかし
てしまうからです。
例えばグローバル変数の構造体のポインタならgstp_xxxとでもするのでしょうか。意味が
わかりませんね。順番も明確に決めておかないとgstp, pgst, stgp等、謎の暗号が次々に
量産されることでしょう。

このスタイルガイドは読み手にメリットがあるなら多少不格好でも受け入れるスタンスで
すが流石に許容できない醜さです。そもそも読み手にも全く優しくないですね。

## プロトタイプや構造体定義等の記述順
別にコンパイルできればどんな順番でもいいのですが、読み手の立場を考えると、一貫し
ている方が嬉しいですね。
では、どんな順番で並べるのがいいのでしょうか？

### 結論

以下の順に書く。

+ include
+ 前方宣言
+ define
+ 型定義
+ プロトタイプ
+ 変数定義
+ 関数定義

依存関係の問題から、100%この並びでできるわけではないですが、大抵はこれでいけま
す。

### 理由
+ include <br>
    includeファイルに依存関係がないことが前提なので、最優先で書きます。

+ 前方宣言 <br>
    前方宣言にも依存項目はないので一番上でもいいですが、見た目的にincludeの次の
    方がいいでしょう。

+ define <br>
    defineはただのテキスト置換なので、以下のように書いてもエラーにはなりません
    ```cpp
    /* struct Foo定義の前にこういうdefineがあってもOK */
    #define FOO     struct Foo

    struct Foo
    {
        ...
    };

    /* 実際にFOOを使用している場所の前にstruct Fooが定義されていればよい */
    FOO foo;
    ```

+ 型定義<br>
    includeと前提宣言があれば問題なくかけます。

+ プロトタイプ<br>
    型定義が必要なので型定義の後ろです。

+ 変数定義<br>
    関数ポインタに代入するためにプロトタイプの後ろにすべきです。

+ 関数定義<br>
    全ての定義が必要なので関数定義は最後です。

```cpp
    #include "baz.h"

    struct Foo;

    #define FOO     struct Foo

    typedef struct Foo
    {
        ...
    } Foo;

    typedef void(*FooFunc)(Foo* foo);

    static void function(Foo* foo);

    static FOO foo;
    static FooFunc = function;

    static void function(Foo* foo)
    {
    }
```

## その他

+ `(`の前にはスペースを入れる
+ `()`の内側にはスペースを入れない
+ `=`等の演算子の前後には1つ以上のスペースを入れる
+ 条件式ごとに必ず`()`で包む<br>
    演算子の優先順位にはかなりクセのあるものもあります。一々優先順位を気にするよ
    り、自明なものでもとにかく`()`で包んで曖昧さを排除しましょう。

```cpp
    if (condition == 0)
    {
        ...
    }
    else if ((condition2 == 0) || (condition3 == 0))
    {
    }
```

## 既存のコードがこのスタイルと異なる時

### 結論
既存のスタイルに従いましょう。

### 理由
何よりも大事なことは一貫性です。まとまった単位でこのスタイルに置き換えるのは構い
ませんが、部分部分でスタイルを替えることはやめましょう。

------------------------------------------------------------

# プリプロセッサ

## enum VS define定数

ヘッダファイルで定数を定義したい場合,enumかdefine定数を使用することになります
が、どちらを使用するべきでしょうか？

### 結論

状況による。

### 理由

enumを使用した方が、デバッガから値を確認する時にシンボルで見れたり、引数の型にす
ることで、受け取れる値の範囲を明示できたりと、便利なことが多いです。
しかし全ての面でdefineより優れているわけではありません。

+ `#if`等のプリプロセッサ条件式にでenumは使用できない。<br>
    これは案外盲点になるのですが、プリプロセスはコンパイル前に走る処理なので、C
    の機能であるenum値をプリプロセッサは解釈できません。

+ sizeof(enum)に注意が必要。<br>
    enumのサイズは規格上定義されていません。殆どの環境でsizeof(int)と等しくなり
    ますが、これは8bit,16bitのmpuへの移植時に列挙値のオーバーフロー問題になるこ
    とがあります。

上記制限を理解した上で、状況に合わせてどちらを使用するか選択しましょう。
個人的にはenumは連番でしか使用せず、ビットフラグ等の連番でない値にはdefineを使用
するのがいいと思います。

------------------------------------------------------------

# インターフェース

## 関数パラメータの順序

### 結論

関数を定義する時は、パラメータの並びを「出力、入力」という順序にしましょう。

### 理由

一貫性があればどちらでもいいですが、C標準ライブラリが大抵このスタイル(memcpy(),
strcpy()等)なので、これに合わせます。
少なくとも「出力、入力、出力、出力」というような互い違いにすることは避けてくださ
い。

### 考察

これは厳格なルールではありません。入力であり出力でもあるパラメータがあり、ルール
がうまく適用できない場合があるためです。しかし一般的にいって、そういったインター
フェースは設計が良くないと考えられます。入力出力兼用パラメータを定義する前に、設
計で回避できないかも検討しましょう。

## ポインタ引数のconst

### 結論
グローバル関数で値を変更しないポインタ引数は、絶対に何がなんでもconstをつける。

### 理由
引数にconstを付けている関数内で、constがついていない関数を呼び出す時、キャストで
constを外す必要があります。

何より、プロトタイプ自身がなるべくその関数の仕様を表すべきです。
constがついていないと、一々ソースコードを確認して、この引数の参照先は変更される
のかどうかを確認しなければなりません。

呼吸と同じくらい自然に、constを使用することは当然という意識をもってください。

## 引数が不正だった場合assertするか戻り値でエラーステータスを返すか

### 結論
極力assert()で異常を検出し、即座に実行を停止させましょう。

### 理由
エラーの検出は早いほど原因究明がラクです。バイナリ配布のライブラリを実装するなら
話は別ですが、アプリケーションコードの関数で異常値の引数を受け取って、悠長にエ
ラーステータスを返して、呼び出し側のエラー処理を期待するより、さっさと停止させて
バグ究明をさせるべきです。

### 考察
標準のassert()は挙動を変更できないのでカスタムアサートを定義した方がいいでしょ
う。省メモリ環境では、式や、ファイル名の文字列化に伴うコード使用量も馬鹿にならな
いので、カスタムアサートで文字列化のOn,Off等をできるようにしておいた方がいいで
す。

------------------------------------------------------------

# スコープ

## ローカル変数

### 結論
関数内で使う変数はできるだけスコープを限定してください。

### 理由
変数のスコープを局所化することは、コードリーディング時の理解の助けになります。

```cpp
    int func(void)
    {
        int x;
        int y;

        ...
        y = x;
        /* xを以降使用しないのなら */
        ...
        return y;
    }

    /* 局所化した方がいい。*/
    int func(void)
    {
        int y;

        {
            int x;
            ...
            y = x;
        }
        ...
        return y;
    }
```

1ヶ月後の自分は他人という言葉があります。コードの複雑さは可能な限り排除しておき
ましょう。

## グローバル変数

### 結論
極力使用しないようにしましょう。

ただし、小規模な組込み開発だと、無駄にややこしくするくらいなら素直にグローバル変
数を使った方がいいという局面もあります。
また、デバッグ環境の貧弱な組込み開発では、グローバル変数にしておくことで、いつで
も変数の値を確認できるようにしておくことが有用だったりします。
頑なにグローバル変数を禁止するのではなく、柔軟に対応しましょう。

### 考察
個人的にオススメのやり方は以下の方法です。

```cpp
    /* ファイルローカルのstatic変数は全て1つの構造体にまとめる */
    typedef struct Foo
    {
        int x;
        int y;
        int z;
    } Foo;

    /* 実際はファイルローカルだがグローバル変数として定義しておく。*/
    Foo g_foo;

    /* privはprivateの略。この名称は何でもいいですが、一貫性はもたせましょう */
    static Foo* const priv = &g_foo;
    /* これでもいいけどdefineじゃないほうが安全です */
    #define priv    (&g_foo)

    static void function(void)
    {
        /* ソースコード内では、privだけを通じて構造体にアクセスする。こうするこ
         * とで読み手にはこの変数が実際にはファイルローカルであることが伝わる
         */
        int x = priv->x;
    }
```

こうしておくと、スコープを限定させるという目的と、デバッガからいつでも値を確認
したり、なんなら変更したりといった利便性の両方を達成できます。

------------------------------------------------------------

# 命名規則

プログラミング言語にかかわらず、主だった命名規則には以下のものがあります。

+ ロアースネークケース `foo_bar`
+ アッパースネークケース `FOO_BAR`
+ ロアーキャメルケース `fooBar`
+ アッパーキャメルケース `FooBar`

他の言語とくらべて、C/C++ではこれといってデファクトスタンダードと呼べる規則があ
りません。その為少なくともライブラリやアプリケーション単位で一貫性があればそれで
よいと考えます。

しかし、`foo_bar_fooBar`等の復数スタイルを混ぜることは避けてください。例外とし
て、先頭部分とそれ以降の要素のスタイルの違いはOKです。

例えばXFifoというモジュールがあるとして
```cpp
    /* これはあり */
    void XFifo_foo_with_bar(void);

    /* これはなし */
    void XFifo_foo_WithBar(void);
```

## 命名規則一覧
以下にこのスタイルガイドが提唱するスタイルを記載します。

### ローカル変数名
```
    ロアースネークケース
```
```cpp
    int foo_bar = 0;
```

### グローバル変数名
```
    g_ロアースネークケース
```
```cpp
    int g_foo_bar = 0;
```

### static変数名
```
    {ライブラリプレフィックス}__ロアースネークケース
```
```cpp
    static int X__foo_bar = 0;
```

### 公開型名
```
    {ライブラリプレフィックス}アッパーキャメルケース
```
```cpp

    typedef int XFooBar;
    typedef struct XFooBar
    {
        ...
    } XFooBar;
    typedef enum XFooBar
    {
        ...
    } XFooBar;
```

`foo_bar_t`というスタイルもよく見かけますが、このスタイルはC限定で、その他言語で
はほぼアッパーキャメルケースが使用されていることから、それにならいます。

また、ライブラリプレフィックスをつけることで、他のライブラリとの名前の重複を避け
ることができます。

さらにライブラリプレフィックスは、名前被りの回避は勿論、開発環境や言語仕様の貧弱
なC/C++でそのシンボルの所属が見た目でわかる重要な情報源になります。

### ローカル型名
```
    {ライブラリローカルプレフィックス}__アッパーキャメルケース
```
```cpp
    typedef int X__FooBar;
    typedef struct X__FooBar
    {
        ...
    } X__FooBar;
    typedef enum X__FooBar
    {
        ...
    } X__FooBar;
```

厳密にいうとC言語ではダブルアンダースコアは処理系が使用する可能性があるので、規
格にやかましい人は拒否反応がでるかもしれませんが、実際の所先頭ダブルアンダースコ
アでなければ、まず問題になることはないでしょう。

ローカルのシンボルに対してまでプレフィックスをつけることについて否定的意見をもつ
こともあるかもしれません。
しかし、C/C++の命名規則にこれといったデファクトがないことから、名前被りを防ぐ為
には最大限の防衛的配慮が必要です。ローカルシンボルであっても、includeファイルか
らどういったシンボルが取り込まれるか、事前に予測することは不可能です。

例えば、組込み環境ではベンダが提供するライブラリのヘッダファイルにwaitやdelayと
いった一等地のシンボル名が関数マクロとして普通に定義されていたりするのです。
windows環境でもmin,maxが関数マクロで定義されますね。恐ろしい話です。

### グローバル関数名
```
    {モジュールプレフィックス}_ロアースネークケース
```
```cpp
    const char* xtok_ref_token(const XTokenizer* self, int col)
```

### ローカル関数名
```
    {ライブラリローカルプレフィックス}__{アッパーキャメルケース}
```
```cpp
    static const char* X__RefToken(const XTokenizer* self, int col);
```

### 公開define定数, 公開enum定数, 公開関数形式マクロ
```
    {大文字モジュールプレフィックス}_{アッパースネークケース}
```
```cpp
    #define XFOO_BAR    (10)
    enum XFoo
    {
        XFOO_BAR,
    };
    #define XFOO_BAR(x)     ((x) + 10)
```

### ローカルdefine定数, ローカルenum定数, ローカル関数形式マクロ
```
    {ライブラリローカルプレフィックス}__{アッパースネークケース}
```
```cpp
    #define X__FOO_BAR    (10)
    enum X__Foo
    {
        X__FOO_BAR,
    };
    #define X__FOO_BAR(x)     ((x) + 10)
```

## アプリケーションコードのプレフィックスについて

ライブラリプレフィックスはいいとして、アプリケーションコードはどうすればいいで
しょうか？
アプリケーションごとにプレフィックスをつけようとすると、冗長な名称になりそうです
し、ライブラリ程ではないにせよ、部分単位でコピペして、別のアプリケーションに流用
する時に、プレフィックスの置換えが面倒ですね。

よって、アプリケーションを略してAppというプレフィックスをつけることを提唱しま
す。これだと名前かぶりはまずないですし、冗長性もさほどでもなく扱いやすいですね。
一部を切り出してライブラリに格上げする時もプレフィックスを置き換えるだけで済むの
で簡単です。

```cpp
    struct AppUart;
    void AppUart_send_byte(AppUart* self, uint8_t b);
    #define APP_FOO     (1)

    /* もっと省略してAだけでいいかも？ */
    struct AUart;
    void AUart_send_byte(AUart* self, uint8_t b);
    #define A_FOO     (1)
```

------------------------------------------------------------

# その他

## 構造体や列挙型のtypedefの是非

Cの知識が全然ないプログラマでも何故か構造体や列挙型のtypedefに限っては異様に普及
しているように思います。入門書等の書籍でも積極的に推奨されているようです。
しかし、不必要な抽象化は混乱を招くとして、typedefを行わない流儀もあります(Linux
カーネル等)。果たしてどちらにすべきか？

### 結論

typedefする。

### 理由

確かに引数や、変数で`struct XXX`, `enum YYY`と明示されている方がわかりやすいと感
じることも多いです。しかし、以下の理由から、typedefをすることにします。

+ typedefする流儀が過半数を占める <br>
    基本的にこのスタイルガイドでは過半数を占めるルールを積極的に採用します。

+ C++との一貫性 <br>
    C++ではtypedefなしで、`struct, class, enum`が省略可能です。省略しないことも
    できますが、省略していない例を見かけたことがありません。基本的にC++で使用す
    ることも想定しているので、一貫性は保ちたい所です。

+ やっぱりちょっと冗長に感じる <br>
    ただでさえCではモジュールプレフィックスを付けた命名やオブジェクト指向スタイ
    ル等を意識すると、型名やプロトタイプ宣言が冗長になってしまうものです。それに
    くわえて`struct, enum`を付加すると流石に冗長すぎると感じます。


## WORD, DWORD, BOOLとかを独自にtypedefしない

### 結論
`stdint.h`, `stdbool.h`等のC標準ヘッダを使用する。

### 理由
WORD、DWORD等の独自定義は以下の理由から百害あって一理なしです。

+ 他のモジュールもtypedefしているとバッティングする。
+ 別の環境に移植する時に環境に合わせて移植する必要がある。
+ 無駄な依存ヘッダが増える。

標準で定義されているものを極力使用しましょう。

### 考察
`stdint.h`, `stdbool.h`はC99の標準ヘッダなので、環境によっては提供されていない可
能性があります。そういった場合でも、自前でstdint.h等を用意してWORDではなく
uint16_t,DWORDではなくuint32_tを定義した方が移植性に優れています。

boolについてはsizeof(bool)は規格で定義されていないので、メモリ節約の観点から
sizeof(char)を保証したBool型を使いたいということもあると思いますが、その時は{モ
ジュールprefix}Bool8といった型名で定義し、直接BOOLという名称は使わない方がいいで
す。


## 文字コード
utf8かsjisの2択だと思いますが、どちらがよいのでしょうか？

### 結論
極力utf8を使用する

### 理由
日本語コメントの項でも書きましたが、sjisはトラブルが発生しやすいです。また、Unix
系のツールと外部連携する場合は、utf8の方が対応状況がいいです。
開発環境がsjisしか対応していない場合を除き、utf8を使用する方がいいと思います。

### 反対意見
+ Windows用のIDEはデフォルトでsjisの設定になっていることが多く変更がわずらわしい。

## ローカル変数のconst

### 結論
なるべくconstな変数にはconstをつけよう

### 理由
このスタイルガイドでは一貫して、読み手への配慮を最大限に行います。他人(1ヶ月後の
自分も他人)が読んだ時にどれだけわかりやすく書くかを考えれば、自然とconstをつける
という選択になるはずです。
この変数が以降変更されない、という情報は読み手に優しいし、コンパイラの最適化の助
けにもなります。

```cpp
    /* 読み手: ふむふむ。この変数はこれ以降変更されないんだな。 */
    const int x = func();
    int* const p = func_ptr();
    /* 読み手: この変数は関数内のどこかで変更されるのかな？ */
    int y = func();
```

ただし、関数の引数にまでconstをつける必要はないです。そこまですると流石に過剰で
しょう。

```cpp
    void memcpy(void* dst, const void* src, size_t n);

    /* これはやりすぎ */
    void memcpy(void* const dst, const void* const src, const size_t n);
```


