/*
   Copyright 2012 Trifork A/S
   Author: Kaspar Pedersen

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
#include <php.h>
#include <zend_exceptions.h>

zend_class_entry *riak_connection_exception_ce;
zend_class_entry *riak_communication_exception_ce;
zend_class_entry *riak_response_exception_ce;

void riak_exceptions_init(TSRMLS_D) 
{
  zend_class_entry ceConnExc, ceCommExc, ceRespExc;
 
  INIT_CLASS_ENTRY(ceConnExc, "RiakConnectionException", NULL);
  riak_connection_exception_ce = zend_register_internal_class_ex(&ceConnExc, (zend_class_entry*)zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);
  
  INIT_CLASS_ENTRY(ceCommExc, "RiakCommunicationException", NULL);
  riak_communication_exception_ce = zend_register_internal_class_ex(&ceCommExc, (zend_class_entry*)zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);

  INIT_CLASS_ENTRY(ceRespExc, "RiakResponseException", NULL);
  riak_response_exception_ce = zend_register_internal_class_ex(&ceRespExc, (zend_class_entry*)zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);
}
