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

#include <php.h>
#include <riack.h>
#include "php_riak.h"
#include "bucket.h"
#include "bucket_properties.h"
#include "connection.h"
#include "object.h"
#include "exceptions.h"
#include "req_inputs.h"
#include "req_outputs.h"

riak_connection *get_riak_connection(zval *zbucket TSRMLS_DC);

zend_class_entry *riak_bucket_ce;

#define RIAK_REQ_PROP_SET_LONG(CLASS_ALIAS, GETTER, TARGET) \
    RIAK_CALL_METHOD(CLASS_ALIAS, GETTER, &ztmp, zinput); \
    if (Z_TYPE(ztmp) == IS_LONG) { TARGET##_use = 1; TARGET = Z_LVAL(ztmp); }

#define RIAK_REQ_PROP_SET_BOOL(CLASS_ALIAS, GETTER, TARGET) \
    RIAK_CALL_METHOD(CLASS_ALIAS, GETTER, &ztmp, zinput); \
    if (Z_TYPE(ztmp) == IS_BOOL && Z_BVAL(ztmp)) { TARGET##_use = TARGET = 1; }

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_ctor, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, client)
    ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_put, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, object)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_get, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_delete, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_fetchprops, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_applyprops, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, bucket_properties)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_indexq, 0, ZEND_RETURN_VALUE, 2)
    ZEND_ARG_INFO(0, index)
    ZEND_ARG_INFO(0, from)
    ZEND_ARG_INFO(0, to)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bucket_streamkeys, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, keystreamer)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_riak_bucket_noargs, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry riak_bucket_methods[] = {
	PHP_ME(RiakBucket, __construct, arginfo_bucket_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(RiakBucket, put, arginfo_bucket_put, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, get, arginfo_bucket_get, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, delete, arginfo_bucket_delete, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, index, arginfo_bucket_indexq, ZEND_ACC_PUBLIC)

    PHP_ME(RiakBucket, getPropertyList, arginfo_bucket_fetchprops, ZEND_ACC_PUBLIC)
    PHP_ME(RiakBucket, setPropertyList, arginfo_bucket_applyprops, ZEND_ACC_PUBLIC)
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
    riak_connection *connection;
    zval* zstreamer;
    int riackstatus;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zstreamer) == FAILURE) {
        return;
    }
    connection = get_riak_connection(getThis() TSRMLS_CC);
    rsbucket = riack_name_from_bucket(getThis() TSRMLS_CC);
#ifdef ZTS
    cb_params.tsrm_ls = TSRMLS_C;
#endif
    cb_params.zstreamer = zstreamer;
    riackstatus = riack_stream_keys(connection->client, rsbucket, riak_stream_key_cb, &cb_params);
    CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackstatus);
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


/* {{{ proto array Riak\Bucket->index(string $index, string $from [, string $to])
Apply given properties to this bucket */
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
    /*
            RIACK_STRING backend;

            uint8_t has_precommit_hooks;
            size_t precommit_hook_count;
            struct RIACK_COMMIT_HOOK* precommit_hooks;

            uint8_t has_postcommit_hooks;
            size_t postcommit_hook_count;
            struct RIACK_COMMIT_HOOK* postcommit_hooks;

            uint8_t linkfun_use;
            struct RIACK_MODULE_FUNCTION linkfun;
            uint8_t crash_keyfun_use;
            struct RIACK_MODULE_FUNCTION crash_keyfun;
     */
    riackResult = riack_set_bucket_props_ext(connection->client, bucketName, &properties);
 	CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackResult);

    RIAK_RETURN_THIS
}
/* }}} */

