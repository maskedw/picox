#include <picox/filesystem/xfile.h>
#include "testutils.h"




TEST_GROUP(xfile);


static const char* path = "tmp/foo.txt";


TEST_SETUP(xfile)
{
    TEST_ASSERT_TRUE(xfile_remove_all("tmp", NULL));
    TEST_ASSERT_TRUE(xfile_mkdir("tmp", NULL));
}


TEST_TEAR_DOWN(xfile)
{
    TEST_ASSERT_TRUE(xfile_remove_all("tmp", NULL));
}


TEST(xfile, open)
{
    XFile* fp;
    int err;

    X_TEST_ASSERTION_FAILED((fp = xfile_open(NULL, "w", &err)));
    X_TEST_ASSERTION_FAILED((fp = xfile_open(path, NULL, &err)));
    X_TEST_ASSERTION_SUCCESS((fp = xfile_open(path, "w", NULL)));
    xfile_close(fp, NULL);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w", &err)));
    xfile_close(fp, &err);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w+", &err)));
    xfile_close(fp, &err);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "wb", &err)));
    xfile_close(fp, &err);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w+b", &err)));
    xfile_close(fp, &err);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "a", &err)));
    xfile_close(fp, &err);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "a+", &err)));
    xfile_close(fp, &err);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "ab", &err)));
    xfile_close(fp, &err);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "a+b", &err)));
    xfile_close(fp, &err);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "r", &err)));
    xfile_close(fp, &err);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "r+", &err)));
    xfile_close(fp, &err);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "rb", &err)));
    xfile_close(fp, &err);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "r+b", &err)));
    xfile_close(fp, &err);
}


TEST(xfile, write)
{
    XFile* fp;
    int err;

    char src[] = "Hello world";
    char dst[20];
    const int len = strlen(src);
    size_t nwritten;
    XFileSize pos;
    XFileSize size;

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w", &err)));

    /* wモード後は常にサイズ0 */
    TEST_ASSERT_TRUE(xfile_size(fp, &size, &err));
    TEST_ASSERT_EQUAL(0, size);

    TEST_ASSERT_TRUE(xfile_write(fp, src, len, &nwritten, &err));
    TEST_ASSERT_EQUAL(len, nwritten);
    TEST_ASSERT_TRUE(xfile_tell(fp, &pos, &err));
    TEST_ASSERT_EQUAL(len, pos);
    TEST_ASSERT_TRUE(xfile_write(fp, src, len, &nwritten, &err));
    TEST_ASSERT_TRUE(xfile_tell(fp, &pos, &err));
    TEST_ASSERT_EQUAL(len * 2, pos);

    /* wは読み込み不可 */
    TEST_ASSERT_TRUE(xfile_seek(fp, 0, &err));
    TEST_ASSERT_TRUE(xfile_tell(fp, &pos, &err));
    TEST_ASSERT_EQUAL(0, pos);
    TEST_ASSERT_FALSE(xfile_read(fp, dst, len, NULL, &err));
    xfile_close(fp, NULL);
}


TEST(xfile, write_plus)
{
    XFile* fp;
    int err;

    char src[] = "Hello world";
    char dst[20];
    const int len = strlen(src);
    size_t nwritten;
    XFileSize pos;
    XFileSize size;

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w", &err)));
    TEST_ASSERT_TRUE(xfile_write(fp, src, len, &nwritten, &err));
    xfile_close(fp, NULL);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w+", &err)));
    TEST_ASSERT_TRUE(xfile_size(fp, &size, &err));

    /* w+でもサイズは0 */
    TEST_ASSERT_EQUAL(0, size);
    TEST_ASSERT_TRUE(xfile_tell(fp, &pos, &err));
    TEST_ASSERT_EQUAL(0, pos);
    TEST_ASSERT_TRUE(xfile_write(fp, src, len, &nwritten, &err));
    TEST_ASSERT_EQUAL(len, nwritten);
    TEST_ASSERT_TRUE(xfile_tell(fp, &pos, &err));
    TEST_ASSERT_EQUAL(len, pos);

    /* w+は読み書き可能 */
    size_t nread;
    TEST_ASSERT_TRUE(xfile_seek(fp, 0, &err));
    TEST_ASSERT_TRUE(xfile_tell(fp, &pos, &err));
    TEST_ASSERT_EQUAL(0, pos);
    TEST_ASSERT_TRUE(xfile_tell(fp, &pos, &err));
    TEST_ASSERT_TRUE(xfile_read(fp, dst, len, &nread, &err));
    TEST_ASSERT_EQUAL(len, nread);
    TEST_ASSERT_TRUE(memcmp(src, dst, len) == 0);
    xfile_close(fp, NULL);
}


