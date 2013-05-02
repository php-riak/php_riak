#!/bin/bash
cd riack/
cmake -D BUILD_SHARED_LIBS:BOOL=OFF -D RIAK_TEST_SERVER_IP=$2 -D RIAK_TEST_SERVER_PB_PORT=$1 src/
make
exit $?
