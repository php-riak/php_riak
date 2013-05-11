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
#include <php.h>
#include <riack.h>
#include "object.h"
#include "php_riak.h"

zend_class_entry *riak_object_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_object_ctor, 0, ZEND_RETURN_VALUE, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

static zend_function_entry riak_object_methods[] = {
	PHP_ME(RiakObject, __construct, arginfo_object_ctor, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	{NULL, NULL, NULL}
};

void riak_object_init(TSRMLS_D)
{
	zend_class_entry ce;
	

	INIT_CLASS_ENTRY(ce, "RiakObject", riak_object_methods);
	riak_object_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(riak_object_ce, "key", sizeof("key")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "vclock", sizeof("vclock")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "contentEncoding", sizeof("contentEncoding")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "charset", sizeof("charset")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_string(riak_object_ce, "contentType", sizeof("contentType")-1, "text/plain", ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "data", sizeof("data")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "isDeleted", sizeof("isDeleted")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "lastModified", sizeof("lastModified")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_property_null(riak_object_ce, "lastModifiedUSecs", sizeof("lastModifiedUSecs")-1, ZEND_ACC_PUBLIC TSRMLS_CC);

	zend_declare_property_null(riak_object_ce, "metadata", sizeof("metadata")-1, ZEND_ACC_PUBLIC TSRMLS_CC);
}

/////////////////////////////////////////////////////////////

PHP_METHOD(RiakObject, __construct)
{
	char *key;
	int keyLen;
	zval *zArrMeta;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &key, &keyLen) == FAILURE) {
		return;
	}
	zend_update_property_stringl(riak_object_ce, getThis(), "key", sizeof("key")-1, key, keyLen TSRMLS_CC);

	// Create empty array for metadata
	MAKE_STD_ZVAL(zArrMeta);
	array_init(zArrMeta);
	add_property_zval_ex(getThis(), "metadata", sizeof("metadata")-1, zArrMeta TSRMLS_CC);
	zval_ptr_dtor(&zArrMeta);
}

/////////////////////////////////////////////////////////////

zval *metadata_from_content(struct RIACK_CONTENT* content TSRMLS_DC) 
{
	zval *zArrMeta;
	size_t i;
	struct RIACK_PAIR* currentMeta;
	char* currentKey;
	MAKE_STD_ZVAL(zArrMeta);
	array_init(zArrMeta);
	for (i=0; i<content->usermeta_count; ++i) {
		currentMeta = &(content->usermetas[i]);
		if (currentMeta) {
			currentKey = pestrndup(currentMeta->key.value, currentMeta->key.len, 0);
			if (currentMeta->value_present) {
				add_assoc_stringl(zArrMeta, currentKey, (char*)currentMeta->value, currentMeta->value_len, 1);
			} else {
				add_assoc_null(zArrMeta, currentKey);
			}
			pefree(currentKey, 0);
		}
	}
	return zArrMeta;
}

void set_object_from_riak_content(zval* object, struct RIACK_CONTENT* content TSRMLS_DC)
{
	zval* zMetadata;
	zend_update_property_stringl(riak_object_ce, object, "data", sizeof("data")-1, 
		(const char*)content->data, content->data_len TSRMLS_CC);
	zend_update_property_stringl(riak_object_ce, object, "contentEncoding", sizeof("contentEncoding")-1, 
		(const char*)content->content_encoding.value, content->content_encoding.len TSRMLS_CC);
	zend_update_property_stringl(riak_object_ce, object, "contentType", sizeof("contentType")-1, 
		(const char*)content->content_type.value, content->content_type.len TSRMLS_CC);
	
	zend_update_property_stringl(riak_object_ce, object, "charset", sizeof("charset")-1, content->charset.value, content->charset.len TSRMLS_CC);

	if (content->deleted_present) {
		zend_update_property_bool(riak_object_ce, object, "isDeleted", sizeof("isDeleted")-1, content->deleted TSRMLS_CC);
	} else {
		zend_update_property_null(riak_object_ce, object, "isDeleted", sizeof("isDeleted")-1 TSRMLS_CC);
	}
	if (content->last_modified_present) {
		zend_update_property_long(riak_object_ce, object, "lastModified", sizeof("lastModified")-1, content->last_modified TSRMLS_CC);
	} else {
		zend_update_property_null(riak_object_ce, object, "lastModified", sizeof("lastModified")-1 TSRMLS_CC);
	}
	if (content->last_modified_usecs_present) {
		zend_update_property_long(riak_object_ce, object, "lastModifiedUSecs", sizeof("lastModifiedUSecs")-1, content->last_modified_usecs TSRMLS_CC);
	} else {
		zend_update_property_null(riak_object_ce, object, "lastModifiedUSecs", sizeof("lastModifiedUSecs")-1 TSRMLS_CC);
	}
	zMetadata = metadata_from_content(content TSRMLS_CC);
	zend_update_property(riak_object_ce, object, "metadata", sizeof("metadata")-1, zMetadata TSRMLS_CC);
	zval_ptr_dtor(&zMetadata);
}

void set_metadata_from_object(struct RIACK_CONTENT* content, zval* zMetadata, struct RIACK_CLIENT* client TSRMLS_DC) 
{
	HashTable *hindex;
	HashPosition pointer;
	RIACK_STRING rKey;
	char *key;
	uint key_len, key_type, metaindex;
	ulong index;
	zval **data, *tmp, datacpy;

	if (zMetadata && Z_TYPE_P(zMetadata) == IS_ARRAY) {
		hindex = Z_ARRVAL_P(zMetadata);
		content->usermeta_count = zend_hash_num_elements(hindex);
		if (content->usermeta_count > 0) {
			metaindex = 0;
			content->usermetas = RMALLOC(client, sizeof(struct RIACK_PAIR) * content->usermeta_count);
			memset(content->usermetas, 0, sizeof(struct RIACK_PAIR) * content->usermeta_count);
			for(zend_hash_internal_pointer_reset_ex(hindex, &pointer);
				zend_hash_get_current_data_ex(hindex, (void**)&data, &pointer) == SUCCESS;
				zend_hash_move_forward_ex(hindex, &pointer)) {

				key_type = zend_hash_get_current_key_ex(hindex, &key, &key_len, &index, 0, &pointer);
				switch (key_type) {
				case HASH_KEY_IS_STRING:
					rKey.value = key;
					rKey.len = key_len;
					content->usermetas[metaindex].key = riack_copy_string(client, rKey);
					break;
				case HASH_KEY_IS_LONG:
					MAKE_STD_ZVAL(tmp);
					ZVAL_LONG(tmp, index);
					convert_to_string(tmp);
					rKey.value = Z_STRVAL_P(tmp);
					rKey.len = Z_STRLEN_P(tmp);
					content->usermetas[metaindex].key = riack_copy_string(client, rKey);
					zval_ptr_dtor(&tmp);
					break;
				}
				ALLOC_ZVAL(tmp);
				*tmp = **data;
				INIT_PZVAL(tmp);
				zval_copy_ctor(tmp);
				if (Z_TYPE_P(tmp) != IS_NULL) {
					convert_to_string(tmp);
					content->usermetas[metaindex].value_present = 1;
					RMALLOCCOPY(client, content->usermetas[metaindex].value, content->usermetas[metaindex].value_len, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
				}
				zval_ptr_dtor(&tmp);
				metaindex++;
			}
		}
	}
}

void set_riak_content_from_object(struct RIACK_CONTENT* content, zval* object, struct RIACK_CLIENT* client TSRMLS_DC)
{
	zval* zTmp;
	zTmp = zend_read_property(riak_object_ce, object, "data", sizeof("data")-1, 1 TSRMLS_CC);
	if (Z_TYPE_P(zTmp) == IS_STRING) {
		content->data_len = Z_STRLEN_P(zTmp);
		content->data = (uint8_t*)Z_STRVAL_P(zTmp);
	}
	// Set content type
	HASH_GET_INTO_RIACK_STRING_OR_ELSE(riak_object_ce, object, "contentType", zTmp, content->content_type) ;
	// Set content encoding
	HASH_GET_INTO_RIACK_STRING_OR_ELSE(riak_object_ce, object, "contentEncoding", zTmp, content->content_encoding) ;

	zTmp = zend_read_property(riak_object_ce, object, "metadata", sizeof("metadata")-1, 1 TSRMLS_CC);
	set_metadata_from_object(content, zTmp, client TSRMLS_CC);
}