TEST(xfile, append)
{
    XFile* fp;
    int err;

    char src[] = "Hello world";
    const int len = strlen(src);
    size_t nwritten;
    XFileSize size;

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w", &err)));
    TEST_ASSERT_TRUE(xfile_write(fp, src, len, &nwritten, &err));
    xfile_close(fp, NULL);

    /* aは追記 */
    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "a", &err)));
    TEST_ASSERT_TRUE(xfile_size(fp, &size, &err));
    TEST_ASSERT_EQUAL(len, size);
    TEST_ASSERT_TRUE(xfile_write(fp, src, len, &nwritten, &err));
    TEST_ASSERT_TRUE(xfile_size(fp, &size, &err));
    TEST_ASSERT_EQUAL(len * 2, size);

    /* 追記で開いてるんだからtellは末尾を指しているべきだと思うのだが、MinGW4.8
     * では0を返す。あとで1度でもwriteすると、末尾を指すようになる。よくわからん
     * 振る舞いだ。
     */
    // TEST_ASSERT_TRUE(xfile_tell(fp, &pos, &err));
    // TEST_ASSERT_EQUAL(len, pos);
    xfile_close(fp, NULL);
}


TEST(xfile, append_plus)
{
    XFile* fp;
    int err;

    char src[] = "Hello world";
    char dst[20];
    const int len = strlen(src);
    size_t nwritten;
    size_t nread;
    XFileSize size;

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w", &err)));
    TEST_ASSERT_TRUE(xfile_write(fp, src, len, &nwritten, &err));
    xfile_close(fp, NULL);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "a+", &err)));
    TEST_ASSERT_TRUE(xfile_size(fp, &size, &err));
    TEST_ASSERT_EQUAL(len, size);
    TEST_ASSERT_TRUE(xfile_write(fp, src, len, &nwritten, &err));
    TEST_ASSERT_TRUE(xfile_size(fp, &size, &err));
    TEST_ASSERT_EQUAL(len * 2, size);

    /* a+は読み込みもできるよ。 */
    TEST_ASSERT_TRUE(xfile_seek(fp, 0, &err));
    TEST_ASSERT_TRUE(xfile_read(fp, dst, len, &nread, &err));
    TEST_ASSERT_EQUAL(len, nread);
    TEST_ASSERT_TRUE(memcmp(src, dst, len) == 0);

    xfile_close(fp, NULL);
}


TEST(xfile, read)
{
    XFile* fp;
    int err;

    char src[] = "Hello world";
    char dst[20];
    const int len = strlen(src);
    size_t nwritten;
    size_t nread;
    XFileSize size;

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w", &err)));
    TEST_ASSERT_TRUE(xfile_write(fp, src, len, &nwritten, &err));
    xfile_close(fp, NULL);

    /* 存在しないパスを指定するとNULLを返す。*/
    TEST_ASSERT_NULL((fp = xfile_open("hello-foo-bar.txt", "r", &err)));
    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "r", &err)));
    TEST_ASSERT_TRUE(xfile_size(fp, &size, &err));
    TEST_ASSERT_EQUAL(len, size);

    TEST_ASSERT_TRUE(xfile_read(fp, dst, len, &nread, &err));
    TEST_ASSERT_EQUAL(len, nread);
    TEST_ASSERT_TRUE(memcmp(src, dst, len) == 0);

    /* 書き込みは失敗する。 */
    TEST_ASSERT_FALSE(xfile_write(fp, src, len, &nwritten, &err));

    xfile_close(fp, NULL);
}