/* {{{ proto Riak\BucketPropertyList Riak\Bucket->getPropertyList()
Fetch and return a RiakBucketProperties object with properties for this bucket */
PHP_METHOD(RiakBucket, getPropertyList)
{
    struct RIACK_BUCKET_PROPERTIES *properties;
	riak_connection *connection;
    RIACK_STRING bucketName;
    int riackResult;
    zval *ztmp, zdummy, *zbucket_props;

    connection = get_riak_connection(getThis() TSRMLS_CC);
    bucketName = riack_name_from_bucket(getThis() TSRMLS_CC);

    riackResult = riack_get_bucket_props_ext(connection->client,  bucketName, &properties);
	CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackResult);

    // TODO get all the properties...
    MAKE_STD_ZVAL(zbucket_props);
    object_init_ex(zbucket_props, riak_bucket_properties_ce);
    RIAK_CALL_METHOD(RiakBucketProperties, __construct, zbucket_props, zbucket_props);

    if (properties->n_val_use) {
        //classname, name, retval, thisptr, param1
        MAKE_STD_ZVAL(ztmp);
        ZVAL_LONG(ztmp, properties->n_val);
        RIAK_CALL_METHOD1(RiakBucketProperties, setNValue, &zdummy, zbucket_props, ztmp);
        zval_ptr_dtor(&ztmp);
    }
    if (properties->allow_mult_use) {
        //classname, name, retval, thisptr, param1
        MAKE_STD_ZVAL(ztmp);
        ZVAL_BOOL(ztmp, properties->allow_mult);
        RIAK_CALL_METHOD1(RiakBucketProperties, setNValue, &zdummy, zbucket_props, ztmp);
        zval_ptr_dtor(&ztmp);
    }

    riack_free_bucket_properties(connection->client, &properties);

    RETVAL_ZVAL(zbucket_props, 0, 1);

    // TODO remove below
/*
    MAKE_STD_ZVAL(zNVal);
    ZVAL_LONG(zNVal, nVal);

    MAKE_STD_ZVAL(zAllowMult);
    ZVAL_BOOL(zAllowMult, allowMult);

    MAKE_STD_ZVAL(zBucketProps);
    object_init_ex(zBucketProps, riak_bucket_properties_ce);
    RIAK_CALL_METHOD2(RiakBucketProperties, __construct, zBucketProps, zBucketProps, zNVal, zAllowMult);
    RETVAL_ZVAL(zBucketProps, 0, 1);

    zval_ptr_dtor(&zNVal);
    zval_ptr_dtor(&zAllowMult);*/
}
/* }}} */

/* {{{ proto void RiakBucket->delete(RiakObject|string $object)
Deletes given object from riak */
PHP_METHOD(RiakBucket, delete)
{
	struct RIACK_DEL_PROPERTIES props;
	riak_connection *connection;
    zval *zparam, *zTmp, *zinput;
	RIACK_STRING bucketName, key;
	int riackResult;
    zinput = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|o", &zparam, &zinput) == FAILURE) {
		return;
	}
    memset(&props, 0, sizeof(props));
    connection = get_riak_connection(getThis() TSRMLS_CC);

    /* Set bucket name */
    bucketName = riack_name_from_bucket(getThis() TSRMLS_CC);
    if (Z_TYPE_P(zparam) == IS_STRING) {
        key.len = Z_STRLEN_P(zparam);
        key.value = Z_STRVAL_P(zparam);
    } else {
        /* If zparam is not a string it should be a RiakObject, get the key from the object. */
        GET_PROPERTY_INTO_RIACK_STR_OR_ELSE(riak_object_ce, zparam, "key", zTmp, key) {
            zend_throw_exception(riak_badarguments_exception_ce, "key missing from object", 5001 TSRMLS_CC);
            return;
        }
    }

    if (zinput != NULL && Z_TYPE_P(zinput) == IS_OBJECT) {
        zval ztmp;
        RIAK_REQ_PROP_SET_LONG(Riak_Input_DeleteInput, getR, props.r);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_DeleteInput, getPR, props.pr);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_DeleteInput, getRW, props.rw);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_DeleteInput, getW, props.w);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_DeleteInput, getDW, props.dw);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_DeleteInput, getPW, props.pw);
        RIAK_CALL_METHOD(Riak_Input_DeleteInput, getVClock, &ztmp, zinput);
        if (Z_TYPE(ztmp) == IS_STRING) {
            RMALLOCCOPY(connection->client, props.vclock.clock, props.vclock.len, Z_STRVAL(ztmp), Z_STRLEN(ztmp));
            zval_dtor(&ztmp);
        }
    }
	riackResult = riack_delete(connection->client, bucketName, key, &props);
    if (props.vclock.clock) {
        RFREE(connection->client, props.vclock.clock);
    }
	CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackResult);
}
/* }}} */

