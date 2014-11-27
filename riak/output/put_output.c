/*
   Copyright 2013: Kaspar Pedersen

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

#include "put_output.h"
#include "output.h"

zend_class_entry *riak_put_output_ce;

static zend_function_entry riak_put_output_methods[] = {
    {NULL, NULL, NULL}
};

void riak_output_put_output_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Output", "PutOutput", riak_put_output_methods);
    riak_put_output_ce = zend_register_internal_class_ex(&ce, riak_output_ce, NULL TSRMLS_CC);
}
/* }}} */


zval *put_output_from_riack_object(riack_object* obj, zval* zkey TSRMLS_DC) /* {{{ */
{
    zval *zoutput;
    MAKE_STD_ZVAL(zoutput);
    object_init_ex(zoutput, riak_put_output_ce);
    riak_set_output_properties(zoutput, zkey, obj TSRMLS_CC);
    return zoutput;
}
/* }}} */

