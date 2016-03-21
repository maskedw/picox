#include <picox/filesystem/xspiffs.h>
#include <spiffs.h>
#include <spiffs_nucleus.h>
#include "testutils.h"


TEST_GROUP(xspiffs);


#define AREA(x) area[(x) - addr_offset]
static unsigned char area[PHYS_FLASH_SIZE];
static u32_t addr_offset = 0;
static spiffs* spiffsbuf;
static XSpiFFs* fs;
static u8_t _work[LOG_PAGE*2];
static u8_t _fds[FD_BUF_SIZE];
static u8_t _cache[CACHE_BUF_SIZE];
static int check_valid_flash = 1;
static const char WRITE_DATA[] = "Hello world";
static const size_t WRITE_LEN = 11; // strlen WRITE_DATA


static s32_t _read(spiffs *fs, u32_t addr, u32_t size, u8_t *dst) {
  if (addr < fs->cfg.phys_addr) {
    printf("FATAL read addr too low %08x < %08x\n", addr, SPIFFS_PHYS_ADDR);
    exit(1);
  }
  if (addr + size > fs->cfg.phys_addr + fs->cfg.phys_size) {
    printf("FATAL read addr too high %08x + %08x > %08x\n", addr, size, SPIFFS_PHYS_ADDR + SPIFFS_FLASH_SIZE);
    exit(1);
  }
  memcpy(dst, &AREA(addr), size);
  return 0;
}


static s32_t _write(spiffs *fs, u32_t addr, u32_t size, u8_t *src) {
  int i;
  //printf("wr %08x %i\n", addr, size);
  if (addr < fs->cfg.phys_addr) {
    printf("FATAL write addr too low %08x < %08x\n", addr, SPIFFS_PHYS_ADDR);
    exit(1);
  }
  if (addr + size > fs->cfg.phys_addr + fs->cfg.phys_size) {
    printf("FATAL write addr too high %08x + %08x > %08x\n", addr, size, SPIFFS_PHYS_ADDR + SPIFFS_FLASH_SIZE);
    exit(1);
  }

  for (i = 0; i < size; i++) {
    if (((addr + i) & (fs->cfg.log_page_size-1)) != offsetof(spiffs_page_header, flags)) {
      if (check_valid_flash && ((AREA(addr + i) ^ src[i]) & src[i])) {
        printf("trying to write %02x to %02x at addr %08x\n", src[i], AREA(addr + i), addr+i);
        spiffs_page_ix pix = (addr + i) / LOG_PAGE;
        dump_page(spiffsbuf, pix);
        return -1;
      }
    }
    AREA(addr + i) &= src[i];
  }
  return 0;
}


static s32_t _erase(spiffs *fs, u32_t addr, u32_t size) {
  if (addr & (fs->cfg.phys_erase_block-1)) {
    printf("trying to erase at addr %08x, out of boundary\n", addr);
    exit(1);
  }
  if (size & (fs->cfg.phys_erase_block-1)) {
    printf("trying to erase at with size %08x, out of boundary\n", size);
    exit(1);
  }
  memset(&AREA(addr), 0xff, size);
  return 0;
}

void hexdump_mem(u8_t *b, u32_t len) {
  while (len--) {
    if ((((intptr_t)b)&0x1f) == 0) {
      printf("\n");
    }
    printf("%02x", *b++);
  }
  printf("\n");
}

void hexdump(u32_t addr, u32_t len) {
  int remainder = (addr % 32) == 0 ? 0 : 32 - (addr % 32);
  u32_t a;
  for (a = addr - remainder; a < addr+len; a++) {
    if ((a & 0x1f) == 0) {
      if (a != addr) {
        printf("  ");
        int j;
        for (j = 0; j < 32; j++) {
          if (a-32+j < addr)
            printf(" ");
          else {
            printf("%c", (AREA(a-32+j) < 32 || AREA(a-32+j) >= 0x7f) ? '.' : AREA(a-32+j));
          }
        }
      }
      printf("%s    %08x: ", a<=addr ? "":"\n", a);
    }
    if (a < addr) {
      printf("  ");
    } else {
      printf("%02x", AREA(a));
    }
  }
  int j;
  printf("  ");
  for (j = 0; j < 32; j++) {
    if (a-32+j < addr)
      printf(" ");
    else {
      printf("%c", (AREA(a-32+j) < 32 || AREA(a-32+j) >= 0x7f) ? '.' : AREA(a-32+j));
    }
  }
  printf("\n");
}

