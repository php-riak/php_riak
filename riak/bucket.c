/*
   Copyright 2013 Trifork A/S
   Author: Kaspar Bach Pedersen

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

#include "bucket.h"
#include "bucket_properties.h"
#include "property/commit_hook.h"
#include "property/commit_hook_list.h"
#include "property/module_function.h"
#include "property/replication_mode/replication_mode.h"
#include "input/delete_input.h"
#include "input/get_input.h"
#include "input/put_input.h"
#include "exception/exception.h"
#include "object.h"
#include "output/output.h"
#include "output/put_output.h"
#include "output/get_output.h"
#include "riak/output/conflict_resolver.h"
#include "riak/input/get_resolver_input.h"
#include "output/index_output.h"
#include "input/index_input.h"
#include "query/index_query.h"
#include "output/key_stream_output.h"
#include "crdt/counter.h"
#include <ext/spl/spl_iterators.h>
#include <ext/spl/spl_array.h>

riak_connection *get_riak_connection(zval *zbucket TSRMLS_DC);

zend_class_entry *riak_bucket_ce;

#define RIAK_REQ_PROP_SET_LONG(CLASS_ALIAS, ZV, GETTER, TARGET) \
    RIAK_CALL_METHOD(CLASS_ALIAS, GETTER, &ZV, zinput); \
    if (Z_TYPE(ZV) == IS_LONG) { TARGET##_use = 1; TARGET = Z_LVAL(ZV); }

#define RIAK_REQ_PROP_SET_BOOL(CLASS_ALIAS, ZV, GETTER, TARGET) \
    RIAK_CALL_METHOD(CLASS_ALIAS, GETTER, &ZV, zinput); \
    if (Z_TYPE(ZV) == IS_BOOL && Z_BVAL(ZV)) { TARGET##_use = TARGET = 1; }

#define RIAK_GET_STR_COPY_TO(CLASSNAME, GETTER, ZOBJ, TARGET, TARGET_LEN) { \
    zval *z_getter_res; MAKE_STD_ZVAL(z_getter_res); \
    RIAK_CALL_METHOD(CLASSNAME, GETTER, z_getter_res, ZOBJ); \
    if (Z_TYPE_P(z_getter_res) == IS_STRING && Z_STRLEN_P(z_getter_res) > 0) { \
        RMALLOCCOPY(connection->client, TARGET, TARGET_LEN, Z_STRVAL_P(z_getter_res), Z_STRLEN_P(z_getter_res)); \
    } \
    zval_ptr_dtor(&z_getter_res); }

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_ctor, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, client)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_put, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, object)
    ZEND_ARG_INFO(0, putInput)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_get, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, getInput)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_delete, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, object)
    ZEND_ARG_INFO(0, delInput)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_fetchprops, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_applyprops, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, bucket_properties)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_set_conflictresolver, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_OBJ_INFO(0, resolver, Riak\\Output\\ConflictResolver, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_indexq, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, index)
    ZEND_ARG_INFO(0, from)
    ZEND_ARG_INFO(0, to)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_indexq_ext, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, query)
    ZEND_ARG_INFO(0, input)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_streamkeys, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, keystreamer)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_counter, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_bucket_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_bucket_methods[] = {
    PHP_ME(RiakBucket, __construct, arginfo_bucket_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakBucket, put, arginfo_bucket_put, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, get, arginfo_bucket_get, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, delete, arginfo_bucket_delete, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, index, arginfo_bucket_indexq, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, indexQuery, arginfo_bucket_indexq_ext, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, counter, arginfo_bucket_counter, ZEND_ACC_PUBLIC)

    PHP_ME(RiakBucket, getPropertyList, arginfo_bucket_fetchprops, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, setPropertyList, arginfo_bucket_applyprops, ZEND_ACC_PUBLIC)

    PHP_ME(RiakBucket, getConflictResolver, arginfo_riak_bucket_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, setConflictResolver, arginfo_bucket_set_conflictresolver, ZEND_ACC_PUBLIC)

    PHP_ME(RiakBucket, getKeyStream, arginfo_bucket_streamkeys, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, getKeyList, arginfo_riak_bucket_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, getName, arginfo_riak_bucket_noargs, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, getConnection, arginfo_riak_bucket_noargs, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void riak_bucket_init(TSRMLS_D) /* {{{ */
{
	zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak", "Bucket", riak_bucket_methods);
	riak_bucket_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(riak_bucket_ce, "name", sizeof("name")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_bucket_ce, "connection", sizeof("connection")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(riak_bucket_ce, "conflictResolver", sizeof("conflictResolver")-1, ZEND_ACC_PROTECTED TSRMLS_CC);
}
/* }}} */

zval* create_bucket_object(zval* zclient, char* name, int name_len TSRMLS_DC) /* {{{ */
{
	zval *zbucket, *zname;
	MAKE_STD_ZVAL(zbucket);
	MAKE_STD_ZVAL(zname);

    ZVAL_STRINGL(zname, name, name_len, 1);

	object_init_ex(zbucket, riak_bucket_ce);
	RIAK_CALL_METHOD2(RiakBucket, __construct, zbucket, zbucket, zclient, zname);

	zval_ptr_dtor(&zname);
	return zbucket;
}
/* }}} */

/* {{{ proto void Riak\Bucket->__construct(Riak\Connection $connection, string $name)
Create a new Riak\Bucket */
PHP_METHOD(RiakBucket, __construct)
{
	char *name;
	int nameLen;
    zval* zconnection;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os", &zconnection, riak_connection_ce, &name, &nameLen) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
		return;
	}
	zend_update_property_stringl(riak_bucket_ce, getThis(), "name", sizeof("name")-1, name, nameLen TSRMLS_CC);
    zend_update_property(riak_bucket_ce, getThis(), "connection", sizeof("connection")-1, zconnection TSRMLS_CC);
}
/* }}} */

struct riak_stream_key_cb_param {/* {{{ */
#ifdef ZTS
    TSRMLS_D;
#endif
    zval *zstreamer;
};
/* }}} */

