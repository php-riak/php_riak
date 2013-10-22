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
#include "stream_output.h"

zend_class_entry *riak_mr_streamer_ce;


ZEND_BEGIN_ARG_INFO_EX(arginfo_mrstreamer_rec, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()


static zend_function_entry riak_mapreduce_out_stream_methods[] = {
    ZEND_ABSTRACT_ME(Riak_MapReduce_Output_StreamOutput, receive, arginfo_mrstreamer_rec)
    {NULL, NULL, NULL}
};


void riak_map_reduce_output_stream_output_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\MapReduce\\Output","StreamOutput", riak_mapreduce_out_stream_methods);
    riak_mr_streamer_ce = zend_register_internal_interface(&ce TSRMLS_CC);
}
/* }}} */
