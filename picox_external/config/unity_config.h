#ifndef unity_config_h_
#define unity_config_h_


#include <picox/core/xcore.h>


#ifndef UNITY_OUTPUT_CHAR
    #define UNITY_OUTPUT_CHAR(c)    x_putc(c)
#endif

#ifndef UNITY_FIXTURE_MALLOC
    #define UNITY_FIXTURE_MALLOC x_malloc
#endif

#ifndef UNITY_FIXTURE_FREE
    #define UNITY_FIXTURE_FREE x_free
#endif

/* #define UNITY_EXCLUDE_FLOAT */
/* #define UNITY_EXCLUDE_DOUBLE */


#endif /* unity_config_h_ */
