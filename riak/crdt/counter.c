/*
   Copyright 2013 Trifork A/S
   Author: Kaspar Bach Pedersen

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
#include "counter.h"
#include "../connection.h"
#include "../bucket.h"
#include "../exception/exception.h"
#include "input/update_input.h"
#include "input/get_input.h"

zend_class_entry *riak_crdt_counter_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_crdt_counter_ctor, 0, ZEND_RETURN_VALUE, 3)
    ZEND_ARG_INFO(0, connection)
    ZEND_ARG_INFO(0, bucket)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_crdt_counter_inc, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, amount)
    ZEND_ARG_INFO(0, updateInput)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_crdt_counter_get, 0, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_INFO(0, getInput)
ZEND_END_ARG_INFO()

static zend_function_entry riak_crdt_counter_methods[] = {
    PHP_ME(Riak_Crdt_Counter, __construct, arginfo_riak_crdt_counter_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Riak_Crdt_Counter, increment, arginfo_riak_crdt_counter_inc, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Counter, incrementAndGet, arginfo_riak_crdt_counter_inc, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Crdt_Counter, get, arginfo_riak_crdt_counter_get, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_crdt_counter_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Crdt", "Counter", riak_crdt_counter_methods);
    riak_crdt_counter_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(riak_crdt_counter_ce, "connection", sizeof("connection")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_crdt_counter_ce, "bucket", sizeof("bucket")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_crdt_counter_ce, "key", sizeof("key")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */

/* {{{ proto void Riak\Crdt\Counter->__construct(Riak\Connection $connection, Riak\Bucket $bucket, string $key)
Create a Riak\Crdt\Counter */
PHP_METHOD(Riak_Crdt_Counter, __construct)
{
    char *key;
    int keylen;
    zval *zconnection, *zbucket;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os", &zconnection, riak_connection_ce, &zbucket, riak_bucket_ce, &key, &keylen) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zend_update_property(riak_crdt_counter_ce, getThis(), "connection", sizeof("connection")-1, zconnection TSRMLS_CC);
    zend_update_property(riak_crdt_counter_ce, getThis(), "bucket", sizeof("bucket")-1, zbucket TSRMLS_CC);
    zend_update_property_stringl(riak_crdt_counter_ce, getThis(), "key", sizeof("key")-1, key, keylen TSRMLS_CC);
}
/* }}} */

PHP_METHOD(Riak_Crdt_Counter, increment)
{
    long amount;
    zval *zinput;
    zinput = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|O", &amount, &zinput, riak_crdt_input_update_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    if (zinput) {
        // TODO
    }
}

PHP_METHOD(Riak_Crdt_Counter, incrementAndGet)
{

}

PHP_METHOD(Riak_Crdt_Counter, get)
{
    zval *zinput;
    zinput = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|O", &zinput, riak_crdt_input_get_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
}
