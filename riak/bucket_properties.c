/*
   Copyright 2013 Kaspar Bach Pedersen

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

#include "bucket_properties.h"
#include "property/module_function.h"
#include "property/commit_hook.h"
#include "property/commit_hook_list.h"
#include "property/replication_mode/replication_mode.h"
#include "exception/exception.h"
#include "bucket.h"
#include "connection.h"
#include "object.h"

zend_class_entry *riak_bucket_properties_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_props_ctor, 0, ZEND_RETURN_VALUE, 0)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_props_set_last_write_wins, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, lastWriteWins)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_props_set_vclock, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, vClockValue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_props_set_rwpr, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_properties_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()


static zend_function_entry riak_bucket_properties_methods[] = {
	PHP_ME(RiakBucketProperties, __construct, arginfo_bucket_props_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakBucketProperties, getNValue, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setNValue, arginfo_bucket_props_set_nval, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getAllowMult, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setAllowMult, arginfo_bucket_props_set_allow_mult, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getLastWriteWins, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setLastWriteWins, arginfo_bucket_props_set_last_write_wins, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getOldVClock, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setOldVClock, arginfo_bucket_props_set_vclock, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getYoungVClock, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setYoungVClock, arginfo_bucket_props_set_vclock, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getSmallVClock, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setSmallVClock, arginfo_bucket_props_set_vclock, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getBigVClock, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setBigVClock, arginfo_bucket_props_set_vclock, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getR, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setR, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getPR, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setPR, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getW, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setW, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getDW, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setDW, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getPW, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setPW, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getRW, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setRW, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)

    PHP_ME(RiakBucketProperties, getBasicQuorum, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setBasicQuorum, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getNotFoundOk, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setNotFoundOk, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getSearchEnabled, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setSearchEnabled, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getBackend, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setBackend, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getPreCommitHookList, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setPreCommitHookList, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getPostCommitHookList, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setPostCommitHookList, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getCHashKeyFun, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setCHashKeyFun, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, getLinkFun, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setLinkFun, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)

    PHP_ME(RiakBucketProperties, getReplicationMode, arginfo_bucket_props_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucketProperties, setReplicationMode, arginfo_bucket_props_set_rwpr, ZEND_ACC_PUBLIC)

	{NULL, NULL, NULL}
};

void riak_bucket_props_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak", "BucketPropertyList", riak_bucket_properties_methods);
	riak_bucket_properties_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(riak_bucket_properties_ce, "nVal", sizeof("nVal")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "allowMult", sizeof("allowMult")-1, ZEND_ACC_PRIVATE TSRMLS_CC);

    zend_declare_property_null(riak_bucket_properties_ce, "lastWriteWins", sizeof("lastWriteWins")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "oldVClock", sizeof("oldVClock")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "youngVClock", sizeof("youngVClock")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "smallVClock", sizeof("smallVClock")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "bigVClock", sizeof("bigVClock")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "r", sizeof("r")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "pr", sizeof("pr")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "w", sizeof("w")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "dw", sizeof("dw")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "pw", sizeof("pw")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "rw", sizeof("rw")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "basicQuorum", sizeof("basicQuorum")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "notFoundOk", sizeof("notFoundOk")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "searchEnabled", sizeof("searchEnabled")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "backend", sizeof("backend")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "preCommitHooks", sizeof("preCommitHooks")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "postCommitHooks", sizeof("postCommitHooks")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "chashKeyFun", sizeof("chashKeyFun")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "linkFun", sizeof("linkFun")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_bucket_properties_ce, "replicationMode", sizeof("replicationMode")-1, ZEND_ACC_PRIVATE TSRMLS_CC);

}
/* }}} */

/* {{{ proto void Riak\BucketProperties->__construct(int $nVal, bool $allowMult)
Creates a new RiakBucketProperties */
PHP_METHOD(RiakBucketProperties, __construct)
{
	long nVal;
    zend_bool allowMult;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lb", &nVal, &allowMult) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
		return;
	}
    if (ZEND_NUM_ARGS() >= 1) {
        zend_update_property_long(riak_bucket_properties_ce, getThis(), "nVal", sizeof("nVal")-1, nVal TSRMLS_CC);
    }
    if (ZEND_NUM_ARGS() == 2) {
        zend_update_property_bool(riak_bucket_properties_ce, getThis(), "allowMult", sizeof("allowMult")-1, allowMult TSRMLS_CC);
    }
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

PHP_METHOD(RiakBucketProperties, getLastWriteWins)
{
    RIAK_GETTER_BOOL(riak_bucket_properties_ce, "lastWriteWins")
}

PHP_METHOD(RiakBucketProperties, setLastWriteWins)
{
    RIAK_SETTER_BOOL(riak_bucket_properties_ce, "lastWriteWins")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getOldVClock)
{
    RIAK_GETTER_LONG(riak_bucket_properties_ce, "oldVClock")
}

PHP_METHOD(RiakBucketProperties, setOldVClock)
{
    RIAK_SETTER_LONG(riak_bucket_properties_ce, "oldVClock")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getYoungVClock)
{
    RIAK_GETTER_LONG(riak_bucket_properties_ce, "youngVClock")
}

PHP_METHOD(RiakBucketProperties, setYoungVClock)
{
    RIAK_SETTER_LONG(riak_bucket_properties_ce, "youngVClock")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getSmallVClock)
{
    RIAK_GETTER_LONG(riak_bucket_properties_ce, "smallVClock")
}

PHP_METHOD(RiakBucketProperties, setSmallVClock)
{
    RIAK_SETTER_LONG(riak_bucket_properties_ce, "smallVClock")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getBigVClock)
{
    RIAK_GETTER_LONG(riak_bucket_properties_ce, "bigVClock")
}

