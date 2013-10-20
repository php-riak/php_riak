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

#include "exception.h"

zend_class_entry *riak_exception_ce;

zend_class_entry *riak_badarguments_exception_ce;
zend_class_entry *riak_connection_exception_ce;
zend_class_entry *riak_communication_exception_ce;
zend_class_entry *riak_response_exception_ce;
zend_class_entry *riak_not_found_exception_ce;

void riak_exceptions_init(TSRMLS_D) 
{
	zend_class_entry ceBadArgs, ceRiak, ceConnExc, ceCommExc, ceRespExc, ceNotFound;

    INIT_NS_CLASS_ENTRY(ceRiak, "Riak\\Exception", "RiakException", NULL);
	riak_exception_ce = zend_register_internal_class_ex(&ceRiak, 
		(zend_class_entry*)zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ceBadArgs, "Riak\\Exception", "BadArgumentsException", NULL);
	riak_badarguments_exception_ce = zend_register_internal_class_ex(&ceBadArgs, 
		riak_exception_ce, NULL TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ceConnExc, "Riak\\Exception", "ConnectionException", NULL);
	riak_connection_exception_ce = zend_register_internal_class_ex(&ceConnExc, 
		riak_exception_ce, NULL TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ceCommExc, "Riak\\Exception","CommunicationException", NULL);
	riak_communication_exception_ce = zend_register_internal_class_ex(&ceCommExc, 
		riak_exception_ce, NULL TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ceRespExc, "Riak\\Exception", "UnexpectedResponseException", NULL);
	riak_response_exception_ce = zend_register_internal_class_ex(&ceRespExc, 
		riak_exception_ce, NULL TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ceNotFound, "Riak\\Exception","NotFoundException", NULL);
	riak_not_found_exception_ce = zend_register_internal_class_ex(&ceNotFound, 
		riak_exception_ce, NULL TSRMLS_CC);

}
