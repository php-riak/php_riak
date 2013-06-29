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
#include "connection.h"
#include "exceptions.h"
#include "mr_result.h"
#include "mr_inputs.h"
#include "mr_phase.h"
#include "ht_utils.h"
#include "php_riak.h"
#include "ext/json/php_json.h"

zend_class_entry *riak_mapreduce_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_mapred_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, client)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_addphase, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, phase)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_setinput, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, input)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mr_run, 0, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_INFO(0, decode)
ZEND_END_ARG_INFO()

static zend_function_entry riak_mrphase_methods[] = {
    PHP_ME(RiakMapreduce, __construct, arginfo_mapred_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakMapreduce, addPhase, arginfo_addphase, ZEND_ACC_PUBLIC)
    PHP_ME(RiakMapreduce, setInput, arginfo_setinput, ZEND_ACC_PUBLIC)
    PHP_ME(RiakMapreduce, toArray, arginfo_mr_run, ZEND_ACC_PUBLIC)
    PHP_ME(RiakMapreduce, toJson, arginfo_mr_run, ZEND_ACC_PUBLIC)
    PHP_ME(RiakMapreduce, run, arginfo_mr_run, ZEND_ACC_PUBLIC)
    {NULL, NULL, NULL}
};

void riak_mapreduce_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\MapReduce", "MapReduce", riak_mrphase_methods);
    riak_mapreduce_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(riak_mapreduce_ce, "phases", sizeof("phases")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_mapreduce_ce, "client", sizeof("client")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_null(riak_mapreduce_ce, "input", sizeof("input")-1, ZEND_ACC_PRIVATE TSRMLS_CC);
}
/* }}} */


/*************************************************************
* Implementation: Riak\MapReduce\MapReduce
*************************************************************/


/* {{{ proto void Riak\MapReduce\MapReduce->__construct(RiakConnection $client)
Create a new MapReduce object */
PHP_METHOD(RiakMapreduce, __construct)
{
    zval *zclient, *zphases;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zclient) == FAILURE) {
        return;
    }
    zend_update_property(riak_mapreduce_ce, getThis(), "client", sizeof("client")-1, zclient TSRMLS_CC);
    MAKE_STD_ZVAL(zphases);
    array_init(zphases);
    zend_update_property(riak_mapreduce_ce, getThis(), "phases", sizeof("phases")-1, zphases TSRMLS_CC);
    zval_ptr_dtor(&zphases);
}
/* }}} */

/* {{{ proto Riak\MapReduce\MapReduce Riak\MapReduce\MapReduce->addPhase(Riak\MapReduce\Phase\Phase $phase)
Add a new phase to this map reduce job, atleast one phase needs to be added before a mapreduce query can succeed */
PHP_METHOD(RiakMapreduce, addPhase)
{
    zval *zphase, *zphasearr;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zphase) == FAILURE) {
        return;
    }
    zphasearr = zend_read_property(riak_mapreduce_ce, getThis(), "phases", sizeof("phases")-1, 1 TSRMLS_CC);
    zval_add_ref(&zphase);
    add_next_index_zval(zphasearr, zphase);
    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto Riak\MapReduce\MapReduce Riak\MapReduce\MapReduce->setInput(Riak\MapReduce\Input\Input $input)
Set mapreduce input, needs to be set for a mapreduce query to succeed */
PHP_METHOD(RiakMapreduce, setInput)
{
    zval *zinput;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zinput) == FAILURE) {
        return;
    }
    zend_update_property(riak_mapreduce_ce, getThis(), "input", sizeof("input")-1, zinput TSRMLS_CC);
    RIAK_RETURN_THIS
}
/* }}} */

struct riak_mr_stream_params {/* {{{ */
#ifdef ZTS
    TSRMLS_D;
#endif
    zval *zstreamer;
};
/* }}} */


void riak_mr_result_cb(struct RIACK_CLIENT* client, void* arg, struct RIACK_MAPRED_RESPONSE* response) /* {{{ */
{
    zval zfuncname, *zresponse, zret;
    struct riak_mr_stream_params *params = (struct riak_mr_stream_params*)arg;
    ZVAL_STRING(&zfuncname, "receive", 0);
#ifdef ZTS
    zresponse = riak_mrresult_from_riack_mapred(response, params->tsrm_ls);
    call_user_function(NULL, &params->zstreamer, &zfuncname, &zret, 1, &zresponse, params->tsrm_ls);
#else
    zresponse = riak_mrresult_from_riack_mapred(response);
    call_user_function(NULL, &params->zstreamer, &zfuncname, &zret, 1, &zresponse);
#endif
    zval_ptr_dtor(&zresponse);
}
/* }}} */

