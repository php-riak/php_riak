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
#include "mr_result.h"
#include "php_riak.h"
#include "ext/json/php_json.h"

zend_class_entry *riak_mrresult_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_result_toarr, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static zend_function_entry riak_mrresult_methods[] = {
    PHP_ME(RiakMrResult, __construct, arginfo_result_toarr, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    {NULL, NULL, NULL}
};

void riak_mrresult_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "RiakMrResult", riak_mrresult_methods);
    riak_mrresult_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(riak_mrresult_ce, "value", sizeof("value")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(riak_mrresult_ce, "phase", sizeof("phase")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
}
/* }}} */

zval *riak_mrresult_from_riack_mapred(struct RIACK_MAPRED_RESULT *mapresult TSRMLS_DC)/* {{{ */
{
    zval *zresult, *zvalue, *zphase;
    MAKE_STD_ZVAL(zresult);
    MAKE_STD_ZVAL(zvalue);

    php_json_decode(zvalue, (char*)mapresult->data, mapresult->data_size, 1, 10 TSRMLS_CC);
    object_init_ex(zresult, riak_mrresult_ce);

    if (mapresult->phase_present) {
        MAKE_STD_ZVAL(zphase);
        ZVAL_LONG(zphase, mapresult->phase);
        RIAK_CALL_METHOD2(RiakMrResult, __construct, zresult, zresult, zvalue, zphase);
        zval_ptr_dtor(&zphase);
    } else {
        RIAK_CALL_METHOD1(RiakMrResult, __construct, zresult, zresult, zvalue);
    }
    zval_ptr_dtor(&zvalue);
    return zresult;
}
/* }}} */

/* {{{ proto void RiakMrResult->__construct($value, [, int $phase])
Create a RiakMrResult */
PHP_METHOD(RiakMrResult, __construct)
{
    zval* zvalue;
    long phase = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &zvalue, &phase) == FAILURE) {
        return;
    }
    zend_update_property_long(riak_mrresult_ce, getThis(), "phase", sizeof("phase")-1, phase TSRMLS_CC);
    zend_update_property(riak_mrresult_ce, getThis(), "value", sizeof("value")-1, zvalue TSRMLS_CC);
}
/* }}} */
