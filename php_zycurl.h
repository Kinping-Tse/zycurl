/* zycurl extension for PHP (c) 2018 Xie Jianping */

#ifndef PHP_ZYCURL_H
# define PHP_ZYCURL_H

extern zend_module_entry zycurl_module_entry;
# define phpext_zycurl_ptr &zycurl_module_entry

# define PHP_ZYCURL_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_ZYCURL)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_ZYCURL_H */
