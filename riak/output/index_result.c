/*
   Copyright 2013: Kaspar Bach Pedersen

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
#include "index_result.h"
#include "../exception/exception.h"

zend_class_entry *riak_index_result_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_output_index_result_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_output_index_result_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_output_index_result_methods[] = {
    PHP_ME(Riak_Output_IndexResult, __construct, arginfo_riak_output_index_result_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Riak_Output_IndexResult, getKey, arginfo_riak_output_index_result_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


void riak_output_index_result_init(TSRMLS_D)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "IndexResult", riak_output_index_result_methods);
    riak_index_result_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_index_result_ce, "key", sizeof("key")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}

/* {{{ proto void Riak\Output\IndexResult->__construct(string $key)
Get list of returned objects */
PHP_METHOD(Riak_Output_IndexResult, __construct)
{
    char *key;
    int key_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &key_len) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zend_update_property_stringl(riak_index_result_ce, getThis(), "key", sizeof("key")-1, key, key_len TSRMLS_CC);
}
/* }}} */


/* {{{ proto Riak\Output\IndexResult Riak\Output\IndexResult->getKey()
Get key */
PHP_METHOD(Riak_Output_IndexResult, getKey)
{
    RIAK_GETTER_OBJECT(riak_index_result_ce, "key")
}
/* }}} */