TEST(xfile, read_plus)
{
    XFile* fp;
    int err;

    char src[] = "Hello world";
    char dst[20];
    const int len = strlen(src);
    size_t nwritten;
    size_t nread;
    XFileSize size;

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w", &err)));
    TEST_ASSERT_TRUE(xfile_write(fp, src, len, &nwritten, &err));
    xfile_close(fp, NULL);

    /* 存在しないパスを指定するとNULLを返す。*/
    TEST_ASSERT_NULL((fp = xfile_open("hello-foo-bar.txt", "r+", &err)));
    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "r+", &err)));
    TEST_ASSERT_TRUE(xfile_size(fp, &size, &err));
    TEST_ASSERT_EQUAL(len, size);

    TEST_ASSERT_TRUE(xfile_read(fp, dst, len, &nread, &err));
    TEST_ASSERT_EQUAL(len, nread);
    TEST_ASSERT_TRUE(memcmp(src, dst, len) == 0);

    /* r+は書き込みできる・・・のだが振る舞いが謎だ。MinGW4.8では一度seekしない
     * と書き込みに失敗する。まあ実際使う時はseekすると思うけども。*/
    TEST_ASSERT_TRUE(xfile_seek(fp, size, &err));
    TEST_ASSERT_TRUE(xfile_write(fp, src, len, &nwritten, &err));

    if (! xfile_write(fp, src, len, &nwritten, &err))
    {
        X_LOG_ERR(("TAG", "write err => %s", xfile_strerror(err)));
    }



    xfile_close(fp, NULL);
}


TEST(xfile, size)
{
    XFile* fp;
    int err;
    XFileSize size;

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w", &err)));
    X_TEST_ASSERTION_FAILED(xfile_size(NULL, &size, &err));
    X_TEST_ASSERTION_FAILED(xfile_size(fp, NULL, &err));
    TEST_ASSERT_TRUE(xfile_size(fp, &size, &err));
    TEST_ASSERT_EQUAL(0, size);
    xfile_close(fp, NULL);

    TEST_ASSERT_TRUE(xfile_size_with_path(path, &size, &err));
    TEST_ASSERT_EQUAL(0, size);
    X_TEST_ASSERTION_FAILED(xfile_size_with_path(NULL, &size, &err));
    X_TEST_ASSERTION_FAILED(xfile_size_with_path(path, NULL, &err));
}


TEST(xfile, exists)
{
    XFile* fp;
    int err;
    bool exists;

    X_TEST_ASSERTION_FAILED(xfile_exists(NULL, &exists, &err));
    X_TEST_ASSERTION_FAILED(xfile_exists(path, NULL, &err));

    TEST_ASSERT_TRUE(xfile_exists(path, &exists, &err));
    TEST_ASSERT_FALSE(exists);

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w", &err)));
    xfile_close(fp, NULL);

    TEST_ASSERT_TRUE(xfile_exists(path, &exists, &err));
    TEST_ASSERT_TRUE(exists);
}


TEST(xfile, seek)
{
    XFile* fp;
    int err;

    char src[] = "Hello world";
    char dst[20];
    const int len = strlen(src);
    size_t nwritten;

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w+", &err)));
    TEST_ASSERT_TRUE(xfile_write(fp, src, len, &nwritten, &err));
    TEST_ASSERT_TRUE(xfile_seek(fp, 2, &err));

    memset(dst, 0, sizeof(dst));
    TEST_ASSERT_TRUE(xfile_read(fp, dst, 2, NULL, &err));
    TEST_ASSERT_EQUAL_STRING("ll", dst);

    X_TEST_ASSERTION_FAILED(xfile_seek(NULL, 0, &err));

    xfile_close(fp, NULL);
}


