--TEST--
Test built-in conflict resolvers with real db fetch
--FILE--
<?php

include_once "connect.inc";

use Riak\ObjectList;
use Riak\Output\ConflictResolver;

$client   = new \Riak\Connection($host, $port);
$bucket   = new \Riak\Bucket($client, 'test_bucket');
$props    = new \Riak\BucketPropertyList();
$props->setAllowMult(true);
$props->setLastWriteWins(false);

$youngestResolver = new \Riak\Output\YoungestSiblingResolver();
$bucket->setConflictResolver($youngestResolver);

// Put two objects on same key with no vclocks
$object1  = new \Riak\Object('key_conflict');
$object1->setContent("content_1");
$object2  = new \Riak\Object('key_conflict');
$object2->setContent("content_2");
$bucket->put($object1);
sleep(1);
$bucket->put($object2);

$getOutput = $bucket->get('key_conflict');
$winner = $getOutput->getObject();
echo "Resolve youngest : " . var_export($winner->getContent() == "content_2", true) . PHP_EOL;

$bucket->delete($winner);
?>
--EXPECT--
Resolve youngest : true
