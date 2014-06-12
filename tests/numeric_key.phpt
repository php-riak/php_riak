--TEST--
Test Segmentation fault with numeric key
--FILE--
<?php

include_once "connect.inc";

use Riak\Output\PutOutput;
use Riak\Connection;
use Riak\Object;
use Riak\Bucket;

$client = new Connection($host, $port);
$bucket = new Bucket($client, 'test_bucket');

function testRiakKey(Bucket $bucket, $key)
{
    echo PHP_EOL;
    echo "Using key         : " . gettype($key) . PHP_EOL;
    echo "Delete object     : " . var_export($bucket->delete($key) === null, true) . PHP_EOL;
    echo "Has object        : " . var_export($bucket->get($key)->hasObject(), true) . PHP_EOL;
}

$object1 = new Object();
$object2 = new Object();

$object1->setKey('9999');
$object1->setContent('test 9999');

$object2->setKey('99.99');
$object2->setContent('test 99.99');

$bucket->put($object1);
$bucket->put($object2);

testRiakKey($bucket, 9999);
testRiakKey($bucket, 99.99);

try {
    testRiakKey($bucket, array(9999));
} catch (\Exception $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    testRiakKey($bucket, new \ArrayObject(array(99.99)));
} catch (\Exception $e) {
    echo $e->getMessage() . PHP_EOL;
}


?>
--EXPECT--

Using key         : integer
Delete object     : true
Has object        : false

Using key         : double
Delete object     : true
Has object        : false

Using key         : array
Argument 1 passed to Riak\Bucket#delete() must be a string or an instance of Riak\Object.

Using key         : object
Argument 1 passed to Riak\Bucket#delete() must be a string or an instance of Riak\Object.
