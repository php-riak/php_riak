/*
   Copyright 2013 Kaspar Bach Pedersen

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

#include "riak/output/conflict_resolver.h"
#include "get_resolver_input.h"
#include "get_input.h"

zend_class_entry *riak_get_resolver_input_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_resolver_input_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_get_resolver_input_ctor, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_OBJ_INFO(0, resolver, Riak\\Output\\ConflictResolver, 0) 
ZEND_END_ARG_INFO()

static zend_function_entry riak_get_resolver_input_methods[] = {
    PHP_ME(Riak_Input_GetResolverInput, __construct, arginfo_riak_get_resolver_input_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Riak_Input_GetResolverInput, getConflictResolver, arginfo_riak_get_resolver_input_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_input_get_resolver_input_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Input", "GetResolverInput", riak_get_resolver_input_methods);
    
    riak_get_resolver_input_ce = zend_register_internal_class_ex(&ce, riak_get_input_ce, NULL TSRMLS_CC);

    zend_declare_property_null(riak_get_resolver_input_ce, "conflictResolver", sizeof("conflictResolver")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */

/*************************************************************************
* Implementation: Riak\Input\GetResolverInput extends Riak\Input\GetInput
**************************************************************************/

/* {{{ proto void Riak\Input\GetResolverInput->__construct(Riak\Output\ConflictResolver $resolver)
Create a new Riak\Input\GetResolverInput */
PHP_METHOD(Riak_Input_GetResolverInput, __construct)
{
    zval* zResolver;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zResolver, riak_output_conflict_resolver_ce) == FAILURE) {
        return;
    }

    zend_update_property(riak_get_resolver_input_ce, getThis(), "conflictResolver", sizeof("conflictResolver")-1, zResolver TSRMLS_CC);
}
/* }}} */

/* {{{ proto Riak\Output\ConflictResolver Riak\Input\GetResolverInput->getConflictResolver() */
PHP_METHOD(Riak_Input_GetResolverInput, getConflictResolver)
{
    RIAK_GETTER_OBJECT(riak_get_resolver_input_ce, "conflictResolver");
}
/* }}} */
