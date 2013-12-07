--TEST--
Test bucket delete using bad arguments
--FILE--
<?php
use \Riak\Object;
include_once "connect.inc";

$key    = uniqid();
$obj    = new \Riak\Object($key);
$client = new \Riak\Connection($host, $port);
$bucket = new \Riak\Bucket($client, "test_bucket");


$obj->setContent("foo");

$bucket->put($obj);

try {
    $bucket->delete(new \Riak\Object());
} catch (Exception $e) {
    echo get_class($e) . ' : ' .$e->getMessage() . PHP_EOL;
}

try {
    $bucket->delete(new stdClass());
} catch (Exception $e) {
    echo get_class($e) . ' : ' .$e->getMessage() . PHP_EOL;
}

$bucket->delete($key);

echo var_export($bucket->get($key)->hasObject()) . PHP_EOL;

?>
--EXPECT--
Riak\Exception\BadArgumentsException : Unable to resolve the object key.
Riak\Exception\BadArgumentsException : Argument 1 passed to Riak\Bucket#delete() must be a string or an instance of Riak\Object.
false
