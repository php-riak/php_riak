/*
   Copyright 2013: Kaspar Bach Pedersen

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
#include "replication_mode.h"


zend_class_entry *riak_replication_mode_ce;
zend_class_entry *riak_replication_mode_full_only_ce;
zend_class_entry *riak_replication_mode_disabled_ce;
zend_class_entry *riak_replication_mode_realtime_and_full_ce;
zend_class_entry *riak_replication_mode_realtime_only_ce;

static zend_function_entry riak_replication_mode_functions[] = {
    {NULL, NULL, NULL}
};


void riak_property_replication_mode_init(TSRMLS_D)/* {{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Riak\\Property\\ReplicationMode", "ReplicationMode", riak_replication_mode_functions);
    riak_replication_mode_ce = zend_register_internal_interface(&ce TSRMLS_CC);

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Property\\ReplicationMode", "FullSyncOnly", riak_replication_mode_functions);
    riak_replication_mode_full_only_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_class_implements(riak_replication_mode_full_only_ce TSRMLS_CC, 1, riak_replication_mode_ce);

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Property\\ReplicationMode", "Disabled", riak_replication_mode_functions);
    riak_replication_mode_disabled_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_class_implements(riak_replication_mode_disabled_ce TSRMLS_CC, 1, riak_replication_mode_ce);

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Property\\ReplicationMode", "RealTimeAndFullSync", riak_replication_mode_functions);
    riak_replication_mode_realtime_and_full_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_class_implements(riak_replication_mode_realtime_and_full_ce TSRMLS_CC, 1, riak_replication_mode_ce);

    INIT_NS_CLASS_ENTRY(ce, "Riak\\Property\\ReplicationMode", "RealTimeOnly", riak_replication_mode_functions);
    riak_replication_mode_realtime_only_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_class_implements(riak_replication_mode_realtime_only_ce TSRMLS_CC, 1, riak_replication_mode_ce);

}
/* }}} */

