/* zycurl extension for PHP (c) 2018 Xie Jianping */

#ifndef PHP_ZYCURL_H
# define PHP_ZYCURL_H

extern zend_module_entry zycurl_module_entry;
# define phpext_zycurl_ptr &zycurl_module_entry

# define PHP_ZYCURL_VERSION "1.0.0"

# if defined(ZTS) && defined(COMPILE_DL_ZYCURL)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#include <curl/curl.h>

#include "zend_smart_str.h"

#define ZYCURL_PERSISTENT 1
#define ZYCURL_P ZYCURL_PERSISTENT

#define ZYCURL_INLINE zend_always_inline
#define ZYCURL_MODULE_NAME "zycurl"
#define ZYCURL_INNER_FUNC_N(name) _zycurl_if_##name
#define ZYCURL_IFN ZYCURL_INNER_FUNC_N
#define ZYCURL_INNER_FUNC_D(name, return_type) static return_type ZYCURL_IFN(name)
#define ZYCURL_IFD ZYCURL_INNER_FUNC_D

typedef struct {
    smart_str buf;
} php_zycurl_write;

typedef struct {
    php_zycurl_write *write;
} php_zycurl_handlers;

typedef struct {
    CURLcode    no;
    char        str[CURL_ERROR_SIZE];
} php_zycurl_error;

typedef struct {
    CURL                *curl;
    zend_bool           is_recycle;
    php_zycurl_error    err;
    php_zycurl_handlers *handlers;
} php_zycurl;

#endif	/* PHP_ZYCURL_H */
