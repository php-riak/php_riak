--TEST--
Test Resolve Siblings
--FILE--
<?php

include_once "connect.inc";

$key     = "my_key";
$client  = new \Riak\Connection($host, $port);
$object1 = new \Riak\Object($key);
$object2 = new \Riak\Object($key);
$props   = new \Riak\BucketPropertyList($nVal = 1, $allowMult = true);
$bucket  = new \Riak\Bucket($client, 'test_bucket');

$bucket->setPropertyList($props);
$object1->setContent("object value 1");
$object2->setContent("object value 2");

$bucket->put($object1);
$bucket->put($object2);

$output  = $bucket->get($key);
$vClock  = $output->getVClock();
$objects = $output->getObjectList();

echo "Has Siblings  : " . var_export($bucket->get($key)->hasSiblings(), true) . PHP_EOL;
echo "Same VClock 1 : " . var_export($objects[0]->getVClock() === $vClock, true) . PHP_EOL;
echo "Same VClock 2 : " . var_export($objects[1]->getVClock() === $vClock, true) . PHP_EOL;

$winner    = $objects[1];
$putInput  = new \Riak\Input\PutInput();
$merged    = new \Riak\Object($key);

$putInput->setVClock($vClock);
$merged->setContent($winner->getContent());

$bucket->put($merged, $putInput);

echo "Has Siblings  : " . var_export($bucket->get($key)->hasSiblings(), true) . PHP_EOL;

$getOutput = $bucket->get($key);
$deleteInput = new \Riak\Input\DeleteInput();
$bucket->delete($key, $deleteInput->setVClock($getOutput->getVClock()));
?>
--EXPECT--
Has Siblings  : true
Same VClock 1 : true
Same VClock 2 : true
Has Siblings  : false
