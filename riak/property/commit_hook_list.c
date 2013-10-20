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
#include "commit_hook_list.h"
#include "riak/exception/exception.h"
#include "commit_hook.h"
#include <zend_interfaces.h>
#include <ext/spl/spl_iterators.h>
#include <ext/spl/spl_array.h>

zend_class_entry *riak_commit_hook_list_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_commit_hook_list_offset_exists, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_commit_hook_list_offset_set, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_commit_hook_list_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_commit_hook_list_methods[] = {
    PHP_ME(RiakCommitHookList, __construct, arginfo_commit_hook_list_noargs, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakCommitHookList, offsetExists, arginfo_commit_hook_list_offset_exists, ZEND_ACC_PUBLIC)
    PHP_ME(RiakCommitHookList, offsetGet, arginfo_commit_hook_list_offset_exists, ZEND_ACC_PUBLIC)
    PHP_ME(RiakCommitHookList, offsetSet, arginfo_commit_hook_list_offset_set, ZEND_ACC_PUBLIC)
    PHP_ME(RiakCommitHookList, offsetUnset, arginfo_commit_hook_list_offset_exists, ZEND_ACC_PUBLIC)
    PHP_ME(RiakCommitHookList, count, arginfo_commit_hook_list_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakCommitHookList, getIterator, arginfo_commit_hook_list_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_property_commit_hook_list_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Property", "CommitHookList", riak_commit_hook_list_methods);
    riak_commit_hook_list_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_class_implements(riak_commit_hook_list_ce TSRMLS_CC, 3, spl_ce_ArrayAccess, spl_ce_Aggregate, spl_ce_Countable);
    zend_declare_property_null(riak_commit_hook_list_ce, "hooks", sizeof("hooks")-1, ZEND_ACC_PRIVATE TSRMLS_CC);

}
/* }}} */


/* {{{ proto void Riak\Property\RiakCommitHookList->__construct()
Creates a new Riak\Property\RiakCommitHookList */
PHP_METHOD(RiakCommitHookList, __construct)
{
    zval* zhooks;
    // Start with an empty array
    MAKE_STD_ZVAL(zhooks);
    object_init_ex(zhooks, spl_ce_ArrayObject);
    zend_update_property(riak_commit_hook_list_ce, getThis(), "hooks", sizeof("hooks")-1, zhooks TSRMLS_CC);
    zval_ptr_dtor(&zhooks);
}/* }}} */

PHP_METHOD(RiakCommitHookList, offsetExists)
{
    zval *zoffset, *zhooks, *zresult;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zoffset) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zhooks = zend_read_property(riak_commit_hook_list_ce, getThis(), "hooks", sizeof("hooks")-1, 1 TSRMLS_CC);
    zend_call_method_with_1_params(&zhooks, spl_ce_ArrayObject, NULL, "offsetexists", &zresult, zoffset);
    RETURN_ZVAL(zresult, 0, 1);
}

PHP_METHOD(RiakCommitHookList, offsetGet)
{
    zval *zoffset, *zhooks, *zresult;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zoffset) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zhooks = zend_read_property(riak_commit_hook_list_ce, getThis(), "hooks", sizeof("hooks")-1, 1 TSRMLS_CC);
    zend_call_method_with_1_params(&zhooks, spl_ce_ArrayObject, NULL, "offsetget", &zresult, zoffset);
    RETURN_ZVAL(zresult, 0, 1);
}

PHP_METHOD(RiakCommitHookList, offsetSet)
{
    zval *zoffset, *zvalue, *zhooks;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zO", &zoffset, &zvalue, riak_commit_hook_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zhooks = zend_read_property(riak_commit_hook_list_ce, getThis(), "hooks", sizeof("hooks")-1, 1 TSRMLS_CC);
    zend_call_method_with_2_params(&zhooks, spl_ce_ArrayObject, NULL, "offsetset", NULL, zoffset, zvalue);
}

PHP_METHOD(RiakCommitHookList, offsetUnset)
{
    zval *zoffset, *zhooks;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zoffset) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    zhooks = zend_read_property(riak_commit_hook_list_ce, getThis(), "hooks", sizeof("hooks")-1, 1 TSRMLS_CC);
    zend_call_method_with_1_params(&zhooks, spl_ce_ArrayObject, NULL, "offsetunset", NULL, zoffset);
}

PHP_METHOD(RiakCommitHookList, count)
{
    zval *zhooks, *zresult;
    zhooks = zend_read_property(riak_commit_hook_list_ce, getThis(), "hooks", sizeof("hooks")-1, 1 TSRMLS_CC);
    zend_call_method_with_0_params(&zhooks, spl_ce_ArrayObject, NULL, "count", &zresult);
    RETURN_ZVAL(zresult, 0, 1);
}

PHP_METHOD(RiakCommitHookList, getIterator)
{
    zval *zhooks, *zresult;
    zhooks = zend_read_property(riak_commit_hook_list_ce, getThis(), "hooks", sizeof("hooks")-1, 1 TSRMLS_CC);
    zend_call_method_with_0_params(&zhooks, spl_ce_ArrayObject, NULL, "getiterator", &zresult);
    RETURN_ZVAL(zresult, 0, 1);
}