TEST(xfile, tell)
{
    XFile* fp;
    int err;
    XFileSize pos;

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w+", &err)));
    X_TEST_ASSERTION_FAILED(xfile_tell(NULL, &pos, &err));
    X_TEST_ASSERTION_FAILED(xfile_tell(fp, NULL, &err));
    xfile_close(fp, NULL);
}


TEST(xfile, flush)
{
    XFile* fp;
    int err;

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w+", &err)));
    X_TEST_ASSERTION_FAILED(xfile_flush(NULL, &err));
    xfile_close(fp, NULL);
}


TEST(xfile, mkdir)
{
    XFile* fp;
    int err;

    TEST_ASSERT_NOT_NULL((fp = xfile_open(path, "w+", &err)));
    X_TEST_ASSERTION_FAILED(xfile_mkdir(NULL, &err));
    TEST_ASSERT_FALSE(xfile_mkdir(path, &err));
    TEST_ASSERT_FALSE(xfile_mkdir("tmp/test/test", &err));
    TEST_ASSERT_TRUE(xfile_mkdir("tmp/test", &err));
    xfile_close(fp, NULL);
}


TEST(xfile, chdir_cwd)
{
    XFile* fp;
    int err;
    char cwd[XFILE_PATH_MAX];

    X_TEST_ASSERTION_FAILED(xfile_chdir(NULL, &err));
    X_TEST_ASSERTION_FAILED(xfile_cwd(NULL, sizeof(cwd), &err));
    X_TEST_ASSERTION_FAILED(xfile_cwd(cwd, 0, &err));

    TEST_ASSERT_NOT_NULL((fp = xfile_open("tmp/test.txt", "w+", &err)));
    xfile_close(fp, NULL);

    TEST_ASSERT_TRUE(xfile_cwd(cwd, sizeof(cwd), &err));
    TEST_ASSERT_TRUE(xfile_chdir("tmp", &err));
    TEST_ASSERT_NOT_NULL((fp = xfile_open("test.txt", "r", &err)));
    xfile_close(fp, NULL);
    TEST_ASSERT_TRUE(xfile_chdir(cwd, &err));
}


TEST(xfile, read_line)
{
    XFile* fp;
    int err;
    const char* hello = "Hello\n";
    const char* world = "World\r\n";
    const char* hw = "Hello World";

    TEST_ASSERT_NOT_NULL((fp = xfile_open("tmp/test.txt", "w+", &err)));
    TEST_ASSERT_TRUE(xfile_write(fp, hello, strlen(hello), NULL, &err));
    TEST_ASSERT_TRUE(xfile_write(fp, world, strlen(world), NULL, &err));
    TEST_ASSERT_TRUE(xfile_write(fp, hw, strlen(hw), NULL, &err));
    TEST_ASSERT_TRUE(xfile_seek(fp, 0, &err));

    char dst[30];
    char* line;
    bool overflow;

    X_TEST_ASSERTION_FAILED(xfile_read_line(NULL, dst, sizeof(dst), &line, &overflow, &err));
    X_TEST_ASSERTION_FAILED(xfile_read_line(fp, NULL, sizeof(dst), &line, &overflow, &err));
    X_TEST_ASSERTION_FAILED(xfile_read_line(fp, dst, 0, &line, &overflow, &err));
    X_TEST_ASSERTION_FAILED(xfile_read_line(fp, dst, sizeof(dst), NULL, &overflow, &err));

    TEST_ASSERT_TRUE(xfile_read_line(fp, dst, sizeof(dst), &line, &overflow, &err));
    TEST_ASSERT_FALSE(overflow);
    TEST_ASSERT_EQUAL_STRING("Hello", line);
    TEST_ASSERT_TRUE(xfile_read_line(fp, dst, sizeof(dst), &line, &overflow, &err));
    TEST_ASSERT_FALSE(overflow);
    TEST_ASSERT_EQUAL_STRING("World", line);
    TEST_ASSERT_TRUE(xfile_read_line(fp, dst, sizeof(dst), &line, &overflow, &err));
    TEST_ASSERT_FALSE(overflow);
    TEST_ASSERT_EQUAL_STRING("Hello World", line);

    TEST_ASSERT_TRUE(xfile_read_line(fp, dst, sizeof(dst), &line, &overflow, &err));
    TEST_ASSERT_NULL(line);


    xfile_close(fp, NULL);
}


