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
zend_class_entry *riak_search_output_ce;
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setfilter, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, sort)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setdefaultfield, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, defaultfield)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setdefaultop, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, defaultop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setpresort, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, presort)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_search_input_setfieldlimit, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, fieldlimit)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_search_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_search_methods[] = {
    PHP_ME(Riak_Search, __construct, arginfo_search_ctor, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search, search, arginfo_search_search, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};


static zend_function_entry riak_search_output_methods[] = {
    {NULL, NULL, NULL}
};


static zend_function_entry riak_search_input_methods[] = {
    PHP_ME(Riak_Search_Input, setRowLimit, arginfo_search_input_setrowlimit, ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getRowLimit, arginfo_search_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setStartOffset, arginfo_search_input_setstart, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getStartOffset, arginfo_search_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setSort, arginfo_search_input_setsort, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getSort, arginfo_search_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setFilter, arginfo_search_input_setfilter, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getFilter, arginfo_search_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setDefaultField, arginfo_search_input_setdefaultfield, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getDefaultField, arginfo_search_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setDefaultOperation, arginfo_search_input_setdefaultop, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getDefaultOperation, arginfo_search_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setPresort, arginfo_search_input_setpresort, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getPresort, arginfo_search_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, setFieldLimits, arginfo_search_input_setfieldlimit, ZEND_ACC_PUBLIC)
    PHP_ME(Riak_Search_Input, getFieldLimits, arginfo_search_noargs, ZEND_ACC_PUBLIC)
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


    INIT_NS_CLASS_ENTRY(ce, "Riak\\Search\\Input", "Parameters", riak_search_input_methods);
    riak_search_input_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "rowLimit", sizeof("rowLimit")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "start", sizeof("start")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "sort", sizeof("sort")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "filter", sizeof("filter")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "defaultField", sizeof("defaultField")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "defaultOperation", sizeof("defaultOperation")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "presort", sizeof("presort")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_search_input_ce, "fieldLimits", sizeof("fieldLimits")-1, ZEND_ACC_PROTECTED TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Search\\Output", "Output", riak_search_output_methods);
    riak_search_output_ce = zend_register_internal_class(&ce TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Search\\Output", "Document", riak_search_doc_methods);
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
    zclient = zend_read_property(riak_search_ce, getThis(), "client", sizeof("client")-1, 1 TSRMLS_CC);
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
    MAKE_STD_ZVAL(zresult);
    array_init(zresult);
    if (search_result.document_count > 0) {
        //
    }
    // TODO Create and return an array of documents
    //add_next_index_stringl(zresult, resultlist.strings[i].value, resultlist.strings[i].len, 1);
    riack_free_search_result(connection->client, &search_result);
    RETURN_ZVAL(zresult, 0, 1);
}
/* }}} */

/*************************************************************
* Implementation: Riak\Search\Output\Output
*************************************************************/


/*************************************************************
* Implementation: Riak\Search\Input\Parameters
*************************************************************/

#define RIAK_SEARCH_INPUT_SETTER_LONG(PROPERTY_NAME) \
    long val; \
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) { return; } \
    zend_update_property_long(riak_search_input_ce, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, val TSRMLS_CC);

#define RIAK_SEARCH_INPUT_GETTER_LONG(PROPERTY_NAME) \
    zval* ztmp = zend_read_property(riak_search_input_ce, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, 1 TSRMLS_CC); \
    if (Z_TYPE_P(ztmp) == IS_LONG) { RETURN_LONG(Z_LVAL_P(ztmp)); } \
    RETURN_NULL();

#define RIAK_SEARCH_INPUT_SETTER_STRING(PROPERTY_NAME) \
    char* val; int val_len; \
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &val, &val_len) == FAILURE) { return; } \
    zend_update_property_stringl(riak_search_input_ce, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, val, val_len TSRMLS_CC);

#define RIAK_SEARCH_INPUT_GETTER_STRING(PROPERTY_NAME) \
    zval* ztmp = zend_read_property(riak_search_input_ce, getThis(), PROPERTY_NAME, sizeof(PROPERTY_NAME)-1, 1 TSRMLS_CC); \
    if (Z_TYPE_P(ztmp) == IS_STRING) { RETURN_ZVAL(ztmp, 1, 0); } \
    RETURN_NULL();

/* {{{ proto void Riak\Search\Input\Parameters->setRowLimit(int $rowLimit)
Set row limit for search */
PHP_METHOD(Riak_Search_Input, setRowLimit)
{
    RIAK_SEARCH_INPUT_SETTER_LONG("rowLimit")
}
/* }}} */