void riak_stream_key_cb(struct RIACK_CLIENT* c, void* p, RIACK_STRING key) {/* {{{ */
    zval *zkey, zret, zfuncname;

    struct riak_stream_key_cb_param *param = (struct riak_stream_key_cb_param*)p;
    MAKE_STD_ZVAL(zkey);
    ZVAL_STRINGL(zkey, key.value, key.len, 1);
    ZVAL_STRING(&zfuncname, "process", 0);
#ifdef ZTS
    call_user_function(NULL, &param->zstreamer, &zfuncname, &zret, 1, &zkey, param->tsrm_ls);
#else
    call_user_function(NULL, &param->zstreamer, &zfuncname, &zret, 1, &zkey);
#endif

    zval_ptr_dtor(&zkey);
}
/* }}} */

/* {{{ proto void Riak\Bucket->getKeyStream(Riak\Output\KeyStreamOutput streamer)
Streams all keys in the bucket */
PHP_METHOD(RiakBucket, getKeyStream)
{
    struct riak_stream_key_cb_param cb_params;
    RIACK_STRING rsbucket;
    riak_connection *connection, *stream_connection;
    zval* zstreamer;
    int riackstatus;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zstreamer, riak_key_streamer_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    // Create a new connection only for this stream
    // LibRiack can only handle one operation at the time pr connection
    connection = get_riak_connection(getThis() TSRMLS_CC);
    stream_connection = take_connection(connection->client->host, strlen(connection->client->host), connection->client->port TSRMLS_CC);
    if (!stream_connection) {
        CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(stream_connection, RIACK_ERROR_COMMUNICATION);
    }
    rsbucket = riack_name_from_bucket(getThis() TSRMLS_CC);
#ifdef ZTS
    cb_params.tsrm_ls = TSRMLS_C;
#endif
    cb_params.zstreamer = zstreamer;
    riackstatus = riack_stream_keys(stream_connection->client, rsbucket, riak_stream_key_cb, &cb_params);
    CHECK_RIACK_STATUS_THROW_ON_ERROR(stream_connection, riackstatus);
    release_connection(stream_connection TSRMLS_CC);
}
/* }}} */


/* {{{ proto string[] Riak\Bucket->getKeyList()
List all keys in the bucket */
PHP_METHOD(RiakBucket, getKeyList)
{
    struct RIACK_STRING_LINKED_LIST* resultlist, *curr;
    RIACK_STRING rsbucket;
    riak_connection *connection;
    zval* zresultarr;
    int riackstatus;
    connection = get_riak_connection(getThis() TSRMLS_CC);
    rsbucket = riack_name_from_bucket(getThis() TSRMLS_CC);
    riackstatus = riack_list_keys(connection->client, rsbucket, &resultlist);
    CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackstatus);

    MAKE_STD_ZVAL(zresultarr);
    array_init(zresultarr);
    curr = resultlist;
    while (curr) {
        add_next_index_stringl(zresultarr, curr->string.value, curr->string.len, 1);
        curr = curr->next;
    }
    riack_free_string_linked_list(connection->client, &resultlist);

    RETURN_ZVAL(zresultarr, 0, 1);
}
/* }}} */


/* {{{ proto Riak\Crdt\Counter Riak\Bucket->counter(string $key)
Creates a new counter on this bucket */
PHP_METHOD(RiakBucket, counter)
{
    zval *zcounter, *zkey;
    char *key;
    int keylen;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &keylen) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    MAKE_STD_ZVAL(zkey);
    ZVAL_STRINGL(zkey, key, keylen, 1);

    MAKE_STD_ZVAL(zcounter);
    object_init_ex(zcounter, riak_crdt_counter_ce);
    RIAK_CALL_METHOD2(Riak_Crdt_Counter, __construct, NULL, zcounter, getThis(), zkey);

    zval_ptr_dtor(&zkey);

    RETURN_ZVAL(zcounter, 0, 1);
}

/* {{{ proto array Riak\Bucket->index(string $index, string $from [, string $to])
Perform a secondary index lookup */
PHP_METHOD(RiakBucket, index)
{
    RIACK_STRING rsbucket, rsindex, rsfrom, rsto;
    RIACK_STRING_LIST resultlist;
    char *index, *from, *to;
    int indexlen, fromlen, tolen, riackstatus, i;
    riak_connection *connection;
    zval *zresult;
    tolen = 0; to = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", &index, &indexlen, &from, &fromlen, &to, &tolen) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    connection = get_riak_connection(getThis() TSRMLS_CC);
    rsbucket = riack_name_from_bucket(getThis() TSRMLS_CC);

    rsindex.len = indexlen;
    rsindex.value = index;
    rsfrom.len = fromlen;
    rsfrom.value = from;
    if (to != NULL && tolen > 0) {
        rsto.len = tolen;
        rsto.value = to;
        riackstatus = riack_2i_query_range(connection->client, rsbucket, rsindex, rsfrom, rsto, &resultlist);
    } else {
        riackstatus = riack_2i_query_exact(connection->client, rsbucket, rsindex, rsfrom, &resultlist);
    }
    CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackstatus);

    MAKE_STD_ZVAL(zresult);
    array_init(zresult);
    for (i=0; i<resultlist.string_count; ++i) {
        add_next_index_stringl(zresult, resultlist.strings[i].value, resultlist.strings[i].len, 1);
    }
    riack_free_string_list(connection->client, &resultlist);
    RETURN_ZVAL(zresult, 0, 1);
}
/* }}} */


