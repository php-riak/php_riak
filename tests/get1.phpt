--TEST--
Simple get test
--FILE--
<?php
include_once "connect.inc";
$client = new RiakClient($host, $port);
$bucket = new RiakBucket($client, "test_bucket");
$props = new RiakBucketProperties(3, false);
$bucket->applyProperties($props);

$obj = new RiakObject("get_test");
try {
	$obj->contentType = "text/plain";
	$obj->data = "test-get plap";
	$bucket->putObject($obj);
	$readdenObj = $bucket->getObject("get_test");
  echo "success!";
} catch (Exception $e) {
	var_dump($e);
}
?>
--EXPECT--
success!