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

#include "conflict_resolver.h"

zend_class_entry *riak_output_conflict_resolver_ce;

RIAK_OUTPUT_CONFLICT_RESOLVER_ARG_INFO_EXEC(arginfo_conflict_resolver_resolve)

static zend_function_entry riak_conflict_resolver_methods[] = {
    ZEND_ABSTRACT_ME(Riak_Output_ConflictResolver, resolve, arginfo_conflict_resolver_resolve)
    {NULL, NULL, NULL}
};

void riak_output_conflict_resolver_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "ConflictResolver", riak_conflict_resolver_methods);
    riak_output_conflict_resolver_ce = zend_register_internal_interface(&ce TSRMLS_CC);
}
/* }}} */