/* {{{ proto array Riak\Bucket->indexQuery(IndexQuery $query[, IndexInput $input])
Apply given properties to this bucket */
PHP_METHOD(RiakBucket, indexQuery)
{
    riak_connection *connection;
    struct RIACK_2I_QUERY_REQ req;
    RIACK_STRING_LIST result_keys;
    RIACK_STRING continuation;
    zval *zquery, *zinput, *zname, *zisrange, *zcontinuation, *zresult;
    int riackstatus;
    zinput = zquery = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|O", &zquery, riak_index_query_ce, &zinput, riak_index_input_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }
    // TODO Validate query have exact or ranged values
    connection = get_riak_connection(getThis() TSRMLS_CC);

    memset(&req, 0, sizeof(req));
    memset(&continuation, 0, sizeof(continuation));
    memset(&result_keys, 0, sizeof(RIACK_STRING_LIST));
    req.bucket = riack_name_from_bucket(getThis() TSRMLS_CC);

    MAKE_STD_ZVAL(zname);
    RIAK_CALL_METHOD(Riak_Query_IndexQuery, getName, zname, zquery);
    req.index.len = Z_STRLEN_P(zname);
    req.index.value = Z_STRVAL_P(zname);

    zcontinuation = NULL;
    if (zinput) {
        zval *zmaxresults;
        MAKE_STD_ZVAL(zmaxresults);
        RIAK_CALL_METHOD(Riak_Input_IndexInput, getMaxResults, zmaxresults, zinput);
        if (Z_TYPE_P(zmaxresults) == IS_LONG) {
            req.max_results = Z_LVAL_P(zmaxresults);
        }
        zval_ptr_dtor(&zmaxresults);

        MAKE_STD_ZVAL(zcontinuation);
        RIAK_CALL_METHOD(Riak_Input_IndexInput, getContinuation, zcontinuation, zinput);
        if (Z_TYPE_P(zcontinuation) == IS_STRING) {
            req.continuation_token.len = Z_STRLEN_P(zcontinuation);
            req.continuation_token.value = Z_STRVAL_P(zcontinuation);
        }
    }
    MAKE_STD_ZVAL(zisrange);
    RIAK_CALL_METHOD(Riak_Query_IndexQuery, isRangeQuery, zisrange, zquery);
    if (Z_BVAL_P(zisrange)) {
        zval *zmin, *zmax;
        // TODO Call getter instead, this is cheat
        zmin = zend_read_property(riak_index_query_ce, zquery, "minValue", sizeof("minValue")-1, 1 TSRMLS_CC);
        zmax = zend_read_property(riak_index_query_ce, zquery, "maxValue", sizeof("maxValue")-1, 1 TSRMLS_CC);
        req.search_min.len = Z_STRLEN_P(zmin);
        req.search_min.value = Z_STRVAL_P(zmin);
        req.search_max.len = Z_STRLEN_P(zmax);
        req.search_max.value = Z_STRVAL_P(zmax);
        riackstatus = riack_2i_query_ext(connection->client, &req, &result_keys, &continuation);
    } else {
        zval *zexact;
        zexact = zend_read_property(riak_index_query_ce, zquery, "exactValue", sizeof("exactValue")-1, 1 TSRMLS_CC);
        req.search_exact.len = Z_STRLEN_P(zexact);
        req.search_exact.value = Z_STRVAL_P(zexact);
        riackstatus = riack_2i_query_ext(connection->client, &req, &result_keys, &continuation);
    }
    zval_ptr_dtor(&zname);
    zval_ptr_dtor(&zisrange);
    if (zcontinuation) {
        zval_ptr_dtor(&zcontinuation);
    }

    CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackstatus);
    zresult = riak_index_output_from_string_list_and_continuation(&result_keys, &continuation TSRMLS_CC);
    if (RSTR_HAS_CONTENT(continuation)) {
        RSTR_SAFE_FREE(connection->client, continuation);
    }

    RETURN_ZVAL(zresult, 0, 1);
}
/* }}} */

/* {{{ */
void riak_set_riack_commit_hook(riak_connection *connection, zval* zhook, struct RIACK_COMMIT_HOOK* hook TSRMLS_DC)
{
    zval *zjs;
    memset(hook, 0, sizeof(struct RIACK_COMMIT_HOOK));
    MAKE_STD_ZVAL(zjs);
    RIAK_CALL_METHOD(RiakCommitHook, isJavascript, zjs, zhook);
    if (Z_BVAL_P(zjs)) {
        RIAK_GET_STR_COPY_TO(RiakCommitHook, getJsName, zhook, hook->name.value, hook->name.len);
    } else {
        RIAK_GET_STR_COPY_TO(RiakCommitHook, getErlModule, zhook, hook->modfun.module.value, hook->modfun.module.len);
        RIAK_GET_STR_COPY_TO(RiakCommitHook, getErlFunction, zhook, hook->modfun.function.value, hook->modfun.function.len);
    }
    zval_ptr_dtor(&zjs);
}
/* }}} */

/* {{{ */
void riak_set_commit_hooks_properties(riak_connection *connection, zval *zhook_list,  uint8_t *has_hooks,
                                      size_t* hook_count, struct RIACK_COMMIT_HOOK** hooks TSRMLS_DC)
{
    *has_hooks = 0;
    *hook_count = 0;
    if (Z_TYPE_P(zhook_list) != IS_NULL && Z_TYPE_P(zhook_list) == IS_OBJECT) {
        zval *ziter, *zcount;
        MAKE_STD_ZVAL(zcount);
        RIAK_CALL_METHOD(RiakCommitHookList, count, zcount, zhook_list)

        MAKE_STD_ZVAL(ziter);
        object_init(ziter);
        RIAK_CALL_METHOD(RiakCommitHookList, getIterator, ziter, zhook_list)
        if (Z_TYPE_P(zcount) == IS_LONG && Z_TYPE_P(ziter) == IS_OBJECT) {
            zval zcurrname, znextname, zvalidname;
            zend_bool done;
            int i=0;
            *has_hooks = 1;
            *hook_count = Z_LVAL_P(zcount);
            *hooks = RMALLOC(connection->client, sizeof(struct RIACK_COMMIT_HOOK) * Z_LVAL_P(zcount));

            ZVAL_STRING(&zcurrname, "current", 0);
            ZVAL_STRING(&znextname, "next", 0);
            ZVAL_STRING(&zvalidname, "valid", 0);

            done = 0;
            while (!done) {
                zval* zvalid;
                MAKE_STD_ZVAL(zvalid);
                call_user_function(NULL, &ziter, &zvalidname, zvalid, 0, NULL TSRMLS_CC);
                if (Z_TYPE_P(zvalid) == IS_BOOL && Z_BVAL_P(zvalid)) {
                    zval *zhook;
                    MAKE_STD_ZVAL(zhook)
                    call_user_function(NULL, &ziter, &zcurrname, zhook, 0, NULL TSRMLS_CC);
                    if (Z_TYPE_P(zhook) == IS_OBJECT) {
                        zval zret;
                        riak_set_riack_commit_hook(connection, zhook, &((*hooks)[i++]) TSRMLS_CC);
                        call_user_function(NULL, &ziter, &znextname, &zret, 0, NULL TSRMLS_CC);
                    }
                    zval_ptr_dtor(&zhook);
                } else {
                    done = 1;
                }
                zval_ptr_dtor(&zvalid);

            }
        }
        zval_ptr_dtor(&ziter);
        zval_ptr_dtor(&zcount);
    }
}
/* }}} */

