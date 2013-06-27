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
#include "riak_session.h"

// #ifdef PHP_SESSION
#include <zend_exceptions.h>
#include "client.h"
#include "bucket.h"
#include "object.h"
#include "req_inputs.h"
#include "req_outputs.h"
#include "SAPI.h"
#include <ext/standard/php_string.h>
#include <ext/standard/url.h>
#include <main/php_variables.h>

#define SET_GET_OPTION_IF_PRESENT(NAME, CLIENT, ZARR, ZPP) \
    if (zend_hash_find(Z_ARRVAL_P(ZARR), NAME, sizeof(NAME), (void**)&ZPP) == SUCCESS) { \
        convert_to_long(*ZPP); \
        zend_update_property(riak_get_input_ce, CLIENT, NAME, sizeof(NAME)-1, *ZPP TSRMLS_CC); \
    }

#define SET_PUT_OPTION_IF_PRESENT(NAME, CLIENT, ZARR, ZPP) \
    if (zend_hash_find(Z_ARRVAL_P(ZARR), NAME, sizeof(NAME), (void**)&ZPP) == SUCCESS) { \
        convert_to_long(*ZPP); \
        zend_update_property(riak_put_input_ce, CLIENT, NAME, sizeof(NAME)-1, *ZPP TSRMLS_CC); \
    }

#define SET_DELETE_OPTION_IF_PRESENT(NAME, CLIENT, ZARR, ZPP) \
    if (zend_hash_find(Z_ARRVAL_P(ZARR), NAME, sizeof(NAME), (void**)&ZPP) == SUCCESS) { \
        convert_to_long(*ZPP); \
        zend_update_property(riak_delete_input_ce, CLIENT, NAME, sizeof(NAME)-1, *ZPP TSRMLS_CC); \
    }

ps_module ps_mod_riak = {
	PS_MOD(riak)
};

typedef struct _riak_session_data {
   char* session_name;
   zval *zbucket;
   zval *zclient;
   zval *zgetprops;
   zval *zputprops;
   zval *zdelprops;
} riak_session_data;

#define PS_RIAK_DATA riak_session_data *data = PS_GET_MOD_DATA()

PS_OPEN_FUNC(riak) /* {{{ */
{
    riak_session_data* session_data;
    zval *zclient, *zbucket, *zgetprops, *zdelprops, *zputprops, *zoptionsarray, **zoption;
    php_url *purl;
    char* stripped_path;

    purl = php_url_parse(save_path);
    if (!purl) {
        return FAILURE;
    }

    zclient = create_client_object(purl->host, purl->port TSRMLS_CC);
    if (EG(exception)) {
        php_url_free(purl);
        zval_ptr_dtor(&zclient);
        PS_SET_MOD_DATA(NULL);
        return FAILURE;
    }

    // Input options for get operations
    MAKE_STD_ZVAL(zgetprops);
    object_init_ex(zgetprops, riak_get_input_ce);

    MAKE_STD_ZVAL(zputprops);
    object_init_ex(zputprops, riak_put_input_ce);

    MAKE_STD_ZVAL(zdelprops);
    object_init_ex(zdelprops, riak_delete_input_ce);

    /* Set w,dw,pw,r,rw,pw if set */
    MAKE_STD_ZVAL(zoptionsarray);
    array_init(zoptionsarray);
    if (purl->query) {
        sapi_module.treat_data(PARSE_STRING, estrdup(purl->query), zoptionsarray TSRMLS_CC);

        SET_GET_OPTION_IF_PRESENT("r", zgetprops, zoptionsarray, zoption)
        SET_GET_OPTION_IF_PRESENT("rw", zgetprops, zoptionsarray, zoption)
        SET_GET_OPTION_IF_PRESENT("pr", zgetprops, zoptionsarray, zoption)

        SET_PUT_OPTION_IF_PRESENT("w", zputprops, zoptionsarray, zoption)
        SET_PUT_OPTION_IF_PRESENT("dw", zputprops, zoptionsarray, zoption)
        SET_PUT_OPTION_IF_PRESENT("pw", zputprops, zoptionsarray, zoption)

        SET_DELETE_OPTION_IF_PRESENT("r", zdelprops, zoptionsarray, zoption)
        SET_DELETE_OPTION_IF_PRESENT("pr", zdelprops, zoptionsarray, zoption)
        SET_DELETE_OPTION_IF_PRESENT("rw", zdelprops, zoptionsarray, zoption)
        SET_DELETE_OPTION_IF_PRESENT("w", zdelprops, zoptionsarray, zoption)
        SET_DELETE_OPTION_IF_PRESENT("dw", zdelprops, zoptionsarray, zoption)
        SET_DELETE_OPTION_IF_PRESENT("pw", zdelprops, zoptionsarray, zoption)
    }
    zval_ptr_dtor(&zoptionsarray);

    stripped_path = php_trim(purl->path, strlen(purl->path), "/", 1, NULL, 3 TSRMLS_CC);
    zbucket = create_bucket_object(zclient, stripped_path TSRMLS_CC);
    php_url_free(purl);
    efree(stripped_path);
    if (EG(exception)) {
        zval_ptr_dtor(&zbucket);
        zval_ptr_dtor(&zclient);
        zval_ptr_dtor(&zgetprops);
        zval_ptr_dtor(&zputprops);
        zval_ptr_dtor(&zdelprops);
        PS_SET_MOD_DATA(NULL);
        return FAILURE;
    } else {
        session_data = ecalloc(1, sizeof(riak_session_data));
        session_data->zbucket = zbucket;
        session_data->zclient = zclient;
        session_data->zgetprops = zgetprops;
        session_data->zputprops = zputprops;
        session_data->zdelprops = zdelprops;
        session_data->session_name = estrdup(session_name);
        PS_SET_MOD_DATA(session_data);
        return SUCCESS;
    }
}
/* }}} */

