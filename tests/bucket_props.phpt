--TEST--
Test bucket properties
--FILE--
<?php
use \Riak\BucketPropertyList;
include_once "connect.inc";
$client = new RiakClient($host, $port);
$bucket = new RiakBucket($client, "test_bucket_props");
$oldProps = $bucket->fetchProperties();
$newProps = new BucketPropertyList(1, false);
$bucket->applyProperties($newProps);
$currentProps = $bucket->fetchProperties();
if ($currentProps->getNValue() === 1 && $currentProps->getAllowMult() === false) {
	echo "success!";
} else {
	var_dump($currentProps);
}
$bucket->applyProperties($oldProps);
?>
--EXPECT--
success!