void dump_page(spiffs *fs, spiffs_page_ix p) {
  printf("page %04x  ", p);
  u32_t addr = SPIFFS_PAGE_TO_PADDR(fs, p);
  if (p % SPIFFS_PAGES_PER_BLOCK(fs) < SPIFFS_OBJ_LOOKUP_PAGES(fs)) {
    // obj lu page
    printf("OBJ_LU");
  } else {
    u32_t obj_id_addr = SPIFFS_BLOCK_TO_PADDR(fs, SPIFFS_BLOCK_FOR_PAGE(fs , p)) +
        SPIFFS_OBJ_LOOKUP_ENTRY_FOR_PAGE(fs, p) * sizeof(spiffs_obj_id);
    spiffs_obj_id obj_id = *((spiffs_obj_id *)&AREA(obj_id_addr));
    // data page
    spiffs_page_header *ph = (spiffs_page_header *)&AREA(addr);
    printf("DATA %04x:%04x  ", obj_id, ph->span_ix);
    printf("%s", ((ph->flags & SPIFFS_PH_FLAG_FINAL) == 0) ? "FIN " : "fin ");
    printf("%s", ((ph->flags & SPIFFS_PH_FLAG_DELET) == 0) ? "DEL " : "del ");
    printf("%s", ((ph->flags & SPIFFS_PH_FLAG_INDEX) == 0) ? "IDX " : "idx ");
    printf("%s", ((ph->flags & SPIFFS_PH_FLAG_USED) == 0) ? "USD " : "usd ");
    printf("%s  ", ((ph->flags & SPIFFS_PH_FLAG_IXDELE) == 0) ? "IDL " : "idl ");
    if (obj_id & SPIFFS_OBJ_ID_IX_FLAG) {
      // object index
      printf("OBJ_IX");
      if (ph->span_ix == 0) {
        printf("_HDR  ");
        spiffs_page_object_ix_header *oix_hdr = (spiffs_page_object_ix_header *)&AREA(addr);
        printf("'%s'  %i bytes  type:%02x", oix_hdr->name, oix_hdr->size, oix_hdr->type);
      }
    } else {
      // data page
      printf("CONTENT");
    }
  }
  printf("\n");
  u32_t len = fs->cfg.log_page_size;
  hexdump(addr, len);
}

void area_write(u32_t addr, u8_t *buf, u32_t size) {
  int i;
  for (i = 0; i < size; i++) {
    AREA(addr + i) = *buf++;
  }
}

void area_read(u32_t addr, u8_t *buf, u32_t size) {
  int i;
  for (i = 0; i < size; i++) {
    *buf++ = AREA(addr + i);
  }
}


TEST_SETUP(xspiffs)
{
    spiffsbuf = x_malloc(sizeof(spiffs));
    fs = x_malloc(sizeof(XSpiFFs));

    TEST_ASSERT_NOT_NULL(spiffsbuf);
    TEST_ASSERT_NOT_NULL(fs);


    memset(spiffsbuf, 0, sizeof(*spiffsbuf));
    memset(area, 0xFF, sizeof(area));
    spiffs_config c;
    c.hal_erase_f = _erase;
    c.hal_read_f = _read;
    c.hal_write_f = _write;
    c.log_block_size = LOG_BLOCK;
    c.log_page_size = LOG_PAGE;
    c.phys_addr = SPIFFS_PHYS_ADDR;
    c.phys_erase_block = SECTOR_SIZE;
    c.phys_size = SPIFFS_FLASH_SIZE;
#if SPIFFS_FILEHDL_OFFSET
  c.fh_ix_offset = TEST_SPIFFS_FILEHDL_OFFSET;
#endif

    int res = SPIFFS_mount(spiffsbuf, &c, _work, _fds, sizeof(_fds), _cache, sizeof(_cache), NULL);
    // printf("ret => %d\n", res);
    // printf("ret => %d\n", SPIFFS_errno(spiffsbuf));

    /* errnoはresが!=0の時だけ有効っぽい。 */
    res = SPIFFS_format(spiffsbuf);
    // printf("ret => %d\n", res);
    // printf("ret => %d\n", SPIFFS_errno(spiffsbuf));

    res = SPIFFS_mount(spiffsbuf, &c, _work, _fds, sizeof(_fds), _cache, sizeof(_cache), NULL);
    // printf("ret => %d\n", res);
    // printf("ret => %d\n", SPIFFS_errno(spiffsbuf));

    xspiffs_init(fs, spiffsbuf);
}