/* {{{ proto Riak\Output\PutOutput Riak\Bucket->put(Riak\Object $object [, RiakPutRequestConfiguration $configuration])
Store a RiakObject in riak, if something goes wrong an RiakException is thrown */
PHP_METHOD(RiakBucket, put)
{
    int riackResult;
    zval *zObject, *zTmp, *zinput, *zout;
	struct RIACK_OBJECT obj, returnedObj;
	struct RIACK_CONTENT riackContent;
	struct RIACK_PUT_PROPERTIES props;
	riak_connection *connection;
    long options;

    options = 0;
    zinput = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|o", &zObject, &zinput) == FAILURE) {
		return;
	}
    connection = get_riak_connection(getThis() TSRMLS_CC);

	memset(&obj, 0, sizeof(obj));
	memset(&returnedObj, 0, sizeof(returnedObj));
	memset(&riackContent, 0, sizeof(riackContent));
    memset(&props, 0, sizeof(props));
    /* fill content */
    set_riak_content_from_object(&riackContent, zObject, connection->client TSRMLS_CC);

    if (zinput != NULL && Z_TYPE_P(zinput) == IS_OBJECT) {
        zval ztmp;
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_PutInput, getReturnHead, props.return_head);
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_PutInput, getReturnBody, props.return_body);
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_PutInput, getIfNotModified, props.if_not_modified);
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_PutInput, getIfNoneMatch, props.if_none_match);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_PutInput, getW, props.w);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_PutInput, getDW, props.dw);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_PutInput, getPW, props.pw);
        RIAK_CALL_METHOD(Riak_Input_PutInput, getVClock, &ztmp, zinput);
        if (Z_TYPE(ztmp) == IS_STRING) {
            RMALLOCCOPY(connection->client, obj.vclock.clock, obj.vclock.len, Z_STRVAL(ztmp), Z_STRLEN(ztmp));
            zval_dtor(&ztmp);
        }
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
	size_t contentCount;
    zval *zKey, *zinput;
	struct RIACK_GET_PROPERTIES props;
	struct RIACK_GET_OBJECT getResult;
	RIACK_STRING rsBucket, rsKey;
	riak_connection *connection;
	
    zinput = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|o", &key, &keyLen, &zinput) == FAILURE) {
		return;
    }
    connection = get_riak_connection(getThis() TSRMLS_CC);

	MAKE_STD_ZVAL(zKey);
	ZVAL_STRINGL(zKey, key, keyLen, 1);
	memset(&props, 0, sizeof(props));
    memset(&getResult, 0, sizeof(getResult));
    if (zinput != NULL && Z_TYPE_P(zinput) == IS_OBJECT) {
        zval ztmp;
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_GetInput, getReturnHead, props.head);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_GetInput, getR, props.r);
        RIAK_REQ_PROP_SET_LONG(Riak_Input_GetInput, getPR, props.pr);
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_GetInput, getBasicQuorum, props.basic_quorum);
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_GetInput, getNotFoundOk, props.notfound_ok);
        RIAK_REQ_PROP_SET_BOOL(Riak_Input_GetInput, getReturnDeletedVClock, props.deletedvclock);
        RIAK_CALL_METHOD(Riak_Input_GetInput, getIfModifiedVClock, &ztmp, zinput);
        if (Z_TYPE(ztmp) == IS_STRING) {
            props.if_modified_use = 1;
            RMALLOCCOPY(connection->client, props.if_modified.clock, props.if_modified.len, Z_STRVAL(ztmp), Z_STRLEN(ztmp));
            zval_dtor(&ztmp);
        }
    }

    rsBucket = riack_name_from_bucket(getThis() TSRMLS_CC);
	rsKey.len = keyLen;
	rsKey.value = key;
	riackResult = riack_get(connection->client, rsBucket, rsKey, &props, &getResult);
    if (props.if_modified.clock) {
        RFREE(connection->client, props.if_modified.clock);
    }
    if (riackResult == RIACK_SUCCESS) {
        contentCount = getResult.object.content_count;
        if (contentCount > 0) {
            zval *zout = get_output_from_riack_get_object(&getResult, zKey TSRMLS_CC);
            RETVAL_ZVAL(zout, 0, 1);
		} else {
            /* Throw not found exception */
            zend_throw_exception(riak_not_found_exception_ce, "Not Found", 2000 TSRMLS_CC);
        }
		riack_free_get_object(connection->client, &getResult);
	} else {
		zval_ptr_dtor(&zKey);
		CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(connection, riackResult);
	}
    zval_ptr_dtor(&zKey);
}
/* }}} */

/* {{{ proto string Riak\Bucket->getName()
Get bucket name */
PHP_METHOD(RiakBucket, getName)
{
    RIAK_GETTER_STRING(riak_object_ce, "name");
}
/* }}} */

/* {{{ proto string Riak\Bucket->getConnection() */
PHP_METHOD(RiakBucket, getConnection)
{
    RIAK_GETTER_OBJECT(riak_object_ce, "connection");
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

