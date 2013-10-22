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
#include "bucket_input.h"
#include "input.h"
#include "riak/bucket.h"

zend_class_entry *riak_mrinput_bucket_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_mr_input_bucket_toarr, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mr_input_bucket_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mr_input_bucket_setindex, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()

static zend_function_entry riak_mrinputbucket_methods[] = {
    PHP_ME(Riak_MapReduce_Input_BucketInput, __construct, arginfo_mr_input_bucket_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Riak_MapReduce_Input_BucketInput, setIndexFilter, arginfo_mr_input_bucket_setindex, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_MapReduce_Input_BucketInput, getValue, arginfo_mr_input_bucket_toarr, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_map_reduce_input_bucket_input_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\MapReduce\\Input", "BucketInput", riak_mrinputbucket_methods);
    riak_mrinput_bucket_ce = zend_register_internal_class_ex(&ce, riak_mrinput_ce, NULL TSRMLS_CC);
    zend_declare_property_null(riak_mrinput_bucket_ce, "name", sizeof("name")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_mrinput_bucket_ce, "keyFilters", sizeof("keyFilters")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(riak_mrinput_bucket_ce, "idxname", sizeof("idxname")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_mrinput_bucket_ce, "idxstart", sizeof("idxstart")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_mrinput_bucket_ce, "idxend", sizeof("idxend")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */

/*************************************************************
* Implementation: Riak\MapReduce\Input\BucketInput
*************************************************************/

/* {{{ proto void Riak\MapReduce\Input\BucketInput->__construct(string $bucketName)
Creates a new BucketInput */
PHP_METHOD(Riak_MapReduce_Input_BucketInput, __construct)
{
    char *name;
    int namelen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &namelen) == FAILURE) {
        return;
    }
    zend_update_property_stringl(riak_mrinput_bucket_ce, getThis(), "name", sizeof("name")-1, name, namelen TSRMLS_CC);
}
/* }}} */

/* {{{ proto void Riak\MapReduce\Input\BucketInput->setIndexFilter($indexname, $start [, $end])
Add a index query filter */
PHP_METHOD(Riak_MapReduce_Input_BucketInput, setIndexFilter)
{
    char *index, *start, *end;
    int indexlen, startlen, endlen;
    end = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &index, &indexlen, &start, &startlen, &end, &endlen) == FAILURE) {
        return;
    }
    zend_update_property_stringl(riak_mrinput_bucket_ce, getThis(), "idxname", sizeof("idxname")-1, index, indexlen TSRMLS_CC);
    zend_update_property_stringl(riak_mrinput_bucket_ce, getThis(), "idxstart", sizeof("idxstart")-1, start, startlen TSRMLS_CC);
    if (end != NULL && endlen > 0) {
        zend_update_property_stringl(riak_mrinput_bucket_ce, getThis(), "idxend", sizeof("idxend")-1, end, endlen TSRMLS_CC);
    }
    RIAK_RETURN_THIS
}

/* {{{ proto string|array Riak\MapReduce\Input\BucketInput->getValue()
Returns value to use in Mapreduce */
PHP_METHOD(Riak_MapReduce_Input_BucketInput, getValue)
{
    zval* zname, *zfilters, *zresult, *zindex;
    zname = zend_read_property(riak_mrinput_bucket_ce, getThis(), "name", sizeof("name")-1, 1 TSRMLS_CC);
    zfilters = zend_read_property(riak_mrinput_bucket_ce, getThis(), "keyFilters", sizeof("keyFilters")-1, 1 TSRMLS_CC);
    zindex = zend_read_property(riak_mrinput_bucket_ce, getThis(), "idxname", sizeof("idxname")-1, 1 TSRMLS_CC);

    if (Z_TYPE_P(zfilters) == IS_ARRAY || Z_TYPE_P(zindex) == IS_STRING) {
        MAKE_STD_ZVAL(zresult);
        array_init(zresult);
        zval_addref_p(zname);
        add_assoc_zval_ex(zresult, "bucket", sizeof("bucket"), zname);
        if (Z_TYPE_P(zindex) == IS_STRING) {
            zval *start, *end;
            start = zend_read_property(riak_mrinput_bucket_ce, getThis(), "idxstart", sizeof("idxstart")-1, 1 TSRMLS_CC);
            end = zend_read_property(riak_mrinput_bucket_ce, getThis(), "idxend", sizeof("idxend")-1, 1 TSRMLS_CC);

            zval_addref_p(zindex);
            add_assoc_zval_ex(zresult, "index", sizeof("index"), zindex);
            if (Z_TYPE_P(end) != IS_STRING) {
                zval_addref_p(start);
                add_assoc_zval_ex(zresult, "key", sizeof("key"), start);
            } else {
                zval_addref_p(start);
                zval_addref_p(end);
                add_assoc_zval_ex(zresult, "start", sizeof("start"), start);
                add_assoc_zval_ex(zresult, "end", sizeof("end"), end);
            }
        }
        if (Z_TYPE_P(zfilters) == IS_ARRAY) {
            zval_addref_p(zfilters);
            add_assoc_zval_ex(zresult, "key_filters", sizeof("key_filters"), zfilters);
        }
        RETURN_ZVAL(zresult, 0, 1);
    } else {
        RETURN_ZVAL(zname, 1, 0);
    }
}
/* }}} */

