#!/bin/sh
export PHP=`which php`
export TEST_PHP_EXECUTABLE=$PHP
$PHP run-tests.php -q -m --show-diff
