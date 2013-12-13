--TEST--
Test built-in conflict resolvers
--FILE--
<?php

include_once "connect.inc";

use Riak\ObjectList;
use Riak\Output\ConflictResolver;

$youngestResolver = new \Riak\Output\YoungestSiblingResolver();

$object1  = new \Riak\Object('key_1');
$object2  = new \Riak\Object('key_2');
$object3  = new \Riak\Object('key_3');
$list     = new \Riak\ObjectList();

echo "Resolve empty : " . var_export($youngestResolver->resolve($list), true) . PHP_EOL;

$list[] = $object1;
$list[] = $object2;
$list[] = $object3;

echo "Is a resolver : " . var_export($youngestResolver instanceof ConflictResolver, true) . PHP_EOL;
echo "Resolve youngest : " . var_export($youngestResolver->resolve($list) === $object1, true) . PHP_EOL;

?>
--EXPECT--
Resolve empty : NULL
Is a resolver : true
Resolve youngest : true