/* {{{ proto int|null Riak\Search\Input\Parameters->getRowLimit()
Set row limit for search */
PHP_METHOD(Riak_Search_Input, getRowLimit)
{
    RIAK_SEARCH_INPUT_GETTER_LONG("rowLimit");
}
/* }}} */

/* {{{ proto void Riak\Search\Input\Parameters->setStartOffset(int $startOffset)
Set start offset for this search */
PHP_METHOD(Riak_Search_Input, setStartOffset)
{
    RIAK_SEARCH_INPUT_SETTER_LONG("start")
}
/* }}} */

/* {{{ proto int|null Riak\Search\Input\Parameters->getStartOffset()
Get start offset */
PHP_METHOD(Riak_Search_Input, getStartOffset)
{
    RIAK_SEARCH_INPUT_GETTER_LONG("start");
}
/* }}} */

/* {{{ proto void Riak\Search\Input\Parameters->setSort(string $sort)
Set the sort string */
PHP_METHOD(Riak_Search_Input, setSort)
{
    RIAK_SEARCH_INPUT_SETTER_STRING("sort");
}
/* }}} */

/* {{{ proto string|null Riak\Search\Input\Parameters->getSort()
Get the sort string */
PHP_METHOD(Riak_Search_Input, getSort)
{
    RIAK_SEARCH_INPUT_GETTER_STRING("sort");
}
/* }}} */

/* {{{ proto void Riak\Search\Input\Parameters->setFilter(string $filter)
Set the filter string */
PHP_METHOD(Riak_Search_Input, setFilter)
{
    RIAK_SEARCH_INPUT_SETTER_STRING("filter");
}
/* }}} */

/* {{{ proto string|null Riak\Search\Input\Parameters->getFilter()
Get the filter string */
PHP_METHOD(Riak_Search_Input, getFilter)
{
    RIAK_SEARCH_INPUT_GETTER_STRING("filter");
}
/* }}} */

/* {{{ proto void Riak\Search\Input\Parameters->setDefaultField(string $defaultField)
Set the default field name */
PHP_METHOD(Riak_Search_Input, setDefaultField)
{
    RIAK_SEARCH_INPUT_SETTER_STRING("defaultField");
}
/* }}} */

/* {{{ proto string|null Riak\Search\Input\Parameters->getDefaultField()
Get the default field name */
PHP_METHOD(Riak_Search_Input, getDefaultField)
{
    RIAK_SEARCH_INPUT_GETTER_STRING("defaultField");
}
/* }}} */

/* {{{ proto void Riak\Search\Input\Parameters->setDefaultOperation(string $defaultOperation)
Set the default opereration */
PHP_METHOD(Riak_Search_Input, setDefaultOperation)
{
    RIAK_SEARCH_INPUT_SETTER_STRING("defaultOperation");
}
/* }}} */

/* {{{ proto string|null Riak\Search\Input\Parameters->getDefaultOperation()
Get the default operation */
PHP_METHOD(Riak_Search_Input, getDefaultOperation)
{
    RIAK_SEARCH_INPUT_GETTER_STRING("defaultOperation");
}
/* }}} */

/* {{{ proto void Riak\Search\Input\Parameters->setPresort(string $presort)
Set the presort string */
PHP_METHOD(Riak_Search_Input, setPresort)
{
    RIAK_SEARCH_INPUT_SETTER_STRING("presort");
}
/* }}} */

/* {{{ proto string|null Riak\Search\Input\Parameters->getPresort()
Get the presort string */
PHP_METHOD(Riak_Search_Input, getPresort)
{
    RIAK_SEARCH_INPUT_GETTER_STRING("presort");
}
/* }}} */

/* {{{ proto void Riak\Search\Input\Parameters->setFieldLimits(array $fieldLimits)
Set field limits */
PHP_METHOD(Riak_Search_Input, setFieldLimits)
{
    zval* zlimits;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &zlimits) == FAILURE) {
        return;
    }
    zend_update_property(riak_search_input_ce, getThis(), "fieldLimits", sizeof("fieldLimits")-1, zlimits TSRMLS_CC);
}
/* }}} */

/* {{{ proto array|null Riak\Search\Input\Parameters->getFieldLimits()
Get the field limits array */
PHP_METHOD(Riak_Search_Input, getFieldLimits)
{
    zval* zlimits = zend_read_property(riak_search_input_ce, getThis(), "fieldLimits", sizeof("fieldLimits")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zlimits) == IS_ARRAY) {
        RETURN_ZVAL(zlimits, 1, 0);
    }
    RETURN_NULL();
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
