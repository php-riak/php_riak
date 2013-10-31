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
#include "index_output.h"
#include "output.h"
#include "index_result_list.h"
#include "index_result.h"
#include "../exception/exception.h"

zend_class_entry *riak_index_output_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_output_index_output_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_output_index_output_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, indexOutput)
    ZEND_ARG_INFO(0, continuation)
ZEND_END_ARG_INFO()

static zend_function_entry riak_output_index_output_methods[] = {
    PHP_ME(Riak_Index_Output, __construct, arginfo_riak_output_index_output_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Riak_Index_Output, getContinuation, arginfo_riak_output_index_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Index_Output, getResultList, arginfo_riak_output_index_output_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


void riak_output_index_output_init(TSRMLS_D)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "IndexOutput", riak_output_index_output_methods);
    riak_index_output_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(riak_index_output_ce, "result", sizeof("result")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_index_output_ce, "continuation", sizeof("continuation")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}

zval *get_index_output_from_riack_string_list(RIACK_STRING_LIST *result_keys TSRMLS_DC)
{
    zval *zresult;
    size_t i;
    MAKE_STD_ZVAL(zresult);
    object_init_ex(zresult, riak_index_result_list_ce);
    RIAK_CALL_METHOD(Riak_Index_Result_List, __construct, NULL, zresult);
    for (i=0; i<result_keys->string_count; ++i) {
        zval *zcurrent, *zkey, *zoffset;
        MAKE_STD_ZVAL(zoffset);
        ZVAL_LONG(zoffset, i);

        MAKE_STD_ZVAL(zkey);
        ZVAL_STRINGL(zkey, result_keys->strings[i].value, result_keys->strings[i].len, 1);

        MAKE_STD_ZVAL(zcurrent);
        object_init_ex(zcurrent, riak_index_result_ce);
        RIAK_CALL_METHOD1(Riak_Output_IndexResult, __construct, NULL, zcurrent, zkey);

        RIAK_CALL_METHOD2(Riak_Index_Result_List, offsetSet, NULL, zresult, zoffset, zcurrent);
        zval_ptr_dtor(&zoffset);
        zval_ptr_dtor(&zkey);
        zval_ptr_dtor(&zcurrent);
    }
    return zresult;
}

PHP_METHOD(Riak_Index_Output, __construct)
{
    char *continuation;
    int continuation_len;
    zval *zresultlist;
    continuation = 0;
    continuation_len = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|s", &zresultlist, riak_index_result_list_ce,
                              &continuation, &continuation_len) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    if (continuation && continuation_len > 0) {
        zend_update_property_stringl(riak_index_output_ce, getThis(), "continuation",
                                     sizeof("continuation")-1, continuation, continuation_len TSRMLS_CC);
    }
    zend_update_property(riak_index_output_ce, getThis(), "result", sizeof("result")-1, zresultlist TSRMLS_CC);
}

PHP_METHOD(Riak_Index_Output, getResultList)
{
    RIAK_GETTER_OBJECT(riak_index_output_ce, "result")
}

PHP_METHOD(Riak_Index_Output, getContinuation)
{
    RIAK_GETTER_STRING(riak_index_output_ce, "continuation");
}