/* {{{ proto array Riak\MapReduce\MapReduce->run([RiakMrStreamer $streamer])
Runs the mapreduce query and returns the results as an array of Riak_MapReduce_Output_Output */
PHP_METHOD(RiakMapreduce, run)
{
    zval* zjson, *zclient, *zresult, *zstreamer;
    riak_connection *connection;
    struct RIACK_MAPRED_RESPONSE_LIST *mapresult;
    struct RIACK_MAPRED_RESPONSE_LIST *mapresult_iter;
    struct riak_mr_stream_params stream_params;
    int riackResult;

    zstreamer = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|o", &zstreamer) == FAILURE) {
        return;
    }

    MAKE_STD_ZVAL(zjson);
    RIAK_CALL_METHOD(RiakMapreduce, toJson, zjson, getThis());


    zclient = zend_read_property(riak_mapreduce_ce, getThis(), "client", sizeof("client")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zclient) == IS_OBJECT) {
        GET_RIAK_CONNECTION(zclient, connection);
        ensure_connected(connection TSRMLS_CC);

        if (zstreamer) {
            stream_params.zstreamer = zstreamer;
            #ifdef ZTS
                stream_params.tsrm_ls = TSRMLS_C;
            #endif
            riackResult = riack_map_reduce_stream(connection->client, Z_STRLEN_P(zjson), (uint8_t*)Z_STRVAL_P(zjson), APPLICATION_JSON, &riak_mr_result_cb, &stream_params);
            CHECK_RIACK_STATUS_THROW_ON_ERROR(connection, riackResult);
        } else {
            riackResult = riack_map_reduce(connection->client, Z_STRLEN_P(zjson), (uint8_t*)Z_STRVAL_P(zjson), APPLICATION_JSON, &mapresult);
            if (riackResult == RIACK_SUCCESS) {
                MAKE_STD_ZVAL(zresult);
                array_init(zresult);
                mapresult_iter = mapresult;
                while (mapresult_iter) {
                    if (mapresult_iter->response.data != NULL && mapresult_iter->response.data_size > 0) {
                        zval *add = riak_mrresult_from_riack_mapred(&mapresult_iter->response TSRMLS_CC);
                        add_next_index_zval(zresult, add);
                    }
                    mapresult_iter = mapresult_iter->next_result;
                }
                riack_free_mapred_result(connection->client, mapresult);
                RETVAL_ZVAL(zresult, 0, 1);
            } else {
                CHECK_RIACK_STATUS_THROW_ON_ERROR(connection, riackResult);
            }
        }
    }
    zval_ptr_dtor(&zjson);
}
/* }}} */

void riak_mr_to_array_cb(void* callingObj, void* custom_ptr, char* key, uint keylen, uint index, zval** data, int cnt TSRMLS_DC)/* {{{ */
{
    zval *zarr, *ztargetarr, zfuncname;
    ztargetarr = (zval*)custom_ptr;

    MAKE_STD_ZVAL(zarr);
    ZVAL_STRING(&zfuncname, "toArray", 0);
    call_user_function(NULL, data, &zfuncname, zarr, 0, NULL TSRMLS_CC);
    //call_user_function(NULL, &zfunc, &zname, zfuncarray, 0, NULL TSRMLS_CC);
    if (zarr && Z_TYPE_P(zarr) == IS_ARRAY) {
        add_next_index_zval(ztargetarr, zarr);
    }
}
/* }}} */

/* {{{ proto array Riak\MapReduce\MapReduce->toArray()
Returns the current mapreduce query as an array, this is mostly usefull when debugging failing mr queries */
PHP_METHOD(RiakMapreduce, toArray)
{
    zval *zinput, *zinputval, *zphasearr, *zarray, zfuncname;
    zval *zqueryarr;

    zinput = zend_read_property(riak_mapreduce_ce, getThis(), "input", sizeof("input")-1, 1 TSRMLS_CC);
    zphasearr = zend_read_property(riak_mapreduce_ce, getThis(), "phases", sizeof("phases")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zinput) != IS_OBJECT || Z_TYPE_P(zphasearr) != IS_ARRAY ||
            zend_hash_num_elements(Z_ARRVAL_P(zphasearr)) <= 0) {
        zend_throw_exception(riak_badarguments_exception_ce, "Missing input or phases", 5001 TSRMLS_CC);
        return;
    }

    MAKE_STD_ZVAL(zinputval);
    ZVAL_STRING(&zfuncname, "getValue", 0);
    call_user_function(NULL, &zinput, &zfuncname, zinputval, 0, NULL TSRMLS_CC);

    MAKE_STD_ZVAL(zqueryarr);
    array_init(zqueryarr);
    foreach_in_hashtable(getThis(), zqueryarr, Z_ARRVAL_P(zphasearr), &riak_mr_to_array_cb TSRMLS_CC);

    /* Build result array */
    MAKE_STD_ZVAL(zarray);
    array_init(zarray);
    add_assoc_zval_ex(zarray, "inputs", sizeof("inputs"), zinputval);
    add_assoc_zval_ex(zarray, "query", sizeof("query"), zqueryarr);

    RETURN_ZVAL(zarray, 0, 1);
}
/* }}} */

/* {{{ proto string Riak\MapReduce\MapReduce->toJson()
Returns the current mapreduce query as an json string, this is mostly usefull when debugging failing mr queries */
PHP_METHOD(RiakMapreduce, toJson)
{
    zval *zarr;
    smart_str buff;

    memset(&buff, 0, sizeof(smart_str));
    MAKE_STD_ZVAL(zarr);
    RIAK_CALL_METHOD(RiakMapreduce, toArray, zarr, getThis());

#ifdef PHP_JSON_PRETTY_PRINT
    php_json_encode(&buff, zarr, PHP_JSON_PRETTY_PRINT TSRMLS_CC);
#else
    php_json_encode(&buff, zarr, 0 TSRMLS_CC);
#endif    
    RETVAL_STRINGL(buff.c, buff.len, 1);
    smart_str_free(&buff);
    zval_ptr_dtor(&zarr);
}
/* }}} */