PS_CLOSE_FUNC(riak) /* {{{ */
{
    PS_RIAK_DATA;
    if (data) {
        efree(data->session_name);
        zval_ptr_dtor(&data->zbucket);
        zval_ptr_dtor(&data->zclient);
        zval_ptr_dtor(&data->zgetprops);
        zval_ptr_dtor(&data->zputprops);
        zval_ptr_dtor(&data->zdelprops);
        efree(data);

        PS_SET_MOD_DATA(NULL);
    }
    return SUCCESS;
}
/* }}} */

PS_READ_FUNC(riak) /* {{{ */
{
    zval *zoutput, *zkey, *zdata, *zex;
    PS_RIAK_DATA;
    *vallen = 0;

    MAKE_STD_ZVAL(zkey);
    ZVAL_STRING(zkey, key, 1);

    MAKE_STD_ZVAL(zoutput);
    RIAK_CALL_METHOD2(RiakBucket, get, zoutput, data->zbucket, zkey, data->zgetprops);

    if (!EG(exception)) {
        zval *zobjarr;
        MAKE_STD_ZVAL(zobjarr);
        RIAK_CALL_METHOD(Riak_Output_GetOutput, getObjectList, zobjarr, zoutput);
        if (Z_TYPE_P(zobjarr) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(zobjarr)) > 0) {
            zval **found = NULL;
            zend_hash_index_find(Z_ARRVAL_P(zobjarr), 0, (void**)&found);
            zdata = zend_read_property(riak_object_ce, *found, "data", sizeof("data")-1, 1 TSRMLS_CC);
            if (zdata->type == IS_STRING && Z_STRLEN_P(zdata) > 0) {
                *vallen = Z_STRLEN_P(zdata);
                *val = emalloc(Z_STRLEN_P(zdata));
                memcpy(*val, Z_STRVAL_P(zdata), Z_STRLEN_P(zdata));
            }
        }
        zval_ptr_dtor(&zobjarr);
    } else {
        zend_clear_exception(TSRMLS_C);
    }
    if (*vallen == 0) {
        *val = STR_EMPTY_ALLOC();
    }
    zval_ptr_dtor(&zoutput);
    zval_ptr_dtor(&zkey);
    return SUCCESS;
}
/* }}} */

PS_WRITE_FUNC(riak) /* {{{ */
{
    zval *zobject;
    PS_RIAK_DATA;
    zobject = create_object_object(key TSRMLS_CC);
    if (EG(exception)) {
        zend_clear_exception(TSRMLS_C);
        zval_ptr_dtor(&zobject);
        return FAILURE;
    }
    zend_update_property_stringl(riak_object_ce, zobject, "data", sizeof("data")-1, val, vallen TSRMLS_CC);
    RIAK_CALL_METHOD2(RiakBucket, put, zobject, data->zbucket, zobject, data->zputprops);
    zval_ptr_dtor(&zobject);
    if (EG(exception)) {
        return FAILURE;
    } else {
        return SUCCESS;
    }
}
/* }}} */

PS_DESTROY_FUNC(riak) /* {{{ */
{
    PS_RIAK_DATA;
    zval *zobject;
    zobject = create_object_object(key TSRMLS_CC);
    RIAK_CALL_METHOD2(RiakBucket, delete, zobject, data->zbucket, zobject, data->zdelprops);
    zval_ptr_dtor(&zobject);
    if (EG(exception)) {
        return FAILURE;
    }
    return SUCCESS;
}

PS_GC_FUNC(riak) /* {{{ */
{
    /* Do nothing riak´s builtin key expire should be used on the session bucket */
    return SUCCESS;
}
/* }}} */

//#endif

