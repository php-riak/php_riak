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
#include "bucket.h"

zend_class_entry *riak_bucket_ce;

static zend_function_entry riak_bucket_methods[] = {
  PHP_ME(RiakBucket, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
  {NULL, NULL, NULL}
};

void riak_bucket_init(TSRMLS_D)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "RiakBucket", riak_bucket_methods);
  riak_bucket_ce = zend_register_internal_class(&ce TSRMLS_CC);
  
  zend_declare_property_null(riak_bucket_ce, "name", sizeof("name")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
  zend_declare_property_null(riak_bucket_ce, "client", sizeof("client")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
  
  zend_declare_property_null(riak_bucket_ce, "w", sizeof("w")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
  zend_declare_property_null(riak_bucket_ce, "dw", sizeof("dw")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
  zend_declare_property_null(riak_bucket_ce, "r", sizeof("r")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
}

/////////////////////////////////////////////////////////////

PHP_METHOD(RiakBucket, __construct)
{
  char *name;
  int nameLen;
  zval* client;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "os", &client, &name, &nameLen) == FAILURE) {
    return;
  }
  zend_update_property_stringl(riak_bucket_ce, getThis(), "name", sizeof("name")-1, name, nameLen TSRMLS_CC);
  zend_update_property(riak_bucket_ce, getThis(), "client", sizeof("client")-1, client TSRMLS_CC);
}
