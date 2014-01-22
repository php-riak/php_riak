#!/bin/bash
git clone https://github.com/TriKaspar/riak-c-client.git
cd riak-c-client
./autogen.sh
./configure
make
cd ..
phpize
./configure --with-riak-c-dir=./riak-c-client/src --quiet
make install
echo "extension=riak.so" >> `php --ini | grep "Loaded Configuration" | sed -e "s|.*:\s*||"`
