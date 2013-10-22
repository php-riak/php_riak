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
#include "phase.h"

zend_class_entry *riak_mrphase_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_map_red_phase_toarr, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_mrphase_methods[] = {
    ZEND_ABSTRACT_ME(Riak_MapReduce_Phase_Phase, toArray, arginfo_map_red_phase_toarr)
    {NULL, NULL, NULL}
};


void riak_map_reduce_phase_phase_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\MapReduce\\Phase", "Phase", riak_mrphase_methods);
    riak_mrphase_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_mrphase_ce, "function", sizeof("functions")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_bool(riak_mrphase_ce, "keep", sizeof("keep")-1, 0, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_mrphase_ce, "arg", sizeof("arg")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */


