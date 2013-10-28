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

#include "index_input.h"
#include "input.h"

zend_class_entry *riak_index_input_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_index_input_set_continuation, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, continuation)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_index_input_set_maxresults, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, continuation)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_index_input_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_index_input_methods[] = {
    PHP_ME(Riak_Input_IndexInput, setContinuation, arginfo_riak_index_input_set_continuation, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_IndexInput, getContinuation, arginfo_riak_index_input_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_IndexInput, setMaxResults, arginfo_riak_index_input_set_maxresults, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Input_IndexInput, getMaxResults, arginfo_riak_index_input_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


void riak_input_index_input_init(TSRMLS_D)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Input", "IndexInput", riak_index_input_methods);
    riak_index_input_ce = zend_register_internal_class_ex(&ce, riak_input_ce, NULL TSRMLS_CC);
    zend_declare_property_null(riak_index_input_ce, "continuation", sizeof("continuation")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_index_input_ce, "maxResults", sizeof("maxResults")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}

PHP_METHOD(Riak_Input_IndexInput, setContinuation)
{
    RIAK_SETTER_STRING(riak_index_input_ce, "continuation");
    RIAK_RETURN_THIS
}

PHP_METHOD(Riak_Input_IndexInput, getContinuation)
{
    RIAK_GETTER_STRING(riak_index_input_ce, "continuation");
}

PHP_METHOD(Riak_Input_IndexInput, setMaxResults)
{
    RIAK_SETTER_STRING(riak_index_input_ce, "maxResults");
    RIAK_RETURN_THIS
}

PHP_METHOD(Riak_Input_IndexInput, getMaxResults)
{
    RIAK_GETTER_LONG(riak_index_input_ce, "maxResults");
}
