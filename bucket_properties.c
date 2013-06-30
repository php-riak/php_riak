/*
   Copyright 2013 Trifork A/S
   Author: Kaspar Bach Pedersen

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
#include "bucket_properties.h"
#include "bucket.h"
#include "connection.h"
#include "object.h"
#include "exceptions.h"
#include "php_riak.h"

zend_class_entry *riak_bucket_properties_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_props_ctor, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, nVal)
    ZEND_ARG_INFO(0, allowMult)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_props_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_props_set_nval, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, nVal)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_props_set_allow_mult, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, allowMult)
ZEND_END_ARG_INFO()

static zend_function_entry riak_bucket_properties_methods[] = {
	PHP_ME(RiakBucketProperties, __construct, arginfo_bucket_props_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakBucketProperties, getNValue, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setNValue, arginfo_bucket_props_set_nval, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getAllowMult, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setAllowMult, arginfo_bucket_props_set_allow_mult, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void riak_bucket_props_init(TSRMLS_D)/* {{{ */
{
	zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak", "BucketPropertyList", riak_bucket_properties_methods);
	riak_bucket_properties_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_long(riak_bucket_properties_ce, "nVal", sizeof("nVal")-1, 3, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_bool(riak_bucket_properties_ce, "allowMult", sizeof("allowMult")-1, 0, ZEND_ACC_PRIVATE TSRMLS_CC);
}
/* }}} */

/* {{{ proto void Riak\BucketProperties->__construct(int $nVal, bool $allowMult)
Creates a new RiakBucketProperties */
PHP_METHOD(RiakBucketProperties, __construct)
{
	long nVal;
	zend_bool allowMult;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lb", &nVal, &allowMult) == FAILURE) {
		return;
	}
	zend_update_property_long(riak_bucket_properties_ce, getThis(), "nVal", sizeof("nVal")-1, nVal TSRMLS_CC);
	zend_update_property_bool(riak_bucket_properties_ce, getThis(), "allowMult", sizeof("allowMult")-1, allowMult TSRMLS_CC);
}
/* }}} */

PHP_METHOD(RiakBucketProperties, getNValue)
{
    RIAK_GETTER_LONG(riak_bucket_properties_ce, "nVal")
}

PHP_METHOD(RiakBucketProperties, setNValue)
{
    RIAK_SETTER_LONG(riak_bucket_properties_ce, "nVal")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getAllowMult)
{
    RIAK_GETTER_BOOL(riak_bucket_properties_ce, "allowMult")
}

PHP_METHOD(RiakBucketProperties, setAllowMult)
{
    RIAK_SETTER_BOOL(riak_bucket_properties_ce, "allowMult")
    RIAK_RETURN_THIS
}
