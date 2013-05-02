#include <php.h>
#include "riak.h"

zend_class_entry *riak_ce_riak;

static zend_function_entry riak_class_methods[] = {
  PHP_ME(Riak, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
  {NULL, NULL, NULL}
};

void riak_init_riak(TSRMLS_D) 
{
  zend_class_entry ce;
 
  INIT_CLASS_ENTRY(ce, "Riak", riak_class_methods);
  riak_ce_riak = zend_register_internal_class(&ce TSRMLS_CC);
  
  zend_declare_property_stringl(riak_ce_riak, "host", sizeof("host")-1, DEFAULT_HOST, sizeof(DEFAULT_HOST)-1, ZEND_ACC_PUBLIC TSRMLS_CC);
  zend_declare_property_long(riak_ce_riak, "port", sizeof("port")-1, DEFAULT_PORT, ZEND_ACC_PUBLIC TSRMLS_CC);
}

PHP_METHOD(Riak, __construct)
{
	char* host;
	int host_len;
	long port = DEFAULT_PORT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &host, &host_len, &port) == FAILURE) {
		return;
	}
	zend_update_property_stringl(riak_class_methods, getThis(), "host", sizeof("host")-1, host, host_len TSRMLS_CC);
	zend_update_property_long(riak_class_methods, getThis(), "port", sizeof("port")-1, port TSRMLS_CC);
}


