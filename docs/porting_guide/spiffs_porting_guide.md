<!--
===============================================================================
AUTHOR:   MaskedW (maskedw00@gmail.com)
CREATED:  2016/11/03
REVISION: 1.0
===============================================================================
-->
## 概要
SPIFFSはFlashをファイルシステムとして使用でき、ウェアレベリング機能も備えた強力
なライブラリですが、設定が細かいので、慣れていない人には難しく感じると思います。
日本語の移植資料が皆無なのも敷居を上げる要因でしょう。

ここではSPIFFSを動かしてみたい読者を対象とし、移植に必要な情報を解説します。

------------------------------------------------------------

## define config値解説
重要な設定値を解説しますが、一度は`spiffs_config.h`の原文の解説を読みましょう。
かなり細かくチューニングできます。

### SPIFFS_SINGLETON (0 or !=0 :default 0)
`SPIFFS_SINGLETON != 0`の場合、SPIFFSに対して、Flashを1パーティションしか使いま
せんよ、と宣言することを意味します。

`SPIFFS_SINGLETON == 0`の場合は1パーティション以上使うかもね、という意味になりま
す。

`struct spiffs_config`の定義を見るとわかりますが、`SPIFFS_SINGLETON == 0`の場合、
`struct spiffs_config`の以下のメンバは除去されます。

+ phys_size
+ phys_addr
+ phys_erase_block
+ log_block_size
+ log_page_size

代わりに、SPIFFSは以下のdefine定義を使用します。

+ SPIFFS_CFG_PHYS_SZ
+ SPIFFS_CFG_PHYS_ERASE_SZ
+ SPIFFS_CFG_PHYS_ADDR
+ SPIFFS_CFG_LOG_PAGE_SZ
+ SPIFFS_CFG_LOG_BLOCK_SZ

この結果、定数に置き換わることによる多少の最適化と、RAMのわずかばかりの削減が期
待できますが、余程効率化したい理由がなければ、ややこしいので常に
`SPIFFS_SINGLETON == 0`でいいと思います。

### SPIFFS_CACHE, SPIFFS_CACHE_WR, SPIFFS_CACHE_STATS (0 or !=0 :default 1)
SPIフラッシュへのアクセスは、コスト(処理時間、消費電力)の高い操作なので、`!=0`の
場合、できるだけRAMにキャッシュして、Flashへのアクセスを減らすように試みます。キ
ャッシュ領域はmount時に引数として渡します。


### SPIFFS_READ_ONLY (0 or !=0 :default 0)
リードオンリーの場合、書込み用の関数や変数が除去されるので、コードとRAMサイズが
削減されます。
ただ、FATファイルシステムと比べて、ホスト環境で先にファイルを書いておくというこ
とはかなりやりづらいので、通常はあまりリードオンリーで使うことはないでしょう。


### SPIFFS_USE_MAGIC (0 or !=0 :default 1)
`!=0`の場合、`SPIFFS_format()`時にSPIFFSでフォーマット済みであることを示すマジッ
クナンバーを書き込むようになります。
これにより、SPIFFSはフォーマット済みであるかどうかを判別できるようになります。

ただし、偶然Flashにマジックナンバーと同じ値が書き込まれていたり、改ざんされてい
た場合は誤判定をすることになってしまうことは、一応認識しておきましょう。


### SPIFFS_USE_MAGIC_LENGTH (0 or !=0 :default 1)
マジックナンバーは、`struct spiffs_config`の一部の設定値を元に生成されます。

`SPIFFS_USE_MAGIC_LENGTH == 0`の場合log_page_sizeだけが生成式に使用されますが、
`SPIFFS_USE_MAGIC_LENGTH != 0`の場合は、phys_size等も使用されます。

つまりどういうことなのかというと、`SPIFFS_USE_MAGIC_LENGTH == 0`の場合は、4MByte
でフォーマットしたSPIFFSを2MByteとしてマウントする、といったことが可能なのだと予
想されます。

ちょっとどういう使い道があるのかわかりませんが、まあそれが便利なこともあるのでし
ょう。 フォーマットしたサイズより大きいサイズ(8MByte等)でもマウントできてしまう
という、危険な行為でもあります。

理由がなければデフォルトのままでよいでしょう。


### SPIFFS_FILEHDL_OFFSET (0 or !=0 :default 1)
`!=0`の場合、`struct spiffs_config`のfh_ix_offsetメンバが有効になります。
そして`SPIFFS_open()`で返される、spiffs_fileに、fh_ix_offsetの値が加算されるよ
うになります。

SPIFFSのファイルハンドルであるspiffs_fileはint16_tのtypedefなので、ただの整数値
です。

この結果、例えば複数のspiffsインスタンスを運用する際に、spiffs_fileに各インスタ
ンスに設定したfh_ix_offsetをマスクすることで、spiffs_fileの値だけで、そのファイ
ルハンドルがどのspiffsインスタンスに属しているかを判定できるようになります。

この説明を呼んでも特にピンとこない場合は、0にして無効にしてしまうか、
fh_ix_offsetは未初期化でも構いません。

------------------------------------------------------------

## struct spiffs_configの設定値解説

`spiffs.h`の定義に説明が書かれていますが、ちょっとわかりづらいので、一つずつ詳し
く解説します。

