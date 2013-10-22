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
#include "key_data_list_input.h"
#include "input.h"
#include "riak/bucket.h"
#include "riak/object.h"
#include "riak/exception/exception.h"

zend_class_entry *riak_mrinput_keydatalist_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_mr_input_keydatalist_input_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mr_input_keydatalist_add, 0, ZEND_RETURN_VALUE, 3)
    ZEND_ARG_INFO(0, bucket)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mr_input_keydatalist_toarr, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_mrinputkeydatalist_methods[] = {
    PHP_ME(Riak_MapReduce_Input_KeyDataListInput, __construct, arginfo_mr_input_keydatalist_input_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Riak_MapReduce_Input_KeyDataListInput, add, arginfo_mr_input_keydatalist_add, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_MapReduce_Input_KeyDataListInput, getValue, arginfo_mr_input_keydatalist_toarr, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_map_reduce_input_key_data_list_input_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\MapReduce\\Input", "KeyDataListInput", riak_mrinputkeydatalist_methods);
    riak_mrinput_keydatalist_ce = zend_register_internal_class_ex(&ce, riak_mrinput_ce, NULL TSRMLS_CC);
    zend_declare_property_null(riak_mrinput_keydatalist_ce, "inputList", sizeof("inputList")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */


/*************************************************************
* Implementation: Riak\MapReduce\Input\KeyDataListInput
*************************************************************/

/* {{{ proto void Riak\MapReduce\Input\KeyDataListInput->__construct()
Create a KeyDataListInput */
PHP_METHOD(Riak_MapReduce_Input_KeyDataListInput, __construct)
{
    zval *list;
    MAKE_STD_ZVAL(list);
    array_init(list);
    zend_update_property(riak_mrinput_keydatalist_ce, getThis(), "inputList", sizeof("inputList")-1, list TSRMLS_CC);
    zval_ptr_dtor(&list);
}
/* }}} */

/* {{{ proto void Riak\MapReduce\Input\KeyDataListInput->add(string|RiakBucket $bucket, string|RiakObject $object, $data)
Add a bucket/key/data set to input */
PHP_METHOD(Riak_MapReduce_Input_KeyDataListInput, add)
{
    zval *zbucket, *zkey, *zdata, *zarr, *zlist;
    char *bucket, *key;
    int bucketlen, keylen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &zbucket, &zkey, &zdata) == FAILURE) {
        return;
    }
    bucket = NULL;
    bucketlen = 0;
    if (Z_TYPE_P(zbucket) == IS_STRING) {
        bucket = Z_STRVAL_P(zbucket);
        bucketlen = Z_STRLEN_P(zbucket);
    } else if (Z_TYPE_P(zbucket) == IS_OBJECT) {
        riak_name_from_bucket(zbucket, &bucket, &bucketlen TSRMLS_CC);
    }
    if (Z_TYPE_P(zkey) == IS_STRING) {
        key = Z_STRVAL_P(zkey);
        keylen = Z_STRLEN_P(zkey);
    } else if (Z_TYPE_P(zkey) == IS_OBJECT) {
        riak_key_from_object(zkey, &key, &keylen TSRMLS_CC);
    }
    if (bucket != NULL && bucketlen > 0 && key != NULL && keylen > 0) {
        MAKE_STD_ZVAL(zarr);
        array_init(zarr);
        add_next_index_stringl(zarr, bucket, bucketlen, 1);
        add_next_index_stringl(zarr, key, keylen, 1);
        zval_addref_p(zdata);
        add_next_index_zval(zarr, zdata);
        zlist = zend_read_property(riak_mrinput_keydatalist_ce, getThis(), "inputList", sizeof("inputList")-1, 1 TSRMLS_CC);
        add_next_index_zval(zlist, zarr);
    } else {
        zend_throw_exception(riak_badarguments_exception_ce, "Key or bucketname missing", 5001 TSRMLS_CC);
    }
    RIAK_RETURN_THIS
}
/* }}} */


/* {{{ proto array Riak\MapReduce\Input\KeyDataListInput->getValue()
Returns value to use in Mapreduce */
PHP_METHOD(Riak_MapReduce_Input_KeyDataListInput, getValue)
{
    zval* zinputlist = zend_read_property(riak_mrinput_keydatalist_ce, getThis(), "inputList", sizeof("inputList")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zinputlist) == IS_ARRAY) {
        RETURN_ZVAL(zinputlist, 1, 0);
    }

}
/* }}} */


