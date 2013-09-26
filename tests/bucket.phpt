--TEST--
Test Riak\Bucket throws exception when connection is NULL
--FILE--
<?php

try {
    $connection = null;
    $bucket     = new \Riak\Bucket($connection, 'test');

    echo "done!" . PHP_EOL;
} catch (\Riak\Exception\BadArgumentsException $e) {
    echo $e->getMessage();
}

?>
--EXPECTF--
Warning: Riak\Bucket::__construct() expects parameter 1 to be Riak\Connection, null given in %s on line %d
First parameter must be an instance of Riak\Connection.
