--TEST--
Test built-in conflict resolvers
--FILE--
<?php

include_once "connect.inc";

use Riak\ObjectList;
use Riak\Output\ConflictResolver;

$firstResolver = new \Riak\Output\FirstSiblingResolver();
$lastResolver  = new \Riak\Output\LastSiblingResolver();

$object1  = new \Riak\Object('key_1');
$object2  = new \Riak\Object('key_2');
$object3  = new \Riak\Object('key_3');
$list     = new \Riak\ObjectList();

echo "Resolve empty : " . var_export($firstResolver->resolve($list), true) . PHP_EOL;
echo "Resolve empty : " . var_export($lastResolver->resolve($list), true) . PHP_EOL;

$list[] = $object1;
$list[] = $object2;
$list[] = $object3;

echo "Is a resolver : " . var_export($firstResolver instanceof ConflictResolver, true) . PHP_EOL;
echo "Is a resolver : " . var_export($lastResolver instanceof ConflictResolver, true) . PHP_EOL;

echo "Resolve first : " . var_export($firstResolver->resolve($list) === $object1, true) . PHP_EOL;
echo "Resolve last  : " . var_export($lastResolver->resolve($list) === $object3, true) . PHP_EOL;

?>
--EXPECT--
Resolve empty : NULL
Resolve empty : NULL
Is a resolver : true
Is a resolver : true
Resolve first : true
Resolve last  : true
