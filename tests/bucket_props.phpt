--TEST--
Test bucket properties old
--FILE--
<?php
use \Riak\BucketPropertyList;
include_once "connect.inc";
$client = new \Riak\Connection($host, $port);
$bucket = new \Riak\Bucket($client, "test_bucket_props");
$oldProps = $bucket->getPropertyList();
$newProps = new BucketPropertyList(1, false);
$bucket->setPropertyList($newProps);
$currentProps = $bucket->getPropertyList();
if ($currentProps->getNValue() === 1 && $currentProps->getAllowMult() === false) {
	echo "success!";
} else {
	var_dump($currentProps);
}
$bucket->setPropertyList($oldProps);
?>
--EXPECT--
success!
