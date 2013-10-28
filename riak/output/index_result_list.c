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
#include "index_result_list.h"

zend_class_entry *riak_index_result_list_ce;

static zend_function_entry riak_output_index_result_list_methods[] = {
    {NULL, NULL, NULL}
};


void riak_output_index_result_list_init(TSRMLS_D)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "IndexResultList", riak_output_index_result_list_methods);
    riak_index_result_list_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_index_result_list_ce, "resultList", sizeof("resultList")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
