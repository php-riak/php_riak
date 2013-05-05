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
#include "bucket.h"
#include "client.h"
#include "object.h"
#include "exceptions.h"
#include "php_riak.h"

zend_class_entry *riak_bucket_ce;

static zend_function_entry riak_bucket_methods[] = {
	PHP_ME(RiakBucket, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(RiakBucket, putObject, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(RiakBucket, getObject, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(RiakBucket, deleteObject, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

void riak_bucket_init(TSRMLS_D)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "RiakBucket", riak_bucket_methods);
	riak_bucket_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(riak_bucket_ce, "name", sizeof("name")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_bucket_ce, "client", sizeof("client")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_bucket_ce, "properties", sizeof("properties")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
}

/////////////////////////////////////////////////////////////

PHP_METHOD(RiakBucket, __construct)
{
	char *name;
	int nameLen;
	zval* client;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "os", &client, &name, &nameLen) == FAILURE) {
		return;
	}
	zend_update_property_stringl(riak_bucket_ce, getThis(), "name", sizeof("name")-1, name, nameLen TSRMLS_CC);
	zend_update_property(riak_bucket_ce, getThis(), "client", sizeof("client")-1, client TSRMLS_CC);
}

PHP_METHOD(RiakBucket, deleteObject)
{
	struct RIACK_DEL_PROPERTIES props;
	struct RIACK_CLIENT *client;
	zval *zObject, **zTmp;
	HashTable *zBucketProps, *zObjectProps;
	RIACK_STRING bucketName, key;
	int riackResult;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &zObject) == FAILURE) {
		return;
	}
	zBucketProps = zend_std_get_properties(getThis() TSRMLS_CC);
	zObjectProps = zend_std_get_properties(zObject TSRMLS_CC);

	if (zend_hash_find(zBucketProps, "client", sizeof("client"), (void**)&zTmp) == SUCCESS) {
		GET_RIACK_CLIENT(*zTmp, client);
	} else {
		// TODO Exception
	}
	// Set bucket name
	HASH_GET_INTO_RIACK_STRING_OR_ELSE(zBucketProps, "name", zTmp, bucketName) {
		// Handle error
	}
	// Set key
	HASH_GET_INTO_RIACK_STRING_OR_ELSE(zObjectProps, "key", zTmp, key) {
		// Handle error
	}
	memset(&props, 0, sizeof(props));
	props.dw_use = 1;
	props.dw = 2;
	riackResult = riack_delete(client, bucketName, key, &props);
	CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(client, riackResult);
}

PHP_METHOD(RiakBucket, putObject)
{
	char *key, *contentType;
	int keyLen, contentTypeLen;
	zval *zObject, **zTmp;
	HashTable *zBucketProps, *zObjectProps;
	struct RIACK_OBJECT obj, returnedObj;
	struct RIACK_CONTENT riackContent;
	struct RIACK_PUT_PROPERTIES props;
	struct RIACK_CLIENT *client;
	int riackResult;
	
	key = NULL;
	keyLen = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|s", &zObject, &key, &keyLen) == FAILURE) {
		return;
	}
	zBucketProps = zend_std_get_properties(getThis() TSRMLS_CC);
	zObjectProps = zend_std_get_properties(zObject TSRMLS_CC);

	if (zend_hash_find(zBucketProps, "client", sizeof("client"), (void**)&zTmp) == SUCCESS) {
		GET_RIACK_CLIENT(*zTmp, client);
	} else {
		// TODO Exception
	}

	memset(&obj, 0, sizeof(obj));
	memset(&returnedObj, 0, sizeof(returnedObj));
	memset(&riackContent, 0, sizeof(riackContent));
	memset(&props, 0, sizeof(props));
	//// fill content ////

	// Set content type
	HASH_GET_INTO_RIACK_STRING_OR_ELSE(zObjectProps, "contentType", zTmp, riackContent.content_type) {
	  // TODO Handle
	}
	// Set content encoding
	HASH_GET_INTO_RIACK_STRING_OR_ELSE(zObjectProps, "contentEncoding", zTmp, riackContent.content_encoding) {
		// TODO Handle
	}

	// Set data
	if (zend_hash_find(zObjectProps, "data", sizeof("data"), (void**)&zTmp) == SUCCESS) {
		if (Z_TYPE_P(*zTmp) == IS_STRING) {
			riackContent.data_len = Z_STRLEN_P(*zTmp);
			riackContent.data = (uint8_t*)Z_STRVAL_P(*zTmp);
		}
	}

	//// fill obj ////
	// Set vclock
	if (zend_hash_find(zObjectProps, "vclock", sizeof("vclock"), (void**)&zTmp) == SUCCESS) {
		if (Z_TYPE_P(*zTmp) == IS_STRING) {
			obj.vclock.len = Z_STRLEN_P(*zTmp);
			obj.vclock.clock = (uint8_t*)Z_STRVAL_P(*zTmp);
		}
	}
	// Set bucket name
	HASH_GET_INTO_RIACK_STRING_OR_ELSE(zBucketProps, "name", zTmp, obj.bucket) {
		// Handle error
	}
	obj.content_count = 1;
	obj.content = &riackContent;

	if (key) {
		// Key was passed to function use it.
		obj.key.len = keyLen;
		obj.key.value = key;
	} else {
		// No ket provided on function call, get it from RiakObject
		HASH_GET_INTO_RIACK_STRING_OR_ELSE(zObjectProps, "key", zTmp, obj.key) {
			// Handle possible errors
		}
	}

	riackResult = riack_put(client, obj, &returnedObj, &props);
	CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(client, riackResult);

	// TODO Return updated object
	riack_free_object(client, &returnedObj);
}