/* {{{ proto Riak\Bucket Riak\Bucket->setPropertyList(Riak\BucketProperties $properties)
Apply given properties to this bucket */
PHP_METHOD(RiakBucket, setPropertyList)
{
#define SET_PROPLIST_LONG(GETTER_NAME, PROP_NAME) MAKE_STD_ZVAL(ztmp); \
                                                  RIAK_CALL_METHOD(RiakBucketProperties, GETTER_NAME, ztmp, zprop_obj); \
                                                  if (Z_TYPE_P(ztmp) == IS_LONG) { \
                                                      properties.PROP_NAME##_use = 1; \
                                                      properties.PROP_NAME = Z_LVAL_P(ztmp); } \
                                                  zval_ptr_dtor(&ztmp);
#define SET_PROPLIST_BOOL(GETTER_NAME, PROP_NAME) MAKE_STD_ZVAL(ztmp); \
                                                  RIAK_CALL_METHOD(RiakBucketProperties, GETTER_NAME, ztmp, zprop_obj); \
                                                  if (Z_TYPE_P(ztmp) == IS_BOOL) { \
                                                      properties.PROP_NAME##_use = 1; \
                                                      properties.PROP_NAME = Z_BVAL_P(ztmp); } \
                                                  zval_ptr_dtor(&ztmp);
	riak_connection *connection;
    struct RIACK_BUCKET_PROPERTIES properties;
	RIACK_STRING bucketName;
    zval *ztmp, *zprop_obj;
	int riackResult;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zprop_obj, riak_bucket_properties_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 501 TSRMLS_CC);
		return;
	}
    memset(&properties, 0, sizeof(struct RIACK_BUCKET_PROPERTIES));

    connection = get_riak_connection(getThis() TSRMLS_CC);
    bucketName = riack_name_from_bucket(getThis() TSRMLS_CC);

    SET_PROPLIST_LONG(getNValue, n_val);
    SET_PROPLIST_BOOL(getAllowMult, allow_mult);

    // 1.4+ below
    SET_PROPLIST_BOOL(getLastWriteWins, last_write_wins);
    SET_PROPLIST_LONG(getOldVClock, old_vclock);
    SET_PROPLIST_LONG(getYoungVClock, young_vclock);
    SET_PROPLIST_LONG(getSmallVClock, small_vclock);
    SET_PROPLIST_LONG(getBigVClock, big_vclock);

    SET_PROPLIST_LONG(getPR, pr);
    SET_PROPLIST_LONG(getR, r);
    SET_PROPLIST_LONG(getW, w);
    SET_PROPLIST_LONG(getDW, dw);
    SET_PROPLIST_LONG(getPW, pw);
    SET_PROPLIST_LONG(getRW, rw);

    SET_PROPLIST_BOOL(getBasicQuorum, basic_quorum);
    SET_PROPLIST_BOOL(getNotFoundOk, notfound_ok);

    SET_PROPLIST_BOOL(getSearchEnabled, search);

    RIAK_GET_STR_COPY_TO(RiakBucketProperties, getBackend, zprop_obj, properties.backend.value, properties.backend.len);

    MAKE_STD_ZVAL(ztmp);
    RIAK_CALL_METHOD(RiakBucketProperties, getLinkFun, ztmp, zprop_obj);
    if (Z_TYPE_P(ztmp) == IS_OBJECT) {
        properties.linkfun_use = 1;
        RIAK_GET_STR_COPY_TO(RiakModuleFunction, getFunction, ztmp,
                             properties.linkfun.function.value, properties.linkfun.function.len);
        RIAK_GET_STR_COPY_TO(RiakModuleFunction, getModule, ztmp,
                             properties.linkfun.module.value, properties.linkfun.module.len);
    }
    zval_ptr_dtor(&ztmp);
    MAKE_STD_ZVAL(ztmp);
    RIAK_CALL_METHOD(RiakBucketProperties, getCHashKeyFun, ztmp, zprop_obj);
    if (Z_TYPE_P(ztmp) == IS_OBJECT) {
        properties.chash_keyfun_use = 1;
        RIAK_GET_STR_COPY_TO(RiakModuleFunction, getFunction, ztmp,
                             properties.chash_keyfun.function.value, properties.chash_keyfun.function.len);
        RIAK_GET_STR_COPY_TO(RiakModuleFunction, getModule, ztmp,
                             properties.chash_keyfun.module.value, properties.chash_keyfun.module.len);
    }
    zval_ptr_dtor(&ztmp);

    MAKE_STD_ZVAL(ztmp);
    RIAK_CALL_METHOD(RiakBucketProperties, getPreCommitHookList, ztmp, zprop_obj);
    riak_set_commit_hooks_properties(connection, ztmp, &properties.has_precommit_hooks, &properties.precommit_hook_count,
                                     &properties.precommit_hooks TSRMLS_CC);
    zval_ptr_dtor(&ztmp);
    MAKE_STD_ZVAL(ztmp);
    RIAK_CALL_METHOD(RiakBucketProperties, getPostCommitHookList, ztmp, zprop_obj);
    riak_set_commit_hooks_properties(connection, ztmp, &properties.has_postcommit_hooks, &properties.postcommit_hook_count,
                                     &properties.postcommit_hooks TSRMLS_CC);
    zval_ptr_dtor(&ztmp);

    MAKE_STD_ZVAL(ztmp);
    RIAK_CALL_METHOD(RiakBucketProperties, getReplicationMode, ztmp, zprop_obj);
    if (Z_TYPE_P(ztmp) == IS_OBJECT) {
        zend_class_entry *ce;
        ce = Z_OBJCE_P(ztmp);
        properties.replication_mode_use = 1;
        if (instanceof_function(ce, riak_replication_mode_disabled_ce TSRMLS_CC)) {
            properties.replication_mode = DISABLED;
        } else if  (instanceof_function(ce, riak_replication_mode_full_only_ce TSRMLS_CC)) {
            properties.replication_mode = FULLSYNC;
        } else if (instanceof_function(ce, riak_replication_mode_realtime_and_full_ce TSRMLS_CC)) {
            properties.replication_mode = REALTIME_AND_FULLSYNC;
        } else if (instanceof_function(ce, riak_replication_mode_realtime_only_ce TSRMLS_CC)) {
            properties.replication_mode = REALTIME;
        } else {
            properties.replication_mode_use = 0;
        }
    }
    zval_ptr_dtor(&ztmp);

    riackResult = riack_set_bucket_props_ext(connection->client, bucketName, &properties);

    RSTR_SAFE_FREE(connection->client, properties.backend);
    RSTR_SAFE_FREE(connection->client, properties.linkfun.module);
    RSTR_SAFE_FREE(connection->client, properties.linkfun.function);
    RSTR_SAFE_FREE(connection->client, properties.chash_keyfun.module);
    RSTR_SAFE_FREE(connection->client, properties.chash_keyfun.function);
    RFREE(connection->client, properties.precommit_hooks);
    RFREE(connection->client, properties.postcommit_hooks);
 	CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackResult);

    RIAK_RETURN_THIS
}
/* }}} */

