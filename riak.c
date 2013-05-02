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
  
  zend_declare_property_stringl(riak_ce_riak, "host", strlen("host"), DEFAULT_HOST, strlen(DEFAULT_HOST), ZEND_ACC_PRIVATE TSRMLS_CC);
  zend_declare_property_long(riak_ce_riak, "port", strlen("port"), DEFAULT_PORT, ZEND_ACC_PRIVATE TSRMLS_CC);
}

PHP_METHOD(Riak, __construct)
{
	char* host;
	int host_len;
	long port = DEFAULT_PORT;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &host, &host_len, &port) == FAILURE) {
		return;
	}
	zend_update_property_stringl(riak_class_methods, getThis(), "host", strlen("host"), host, host_len TSRMLS_CC);
	zend_update_property_long(riak_class_methods, getThis(), "port", strlen("port"), port TSRMLS_CC);
}
