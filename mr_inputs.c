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
#include "mr_inputs.h"
#include "php_riak.h"

zend_class_entry *riak_mrinput_ce;
zend_class_entry *riak_mrinput_bucket_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_mrinput_toarr, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mrinput_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

static zend_function_entry riak_mrinput_methods[] = {
    ZEND_ABSTRACT_ME(RiakMrInput, getValue, arginfo_mrinput_toarr)
    {NULL, NULL, NULL}
};

static zend_function_entry riak_mrinputbucket_methods[] = {
    PHP_ME(RiakMrInputBucket, __construct, arginfo_mrinput_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakMrInputBucket, getValue, arginfo_mrinput_toarr, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_mrinputs_init(TSRMLS_D)
{
    zend_class_entry ce, bucket_ce;

    INIT_CLASS_ENTRY(ce, "RiakMrInput", riak_mrinput_methods);
    riak_mrinput_ce = zend_register_internal_interface(&ce TSRMLS_CC);

    INIT_CLASS_ENTRY(bucket_ce, "RiakMrInputBucket", riak_mrinputbucket_methods);
    riak_mrinput_bucket_ce = zend_register_internal_class_ex(&bucket_ce, riak_mrinput_ce, NULL TSRMLS_CC);

    zend_declare_property_null(riak_mrinput_bucket_ce, "name", sizeof("name")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}

/////////////////////////////////////////////////////////////

PHP_METHOD(RiakMrInputBucket, __construct)
{
    char *name;
    int namelen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &namelen) == FAILURE) {
        return;
    }
    zend_update_property_stringl(riak_mrinput_bucket_ce, getThis(), "name", sizeof("name")-1, name, namelen TSRMLS_CC);
}

PHP_METHOD(RiakMrInputBucket, getValue)
{
    zval* name = zend_read_property(riak_mrinput_bucket_ce, getThis(), "name", strlen("name")-1, 1 TSRMLS_CC);
    RETURN_ZVAL(name, 1, 0);
}