TEST_TEAR_DOWN(xspiffs)
{
    xspiffs_deinit(fs);
    SPIFFS_unmount(spiffsbuf);
    x_free(spiffsbuf);
    x_free(fs);
}



TEST(xspiffs, open_write)
{
    XFile* fp;
    XError err;

    err = xspiffs_open(fs, "foo.txt", X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xspiffs_close(fp);
}


TEST(xspiffs, open_read)
{
    XFile* fp;
    XError err;
    const char name[] = "foo.txt";

    err = xspiffs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
    TEST_ASSERT_NULL(fp);

    err = xspiffs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xspiffs_close(fp);

    err = xspiffs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xspiffs_close(fp);
}


TEST(xspiffs, write)
{
    XFile* fp;
    XError err;
    size_t nwritten;
    const char name[] = "foo.txt";


    err = xspiffs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xspiffs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    xspiffs_close(fp);
}


TEST(xspiffs, read)
{
    XFile* fp;
    XError err;
    char buf[256];
    size_t nread;
    const char name[] = "foo.txt";

    err = xspiffs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xspiffs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xspiffs_close(fp);

    err = xspiffs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xspiffs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    xspiffs_close(fp);
}


TEST(xspiffs, stat)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    const char name[] = "foo.txt";

    err = xspiffs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xspiffs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xspiffs_close(fp);

    err = xspiffs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);
}


TEST(xspiffs, tell)
{
    XFile* fp;
    XError err;
    XSize pos;
    const char name[] = "foo.txt";

    err = xspiffs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xspiffs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, pos);

    err = xspiffs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xspiffs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, pos);

    xspiffs_close(fp);
}


