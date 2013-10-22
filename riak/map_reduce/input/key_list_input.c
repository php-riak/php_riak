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
#include "key_list_input.h"
#include "input.h"
#include "riak/object.h"
#include "riak/bucket.h"
#include "riak/exception/exception.h"
#include "ht_utils.h"

zend_class_entry *riak_mrinput_keylist_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_mr_input_keylist_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, bucketkeys_arr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mr_input_keylist_single, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, bucket)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mr_input_keylist_toarr, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_mrinputlist_methods[] = {
    PHP_ME(Riak_MapReduce_Input_KeyListInput, __construct, arginfo_mr_input_keylist_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Riak_MapReduce_Input_KeyListInput, addArray, arginfo_mr_input_keylist_ctor, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_MapReduce_Input_KeyListInput, addSingle, arginfo_mr_input_keylist_single, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_MapReduce_Input_KeyListInput, getValue, arginfo_mr_input_keylist_toarr, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_map_reduce_input_key_list_input_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\MapReduce\\Input", "KeyListInput", riak_mrinputlist_methods);
    riak_mrinput_keylist_ce = zend_register_internal_class_ex(&ce, riak_mrinput_ce, NULL TSRMLS_CC);
    zend_declare_property_null(riak_mrinput_keylist_ce, "inputList", sizeof("inputList")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */


/*************************************************************
* Implementation: Riak\MapReduce\Input\KeyListInput
*************************************************************/

zval *riak_create_kv_pair(char* bucket, int bucketlen, char* key, int keylen)/* {{{{ */
{
    zval *pair;
    MAKE_STD_ZVAL(pair);
    array_init(pair);
    add_next_index_stringl(pair, bucket, bucketlen, 1);
    add_next_index_stringl(pair, key, keylen, 1);
    return pair;
}
/* }}} */

void riak_array_to_tupple_array_deep_cb(void* callingObj, void* custom_ptr, char* key,
                                   uint keylen, uint index, zval** data, int cnt TSRMLS_DC)/* {{{{ */
{
    zval *add;
    char* addkey;
    int addkeylen;
    zval *bucket = (zval*)callingObj;
    zval *to = (zval*)custom_ptr;

    if (Z_TYPE_PP(data) == IS_STRING) {
        add = riak_create_kv_pair(Z_STRVAL_P(bucket), Z_STRLEN_P(bucket), Z_STRVAL_PP(data), Z_STRLEN_PP(data));
        add_next_index_zval(to, add);
    } else if (Z_TYPE_PP(data) == IS_OBJECT) {
        riak_key_from_object(*data, &addkey, &addkeylen TSRMLS_CC);
        add = riak_create_kv_pair(Z_STRVAL_P(bucket), Z_STRLEN_P(bucket), addkey, addkeylen);
        add_next_index_zval(to, add);
    }
}
/* }}} */

void riak_array_to_tupple_array_cb(void* callingObj, void* custom_ptr, char* arraykey,
                                   uint arraykeylen, uint index, zval** data, int cnt TSRMLS_DC)/* {{{{ */
{
    char* riakkey;
    int riakkeylen;
    zval *add, bucket;
    zval *to = (zval*)custom_ptr;

    /* arraykey is actually the bucket name */
    if (arraykey == NULL || arraykeylen == 0) {
        return;
    }
    if (Z_TYPE_PP(data) == IS_ARRAY) {
        /* Data is array of keys, iterate those aswell */
        ZVAL_STRINGL(&bucket, arraykey, arraykeylen-1, 0);
        foreach_in_hashtable(&bucket, to, Z_ARRVAL_PP(data), &riak_array_to_tupple_array_deep_cb TSRMLS_CC);
    } else if (Z_TYPE_PP(data) == IS_STRING) {
        /* Data is a string with the riak key */
        add = riak_create_kv_pair(arraykey, arraykeylen-1, Z_STRVAL_PP(data), Z_STRLEN_PP(data));
        add_next_index_zval(to, add);
    } else if (Z_TYPE_PP(data) == IS_OBJECT) {
        /* Data is RiakObject */
        riak_key_from_object(*data, &riakkey, &riakkeylen TSRMLS_CC);
        add = riak_create_kv_pair(arraykey, arraykeylen-1, riakkey, riakkeylen);
        add_next_index_zval(to, add);
    }
}
/* }}} */

zval* riak_array_to_tupple_array(HashTable* from TSRMLS_DC)/* {{{{ */
{
    zval *to;
    MAKE_STD_ZVAL(to);
    array_init(to);
    foreach_in_hashtable(NULL, to, from, &riak_array_to_tupple_array_cb TSRMLS_CC);
    return to;
}
/* }}} */

/* {{{ proto void Riak\MapReduce\Input\KeyListInput->__construct(array $bucketKeys)
Create a KeyListInput, input should be an array with ["bucket" => "key", "bucket2" => "key2"] or ["bucket" => ["key1", "key2"]] */
PHP_METHOD(Riak_MapReduce_Input_KeyListInput, __construct)
{
    zval *zarr;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &zarr) == FAILURE) {
        return;
    }
    zend_update_property(riak_mrinput_keylist_ce, getThis(), "inputList", sizeof("inputList")-1, zarr TSRMLS_CC);
}
/* }}} */

/* {{{ proto Riak\MapReduce\Input\KeyListInput Riak\MapReduce\Input\KeyListInput->addArray(array $bucketKeys)
Add bucket/key inputs to the current list, $bucketKeys must be an array with ["bucket" => "key", "bucket2" => "key2"] or ["bucket" => ["key1", "key2"]] */
PHP_METHOD(Riak_MapReduce_Input_KeyListInput, addArray)
{
    zval *zarr[2], zfuncname, *zcombinedarr;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &zarr[1]) == FAILURE) {
        return;
    }
    ZVAL_STRING(&zfuncname, "array_merge_recursive", 0);
    zarr[0] = zend_read_property(riak_mrinput_keylist_ce, getThis(), "inputList", sizeof("inputList")-1, 1 TSRMLS_CC);

    MAKE_STD_ZVAL(zcombinedarr);
    call_user_function(EG(function_table), NULL, &zfuncname, zcombinedarr, 2, zarr TSRMLS_CC);

    zend_update_property(riak_mrinput_keylist_ce, getThis(), "inputList", sizeof("inputList")-1, zcombinedarr TSRMLS_CC);
    //zval_ptr_dtor(&zarr[0]);
    zval_ptr_dtor(&zcombinedarr);

    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto Riak\MapReduce\Input\KeyListInput Riak\MapReduce\Input\KeyListInput->addSingle(string $bucket, string $key)
