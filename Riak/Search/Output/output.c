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
#include <php.h>
#include <riack.h>
#include "php_riak.h"
#include "output.h"
#include "document_output.h"

zend_class_entry *riak_search_output_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_output_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_search_output_methods[] = {
    PHP_ME(Riak_Search_Output_Output, hasMaxScore, arginfo_search_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Output_Output, getMaxScore, arginfo_search_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Output_Output, hasNumFound, arginfo_search_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Output_Output, getNumFound, arginfo_search_output_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Output_Output, getDocuments, arginfo_search_output_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_search_output_output_init(TSRMLS_D) /* {{{  */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Search\\Output", "Output", riak_search_output_methods);
    riak_search_output_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_search_output_ce, "maxScore", sizeof("maxScore")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_search_output_ce, "numFound", sizeof("numFound")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_search_output_ce, "documents", sizeof("documents")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
}
/* }}} */

/*************************************************************
* Implementation: Riak\Search\Output\Output
*************************************************************/

zval* riak_search_output_from_riack_search_result(struct RIACK_SEARCH_RESULT *search_result TSRMLS_DC)
{
    zval *zresult;
    MAKE_STD_ZVAL(zresult);
    object_init_ex(zresult, riak_search_output_ce);
    if (search_result->document_count > 0) {
        int cnt, i;
        zval* zdocs;
        cnt = search_result->document_count;
        MAKE_STD_ZVAL(zdocs);
        array_init(zdocs);
        for (i=0; i<cnt; ++i) {
            zval* doc = riak_search_document_from_riack_document(&search_result->documents[i] TSRMLS_CC);
            add_next_index_zval(zdocs, doc);
        }
        zend_update_property(riak_search_output_ce, zresult, "documents", sizeof("documents")-1, zdocs TSRMLS_CC);
        zval_ptr_dtor(&zdocs);
    }
    if (search_result->max_score_present) {
        zend_update_property_double(riak_search_output_ce, zresult, "maxScore", sizeof("maxScore")-1, search_result->max_score TSRMLS_CC);
    }
    if (search_result->num_found_present) {
        zend_update_property_long(riak_search_output_ce, zresult, "numFound", sizeof("numFound")-1, search_result->num_found TSRMLS_CC);
    }
    return zresult;
}

PHP_METHOD(Riak_Search_Output_Output, hasMaxScore)
{
    zval *zprop = zend_read_property(riak_search_output_ce, getThis(), "maxScore", sizeof("maxScore")-1, 1 TSRMLS_CC);
    if (!zprop || Z_TYPE_P(zprop) == IS_NULL) {
        RETURN_BOOL(0);
    }
    RETURN_BOOL(1);
}

PHP_METHOD(Riak_Search_Output_Output, getMaxScore)
{
    RIAK_GETTER_DOUBLE(riak_search_output_ce, "maxScore");
}

PHP_METHOD(Riak_Search_Output_Output, hasNumFound)
{
    zval *zprop = zend_read_property(riak_search_output_ce, getThis(), "numFound", sizeof("numFound")-1, 1 TSRMLS_CC);
    if (zprop == NULL || Z_TYPE_P(zprop) == IS_NULL) {
        RETURN_BOOL(0);
    }
    RETURN_BOOL(1);
}

PHP_METHOD(Riak_Search_Output_Output, getNumFound)
{
    RIAK_GETTER_LONG(riak_search_output_ce, "numFound");
}

PHP_METHOD(Riak_Search_Output_Output, getDocuments)
{
    RIAK_GETTER_ARRAY(riak_search_output_ce, "documents");
}

