--TEST--
Simple get test
--FILE--
<?php
include_once "connect.inc";
$client = new RiakClient($host, $port);
$bucket = new RiakBucket($client, "test_bucket");
$vclock = null;

$obj = new RiakObject("get_test");
$bucket->deleteObject($obj);
// TODO Come back here and disable mults so we dont need to wait for the delete
sleep(10);
try {
	$obj->contentType = "text/plain";
	$obj->data = "test-get plap";
	$bucket->putObject($obj);
	sleep(1);
	$readdenObj = $bucket->getObject("get_test");
  echo "success!";
} catch (Exception $e) {
	var_dump($e);
}
?>
--EXPECT--
success!