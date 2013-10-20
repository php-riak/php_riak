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

#ifndef RIAK_PROPERTY__REPLICATION_MODE__REPLICATION_MODE__H__
#define RIAK_PROPERTY__REPLICATION_MODE__REPLICATION_MODE__H__

#include "php_riak_internal.h"

void riak_property_replication_mode_init(TSRMLS_D);

extern zend_class_entry *riak_replication_mode_ce;
extern zend_class_entry *riak_replication_mode_full_only_ce;
extern zend_class_entry *riak_replication_mode_disabled_ce;
extern zend_class_entry *riak_replication_mode_realtime_and_full_ce;
extern zend_class_entry *riak_replication_mode_realtime_only_ce;

#endif // RIAK_PROPERTY__REPLICATION_MODE__REPLICATION_MODE__H__
