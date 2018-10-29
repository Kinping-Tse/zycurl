/* zycurl extension for PHP (c) 2018 Xie Jianping */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "ext/standard/url.h"

#include "php_zycurl.h"

static HashTable php_zycurl_pool;

static int res_type_curl_easy = 0;

static const char* res_type_name_curl_easy = "zycurl";

ZYCURL_IFD(curl_init, php_zycurl*)(zend_string* url);
ZYCURL_IFD(curl_close, void)(php_zycurl* pc);
ZYCURL_IFD(curl_setopt, int)(php_zycurl *pc, zend_long opt_name, zval *opt_value);

ZYCURL_IFD(pr_error, void)(char *errmsg, ...);
ZYCURL_IFD(get_php_curl, php_zycurl*)(zval *res);

static ZEND_RSRC_DTOR_FUNC(ZYCURL_IFN(curl_dtor));

ZYCURL_IFD(new_zycurl, php_zycurl*)();
ZYCURL_IFD(free_zycurl, void)(php_zycurl* pc);
ZYCURL_IFD(curl_cb_write, size_t)(char *ptr, size_t size, size_t nmemb, void *userdata);
ZYCURL_IFD(curl_cb_write_none, size_t)(char *ptr, size_t size, size_t nmemb, void *userdata);

/* {{{ resource zycurl_init( [ string $url ] )
 */
PHP_FUNCTION(zycurl_init)
{
	zend_string *url = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(url)
	ZEND_PARSE_PARAMETERS_END();

	php_zycurl* pc = ZYCURL_IFN(curl_init)(url);
	if (!pc) {
		ZYCURL_IFN(pr_error)("Could not initialize a new ZYcURL handle");
		RETURN_FALSE;
	}

	RETURN_RES(zend_register_resource(pc, res_type_curl_easy));
}
/* }}} */

/* {{{ resource zycurl_reset( resource $res )
 */
PHP_FUNCTION(zycurl_reset)
{

}
/* }}} */

/* {{{ resource zycurl_setopt( resource $res, int $opt_name, mixed $opt_value )
 */