PHP_METHOD(RiakBucket, getObject)
{
	char *key;
	int i, keyLen, riackResult;
	size_t contentCount;
	zval *zBucket, *zKey, *zVclock, *zExc, **zTmp, *zObjArr, *zObj;
	HashTable *zBucketProps;
	struct RIACK_GET_PROPERTIES props;
	struct RIACK_GET_OBJECT getResult;
	RIACK_STRING rsBucket, rsKey;
	struct RIACK_CLIENT* client;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &keyLen) == FAILURE) {
		return;
	}
	client = get_riack_client(getThis() TSRMLS_CC);
	zBucketProps = zend_std_get_properties(getThis() TSRMLS_CC);
	MAKE_STD_ZVAL(zKey);
	ZVAL_STRINGL(zKey, key, keyLen, 1);

	memset(&props, 0, sizeof(props));
	memset(&getResult, 0, sizeof(getResult));
	HASH_GET_INTO_RIACK_STRING_OR_ELSE(zBucketProps, "name", zTmp, rsBucket) {
		// Handle error
	}
	rsKey.len = keyLen;
	rsKey.value = key;
	riackResult = riack_get(client, rsBucket, rsKey, &props, &getResult);
	if (riackResult == RIACK_SUCCESS) {
		MAKE_STD_ZVAL(zVclock);
		ZVAL_STRINGL(zVclock, (char*)getResult.object.vclock.clock, getResult.object.vclock.len, 1);
		contentCount = getResult.object.content_count;
		if (contentCount > 1) {
			MAKE_STD_ZVAL(zObjArr);
			MAKE_STD_ZVAL(zExc);
			
			array_init(zObjArr);
			while (contentCount > 0) {
				zObj = object_from_riak_content(zKey, &getResult.object.content[--contentCount] TSRMLS_CC);
				add_next_index_zval(zObjArr, zObj);
			}
			object_init_ex(zExc, riak_conflicted_object_exception_ce);
			CALL_METHOD2(RiakConflictedObjectException, __construct, zExc, zExc, zVclock, zObjArr);
			zend_throw_exception_object(zExc TSRMLS_CC);
			
			zval_ptr_dtor(&zObjArr);
		} else if (contentCount == 1) {
			// Create a new RiakObject
			zObj = object_from_riak_content(zKey, &getResult.object.content[0] TSRMLS_CC);
			// Update vclock on RiakObject
			zend_update_property(riak_object_ce, zObj, "vclock", sizeof("vclock")-1, zVclock TSRMLS_CC);
			RETVAL_ZVAL(zObj, 0, 1);
		} else {
			// Throw not found exception
			zend_throw_exception(riak_not_found_exception_ce, "Not Found", 2000 TSRMLS_CC);
		}
		zval_ptr_dtor(&zVclock);
		riack_free_get_object(client, &getResult);
	} else {
		zval_ptr_dtor(&zKey);
		CHECK_RIACK_STATUS_THROW_AND_RETURN_ON_ERROR(client, riackResult);
	}
	zval_ptr_dtor(&zKey);
}

///////////////////////////////////////////////////////////////////////////////

zval* object_from_riak_content(zval* key, struct RIACK_CONTENT* content TSRMLS_DC)
{
	zval *object;
	MAKE_STD_ZVAL(object);
	object_init_ex(object, riak_object_ce);
	CALL_METHOD1(RiakObject, __construct, object, object, key);
	set_object_from_riak_content(object, content TSRMLS_CC);
	return object;
}

struct RIACK_CLIENT* get_riack_client(zval *zbucket TSRMLS_DC)
{
	zval **zTmp;
	HashTable *zBucketProps;
	struct RIACK_CLIENT *client;
	zBucketProps = zend_std_get_properties(zbucket TSRMLS_CC);
	if (zend_hash_find(zBucketProps, "client", sizeof("client"), (void**)&zTmp) == SUCCESS) {
		GET_RIACK_CLIENT(*zTmp, client);
	} else {
		return NULL;
	}
	return client;
}