Add a single bucket/key to the current list of inputs */
PHP_METHOD(Riak_MapReduce_Input_KeyListInput, addSingle)
{
    zval *zbucket, *zobject, *zarray;
    char *bucket, *key;
    int bucketlen, keylen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &zbucket, &zobject) == FAILURE) {
        return;
    }
    key = bucket = NULL;
    keylen = bucketlen = 0;
    if (Z_TYPE_P(zbucket) == IS_STRING) {
        bucket = Z_STRVAL_P(zbucket);
        bucketlen = Z_STRLEN_P(zbucket);
    } else if (Z_TYPE_P(zbucket) == IS_OBJECT) {
        riak_name_from_bucket(zbucket, &bucket, &bucketlen TSRMLS_CC);
    }
    if (Z_TYPE_P(zobject) == IS_STRING) {
        key = Z_STRVAL_P(zobject);
        keylen = Z_STRLEN_P(zobject);
    } else if (Z_TYPE_P(zobject) == IS_OBJECT) {
        riak_key_from_object(zobject, &key, &keylen TSRMLS_CC);
    }
    if (keylen > 0 && bucketlen > 0) {
        MAKE_STD_ZVAL(zarray);
        array_init(zarray);
        add_assoc_stringl_ex(zarray, bucket, bucketlen, key, keylen, 1);
        RIAK_CALL_METHOD1(Riak_MapReduce_Input_KeyListInput, addArray, return_value, getThis(), zarray);
        zval_ptr_dtor(&zarray);
    } else {
        zend_throw_exception(riak_badarguments_exception_ce, "Key or bucketname missing", 5001 TSRMLS_CC);
    }
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto array Riak\MapReduce\Input\KeyListInput->getValue()
Returns value to use in Mapreduce */
PHP_METHOD(Riak_MapReduce_Input_KeyListInput, getValue)
{
    zval* zresult;
    zval* zinputlist = zend_read_property(riak_mrinput_keylist_ce, getThis(), "inputList", sizeof("inputList")-1, 1 TSRMLS_CC);
    zresult = riak_array_to_tupple_array(Z_ARRVAL_P(zinputlist) TSRMLS_CC);
    RETURN_ZVAL(zresult, 0, 1);
}
/* }}} */

