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
#include "mapreduce.h"
#include "php_riak.h"

zend_class_entry *riak_mapreduce_ce;


ZEND_BEGIN_ARG_INFO_EX(arginfo_mapred_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, client)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_addphase, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, phase)
ZEND_END_ARG_INFO()

static zend_function_entry riak_mrphase_methods[] = {
    PHP_ME(RiakMapreduce, __construct, arginfo_mapred_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakMapreduce, addPhase, arginfo_addphase, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_mapreduce_init(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "RiakMapreduce", NULL);
    riak_mapreduce_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(riak_mapreduce_ce, "phases", sizeof("phases")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_mapreduce_ce, "client", sizeof("client")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
}

PHP_METHOD(RiakMapreduce, __construct)
{
    zval *zclient, *zphases;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zclient) == FAILURE) {
        return;
    }
    zend_update_property(riak_mapreduce_ce, getThis(), "client", sizeof("client")-1, zclient TSRMLS_CC);
    MAKE_STD_ZVAL(zphases);
    array_init(zphases);
    add_property_zval_ex(getThis(), "phases", sizeof("phases")-1, zphases TSRMLS_CC);
    zval_ptr_dtor(&zphases);
}

PHP_METHOD(RiakMapreduce, addPhase)
{
    zval *zphase;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zphase) == FAILURE) {
        return;
    }
    // TODO
    RETURN_ZVAL(getThis(), 0, 0);
}
