#include "minGlue.h"


XFile* x_iniglue_openread(const char* path)
{
    XFile* fp;
    xfs_open(path, X_OPEN_MODE_READ, &fp);
    return fp;
}


XFile* x_iniglue_openwrite(const char* path)
{
    XFile* fp;
    xfs_open(path, X_OPEN_MODE_WRITE, &fp);
    return fp;
}


XFile* x_iniglue_openrewrite(const char* path)
{
    XFile* fp;
    xfs_open(path, X_OPEN_MODE_READ_PLUS, &fp);
    return fp;
}


bool x_iniglue_close(XFile* fp)
{
    const XError err = xfs_close(fp);
    return err == X_ERR_NONE;
}


char* x_iniglue_read(char* dst, size_t size, XFile* fp)
{
    bool overflow;
    char* result;
    const XError err = xfs_gets(fp, dst, size - 1, &result, &overflow);

    if ((err != X_ERR_NONE) || !result || overflow)
        return NULL;

    char* p = dst + strlen(dst);
    *p++ = '\n';
    *p   = '\0';

    return dst;
}


bool x_iniglue_write(const char* src, XFile* fp)
{
    const int ret = xfs_puts(fp, src);
    return ret >= 0;
}


bool x_iniglue_rename(const char* old_path, const char* new_path)
{
    const XError err = xfs_rename(old_path, new_path);
    return err == X_ERR_NONE;
}


bool x_iniglue_remove(const char* path)
{
    const XError err = xfs_remove(path);
    return err == X_ERR_NONE;
}


XSize x_iniglue_tell(XFile* fp)
{
    XSize pos;
    const XError err = xfs_tell(fp, &pos);
    return (err == X_ERR_NONE) ? pos : 0;
}


bool x_iniglue_seek(XFile* fp, XSize pos)
{
    const XError err = xfs_seek(fp, pos, X_SEEK_SET);
    return (err == X_ERR_NONE);
}