zval *riak_linkfun_from_riack(struct RIACK_MODULE_FUNCTION* modfun TSRMLS_DC) {
    zval *zmodfun, *zmod, *zfunc;
    MAKE_STD_ZVAL(zmodfun);

    MAKE_STD_ZVAL(zmod);
    ZVAL_STRINGL(zmod, modfun->module.value, modfun->module.len, 1);

    MAKE_STD_ZVAL(zfunc);
    ZVAL_STRINGL(zfunc, modfun->function.value, modfun->function.len, 1);

    object_init_ex(zmodfun, riak_module_function_ce);
    RIAK_CALL_METHOD2(RiakModuleFunction, __construct, NULL, zmodfun, zmod, zfunc)

    zval_ptr_dtor(&zmod);
    zval_ptr_dtor(&zfunc);
    return zmodfun;
}

/* {{{ */
zval* zhook_list_from_riack(size_t hook_count, struct RIACK_COMMIT_HOOK* hooks TSRMLS_DC)
{
    zval *zhook_list;
    size_t i;
    MAKE_STD_ZVAL(zhook_list);
    object_init_ex(zhook_list, riak_commit_hook_list_ce);
    RIAK_CALL_METHOD(RiakCommitHookList, __construct, zhook_list, zhook_list);
    for (i=0; i<hook_count; ++i) {
        struct RIACK_COMMIT_HOOK* current_hook;
        zval* zhook, *zoffset;
        current_hook = &hooks[i];

        // Create hook
        MAKE_STD_ZVAL(zhook);
        object_init_ex(zhook, riak_commit_hook_ce);

        if (RSTR_HAS_CONTENT(current_hook->name)) {
            zval *zname;
            // We have name so this is a js function
            MAKE_STD_ZVAL(zname);
            ZVAL_STRINGL(zname, current_hook->name.value, current_hook->name.len, 1);
            RIAK_CALL_METHOD1(RiakCommitHook, __construct, zhook, zhook, zname);
            zval_ptr_dtor(&zname);
        } else {
            zval *zmod, *zfun;
            // No name present, so must be mod/fun
            MAKE_STD_ZVAL(zfun);
            MAKE_STD_ZVAL(zmod);
            ZVAL_STRINGL(zfun, current_hook->modfun.function.value, current_hook->modfun.function.len, 1);
            ZVAL_STRINGL(zmod, current_hook->modfun.module.value, current_hook->modfun.module.len, 1);
            RIAK_CALL_METHOD2(RiakCommitHook, __construct, zhook, zhook, zmod, zfun);
            zval_ptr_dtor(&zfun);
            zval_ptr_dtor(&zmod);
        }
        MAKE_STD_ZVAL(zoffset);
        ZVAL_LONG(zoffset, i);
        RIAK_CALL_METHOD2(RiakCommitHookList, offsetSet, NULL, zhook_list, zoffset, zhook);
        zval_ptr_dtor(&zoffset);
        zval_ptr_dtor(&zhook);
    }
    return zhook_list;
}/* }}} */

