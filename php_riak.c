/*
   Copyright 2012 Trifork A/S
   Author: Kaspar Pedersen

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <php.h>
#include <riack.h>
#include "php_riak.h"
#include "client.h"
#include "object.h"
#include "bucket.h"
#include "exceptions.h"

int le_riack_clients;

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
  PHP_MSHUTDOWN(riak),
  NULL,
  NULL,
  NULL,
  PHP_RIAK_VERSION,
  STANDARD_MODULE_PROPERTIES
};

// install module
ZEND_GET_MODULE(riak)

// Module constructor
PHP_MINIT_FUNCTION(riak) 
{
  riack_init();
  // TODO Store persistant connections here
  le_riack_clients = zend_register_list_destructors_ex(NULL, le_riack_clients_pefree, "Persistent clients", module_number);
  riak_client_init(TSRMLS_C);
  riak_object_init(TSRMLS_C);
  riak_bucket_init(TSRMLS_C);
  riak_exceptions_init(TSRMLS_C);
  return SUCCESS;
}

// Module destructor
PHP_MSHUTDOWN_FUNCTION(riak)
{
  riack_cleanup();
  return SUCCESS;
}

void le_riack_clients_pefree(zend_rsrc_list_entry *rsrc TSRMLS_DC) {
	/*
	void *client = rsrc->ptr;
    pefree(ptr, 1);
	*/
}