PHP_FUNCTION(zycurl_setopt)
{
	zval* res = NULL;
	zend_long opt_name = 0;
	zval *opt_value = NULL;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_RESOURCE(res)
		Z_PARAM_LONG(opt_name)
		Z_PARAM_ZVAL(opt_value)
	ZEND_PARSE_PARAMETERS_END();

	php_zycurl *pc;
	if ((pc = ZYCURL_IFN(get_php_curl)(res)) == NULL) {
		ZYCURL_IFN(pr_error("Invalid curl resource"));
		RETURN_FALSE;
	}

	if (opt_name <= 0) {
		ZYCURL_IFN(pr_error)("Invalid curl configuration option");
		RETURN_FALSE;
	}

	if (ZYCURL_IFN(curl_setopt)(pc, opt_name, opt_value) == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ resource zycurl_setopt_array( resource $res, array $options )
 */
PHP_FUNCTION(zycurl_setopt_array)
{
	zval *res = NULL;
	zval *options = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(res)
		Z_PARAM_ARRAY(options)
	ZEND_PARSE_PARAMETERS_END();

	php_zycurl *pc = NULL;

	if ((pc = ZYCURL_IFN(get_php_curl)(res)) == NULL) {
		RETURN_FALSE;
	}

	zend_long opt_name = 0;
	zend_string *opt_name_str = NULL;
	zval *opt_value = NULL;

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(options), opt_name, opt_name_str, opt_value) {
		if (opt_name_str) {
			ZYCURL_IFN(pr_error("Array keys must be CURLOPT constants or equivalent integer values"));
			RETURN_FALSE;
		}
		if (ZYCURL_IFN(curl_setopt)(pc, opt_name, opt_value) == FAILURE) {
			RETURN_FALSE;
		}
	} ZEND_HASH_FOREACH_END();

	RETURN_TRUE;
}
/* }}} */

/* {{{ bool zycurl_exec( resource $res )
 */
PHP_FUNCTION(zycurl_exec)
{
	zval* res = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	php_zycurl *pc;
	if ((pc = ZYCURL_IFN(get_php_curl)(res)) == NULL) {
		ZYCURL_IFN(pr_error)("Invalid curl resource");
		RETURN_FALSE;
	}

	smart_str_free(&pc->handlers->write->buf);
	bzero(pc->err.str, CURL_ERROR_SIZE);
	pc->err.no = CURLE_OK;

	CURLcode err_code = curl_easy_perform(pc->curl);
	pc->err.no = err_code;

	if (err_code != CURLE_OK) {
		RETURN_FALSE;
	}

	smart_str_0(&pc->handlers->write->buf);

	RETURN_STR_COPY(pc->handlers->write->buf.s);
}

/* {{{ void zycurl_close( resource $res )
 */
PHP_FUNCTION(zycurl_close)
{
	zval* res = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	php_zycurl *pc = NULL;
	if ((pc = ZYCURL_IFN(get_php_curl)(res)) == NULL) {
		RETURN_FALSE;
	}

	if (Z_REFCOUNT_P(res) <= 2) {
		zend_list_close(Z_RES_P(res));
	}

	ZYCURL_IFN(curl_close)(pc);
}
/* }}} */

/* {{{ int zycurl_errno( resource $res )
 */
PHP_FUNCTION(zycurl_errno)
{
	zval* res = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	php_zycurl *pc = NULL;
	if ((pc = ZYCURL_IFN(get_php_curl)(res)) == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(pc->err.no);
}
/* }}} */

/* {{{ string zycurl_error( resource $res )
 */
PHP_FUNCTION(zycurl_error)
{
	zval* res = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(res)
	ZEND_PARSE_PARAMETERS_END();

	php_zycurl *pc = NULL;
	if ((pc = ZYCURL_IFN(get_php_curl)(res)) == NULL) {
		RETURN_FALSE;
	}

	pc->err.str[CURL_ERROR_SIZE-1] = '\0';
	RETURN_STRING(pc->err.str);
}
/* }}} */

/* {{{ string zycurl_errno( int $errno )
 */
PHP_FUNCTION(zycurl_strerror)
{
	zend_long err_code;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(err_code)
	ZEND_PARSE_PARAMETERS_END();

	const char *err_str = curl_easy_strerror(err_code);
	if (err_str) {
		RETURN_STRING(err_str);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

static ZEND_RSRC_DTOR_FUNC(ZYCURL_IFN(curl_dtor))
{
	zval zres;
	ZVAL_RES(&zres, res);

	php_zycurl *pc = ZYCURL_IFN(get_php_curl)(&zres);

	ZYCURL_IFN(curl_close)(pc);
}

ZYCURL_IFD(curl_init, php_zycurl*)(zend_string* url)
{
	/* Build persistent pool key by host and port */
	php_url *url_info = NULL;
	zend_string* php_zycurl_pool_key = NULL;

	if (!url) {
		char *tmp_url = "http://xxx.xjp.xxx.com";
		url = zend_string_init(tmp_url, strlen(tmp_url), 0);
	} else {
		url = zend_string_copy(url);
	}
	url_info = php_url_parse_ex(ZSTR_VAL(url), ZSTR_LEN(url));
	php_zycurl_pool_key = strpprintf(0, "%s_%d", url_info->host, url_info->port);

	/* Get php_zycurl from pool or create new one */
	zval *zcurl = NULL;
	php_zycurl *pc = NULL;
	CURL *curl = NULL;

	if ((zcurl = zend_hash_find(&php_zycurl_pool, php_zycurl_pool_key)) != NULL) {
		pc = Z_PTR_P(zcurl);
		curl = pc->curl;
	} else {
		curl = curl_easy_init();
		if (!curl) {
			return NULL;
		}

		pc = ZYCURL_IFN(new_zycurl)();
		pc->curl = curl;
		pc->is_recycle = zend_hash_num_elements(&php_zycurl_pool) < INI_INT("zycurl.max_persistent_count") ? 1 : 0;

		if (pc->is_recycle) {
			zval zptr;
			zcurl = &zptr;
			ZVAL_PTR(zcurl, pc);

			zend_hash_str_add(&php_zycurl_pool, ZSTR_VAL(php_zycurl_pool_key), ZSTR_LEN(php_zycurl_pool_key), zcurl);
		}
	}

	curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, pc->err.str);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ZYCURL_IFN(curl_cb_write));
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, pc);

	php_url_free(url_info);
	zend_string_release(url);
	zend_string_release(php_zycurl_pool_key);

	return pc;
}

ZYCURL_IFD(curl_close, void)(php_zycurl* pc)
{
	CURL *curl = pc->curl;

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, ZYCURL_IFN(curl_cb_write_none));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ZYCURL_IFN(curl_cb_write_none));
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);

	if (!pc->is_recycle) {
		curl_easy_cleanup(curl);

		ZYCURL_IFN(free_zycurl)(pc);
	}
}

ZYCURL_IFD(pr_error, void)(char *errmsg, ...)
{
	int level = E_WARNING;

	va_list vl;
	va_start(vl, errmsg);
	int tmp_level = va_arg(vl, int);
	if (tmp_level >= 0) {
		level = tmp_level;
	}
	va_end(vl);

	php_error_docref(NULL, level, errmsg);
}