```c
// spiffs spi configuration struct
typedef struct {
    // この3つは特に説明不要でしょう。適切な関数をセットしてください。
    spiffs_read hal_read_f;
    spiffs_write hal_write_f;
    spiffs_erase hal_erase_f;

#if SPIFFS_SINGLETON == 0

    // physical size of the spi flash

    // SPIFFSとして使用するサイズ(バイト数)
    // 1MByteのFlashだからといって、1MByteを指定しなければならないわけではありま
    // せん。
    uint32_t phys_size;

    // physical offset in spi flash used for spiffs,
    // must be on block boundary

    // FlashのアドレスのどこからをSPIFFSとして使用するか。
    // HAL関数側で適当に調整すればいいので、仮にFlashの0x10000からをSPIFFSとして
    // 使用するとしても、0x10000とする必要は別にありません。まあ特に理由がなけれ
    // ば物理アドレスを指定しておけばよいでしょう。
    // phys_erase_blockの倍数である必要があることには注意。
    uint32_t phys_addr;

    // physical size when erasing a block

    // 消去単位となるバイト数を指定します。例えばFlashが4KByte単位でしか消去でき
    // ないなら1024 * 4を指定します。
    uint32_t phys_erase_block;

    // logical size of a block, must be on physical
    // block size boundary and must never be less than
    // a physical block
    //
    // logという名称からloggingとかの方のlogと誤解しそうですが、logical(論理的な)
    // の略です。phys_erase_block以上かつ、phys_erase_blockの倍数である必要があ
    // あります。SPIFFSはlog_block_sizeを1ブロックとして管理します。

    // log_block_size != phys_erase_blockとした場合にに何が嬉しいのか？というの
    // は推測になりますが、log_page_sizeの最小値がlog_block_size / 8という制限が
    // あることから、キャッシュするデータ量を増やすという意味があるのではないで
    // しょうか。
    // 特に必要性がなければphys_erase_blockと同じ値に設定しておけばよいでしょう
    uint32_t log_block_size;

    // logical size of a page, must be at least
    // log_block_size / 8

    // log_block_sizeで定義した論理ブロックは、log_page_sizeで分割されたページと
    // して管理されるようです。
    // SPIFFS_mount()時に、ワーク領域として、log_page_size * 2の領域が要求される
    // ので、メモリに余裕がない環境では極力小さな値になるように設定しておくとよ
    // いでしょう。
    // 最小値はlog_block_size / 8とのことです。また、特に記載はされてないですが
    // 、ブロックを無駄なく使用するために、log_block_sizeに余りがでない値を指定
    // しておいた方がよいと思います。
    uint32_t log_page_size;

#endif
#if SPIFFS_FILEHDL_OFFSET
    // an integer offset added to each file handle

    // define config値解説の説明をみてください。
    uint16_t fh_ix_offset;
#endif
} spiffs_config;
#endif
```

------------------------------------------------------------

## SPIFFSをMicroChip SST25VF016Bで動かす

[MicroChipの16Mbit SPI Serial Flash](http://ww1.microchip.com/downloads/en/DeviceDoc/25044A.pdf)でSPIFFSを動かしてみましょう。

2MByteのアドレス空間の、0x10000から256KByteをSPIFFSとして使用する、という条件で
考えてみます。

spiffs_configの設定値はこんな感じでしょう。
```c
  spiffs_config c;

  // HALは適当に用意してね
  c.hal_read_f = sst25v_read;
  c.hal_write_f = sst25v_write;
  c.hal_erase_f = sst25v_erase;

  // これは必要なければ値は適当でよい。
  c.fh_ix_offset = 0;

  c.phys_size = 1024 * 256; // 256KByte
  c.phys_addr = 0x10000;    // 0x10000 ~を使用する。
  c.phys_erase_block = 1024 * 4; // erase単位は最小4KByte(データシート参照)
  c.log_block_size = c.phys_erase_block; // 理由がなければ論理ブロックサイズ == 物理ブロックサイズでいい
  c.log_page_size = c.log_block_size / 8; // メモリを節約しておきたいので最小サイズにしておく

  // 同時に開けるファイルは最大２つとする
  const int MAX_NUM_FILES (2)

  // 3ページ分のキャッシュを持たせるとする
  const int NUM_CACHE_PAGES (3)

  // work領域はspiffs_config.log_page_size * 2との指定がspiffs.hに書かれている
  void* work = malloc(c.log_page_size * 2);

  // SPIFFS_buffer_bytes_for_filedescs()という関数で同様の計算が行われている。
  // それをそのまま使ってもいいが、関数なので静的メモリ確保に使えないのが困る。
  // その時は下記のようにサイズを計算すればよい。
  // ただし、SPIFFS_mount()の中で、fd_space領域のアライメントの切り上げを行って
  // いるので、char型等で静的なメモリ確保を行う場合アライメントに注意すること。
  // 静的確保にはpicoxのX_DEF_MAX_ALIGNEDを使用することを推奨する。
  //
  // また、spiffs_fdはspiffs.hではなく、spiffs_nucleus.hで定義されていることに注意。
  // spiffs.hだけをincludeしてもエラーになる。
  uint32_t fd_space_size = (MAX_NUM_FILES * sizeof(spiffs_fd));
  void* fd_space = malloc(fd_space_size);

  // これもSPIFFS_buffer_bytes_for_cache()という関数が用意されている。fd_spaceの
  // 時と注意点は同じだ。
  uint32_t cache_size = sizeof(spiffs_cache) + NUM_CACHE_PAGES * (sizeof(spiffs_cache_page) + c.log_page_size)
  void* cache = malloc(cache_size);

  int32_t ret;
  spiffs fs;
  ret = SPIFFS_mount(&fs, &c, work, fd_space, fd_space_size, cache, cache_size, NULL);
  if (!ret)
  {
      // mountのエラーは未フォーマットだからだと推測し、フォーマットする。
      ret = SPIFFS_format(&fs);
      assert(ret == 0);

      // フォーマットしたので、もう一度マウントを試みる。
      ret = SPIFFS_mount(&fs, &c, work, fd_space, fd_space_size, cache, cache_size, NULL);
      assert(ret == 0);
  }

  // これで使用準備完了。後はお好きなように。
```
