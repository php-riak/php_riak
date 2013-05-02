#!/bin/bash

phpize
./configure --quiet
make install
echo "extension=riak.so" >> `php --ini | grep "Loaded Configuration" | sed -e "s|.*:\s*||"`