TEST(xfile, is_directory)
{
    XFile* fp;
    int err;

    TEST_ASSERT_NOT_NULL((fp = xfile_open("tmp/test.txt", "w+", &err)));
    xfile_close(fp, NULL);

    bool is_dir;
    X_TEST_ASSERTION_FAILED(xfile_is_directory(NULL, &is_dir, &err));
    X_TEST_ASSERTION_FAILED(xfile_is_directory("tmp", NULL, &err));

    TEST_ASSERT_TRUE(xfile_is_directory("tmp", &is_dir, &err));
    TEST_ASSERT_TRUE(is_dir);
    TEST_ASSERT_TRUE(xfile_is_directory("tmp/test.txt", &is_dir, &err));
    TEST_ASSERT_FALSE(is_dir);

    /* 存在しないパスを指定した場合は戻り値trueだが、is_dirはfalseとなる。*/
    TEST_ASSERT_TRUE(xfile_is_directory("tmp-tmp", &is_dir, &err));
    TEST_ASSERT_FALSE(is_dir);
}


static bool TestVPrintf(XFile* fp, size_t* nprint, int* err, const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    const bool ok = xfile_vprintf(fp, nprint, err, fmt, arg);
    va_end(arg);

    return ok;
}


TEST(xfile, print)
{
    XFile* fp;
    int err;

    TEST_ASSERT_NOT_NULL((fp = xfile_open("tmp/test.txt", "w+", &err)));

    const char* src = "Hello World!!";
    const size_t len = strlen(src);
    size_t nprint;

    TEST_ASSERT_TRUE(xfile_printf(fp, &nprint, &err, "%s\n", src));
    TEST_ASSERT_EQUAL(len + 1, nprint);
    TEST_ASSERT_TRUE(xfile_seek(fp, 0, &err));

    char dst[30];
    char* line;
    bool overflow;

    TEST_ASSERT_TRUE(xfile_read_line(fp, dst, sizeof(dst), &line, &overflow, &err));
    TEST_ASSERT_EQUAL_STRING(src, line);

    xfile_close(fp, NULL);

    TEST_ASSERT_NOT_NULL((fp = xfile_open("tmp/test.txt", "w+", &err)));
    TEST_ASSERT_TRUE(TestVPrintf(fp, &nprint, &err, "%s\n", src));
    TEST_ASSERT_EQUAL(len + 1, nprint);
    TEST_ASSERT_TRUE(xfile_seek(fp, 0, &err));
    TEST_ASSERT_TRUE(xfile_read_line(fp, dst, sizeof(dst), &line, &overflow, &err));
    TEST_ASSERT_EQUAL_STRING(src, line);
    xfile_close(fp, NULL);
}


TEST_GROUP_RUNNER(xfile)
{
    RUN_TEST_CASE(xfile, open);
    RUN_TEST_CASE(xfile, write);
    RUN_TEST_CASE(xfile, write_plus);
    RUN_TEST_CASE(xfile, append);
    RUN_TEST_CASE(xfile, append_plus);
    RUN_TEST_CASE(xfile, read);
    RUN_TEST_CASE(xfile, read_plus);
    RUN_TEST_CASE(xfile, size);
    RUN_TEST_CASE(xfile, exists);
    RUN_TEST_CASE(xfile, seek);
    RUN_TEST_CASE(xfile, tell);
    RUN_TEST_CASE(xfile, flush);
    RUN_TEST_CASE(xfile, mkdir);
    RUN_TEST_CASE(xfile, chdir_cwd);
    RUN_TEST_CASE(xfile, read_line);
    RUN_TEST_CASE(xfile, is_directory);
    RUN_TEST_CASE(xfile, print);
}