ZYCURL_IFD(curl_setopt, int)(php_zycurl *pc, zend_long opt_name, zval *opt_value)
{
	CURL* curl = pc->curl;
	CURLcode err_code = CURLE_OK;

	switch (opt_name) {
		/* Long options */
		case CURLOPT_LOCALPORT:
		case CURLOPT_LOCALPORTRANGE:
		case CURLOPT_TIMEOUT:
		case CURLOPT_TIMEOUT_MS:
		case CURLOPT_NOSIGNAL:
		{
			zend_long opt_value_long = zval_get_long(opt_value);
			err_code = curl_easy_setopt(curl, opt_name, opt_value_long);
			break;
		}
		/* String options */
		case CURLOPT_URL:
		case CURLOPT_PROXY:
		{
			zend_string *opt_value_str = zval_get_string(opt_value);
			err_code = curl_easy_setopt(curl, opt_name, ZSTR_VAL(opt_value_str));
			zend_string_release(opt_value_str);
			break;
		}
		/* Array optios */
		case CURLOPT_HTTPHEADER:
		{
			break;
		}
		default:
			ZYCURL_IFN(pr_error)("Unsupported curl configuration option");
			return FAILURE;
	}
	pc->err.no = err_code;

	if (err_code != CURLE_OK) {
		return FAILURE;
	}

	return SUCCESS;
}

ZYCURL_IFD(get_php_curl, ZYCURL_INLINE php_zycurl*)(zval *res)
{
	return zend_fetch_resource(Z_RES_P(res), res_type_name_curl_easy, res_type_curl_easy);
}

ZYCURL_IFD(new_zycurl, php_zycurl*)()
{
	php_zycurl *pc = pecalloc(1, sizeof(php_zycurl), ZYCURL_P);
	pc->handlers = pecalloc(1, sizeof(php_zycurl_handlers), ZYCURL_P);
	pc->handlers->write = pecalloc(1, sizeof(php_zycurl_write), ZYCURL_P);

	return pc;
}

ZYCURL_IFD(free_zycurl, void)(php_zycurl* pc)
{
	smart_str_free(&pc->handlers->write->buf);
	pefree(pc->handlers->write, ZYCURL_P);

	pefree(pc->handlers, ZYCURL_P);
	pefree(pc, ZYCURL_P);
}

ZYCURL_IFD(curl_cb_write, size_t)(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t len = size * nmemb;
	php_zycurl *pc = userdata;
	php_zycurl_write *write = pc->handlers->write;

	if (len > 0) {
		smart_str_appendl_ex(&write->buf, ptr, len, ZYCURL_P);
	}

	return len;
}

ZYCURL_IFD(curl_cb_write_none, size_t)(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	return size * nmemb;
}

PHP_INI_BEGIN()
PHP_INI_ENTRY("zycurl.max_persistent_count", "100", PHP_INI_SYSTEM, NULL)
PHP_INI_END()

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(zycurl)
{
	if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
		return FAILURE;
	}

	REGISTER_INI_ENTRIES();

	res_type_curl_easy = zend_register_list_destructors_ex(
		NULL, ZYCURL_IFN(curl_dtor), res_type_name_curl_easy, module_number
	);

	zend_hash_init(&php_zycurl_pool, INI_INT("zycurl.max_persistent_count"), NULL, NULL, ZYCURL_P);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(zycurl)
{
	zend_hash_destroy(&php_zycurl_pool);

	curl_global_cleanup();

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(zycurl)
{
#if defined(ZTS) && defined(COMPILE_DL_ZYCURL)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(zycurl)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(zycurl)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "ZYcURL support", "enabled");
	php_info_print_table_row(2, "Version", PHP_ZYCURL_VERSION);
	php_info_print_table_row(2, "Author", "Xie Jianping <xiejianping@zhangyue.com>");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_zycurl_init, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_zycurl_setopt, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ zycurl_functions[]
 */
static const zend_function_entry zycurl_functions[] = {
	PHP_FE(zycurl_init, NULL)
	PHP_FE(zycurl_reset, NULL)
	PHP_FE(zycurl_setopt, NULL)
	PHP_FE(zycurl_setopt_array, NULL)
	PHP_FE(zycurl_exec, NULL)
	PHP_FE(zycurl_close, NULL)
	PHP_FE(zycurl_errno, NULL)
	PHP_FE(zycurl_error, NULL)
	PHP_FE(zycurl_strerror, NULL)
	PHP_FE_END
};
/* }}} */

/* {{{ zycurl_module_entry
 */
zend_module_entry zycurl_module_entry = {
	STANDARD_MODULE_HEADER,
	ZYCURL_MODULE_NAME,			/* Extension name */
	zycurl_functions,			/* zend_function_entry */
	PHP_MINIT(zycurl),			/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(zycurl),		/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(zycurl),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(zycurl),		/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(zycurl),			/* PHP_MINFO - Module info */
	PHP_ZYCURL_VERSION,			/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */


#ifdef COMPILE_DL_ZYCURL
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(zycurl)
#endif
