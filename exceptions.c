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
#include "exceptions.h"

zend_class_entry *riak_exception_ce;

zend_class_entry *riak_badarguments_exception_ce;
zend_class_entry *riak_connection_exception_ce;
zend_class_entry *riak_communication_exception_ce;
zend_class_entry *riak_response_exception_ce;
zend_class_entry *riak_conflicted_object_exception_ce;
zend_class_entry *riak_not_found_exception_ce;

static zend_function_entry riak_conflicted_exception_methods[] = {
	PHP_ME(RiakConflictedObjectException, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	{NULL, NULL, NULL}
};

void riak_exceptions_init(TSRMLS_D) 
{
	zend_class_entry ceBadArgs, ceRiak, ceConnExc, ceCommExc, ceRespExc, ceConflict, ceNotFound;

	INIT_CLASS_ENTRY(ceRiak, "RiakException", NULL);
	riak_exception_ce = zend_register_internal_class_ex(&ceRiak, 
		(zend_class_entry*)zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);

	INIT_CLASS_ENTRY(ceBadArgs, "RiakBadArgumentsException", NULL);
	riak_badarguments_exception_ce = zend_register_internal_class_ex(&ceBadArgs, 
		riak_exception_ce, NULL TSRMLS_CC);

	INIT_CLASS_ENTRY(ceConnExc, "RiakConnectionException", NULL);
	riak_connection_exception_ce = zend_register_internal_class_ex(&ceConnExc, 
		riak_exception_ce, NULL TSRMLS_CC);

	INIT_CLASS_ENTRY(ceCommExc, "RiakCommunicationException", NULL);
	riak_communication_exception_ce = zend_register_internal_class_ex(&ceCommExc, 
		riak_exception_ce, NULL TSRMLS_CC);

	INIT_CLASS_ENTRY(ceRespExc, "RiakResponseException", NULL);
	riak_response_exception_ce = zend_register_internal_class_ex(&ceRespExc, 
		riak_exception_ce, NULL TSRMLS_CC);

	INIT_CLASS_ENTRY(ceNotFound, "RiakNotFoundException", NULL);
	riak_not_found_exception_ce = zend_register_internal_class_ex(&ceNotFound, 
		riak_exception_ce, NULL TSRMLS_CC);

	INIT_CLASS_ENTRY(ceConflict, "RiakConflictedObjectException", riak_conflicted_exception_methods);
	riak_conflicted_object_exception_ce = zend_register_internal_class_ex(&ceConflict, 
		riak_exception_ce, NULL TSRMLS_CC);

	zend_declare_property_null(riak_conflicted_object_exception_ce, "vclock", sizeof("vclock")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_conflicted_object_exception_ce, "objects", sizeof("objects")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
}

PHP_METHOD(RiakConflictedObjectException, __construct)
{
	char *vclock;
	int vclock_len;
	zval *arr_objects;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", &vclock, &vclock_len, &arr_objects) == FAILURE) {
		return;
	}
	zend_update_property_stringl(riak_conflicted_object_exception_ce, getThis(), 
		"vclock", sizeof("vclock")-1, vclock, vclock_len TSRMLS_CC);
	zend_update_property(riak_conflicted_object_exception_ce, getThis(), "objects", sizeof("objects")-1, arr_objects TSRMLS_CC);
}
