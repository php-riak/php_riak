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

#include "req_outputs.h"
#include "php_riak.h"

zend_class_entry *riak_get_output_ce;

static zend_function_entry riak_get_output_methods[] = {
    {NULL, NULL, NULL}
};

void riak_req_outputs_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;


    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "GetOutput", riak_get_output_methods);
    riak_get_output_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(riak_get_output_ce, "vClock", sizeof("vClock")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_get_output_ce, "objectList", sizeof("objectList")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_get_output_ce, "unchanged", sizeof("unchanged")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */
