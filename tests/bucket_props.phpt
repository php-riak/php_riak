--TEST--
Test bucket properties
--FILE--
<?php
include_once "connect.inc";
$client = new RiakClient($host, $port);
$bucket = new RiakBucket($client, "test_bucket_props");
$oldProps = $bucket->fetchProperties();
$newProps = new RiakBucketProperties(1, false);
$bucket->applyProperties($newProps);
$currentProps = $bucket->fetchProperties();
if ($currentProps->nVal === 1 && $currentProps->allowMult === false) {
	echo "success!";
} else {
	var_dump($currentProps);
}
$bucket->applyProperties($oldProps);
?>
--EXPECT--
success!