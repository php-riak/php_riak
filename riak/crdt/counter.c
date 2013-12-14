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

#define RIAK_COUNTER_GET_BUCKET_KEY_CONN zbucket = zend_read_property(riak_crdt_counter_ce, getThis(), "bucket", sizeof("bucket")-1, 1 TSRMLS_CC); \
                                         zkey = zend_read_property(riak_crdt_counter_ce, getThis(), "key", sizeof("key")-1, 1 TSRMLS_CC); \
                                         connection = get_riak_connection(zbucket TSRMLS_CC); \
                                         bucket = riack_name_from_bucket(zbucket TSRMLS_CC); \
                                         key.len = Z_STRLEN_P(zkey); \
                                         key.value = Z_STRVAL_P(zkey) \

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_crdt_counter_ctor, 0, ZEND_RETURN_VALUE, 2)
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
    zval *zbucket;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os", &zbucket, riak_bucket_ce, &key, &keylen) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zend_update_property(riak_crdt_counter_ce, getThis(), "bucket", sizeof("bucket")-1, zbucket TSRMLS_CC);
    zend_update_property_stringl(riak_crdt_counter_ce, getThis(), "key", sizeof("key")-1, key, keylen TSRMLS_CC);
}
/* }}} */


void riak_update_riack_counter_inc_props_from_input(struct RIACK_COUNTER_UPDATE_PROPERTIES *props, zval* zinput TSRMLS_DC) { /* {{{ */
#define SET_RIAK_CNT_UPDATE_PROP_LONG(GETTER_NAME, PROP_NAME) MAKE_STD_ZVAL(ztmp); \
                                                  RIAK_CALL_METHOD(Riak_Crdt_Input_UpdateInput, GETTER_NAME, ztmp, zinput); \
                                                  props->PROP_NAME##_use = 1; \
                                                  props->PROP_NAME = Z_LVAL_P(ztmp); \
                                                  zval_ptr_dtor(&ztmp)
    zval* ztmp;
    if (zinput && Z_TYPE_P(zinput) == IS_OBJECT) {
        SET_RIAK_CNT_UPDATE_PROP_LONG(getDW, dw);
        SET_RIAK_CNT_UPDATE_PROP_LONG(getPW, pw);
        SET_RIAK_CNT_UPDATE_PROP_LONG(getW, w);
    }
}/* }}} */


/* {{{ proto void Riak\Crdt\Counter->increment($amount [, Riak\Crdt\Input\UpdateInput $input])
Increment counter, use negative amount to decrement */
PHP_METHOD(Riak_Crdt_Counter, increment)
{
    struct RIACK_COUNTER_UPDATE_PROPERTIES props;
    zval *zinput, *zbucket, *zkey;
    riak_connection *connection;
    RIACK_STRING bucket, key;
    long amount;
    int riackresult;
    zinput = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|O", &amount, &zinput, riak_crdt_input_update_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    memset(&props, 0, sizeof(props));
    riak_update_riack_counter_inc_props_from_input(&props, zinput TSRMLS_CC);
    RIAK_COUNTER_GET_BUCKET_KEY_CONN;

    riackresult = riack_counter_increment(connection->client, bucket, key, (int64_t)amount, &props, 0);
    CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackresult);
}/* }}} */


/* {{{ proto int Riak\Crdt\Counter->increment($amount [, Riak\Crdt\Input\UpdateInput $input])
Increment counter, use negative amount to decrement, returns updated value */
PHP_METHOD(Riak_Crdt_Counter, incrementAndGet)
{
    struct RIACK_COUNTER_UPDATE_PROPERTIES props;
    zval *zinput, *zbucket, *zkey;
    riak_connection *connection;
    RIACK_STRING bucket, key;
    long amount;
    int64_t return_val;
    int riackresult;
    zinput = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|O", &amount, &zinput, riak_crdt_input_update_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    memset(&props, 0, sizeof(props));
    riak_update_riack_counter_inc_props_from_input(&props, zinput TSRMLS_CC);
    RIAK_COUNTER_GET_BUCKET_KEY_CONN;

    riackresult = riack_counter_increment(connection->client, bucket, key, (int64_t)amount, &props, &return_val);
    CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackresult);
    RETURN_LONG((long)return_val);
}/* }}} */


/* {{{ proto int Riak\Crdt\Counter->get([Riak\Crdt\Input\GetInput $input])
Get counter value from riak */
PHP_METHOD(Riak_Crdt_Counter, get)
{
#define SET_RIAK_CNT_GET_PROP_LONG(GETTER_NAME, PROP_NAME) MAKE_STD_ZVAL(ztmp); \
                                                  RIAK_CALL_METHOD(Riak_Crdt_Input_GetInput, GETTER_NAME, ztmp, zinput); \
                                                  props.PROP_NAME##_use = 1; \
                                                  props.PROP_NAME = Z_LVAL_P(ztmp); \
                                                  zval_ptr_dtor(&ztmp)
#define SET_RIAK_CNT_GET_PROP_BOOL(GETTER_NAME, PROP_NAME) MAKE_STD_ZVAL(ztmp); \
                                                  RIAK_CALL_METHOD(Riak_Crdt_Input_GetInput, GETTER_NAME, ztmp, zinput); \
                                                  props.PROP_NAME##_use = 1; \
                                                  props.PROP_NAME = Z_BVAL_P(ztmp); \
                                                  zval_ptr_dtor(&ztmp)
    struct RIACK_COUNTER_GET_PROPERTIES props;
    zval *zinput, *zbucket, *zkey;
    riak_connection *connection;
    RIACK_STRING bucket, key;
    int riackresult;
    int64_t result;
    zinput = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|O", &zinput, riak_crdt_input_get_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    memset(&props, 0, sizeof(props));
    RIAK_COUNTER_GET_BUCKET_KEY_CONN;
    if (zinput) {
        zval* ztmp;
        SET_RIAK_CNT_GET_PROP_LONG(getR, r);
        SET_RIAK_CNT_GET_PROP_LONG(getPR, pr);
        SET_RIAK_CNT_GET_PROP_BOOL(getBasicQuorum, basic_quorum);
        SET_RIAK_CNT_GET_PROP_BOOL(getNotFoundOk, notfound_ok);
    }
    riackresult = riack_counter_get(connection->client, bucket, key, &props, &result);
    CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackresult);
    RETURN_LONG((long)result);
}/* }}} */
