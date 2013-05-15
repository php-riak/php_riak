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
#include "mr_phase.h"
#include "php_riak.h"

zend_class_entry *riak_mrphase_ce;


ZEND_BEGIN_ARG_INFO_EX(arginfo_phase_ctor, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, type)
    ZEND_ARG_INFO(0, functions)
    ZEND_ARG_INFO(0, language)
    ZEND_ARG_INFO(0, keep)
    ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

static zend_function_entry riak_mrphase_methods[] = {
    PHP_ME(RiakMapreducePhase, __construct, arginfo_phase_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    {NULL, NULL, NULL}
};

void riak_mrphase_init(TSRMLS_D)
{
    zend_class_entry ce;

    INIT_CLASS_ENTRY(ce, "RiakMapreducePhase", riak_mrphase_methods);
    riak_mrphase_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(riak_mrphase_ce, "type", sizeof("type")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(riak_mrphase_ce, "function", sizeof("functions")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_bool(riak_mrphase_ce, "keep", sizeof("keep")-1, 0, ZEND_ACC_PUBLIC TSRMLS_CC);
    zend_declare_property_null(riak_mrphase_ce, "args", sizeof("args")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
}

/////////////////////////////////////////////////////////////

PHP_METHOD(RiakMapreducePhase, __construct)
{
    char *type;
    int typelen;
    zval *zfunction, *zargs;
    zend_bool keep;
    keep = 0;
    zfunction = zargs = NULL;
    type = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "so|ba", &type, &typelen, &zfunction, &keep, &zargs) == FAILURE) {
        return;
    }
    zend_update_property_stringl(riak_mrphase_ce, getThis(), "type", sizeof("type")-1, type, typelen TSRMLS_CC);
    zend_update_property(riak_mrphase_ce, getThis(), "function", sizeof("function")-1, zfunction TSRMLS_CC);
    zend_update_property_bool(riak_mrphase_ce, getThis(), "keep", sizeof("keep")-1, keep TSRMLS_CC);
    if (zargs) {
        zend_update_property(riak_mrphase_ce, getThis(), "args", sizeof("args")-1, zargs TSRMLS_CC);
    }
}