/* {{{ proto Riak\BucketPropertyList Riak\Bucket->getPropertyList()
Fetch and return a RiakBucketProperties object with properties for this bucket */
PHP_METHOD(RiakBucket, getPropertyList)
{
#define GET_PROP_SET_ON_LIST(ZVAL_METHOD, SETTER_NAME, PROP_NAME) if (PROP_NAME##_use) { \
                                                        MAKE_STD_ZVAL(ztmp); \
                                                        ZVAL_METHOD(ztmp, PROP_NAME); \
                                                        RIAK_CALL_METHOD1(RiakBucketProperties, SETTER_NAME, zbucket_props, zbucket_props, ztmp); \
                                                        zval_ptr_dtor(&ztmp); }
    struct RIACK_BUCKET_PROPERTIES *properties;
	riak_connection *connection;
    RIACK_STRING bucketName;
    int riackResult;
    zval *ztmp, *zbucket_props;

    connection = get_riak_connection(getThis() TSRMLS_CC);
    bucketName = riack_name_from_bucket(getThis() TSRMLS_CC);

    riackResult = riack_get_bucket_props_ext(connection->client,  bucketName, &properties);
	CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackResult);

    MAKE_STD_ZVAL(zbucket_props);
    object_init_ex(zbucket_props, riak_bucket_properties_ce);
    RIAK_CALL_METHOD(RiakBucketProperties, __construct, NULL, zbucket_props);

    GET_PROP_SET_ON_LIST(ZVAL_LONG, setNValue, properties->n_val);
    GET_PROP_SET_ON_LIST(ZVAL_BOOL, setAllowMult, properties->allow_mult);
    GET_PROP_SET_ON_LIST(ZVAL_BOOL, setLastWriteWins, properties->last_write_wins);
    GET_PROP_SET_ON_LIST(ZVAL_LONG, setOldVClock, properties->old_vclock);
    GET_PROP_SET_ON_LIST(ZVAL_LONG, setYoungVClock, properties->young_vclock);
    GET_PROP_SET_ON_LIST(ZVAL_LONG, setSmallVClock, properties->small_vclock);
    GET_PROP_SET_ON_LIST(ZVAL_LONG, setBigVClock, properties->big_vclock);

    GET_PROP_SET_ON_LIST(ZVAL_LONG, setR, properties->r);
    GET_PROP_SET_ON_LIST(ZVAL_LONG, setPR, properties->pr);
    GET_PROP_SET_ON_LIST(ZVAL_LONG, setW, properties->w);
    GET_PROP_SET_ON_LIST(ZVAL_LONG, setDW, properties->dw);
    GET_PROP_SET_ON_LIST(ZVAL_LONG, setPW, properties->pw);
    GET_PROP_SET_ON_LIST(ZVAL_LONG, setRW, properties->rw);

    GET_PROP_SET_ON_LIST(ZVAL_BOOL, setBasicQuorum, properties->basic_quorum);
    GET_PROP_SET_ON_LIST(ZVAL_BOOL, setNotFoundOk, properties->notfound_ok);
    GET_PROP_SET_ON_LIST(ZVAL_BOOL, setSearchEnabled, properties->search);
    if (RSTR_HAS_CONTENT(properties->backend)) {
        MAKE_STD_ZVAL(ztmp);
        ZVAL_STRINGL(ztmp, properties->backend.value, properties->backend.len, 1);
        RIAK_CALL_METHOD1(RiakBucketProperties, setBackend, zbucket_props, zbucket_props, ztmp);
        zval_ptr_dtor(&ztmp);
    }
    if (properties->linkfun_use) {
        ztmp = riak_linkfun_from_riack(&properties->linkfun TSRMLS_CC);
        RIAK_CALL_METHOD1(RiakBucketProperties, setLinkFun, zbucket_props, zbucket_props, ztmp);
        zval_ptr_dtor(&ztmp);
    }
    if (properties->chash_keyfun_use) {
        ztmp = riak_linkfun_from_riack(&properties->chash_keyfun TSRMLS_CC);
        RIAK_CALL_METHOD1(RiakBucketProperties, setCHashKeyFun, zbucket_props, zbucket_props, ztmp);
        zval_ptr_dtor(&ztmp);
    }
    if (properties->has_precommit_hooks) {
        zval *zhook_list;
        zhook_list = zhook_list_from_riack(properties->precommit_hook_count, properties->precommit_hooks TSRMLS_CC);
        RIAK_CALL_METHOD1(RiakBucketProperties, setPreCommitHookList, zbucket_props, zbucket_props, zhook_list);
        zval_ptr_dtor(&zhook_list);
    }
    if (properties->has_postcommit_hooks) {
        zval *zhook_list;
        zhook_list = zhook_list_from_riack(properties->postcommit_hook_count, properties->postcommit_hooks TSRMLS_CC);
        RIAK_CALL_METHOD1(RiakBucketProperties, setPostCommitHookList, zbucket_props, zbucket_props, zhook_list);
        zval_ptr_dtor(&zhook_list);
    }
    if (properties->replication_mode_use) {
        zend_class_entry *ce = NULL;
        switch (properties->replication_mode) {
        case FULLSYNC:
            ce = riak_replication_mode_full_only_ce;
            break;
        case REALTIME:
            ce = riak_replication_mode_realtime_only_ce;
            break;
        case REALTIME_AND_FULLSYNC:
            ce = riak_replication_mode_realtime_and_full_ce;
            break;
        case DISABLED:
            ce = riak_replication_mode_disabled_ce;
            break;
        default:
            break;
        }
        if (ce != NULL) {
            zval *zrepclass;
            MAKE_STD_ZVAL(zrepclass);
            object_init_ex(zrepclass, ce);
            RIAK_CALL_METHOD1(RiakBucketProperties, setReplicationMode, zbucket_props, zbucket_props, zrepclass);
            zval_ptr_dtor(&zrepclass);
        }
    }
    riack_free_bucket_properties(connection->client, &properties);
    RETURN_ZVAL(zbucket_props, 0, 1);
}
/* }}} */

