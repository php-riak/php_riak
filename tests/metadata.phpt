--TEST--
Test metadata gets read and written
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
	$obj->metadata["ost"] = 7;
	$bucket->putObject($obj);

	$readdenObj = $bucket->getObject("get_test");
	if (isset($readdenObj->metadata["ost"]) && $readdenObj->metadata["ost"] == 7) {
		echo "success!";
	} else {
		var_dump($readdenObj->metadata);
	}
} catch (Exception $e) {
	var_dump($e);
}
?>
--EXPECT--
success!