TEST(xspiffs, seek)
{
    XFile* fp;
    XError err;
    char name[] = "foo.txt";
    XSize pos;

    err = xspiffs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xspiffs_seek(fp, 10, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(10, (xspiffs_tell(fp, &pos), pos));

    err = xspiffs_seek(fp, 10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(20, (xspiffs_tell(fp, &pos), pos));

    err = xspiffs_seek(fp, 10, X_SEEK_END);
    TEST_ASSERT_EQUAL(10, (xspiffs_tell(fp, &pos), pos));


    err = xspiffs_seek(fp, 9, X_SEEK_END);
    xspiffs_write(fp, "A", 1, NULL);
    TEST_ASSERT_EQUAL(10, (xspiffs_tell(fp, &pos), pos));

    err = xspiffs_seek(fp, 10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(20, (xspiffs_tell(fp, &pos), pos));

    err = xspiffs_seek(fp, -10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(10, (xspiffs_tell(fp, &pos), pos));

    err = xspiffs_seek(fp, -10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(0, (xspiffs_tell(fp, &pos), pos));

    xspiffs_close(fp);
}


TEST(xspiffs, flush)
{
    XFile* fp;
    XError err;
    char name[] = "foo.txt";

    err = xspiffs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xspiffs_write(fp, "A", 1, NULL);
    err = xspiffs_flush(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xspiffs_close(fp);
}


TEST(xspiffs, opendir)
{
    XDir* dir;
    XError err;
    char name[] = "/";

    err = xspiffs_opendir(fs, name, &dir);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(dir);
    xspiffs_closedir(dir);
}


TEST(xspiffs, readdir)
{
    XFile* fp;
    XDir* dir;
    XError err;
    XDirEnt entbuf;
    XDirEnt* ent;
    char name[] = "/";
    char name2[] = "bar.txt";

    xspiffs_open(fs, name2, X_OPEN_MODE_WRITE, &fp);
    xspiffs_close(fp);
    xspiffs_opendir(fs, name, &dir);

    while ((err = xspiffs_readdir(dir, &entbuf, &ent)), ent)
    {
        if (strcmp(".", ent->name) == 0)
            continue;

        if (strcmp("..", ent->name) == 0)
            continue;
        TEST_ASSERT_EQUAL_STRING(name2, ent->name);
    }
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xspiffs_closedir(dir);
}


TEST(xspiffs, remove)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    char name[] = "foo";

    err = xspiffs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    xspiffs_open(fs, name, X_OPEN_MODE_WRITE, &fp);

    xspiffs_close(fp);

    err = xspiffs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xspiffs_remove(fs, name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xspiffs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
}


TEST(xspiffs, rename)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    char name[] = "foo";
    char name2[] = "bar";

    xspiffs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    xspiffs_close(fp);

    err = xspiffs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xspiffs_stat(fs, name2, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xspiffs_rename(fs, name, name2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xspiffs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xspiffs_stat(fs, name2, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

}


TEST(xspiffs, open_write_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合は新規作成される */
    err = xspiffs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    err = xspiffs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xspiffs_close(fp);

    err = xspiffs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はサイズは0に切り詰められる */
    err = xspiffs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    xspiffs_close(fp);
    err = xspiffs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, statbuf.size);

    /* 読み込みも可能 */
    err = xspiffs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    err = xspiffs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xspiffs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xspiffs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);

    xspiffs_close(fp);
}


TEST(xspiffs, open_read_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN * 2];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合はエラーになる。 */
    err = xspiffs_open(fs, name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NULL(fp);

    err = xspiffs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xspiffs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xspiffs_close(fp);

    err = xspiffs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合、サイズ0にはならない */
    err = xspiffs_open(fs, name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    xspiffs_close(fp);
    err = xspiffs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* 読み書きできる */
    err = xspiffs_open(fs, name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xspiffs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);

    err = xspiffs_seek(fp, 0, X_SEEK_END);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xspiffs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xspiffs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xspiffs_read(fp, buf, WRITE_LEN * 2, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN * 2, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);
    TEST_ASSERT_TRUE(memcmp(buf + WRITE_LEN, WRITE_DATA, WRITE_LEN) == 0);

    xspiffs_close(fp);
}


TEST(xspiffs, open_append)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    XSize pos;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN * 2];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合は新規作成される */
    err = xspiffs_open(fs, name, X_OPEN_MODE_APPEND, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xspiffs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xspiffs_close(fp);

    err = xspiffs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はファイルポインタは末尾にセットされる */
    err = xspiffs_open(fs, name, X_OPEN_MODE_APPEND, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xspiffs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, pos);

    /* 読み込みは不可 */
    err = xspiffs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_NOT_EQUAL(X_ERR_NONE, err);

    /* 書き込みは可 */
    err = xspiffs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    xspiffs_close(fp);
}


TEST(xspiffs, open_append_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    XSize pos;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN * 2];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合は新規作成される */
    err = xspiffs_open(fs, name, X_OPEN_MODE_APPEND_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xspiffs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xspiffs_close(fp);

    err = xspiffs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はファイルポインタは先頭にセットされる */
    err = xspiffs_open(fs, name, X_OPEN_MODE_APPEND_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xspiffs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, pos);

    /* 読み書きできる */
    err = xspiffs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xspiffs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xspiffs_read(fp, buf, WRITE_LEN * 2, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN * 2, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);
    TEST_ASSERT_TRUE(memcmp(buf + WRITE_LEN, WRITE_DATA, WRITE_LEN) == 0);

    xspiffs_close(fp);
}



TEST_GROUP_RUNNER(xspiffs)
{
    RUN_TEST_CASE(xspiffs, open_write);
    RUN_TEST_CASE(xspiffs, open_read);
    RUN_TEST_CASE(xspiffs, write);
    RUN_TEST_CASE(xspiffs, read);
    RUN_TEST_CASE(xspiffs, stat);
    RUN_TEST_CASE(xspiffs, tell);
    RUN_TEST_CASE(xspiffs, seek);
    RUN_TEST_CASE(xspiffs, flush);
    RUN_TEST_CASE(xspiffs, opendir);
    RUN_TEST_CASE(xspiffs, readdir);
    RUN_TEST_CASE(xspiffs, remove);
    RUN_TEST_CASE(xspiffs, rename);
    RUN_TEST_CASE(xspiffs, open_write_plus);
    RUN_TEST_CASE(xspiffs, open_read_plus);
    RUN_TEST_CASE(xspiffs, open_append);
    RUN_TEST_CASE(xspiffs, open_append_plus);
}
