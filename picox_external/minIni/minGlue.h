/*  Glue functions for the minIni library, based on the C/C++ stdio library
 *
 *  Or better said: this file contains macros that maps the function interface
 *  used by minIni to the standard C/C++ file I/O functions.
 *
 *  By CompuPhase, 2008-2014
 *  This "glue file" is in the public domain. It is distributed without
 *  warranties or conditions of any kind, either express or implied.
 */

#include <picox/filesystem/xfs.h>


#define INI_FILETYPE                    XFile*
#define INI_FILEPOS                     XSize
#define INI_REAL                        float
#define INI_BUFFERSIZE                  X_LINE_MAX

XFile* x_iniglue_openread(const char* path);
XFile* x_iniglue_openwrite(const char* path);
XFile* x_iniglue_openrewrite(const char* path);
bool x_iniglue_close(XFile* fp);
char* x_iniglue_read(char* dst, size_t size, XFile* fp);
bool x_iniglue_write(const char* src, XFile* fp);
bool x_iniglue_rename(const char* old_path, const char* new_path);
bool x_iniglue_remove(const char* path);
XSize x_iniglue_tell(XFile* fp);
bool x_iniglue_seek(XFile* fp, XSize pos);

#define ini_openread(filename,file)     ((*(file) = x_iniglue_openread(filename)) != NULL)
#define ini_openwrite(filename,file)    ((*(file) = x_iniglue_openwrite(filename)) != NULL)
#define ini_openrewrite(filename,file)  ((*(file) = x_iniglue_openrewrite(filename)) != NULL)
#define ini_close(file)                 x_iniglue_close(*(file))
#define ini_read(buffer,size,file)      (x_iniglue_read(buffer, size, *(file)) != NULL)
#define ini_write(buffer,file)          x_iniglue_write(buffer, *(file))
#define ini_rename(source,dest)         x_iniglue_rename(source, dest)
#define ini_remove(filename)            x_iniglue_remove(filename)
#define ini_tell(file,pos)              (*(pos) = x_iniglue_tell(*(file)))
#define ini_seek(file,pos)              x_iniglue_seek(*(file), *(pos))
#define ini_ftoa(string,value)          x_sprintf((string),"%f",(value))
#define ini_atof(string)                (INI_REAL)strtod((string),NULL)
