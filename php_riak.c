#include <php.h>
#include "php_riak.h"
#include "riak.h"

zend_function_entry riak_functions[] = {
  { NULL, NULL, NULL }
};

// "rlyeh_functions" refers to the struct defined above
// we'll be filling in more of this later: you can use this to specify
// globals, php.ini info, startup and teardown functions, etc.
zend_module_entry riak_module_entry = {
  STANDARD_MODULE_HEADER,
  PHP_RIAK_EXTNAME,
  riak_functions,
  PHP_MINIT(riak),
  NULL,
  NULL,
  NULL,
  NULL,
  PHP_RIAK_VERSION,
  STANDARD_MODULE_PROPERTIES
};

// install module
ZEND_GET_MODULE(riak)

PHP_MINIT_FUNCTION(riak) 
{
	riak_init_riak(TSRMLS_C);
}
