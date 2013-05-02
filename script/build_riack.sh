#!/bin/bash
cd riack/
cmake -D BUILD_SHARED_LIBS:BOOL=OFF -D RIAK_TEST_SERVER_IP=$1 -D RIAK_TEST_SERVER_PB_PORT=$2 src/
make clean all
exit $?
