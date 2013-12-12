/*
   Copyright 2013: Kaspar Pedersen

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

#include "zend_interfaces.h"
#include "conflict_resolver.h"
#include "youngest_sibling_resolver.h"

zend_class_entry *riak_output_youngest_sibling_resolver_ce;

RIAK_OUTPUT_CONFLICT_RESOLVER_ARG_INFO_EXEC(arginfo_youngest_sibling_resolver_resolve)

static zend_function_entry riak_output_first_sibling_resolver_methods[] = {
    PHP_ME(Riak_Output_YoungestSiblingResolver, resolve, arginfo_youngest_sibling_resolver_resolve, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_output_youngest_sibling_resolver_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "YoungestSiblingResolver", riak_output_first_sibling_resolver_methods);

    riak_output_youngest_sibling_resolver_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_class_implements(riak_output_youngest_sibling_resolver_ce TSRMLS_CC, 1, riak_output_conflict_resolver_ce);
}
/* }}} */

/* {{{ proto \Riak\Object Riak\Output\GetOutput->resolve(\Riak\ObjectList $objects) */
PHP_METHOD(Riak_Output_YoungestSiblingResolver, resolve)
{
    zval *zObjects, *ziter, *zwinner;
    long winner_ts, winner_ts_us;
    zend_bool valid;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zObjects) == FAILURE) {
        return;
    }
    zwinner = NULL;
    winner_ts = winner_ts_us = 0;
    zend_call_method_with_0_params(&zObjects, NULL, NULL, "getiterator", &ziter);
    if (Z_TYPE_P(ziter) == IS_OBJECT) {
        zval zvalidname, zcurrname, zlastmodname, zlastmodusname, znextname, zdeletedname, *zvalid;
        ZVAL_STRING(&zvalidname, "valid", 0);
        ZVAL_STRING(&zcurrname, "current", 0);
        ZVAL_STRING(&znextname, "next", 0);
        ZVAL_STRING(&zlastmodname, "getLastModified", 0);
        ZVAL_STRING(&zlastmodusname, "getLastModifiedUSecs", 0);
        ZVAL_STRING(&zdeletedname, "isDeleted", 0);
        valid = 1;

        while (valid) {
            MAKE_STD_ZVAL(zvalid);
            call_user_function(NULL, &ziter, &zvalidname, zvalid, 0, NULL TSRMLS_CC);
            if (Z_TYPE_P(zvalid) == IS_BOOL && Z_BVAL_P(zvalid)) {
                zval *zobject;
                MAKE_STD_ZVAL(zobject);
                call_user_function(NULL, &ziter, &zcurrname, zobject, 0, NULL TSRMLS_CC);
                if (Z_TYPE_P(zobject) == IS_OBJECT) {
                    zval zlastmod, zlastmod_us, zdeleted;
                    long lastmod, lastmod_us;
                    lastmod = lastmod_us = 0;
                    call_user_function(NULL, &zobject, &zdeletedname, &zdeleted, 0, NULL TSRMLS_CC);

                    // Ignore deleted objects
                    if (Z_TYPE(zdeleted) != IS_BOOL || Z_BVAL(zdeletedname) == 0) {
                        call_user_function(NULL, &zobject, &zlastmodname, &zlastmod, 0, NULL TSRMLS_CC);
                        call_user_function(NULL, &zobject, &zlastmodname, &zlastmod_us, 0, NULL TSRMLS_CC);
                        if (Z_TYPE(zlastmod) == IS_LONG) {
                            lastmod = Z_LVAL(zlastmod);
                        }
                        if (Z_TYPE(zlastmod_us) == IS_LONG) {
                            lastmod_us = Z_LVAL(zlastmod_us);
                        }
                        if ((lastmod >= winner_ts) ||
                                ((lastmod == winner_ts) && (lastmod_us > winner_ts_us))) {
                            winner_ts = lastmod;
                            winner_ts_us = lastmod_us;
                            if (zwinner != NULL) {
                                // dtor old winner
                                zval_ptr_dtor(&zwinner);
                            }
                            zwinner = zobject;
                            // Addref to new winner so it wont get dtored
                            zval_addref_p(zwinner);
                        }
                    }
                }
                zval_ptr_dtor(&zobject);
                call_user_function(NULL, &ziter, &znextname, NULL, 0, NULL TSRMLS_CC);
            } else {
                valid = 0;
            }
        }
    }
    if (zwinner != NULL) {
        RETURN_ZVAL(zwinner, 0, 1);
    } else {
        RETURN_NULL();
    }
}

/* }}} */
