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
#include "Riak/connection.h"
#include "Input/parameter_bag.h"
#include "Output/output.h"
#include "search.h"

zend_class_entry *riak_search_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, client)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_search, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, index)
    ZEND_ARG_INFO(0, query)
    ZEND_ARG_INFO(0, parameters)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_search_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_search_methods[] = {
    PHP_ME(Riak_Search, __construct, arginfo_search_ctor, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search, search, arginfo_search_search, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


void riak_search_init(TSRMLS_D) /* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Search", "Search", riak_search_methods);
    riak_search_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_search_ce, "connection", sizeof("connection")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */


/*************************************************************
* Implementation: Riak\Search\Search
*************************************************************/


/* {{{ proto void Riak\Search->__construct(Riak\Connection $client)
Create a new Search object */
PHP_METHOD(Riak_Search, __construct)
{
    zval *zclient;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zclient) == FAILURE) {
        return;
    }
    zend_update_property(riak_search_ce, getThis(), "connection", sizeof("connection")-1, zclient TSRMLS_CC);
}
/* }}} */

/* {{{ proto Riak\Search\Output\Output Riak\Search->search(string $index, string $query[, Riak\Search\Input] $parameters)
Create a new Search object */
PHP_METHOD(Riak_Search, search)
{
    riak_connection *connection;
    struct RIACK_SEARCH_OPTIONAL_PARAMETERS search_params;
    struct RIACK_SEARCH_RESULT search_result;
    RIACK_STRING rsquery, rsindex;
    char* index, *query;
    int index_len, query_len, riackstatus;
    zval *zclient, *zresult, *zparams = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|o", &index, &index_len, &query, &query_len, &zparams) == FAILURE) {
        return;
    }
    zclient = zend_read_property(riak_search_ce, getThis(), "connection", sizeof("connection")-1, 1 TSRMLS_CC);
    GET_RIAK_CONNECTION(zclient, connection);
    memset(&search_params, 0, sizeof(struct RIACK_SEARCH_OPTIONAL_PARAMETERS));
    rsquery.value = query;
    rsquery.len = query_len;
    rsindex.value = index;
    rsindex.len = index_len;
    riak_search_set_optional_params(connection->client, zparams, &search_params TSRMLS_CC);
    riackstatus = riack_search(connection->client, rsquery, rsindex, &search_params, &search_result);
    riak_search_free_optional_params(connection->client, &search_params TSRMLS_CC);
    CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackstatus);
    zresult = riak_search_output_from_riack_search_result(&search_result TSRMLS_CC);
    riack_free_search_result(connection->client, &search_result);

    RETURN_ZVAL(zresult, 0, 1);
}
/* }}} */

/*************************************************************
* Helpers: Riak\Search\
*************************************************************/

#define RIAK_SEARCH_OPT_READ_PROP_AND_SET_STRING(ZPARAMS, PROPERTY_NAME, TARGET, ZTMP) \
    ZTMP = zend_read_property(riak_search_input_ce, ZPARAMS, PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, 1 TSRMLS_CC); \
    if (Z_TYPE_P(ZTMP) == IS_STRING) { \
        TARGET##_present = 1; \
        TARGET.len = Z_STRLEN_P(ZTMP); \
        TARGET.value = Z_STRVAL_P(ZTMP); }

#define RIAK_SEARCH_OPT_READ_PROP_AND_SET_LONG(ZPARAMS, PROPERTY_NAME, TARGET, ZTMP) \
    ZTMP = zend_read_property(riak_search_input_ce, ZPARAMS, PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, 1 TSRMLS_CC); \
    if (Z_TYPE_P(ZTMP) == IS_LONG) { \
        TARGET##_present = 1; \
        TARGET= Z_LVAL_P(ZTMP); }

void riak_search_free_optional_params(struct RIACK_CLIENT *client, struct RIACK_SEARCH_OPTIONAL_PARAMETERS* search_params TSRMLS_DC)
{
    size_t cnt = search_params->field_limits_count;
    if (cnt > 0) {
        RFREE(client, search_params->field_limits);
        /* The actual strings was not copied so they do not need to be freed */
    }
}

void riak_search_set_optional_params(struct RIACK_CLIENT *client, zval* zparams, struct RIACK_SEARCH_OPTIONAL_PARAMETERS* search_params TSRMLS_DC)
{
    zval *zprop;
    if (!zparams || Z_TYPE_P(zparams) == IS_NULL) {
        return;
    }
    RIAK_SEARCH_OPT_READ_PROP_AND_SET_LONG(zparams, "rowLimit", search_params->rowlimit, zprop);
    RIAK_SEARCH_OPT_READ_PROP_AND_SET_LONG(zparams, "start", search_params->start, zprop);
    RIAK_SEARCH_OPT_READ_PROP_AND_SET_STRING(zparams, "sort", search_params->sort, zprop);
    RIAK_SEARCH_OPT_READ_PROP_AND_SET_STRING(zparams, "filter", search_params->filter, zprop);
    RIAK_SEARCH_OPT_READ_PROP_AND_SET_STRING(zparams, "defaultField", search_params->default_field, zprop);
    RIAK_SEARCH_OPT_READ_PROP_AND_SET_STRING(zparams, "defaultOperation", search_params->default_operation, zprop);
    RIAK_SEARCH_OPT_READ_PROP_AND_SET_STRING(zparams, "presort", search_params->presort, zprop);
    zprop = zend_read_property(riak_search_input_ce, zparams, "fieldLimits", sizeof("fieldLimits")-1, 1 TSRMLS_CC); \
    if (Z_TYPE_P(zprop) == IS_ARRAY) {
        int i, cnt = zend_hash_num_elements(Z_ARRVAL_P(zprop));
        if (cnt > 0) {
            search_params->field_limits_count = cnt;
            search_params->field_limits = RMALLOC(client, sizeof(RIACK_STRING) * cnt);
            for (i=0; i<cnt; ++i) {
                zval **zflpp;
                /* important these string are not copied so only the array should be freed later */
                if (zend_hash_index_find(Z_ARRVAL_P(zprop), i, (void**)&zflpp) == SUCCESS) {
                    if (Z_TYPE_PP(zflpp) == IS_STRING) {
                        search_params->field_limits[i].len = Z_STRLEN_PP(zflpp);
                        search_params->field_limits[i].value = Z_STRVAL_PP(zflpp);
                    }
                }
            }
        }
    }
}
