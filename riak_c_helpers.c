/*
   Copyright 2014: Kaspar Bach Pedersen

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

#include "riak_c_helpers.h"

riak_binary* riak_binary_shallow_from_property(riak_config *cfg,
        zend_class_entry *scope, zval* zobj, char* property_name TSRMLS_DC)/* {{{ */
{
    zval *zproperty;
    riak_binary *bin;
    zproperty = zend_read_property(scope, zobj, property_name, sizeof(property_name)-1, 1 TSRMLS_CC);
    if (Z_TYPE_P(zproperty) == IS_STRING) {
        bin = riak_binary_new(cfg, Z_STRLEN_P(zproperty), (riak_uint8_t*)Z_STRVAL_P(zproperty));
    } else {
        bin = 0;
    }
    return bin;
}
/* }}} */

riak_binary* riak_binary_deep_from_stringl(riak_config *cfg, int len, const char* val)/* {{{ */
{
    riak_binary* bin;
    bin = riak_binary_new(cfg, 0, 0);
    riak_binary_from_stringl(bin, (riak_size_t)len, (riak_uint8_t*)val);
    return bin;
}
/* }}} */
