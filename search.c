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
#include "client.h"
#include "search.h"

zend_class_entry *riak_search_ce;
zend_class_entry *riak_search_input_ce;
zend_class_entry *riak_search_doc_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, client)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_search_search, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, index)
    ZEND_ARG_INFO(0, query)
    ZEND_ARG_INFO(0, parameters)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setrowlimit, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, rowlimit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setstart, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, start)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setsort, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, sort)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_search_methods[] = {
    PHP_ME(Riak_Search, __construct, arginfo_search_ctor, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search, search, arginfo_search_search, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

static zend_function_entry riak_search_input_methods[] = {
    PHP_ME(Riak_Search_Input, setRowLimit, arginfo_search_input_setrowlimit, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getRowLimit, arginfo_search_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setStartOffset, arginfo_search_input_setstart, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getStartOffset, arginfo_search_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setSort, arginfo_search_input_setsort, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getSort, arginfo_search_noargs, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

static zend_function_entry riak_search_doc_methods[] = {
    {NULL, NULL, NULL}
};

void riak_search_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Search", "Search", riak_search_methods);
    riak_search_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_search_ce, "client", sizeof("client")-1, ZEND_ACC_PROTECTED TSRMLS_CC);


    INIT_NS_CLASS_ENTRY(ce, "Riak\\Search", "Input", riak_search_input_methods);
    riak_search_input_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "rowLimit", sizeof("rowLimit")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "start", sizeof("start")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "sort", sizeof("sort")-1, ZEND_ACC_PROTECTED TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Search", "Document", riak_search_doc_methods);
    riak_search_doc_ce = zend_register_internal_class(&ce TSRMLS_CC);
}
/* }}} */


/*************************************************************
* Implementation: Riak\Search\Search
*************************************************************/


/* {{{ proto void Riak\Search->__construct(RiakClient $client)
Create a new Search object */
PHP_METHOD(Riak_Search, __construct)
{
    zval *zclient;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zclient) == FAILURE) {
        return;
    }
    zend_update_property(riak_search_ce, getThis(), "client", sizeof("client")-1, zclient TSRMLS_CC);
}
/* }}} */

/* {{{ proto Riak\Search\Document[] Riak\Search->search(string $index, string $query[, Riak\Search\Input] $parameters)
Create a new Search object */
PHP_METHOD(Riak_Search, search)
{
    riak_connection *connection;
    struct RIACK_SEARCH_OPTIONAL_PARAMETERS search_params;
    struct RIACK_SEARCH_RESULT search_result;
    RIACK_STRING rsquery, rsindex;
    char* index, *query;
    int index_len, query_len, riackstatus;
    zval *zclient, *zparams = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|o", &index, &index_len, &query, &query_len, &zparams) == FAILURE) {
        return;
    }
    zclient = zend_read_property(riak_search_ce, getThis(), "client", sizeof("client")-1, 1 TSRMLS_CC);
    GET_RIAK_CONNECTION(zclient, connection);
    memset(&search_params, 0, sizeof(struct RIACK_SEARCH_OPTIONAL_PARAMETERS));
    riak_search_set_optional_params(zparams, &search_params TSRMLS_CC);
    rsquery.value = query;
    rsquery.len = query_len;
    rsindex.value = index;
    rsindex.len = index_len;
    riackstatus = riack_search(connection->client, rsquery, rsindex, &search_params, &search_result);
    CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackstatus);
    // TODO Create and return an array of documents
    riack_free_search_result(connection->client, &search_result);
}
/* }}} */


/*************************************************************
* Implementation: Riak\Search\Input
*************************************************************/

/* {{{ proto void Riak\Search\Input->setRowLimit(int $rowLimit)
Set row limit for search */
PHP_METHOD(Riak_Search_Input, setRowLimit)
{
    long rowlimit;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &rowlimit) == FAILURE) {
        return;
    }
    zend_update_property_long(riak_search_input_ce, getThis(), "rowLimit", sizeof("rowLimit")-1, rowlimit TSRMLS_CC);
}
/* }}} */

/* {{{ proto int|null Riak\Search\Input->getRowLimit()
Set row limit for search */
PHP_METHOD(Riak_Search_Input, getRowLimit)
{
    zval* zlimit = zend_read_property(riak_search_input_ce, getThis(), "rowLimit", sizeof("rowLimit")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zlimit) == IS_LONG) {
        RETURN_LONG(Z_LVAL_P(zlimit));
    }
    RETURN_NULL();
}
/* }}} */

/* {{{ proto void Riak\Search\Input->setStartOffset(int $startOffset)
Set start offset for this search */
PHP_METHOD(Riak_Search_Input, setStartOffset)
{
    long start;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &start) == FAILURE) {
        return;
    }
    zend_update_property_long(riak_search_input_ce, getThis(), "start", sizeof("start")-1, start TSRMLS_CC);
}
/* }}} */

/* {{{ proto int|null Riak\Search\Input->getStartOffset()
Get start offset */
PHP_METHOD(Riak_Search_Input, getStartOffset)
{
    zval* zstart = zend_read_property(riak_search_input_ce, getThis(), "start", sizeof("start")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zstart) == IS_LONG) {
        RETURN_LONG(Z_LVAL_P(zstart));
    }
    RETURN_NULL();
}
/* }}} */

/* {{{ proto void Riak\Search\Input->setSort(string $sort)
Set the sort string */
PHP_METHOD(Riak_Search_Input, setSort)
{
    char* sort;
    int sort_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &sort, &sort_len) == FAILURE) {
        return;
    }
    zend_update_property_stringl(riak_search_input_ce, getThis(), "sort", sizeof("sort")-1, sort, sort_len TSRMLS_CC);
}
/* }}} */

/* {{{ proto int|null Riak\Search\Input->getSort()
Get the sort string */
PHP_METHOD(Riak_Search_Input, getSort)
{
    zval* zstart = zend_read_property(riak_search_input_ce, getThis(), "sort", sizeof("sort")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zstart) == IS_STRING) {
        RETURN_ZVAL(zstart, 1, 0);
    }
    RETURN_NULL();
}
/* }}} */

/*************************************************************
* Helpers: Riak\Search\
*************************************************************/

void riak_search_set_optional_params(zval* zparams, struct RIACK_SEARCH_OPTIONAL_PARAMETERS* search_params TSRMLS_DC)
{
    zval *zprop;
    if (!zparams || Z_TYPE_P(zparams) == IS_NULL) {
        return;
    }
    zprop = zend_read_property(riak_search_input_ce, zparams, "rowLimit", sizeof("rowLimit")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zprop) == IS_LONG) {
        search_params->rowlimit_present = 1;
        search_params->rowlimit = Z_LVAL_P(zprop);
    }
    zprop = zend_read_property(riak_search_input_ce, zparams, "start", sizeof("start")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zprop) == IS_LONG) {
        search_params->start_present = 1;
        search_params->start = Z_LVAL_P(zprop);
    }
    zprop = zend_read_property(riak_search_input_ce, zparams, "sort", sizeof("sort")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zprop) == IS_STRING) {
        search_params->sort_present = 1;
        search_params->sort.len = Z_STRLEN_P(zprop);
        search_params->sort.value = Z_STRVAL_P(zprop);
    }
    // TODO Continue with rest of the properties
    /*
    uint8_t filter_present;
    RIACK_STRING filter;
    uint8_t default_field_present;
    RIACK_STRING default_field;
    uint8_t default_operation_present;
    RIACK_STRING default_operation;
    uint8_t presort_present;
    RIACK_STRING presort;
    size_t field_limits_count;
    RIACK_STRING *field_limits;
*/
}