/* {{{ proto void Riak\Bucket->delete(\Riak\Object|string $object [, \Riak\Input\DeleteInput $deleteInput])
Deletes given object from riak */
PHP_METHOD(RiakBucket, delete)
{
    struct RIACK_DEL_PROPERTIES props;
    riak_connection *connection;
    zval *zparam, *zinput;
    RIACK_STRING bucketName, key;
    int riackResult;

    zinput = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|o", &zparam, &zinput) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
        return;
    }

    if (Z_TYPE_P(zparam) != IS_STRING && ! instanceof_function(Z_OBJCE_P(zparam), riak_object_ce TSRMLS_CC)) {
        zend_throw_exception(riak_badarguments_exception_ce, "Argument 1 passed to Riak\\Bucket#delete() must be a string or an instance of Riak\\Object.", 5001 TSRMLS_CC);
        return;
    }

    memset(&props, 0, sizeof(props));

    // Set connection and bucket name
    connection = get_riak_connection(getThis() TSRMLS_CC);
    bucketName = riack_name_from_bucket(getThis() TSRMLS_CC);

    key.len   = 0;
    key.value = NULL;

    // Set input properties if provided
    if (zinput != NULL && Z_TYPE_P(zinput) == IS_OBJECT) {
        zval zget;
        RIAK_REQ_PROP_SET_LONG(Riak_Input_DeleteInput, zget, getR, props.r);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_DeleteInput, zget, getPR, props.pr);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_DeleteInput, zget, getRW, props.rw);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_DeleteInput, zget, getW, props.w);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_DeleteInput, zget, getDW, props.dw);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_DeleteInput, zget, getPW, props.pw);

        RIAK_CALL_METHOD(Riak_Input_DeleteInput, getVClock, &zget, zinput);
        if (Z_TYPE(zget) == IS_STRING) {
            RMALLOCCOPY(connection->client, props.vclock.clock, props.vclock.len, Z_STRVAL(zget), Z_STRLEN(zget));
        }
        zval_dtor(&zget);
    }

    if (Z_TYPE_P(zparam) == IS_OBJECT) {
        zval zKey;
        RIAK_CALL_METHOD(RiakObject, getKey, &zKey, zparam);
        if (Z_TYPE(zKey) == IS_STRING && Z_STRVAL(zKey) > 0) {
            key.len   = Z_STRLEN(zKey);
            key.value = Z_STRVAL(zKey);
        }
        zval_dtor(&zKey);

        // If vclock is not set retrieve from object
        if (!props.vclock.clock) {
            zval zObjVClock;
            RIAK_CALL_METHOD(RiakObject, getVClock, &zObjVClock, zparam);
            if (Z_TYPE(zObjVClock) == IS_STRING) {
                RMALLOCCOPY(connection->client, props.vclock.clock, props.vclock.len, Z_STRVAL(zObjVClock), Z_STRLEN(zObjVClock));
            }
            zval_dtor(&zObjVClock);
        }
    } else if (Z_TYPE_P(zparam) == IS_STRING && Z_STRVAL_P(zparam) > 0) {
        key.len   = Z_STRLEN_P(zparam);
        key.value = Z_STRVAL_P(zparam);
    }

    if (key.len == 0) {
        zend_throw_exception(riak_badarguments_exception_ce, "Unable to resolve the object key.", 5001 TSRMLS_CC);
    }
    riackResult = riack_delete(connection->client, bucketName, key, &props);
    RFREE(connection->client, props.vclock.clock);

    CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackResult);
}
/* }}} */

/* {{{ proto Riak\Output\PutOutput Riak\Bucket->put(Riak\Object $object [, \Riak\Input\PutInput $putInput])
Store a RiakObject in riak, if something goes wrong an RiakException is thrown */
PHP_METHOD(RiakBucket, put)
{
    int riackResult;
    zval *zObject, *zTmp, *zinput, *zout, zvclock;
	struct RIACK_OBJECT obj, returnedObj;
	struct RIACK_CONTENT riackContent;
	struct RIACK_PUT_PROPERTIES props;
	riak_connection *connection;
    long options;

    options = 0;
    zinput = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|O", &zObject, riak_object_ce, &zinput, riak_put_input_ce) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
		return;
	}
    connection = get_riak_connection(getThis() TSRMLS_CC);

	memset(&obj, 0, sizeof(obj));
	memset(&returnedObj, 0, sizeof(returnedObj));
	memset(&riackContent, 0, sizeof(riackContent));
    memset(&props, 0, sizeof(props));
    /* fill content */
    set_riak_content_from_object(&riackContent, zObject, connection->client TSRMLS_CC);
    ZVAL_NULL(&zvclock);
    if (zinput != NULL && Z_TYPE_P(zinput) == IS_OBJECT) {
        zval zget;
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_PutInput, zget, getReturnHead, props.return_head);
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_PutInput, zget, getReturnBody, props.return_body);
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_PutInput, zget, getIfNotModified, props.if_not_modified);
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_PutInput, zget, getIfNoneMatch, props.if_none_match);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_PutInput, zget, getW, props.w);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_PutInput, zget, getDW, props.dw);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_PutInput, zget, getPW, props.pw);
        RIAK_CALL_METHOD(Riak_Input_PutInput, getVClock, &zvclock, zinput);
    }
    if (Z_TYPE(zvclock) == IS_NULL) {
        // vclock was not set from input, try getting it from object instead
        RIAK_CALL_METHOD(RiakObject, getVClock, &zvclock, zObject);
    }
    if (Z_TYPE(zvclock) == IS_STRING) {
        RMALLOCCOPY(connection->client, obj.vclock.clock, obj.vclock.len, Z_STRVAL(zvclock), Z_STRLEN(zvclock));
        zval_dtor(&zvclock);
    }

    /* Set bucket name */
    obj.bucket = riack_name_from_bucket(getThis() TSRMLS_CC);
	obj.content_count = 1;
	obj.content = &riackContent;
    /* No ket provided on function call, get it from RiakObject */
    GET_PROPERTY_INTO_RIACK_STR_OR_ELSE(riak_object_ce, zObject, "key", zTmp, obj.key) {
        // Key is null this is ok
        obj.key.len = 0;
        obj.key.value = 0;
    }
	riackResult = riack_put(connection->client, obj, &returnedObj, &props);
    if (obj.vclock.clock) {
        RFREE(connection->client, obj.vclock.clock);
    }
	CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackResult);

    // Now make put output from the response
    MAKE_STD_ZVAL(zTmp);
    if (returnedObj.key.len > 0) {
        ZVAL_STRINGL(zTmp, (char*)returnedObj.key.value, returnedObj.key.len, 1);
    } else {
        ZVAL_NULL(zTmp);
    }
    zout = put_output_from_riack_object(&returnedObj, zTmp TSRMLS_CC);
    RETVAL_ZVAL(zout, 0, 1);

    zval_ptr_dtor(&zTmp);
    riack_free_object(connection->client, &returnedObj);
}
/* }}} */

