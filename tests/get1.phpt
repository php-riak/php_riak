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
        $obj->metadata["test"] = "test";
	$bucket->putObject($obj);
	$readdenObj = $bucket->getObject("get_test");
        if (strcmp($readdenObj->data, $obj->data) == 0 || strcmp($readdenObj->metadata["test"], "test") !== 0 ) {
		echo "success!";
	}
} catch (Exception $e) {
	var_dump($e);
}
?>
--EXPECT--
success!
