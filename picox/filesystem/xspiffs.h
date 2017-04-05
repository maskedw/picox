/**
 *       @file  xspiffs.h
 *      @brief  spiffsをバックエンドとするファイル操作定義
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/03/06
 * ===================================================================
 */


#ifndef picox_filesystem_xspiffs_h_
#define picox_filesystem_xspiffs_h_


#include <picox/filesystem/xfscore.h>
#include <spiffs.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @addtogroup filesystem
 *  @{
 *  @addtogroup  xspiffs
 *  @brief spiffsをバックエンドとするファイルシステムモジュールです
 *
 *  SPIフラッシュROM向けに実装された[spiffs](https://github.com/pellepl/spiffs)
 *  をバックエンドとし、picoxファイルシステムインターフェースを実装しています。
 *  spiffsはSPIフラッシュの書き換え回数制限を考慮して、ウェアレベリング機能が実
 *  装されていることが大きな特徴です。
 *
 *  spiffsを使用するために必要な下位ドライバ実装には関知していないので、新規ター
 *  ゲットへの移植時は、まずはspiffsを直接使用して、R/Wが正常に行えることを確認
 *  することからはじめてください。
 *
 *  @note
 *  spiffsではフラットファイル構造が採用されているため、ディレクトリを持てないこ
 *  とに注意してください。
 *
 *  @see xunionfs
 *  @see xvfs
 *  @{
 */


typedef struct
{
    const void* m_fstype_tag;
    spiffs*     m_spiffs;
} XSpiFFs;
X_DECLEAR_RTTI_TAG(XSPIFFS_RTTI_TAG);


/** @brief ファイルシステムを初期化します
 *
 *  @param src  初期化済みspiffsオブジェクト
 *  @pre
 *  + fs    != NULL
 *  + src   != NULL
 *
 *  spiffsの本体を利用可能にするためには、別途SPIFFS_mount()等の呼び出しが必要で
 *  す。
 */
void xspiffs_init(XSpiFFs* fs, spiffs* src);


/** @brief ファイルシステムの終了処理を行います
 *
 *  @pre
 *  + fs    != NULL
 */
void xspiffs_deinit(XSpiFFs* fs);


/** @brief 仮想ファイルシステムを初期化します
 *
 *  @pre
 *  + fs    != NULL
 *  + vfs   != NULL
 */
XVirtualFs* xspiffs_init_vfs(XSpiFFs* fs, XVirtualFs* vfs);
XStream* xspiffs_init_stream(XStream* stream, XFile* fp);
XError xspiffs_open(XSpiFFs* fs, const char* path, XOpenMode mode, XFile** o_fp);
XError xspiffs_close(XFile* fp);
XError xspiffs_read(XFile* fp, void* dst, size_t size, size_t* nread);
XError xspiffs_write(XFile* fp, const void* src, size_t size, size_t* nwritten);
XError xspiffs_seek(XFile* fp, XOffset pos, XSeekMode whence);
XError xspiffs_tell(XFile* fp, XSize* pos);
XError xspiffs_flush(XFile* fp);
XError xspiffs_opendir(XSpiFFs* fs, const char* path, XDir** o_dir);
XError xspiffs_readdir(XDir* dir, XDirEnt* dirent, XDirEnt** result);
XError xspiffs_closedir(XDir* dir);
XError xspiffs_getcwd(XSpiFFs* fs, char* buf, size_t size);
XError xspiffs_remove(XSpiFFs* fs, const char* path);
XError xspiffs_rename(XSpiFFs* fs, const char* oldpath, const char* newpath);
XError xspiffs_stat(XSpiFFs* fs, const char* path, XStat* statbuf);


/** @} end of addtogroup xspiffs
 *  @} end of addtogroup filesystem
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_filesystem_xspiffs_h_ */