/* {{{ proto Output\GetOutput Riak\Bucket->get(string $key [, Riak\Input\GetInput $input])
Get value from riak */
PHP_METHOD(RiakBucket, get)
{
	char *key;
    int keyLen, riackResult;
    zval *zKey, *zinput;
	struct RIACK_GET_PROPERTIES props;
	struct RIACK_GET_OBJECT getResult;
	RIACK_STRING rsBucket, rsKey;
	riak_connection *connection;
	
    zinput = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|o", &key, &keyLen, &zinput) == FAILURE) {
        zend_throw_exception(riak_badarguments_exception_ce, "Bad or missing argument", 500 TSRMLS_CC);
		return;
    }
    connection = get_riak_connection(getThis() TSRMLS_CC);

	MAKE_STD_ZVAL(zKey);
	ZVAL_STRINGL(zKey, key, keyLen, 1);
	memset(&props, 0, sizeof(props));
    memset(&getResult, 0, sizeof(getResult));
    if (zinput != NULL && Z_TYPE_P(zinput) == IS_OBJECT) {
        zval zget;
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_GetInput, zget, getReturnHead, props.head);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_GetInput, zget, getR, props.r);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_GetInput, zget, getPR, props.pr);
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_GetInput, zget, getBasicQuorum, props.basic_quorum);
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_GetInput, zget, getNotFoundOk, props.notfound_ok);
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_GetInput, zget, getReturnDeletedVClock, props.deletedvclock);
        RIAK_CALL_METHOD(Riak_Input_GetInput, getIfModifiedVClock, &zget, zinput);
        if (Z_TYPE(zget) == IS_STRING) {
            props.if_modified_use = 1;
            RMALLOCCOPY(connection->client, props.if_modified.clock, props.if_modified.len, Z_STRVAL(zget), Z_STRLEN(zget));
            zval_dtor(&zget);
        }
    }

    rsKey.len   = keyLen;
    rsKey.value = key;
    rsBucket    = riack_name_from_bucket(getThis() TSRMLS_CC);
    riackResult = riack_get(connection->client, rsBucket, rsKey, &props, &getResult);

    if (props.if_modified.clock) {
        RFREE(connection->client, props.if_modified.clock);
    }

    if (riackResult == RIACK_SUCCESS) {
        zval *zout, *zResolver;
        zout      = get_output_from_riack_get_object(&getResult, zKey TSRMLS_CC);
        zResolver = (zinput != NULL && Z_TYPE_P(zinput) == IS_OBJECT)
            ? zend_read_property(riak_get_resolver_input_ce, zinput, "conflictResolver", sizeof("conflictResolver")-1, 1 TSRMLS_CC)
            : zend_read_property(riak_bucket_ce, getThis(), "conflictResolver", sizeof("conflictResolver")-1, 1 TSRMLS_CC);

        if (Z_TYPE_P(zResolver) == IS_OBJECT) {
            zend_update_property(riak_output_ce, zout, "conflictResolver", sizeof("conflictResolver")-1, zResolver TSRMLS_CC);
        }
        zend_update_property(riak_output_ce, zout, "bucket", sizeof("bucket")-1, getThis() TSRMLS_CC);
        RETVAL_ZVAL(zout, 0, 1);
        riack_free_get_object(connection->client, &getResult);
    } else {
        connection->needs_reconnect = 1;
        // Hack warning, same as above
        if (!EG(current_execute_data)) {
            // No stack dont throw
            RETVAL_NULL();
        } else {
            riak_throw_exception(connection->client,  riackResult TSRMLS_CC);
        }
	}
    zval_ptr_dtor(&zKey);
}
/* }}} */

/* {{{ proto string Riak\Bucket->getName()
Get bucket name */
PHP_METHOD(RiakBucket, getName)
{
    RIAK_GETTER_STRING(riak_bucket_ce, "name");
}
/* }}} */

/* {{{ proto string Riak\Bucket->getConnection() */
PHP_METHOD(RiakBucket, getConnection)
{
    RIAK_GETTER_OBJECT(riak_bucket_ce, "connection");
}
/* }}} */

/* {{{ proto \Riak\Output\ConflictResolver Riak\Bucket->getConflictResolver()
 */
PHP_METHOD(RiakBucket, getConflictResolver)
{
    RIAK_GETTER_OBJECT(riak_bucket_ce, "conflictResolver");
}
/* }}} */

/* {{{ proto Riak\Bucket Riak\Object->setConflictResolver(\Riak\Output\ConflictResolver $resolver)
 */
PHP_METHOD(RiakBucket, setConflictResolver)
{
    zval* zResolver; 

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zResolver, riak_output_conflict_resolver_ce) == FAILURE) {
        return;
    }

    zend_update_property(riak_bucket_ce, getThis(), "conflictResolver", sizeof("conflictResolver")-1, zResolver TSRMLS_CC);

    RIAK_RETURN_THIS
}
/* }}} */

zval* object_from_riak_content(zval* key, struct RIACK_CONTENT* content TSRMLS_DC)/* {{{ */
{
	zval *object;
	MAKE_STD_ZVAL(object);
	object_init_ex(object, riak_object_ce);
	RIAK_CALL_METHOD1(RiakObject, __construct, object, object, key);

	set_object_from_riak_content(object, content TSRMLS_CC);

	return object;
}
/* }}} */

void riak_name_from_bucket(zval* bucket, char **name, int *namelen TSRMLS_DC)/* {{{ */
{
    zval *zname = zend_read_property(riak_bucket_ce, bucket, "name", sizeof("name")-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zname) == IS_STRING) {
        *name = Z_STRVAL_P(zname);
        *namelen = Z_STRLEN_P(zname);
    } else {
        *name = NULL;
        *namelen = 0;
    }
}
/* }}} */

RIACK_STRING riack_name_from_bucket(zval* bucket TSRMLS_DC)/* {{{ */
{
    RIACK_STRING bucketName;
    int namelen;
    riak_name_from_bucket(bucket, &bucketName.value, &namelen TSRMLS_CC);
    bucketName.len = namelen;
    return bucketName;
}
/* }}} */

riak_connection *get_riak_connection(zval *zbucket TSRMLS_DC)/* {{{ */
{
    zval *zclient;
    riak_connection *connection = NULL;
    zclient = zend_read_property(riak_bucket_ce, zbucket, "connection", sizeof("connection")-1, 1 TSRMLS_CC);
    if (zclient) {
        GET_RIAK_CONNECTION(zclient, connection);
		ensure_connected(connection TSRMLS_CC);
    }
	return connection;
}
/* }}} */
