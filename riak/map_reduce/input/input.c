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
#include "input.h"

zend_class_entry *riak_mrinput_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_mr_input_toarr, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_mrinput_methods[] = {
    ZEND_ABSTRACT_ME(RiakMrInput, getValue, arginfo_mr_input_toarr)
    {NULL, NULL, NULL}
};

void riak_map_reduce_input_input_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\MapReduce\\Input", "Input", riak_mrinput_methods);
    riak_mrinput_ce = zend_register_internal_class(&ce TSRMLS_CC);
}
/* }}} */

