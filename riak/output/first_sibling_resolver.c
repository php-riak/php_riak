/*
   Copyright 2013: Kaspar Pedersen

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

#include "zend_interfaces.h"
#include "conflict_resolver.h"
#include "first_sibling_resolver.h"

zend_class_entry *riak_output_first_sibling_resolver_ce;

RIAK_OUTPUT_CONFLICT_RESOLVER_ARG_INFO_EXEC(arginfo_first_sibling_resolver_resolve)

static zend_function_entry riak_output_first_sibling_resolver_methods[] = {
    PHP_ME(Riak_Output_FirstSiblingResolver, resolve, arginfo_first_sibling_resolver_resolve, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_output_first_sibling_resolver_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "FirstSiblingResolver", riak_output_first_sibling_resolver_methods);

    riak_output_first_sibling_resolver_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_class_implements(riak_output_first_sibling_resolver_ce TSRMLS_CC, 1, riak_output_conflict_resolver_ce);
}
/* }}} */

/* {{{ proto \Riak\Object Riak\Output\GetOutput->resolve(\Riak\ObjectList $objects) */
PHP_METHOD(Riak_Output_FirstSiblingResolver, resolve)
{
    zval *zObject, *zObjects;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zObjects) == FAILURE) {
        return;
    }

    zend_call_method_with_0_params(&zObjects, NULL, NULL, "first", &zObject);

    if (zObject && Z_TYPE_P(zObject) == IS_OBJECT) {
        RETURN_ZVAL(zObject, 0, 1)
    }
    zval_ptr_dtor(&zObject);
    RETURN_NULL();
}
/* }}} */
