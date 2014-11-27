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
#include "output.h"
#include "ext/json/php_json.h"

zend_class_entry *riak_mroutput_ce;

ZEND_BEGIN_ARG_INFO_EX(riak_arginfo_output_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(1, phase)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(riak_arginfo_output_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_mroutput_methods[] = {
    PHP_ME(Riak_MapReduce_Output_Output, __construct, riak_arginfo_output_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Riak_MapReduce_Output_Output, getValue, riak_arginfo_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_MapReduce_Output_Output, getPhaseNumber, riak_arginfo_output_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};



void riak_map_reduce_output_output_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\MapReduce\\Output", "Output", riak_mroutput_methods);
    riak_mroutput_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_mroutput_ce, "value", sizeof("value")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_mroutput_ce, "phase", sizeof("phase")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
}
/* }}} */


zval *riak_mroutput_from_riack_mapred(riack_mapred_response *mapresult TSRMLS_DC)/* {{{ */
{
    zval *zresult, *zvalue, *zphase;
    MAKE_STD_ZVAL(zresult);
    MAKE_STD_ZVAL(zvalue);

    php_json_decode(zvalue, (char*)mapresult->data, mapresult->data_size, 1, 10 TSRMLS_CC);
    object_init_ex(zresult, riak_mroutput_ce);

    if (mapresult->phase_present) {
        MAKE_STD_ZVAL(zphase);
        ZVAL_LONG(zphase, mapresult->phase);
        RIAK_CALL_METHOD2(Riak_MapReduce_Output_Output, __construct, zresult, zresult, zvalue, zphase);
        zval_ptr_dtor(&zphase);
    } else {
        RIAK_CALL_METHOD1(Riak_MapReduce_Output_Output, __construct, zresult, zresult, zvalue);
    }
    zval_ptr_dtor(&zvalue);
    return zresult;
}
/* }}} */

/* {{{ proto void Riak\MapReduce\Output\Output->__construct($value, [, int $phase])
Create a Riak_MapReduce_Output_Output */
PHP_METHOD(Riak_MapReduce_Output_Output, __construct)
{
    zval* zvalue;
    long phase = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l", &zvalue, &phase) == FAILURE) {
        return;
    }
    zend_update_property_long(riak_mroutput_ce, getThis(), "phase", sizeof("phase")-1, phase TSRMLS_CC);
    zend_update_property(riak_mroutput_ce, getThis(), "value", sizeof("value")-1, zvalue TSRMLS_CC);
}
/* }}} */

/* {{{ proto mixed Riak\MapReduce\Output\Output->getValue()
Get decoded value from this mr output */
PHP_METHOD(Riak_MapReduce_Output_Output, getValue)
{
    RIAK_GETTER_MIXED(riak_mroutput_ce, "value");
}
/* }}} */

/* {{{ proto int|null Riak\MapReduce\Output\Output->getPhaseNumber()
Get phase number if set in response */
PHP_METHOD(Riak_MapReduce_Output_Output, getPhaseNumber)
{
    RIAK_GETTER_LONG(riak_mroutput_ce, "phase");
}
/* }}} */