PHP_METHOD(RiakBucketProperties, setBigVClock)
{
    RIAK_SETTER_LONG(riak_bucket_properties_ce, "bigVClock")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getR)
{
    RIAK_GETTER_LONG(riak_bucket_properties_ce, "r")
}

PHP_METHOD(RiakBucketProperties, setR)
{
    RIAK_SETTER_LONG(riak_bucket_properties_ce, "r")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getPR)
{
    RIAK_GETTER_LONG(riak_bucket_properties_ce, "pr")
}

PHP_METHOD(RiakBucketProperties, setPR)
{
    RIAK_SETTER_LONG(riak_bucket_properties_ce, "pr")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getW)
{
    RIAK_GETTER_LONG(riak_bucket_properties_ce, "w")
}

PHP_METHOD(RiakBucketProperties, setW)
{
    RIAK_SETTER_LONG(riak_bucket_properties_ce, "w")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getDW)
{
    RIAK_GETTER_LONG(riak_bucket_properties_ce, "dw")
}

PHP_METHOD(RiakBucketProperties, setDW)
{
    RIAK_SETTER_LONG(riak_bucket_properties_ce, "dw")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getPW)
{
    RIAK_GETTER_LONG(riak_bucket_properties_ce, "pw")
}

PHP_METHOD(RiakBucketProperties, setPW)
{
    RIAK_SETTER_LONG(riak_bucket_properties_ce, "pw")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getRW)
{
    RIAK_GETTER_LONG(riak_bucket_properties_ce, "rw")
}

PHP_METHOD(RiakBucketProperties, setRW)
{
    RIAK_SETTER_LONG(riak_bucket_properties_ce, "rw")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getBasicQuorum)
{
    RIAK_GETTER_BOOL(riak_bucket_properties_ce, "basicQuorum")
}

PHP_METHOD(RiakBucketProperties, setBasicQuorum)
{
    RIAK_SETTER_BOOL(riak_bucket_properties_ce, "basicQuorum")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getNotFoundOk)
{
    RIAK_GETTER_BOOL(riak_bucket_properties_ce, "notFoundOk")
}

PHP_METHOD(RiakBucketProperties, setNotFoundOk)
{
    RIAK_SETTER_BOOL(riak_bucket_properties_ce, "notFoundOk")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getSearchEnabled)
{
    RIAK_GETTER_BOOL(riak_bucket_properties_ce, "searchEnabled")
}

PHP_METHOD(RiakBucketProperties, setSearchEnabled)
{
    RIAK_SETTER_BOOL(riak_bucket_properties_ce, "searchEnabled")
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getBackend)
{
    RIAK_GETTER_STRING(riak_bucket_properties_ce, "backend")
}

PHP_METHOD(RiakBucketProperties, setBackend)
{
    RIAK_SETTER_STRING(riak_bucket_properties_ce, "backend")
    RIAK_RETURN_THIS
}


PHP_METHOD(RiakBucketProperties, getPreCommitHookList)
{
    RIAK_GETTER_OBJECT(riak_bucket_properties_ce, "preCommitHooks")
}

PHP_METHOD(RiakBucketProperties, setPreCommitHookList)
{
    zval* zhooks;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zhooks, riak_commit_hook_list_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "", 501 TSRMLS_CC);
        return;
    }
    zend_update_property(riak_bucket_properties_ce, getThis(), "preCommitHooks", sizeof("preCommitHooks")-1, zhooks TSRMLS_CC);
    RIAK_RETURN_THIS
}


PHP_METHOD(RiakBucketProperties, getPostCommitHookList)
{
    RIAK_GETTER_OBJECT(riak_bucket_properties_ce, "postCommitHooks")
}

PHP_METHOD(RiakBucketProperties, setPostCommitHookList)
{
    zval* zhooks;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zhooks, riak_commit_hook_list_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "", 501 TSRMLS_CC);
        return;
    }
    zend_update_property(riak_bucket_properties_ce, getThis(), "postCommitHooks", sizeof("postCommitHooks")-1, zhooks TSRMLS_CC);
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getCHashKeyFun)
{
    RIAK_GETTER_OBJECT(riak_bucket_properties_ce, "chashKeyFun")
}

PHP_METHOD(RiakBucketProperties, setCHashKeyFun)
{
    zval* zhooks;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zhooks, riak_module_function_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "", 501 TSRMLS_CC);
        return;
    }
    zend_update_property(riak_bucket_properties_ce, getThis(), "chashKeyFun", sizeof("chashKeyFun")-1, zhooks TSRMLS_CC);
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getLinkFun)
{
    RIAK_GETTER_OBJECT(riak_bucket_properties_ce, "linkFun")
}

PHP_METHOD(RiakBucketProperties, setLinkFun)
{
    zval* zhooks;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zhooks, riak_module_function_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "", 501 TSRMLS_CC);
        return;
    }
    zend_update_property(riak_bucket_properties_ce, getThis(), "linkFun", sizeof("linkFun")-1, zhooks TSRMLS_CC);
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, setReplicationMode)
{
    zval* zmode;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zmode, riak_replication_mode_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "", 501 TSRMLS_CC);
        return;
    }
    zend_update_property(riak_bucket_properties_ce, getThis(), "replicationMode", sizeof("replicationMode")-1, zmode TSRMLS_CC);
    RIAK_RETURN_THIS
}

PHP_METHOD(RiakBucketProperties, getReplicationMode)
{
    RIAK_GETTER_OBJECT(riak_bucket_properties_ce, "replicationMode")
}

