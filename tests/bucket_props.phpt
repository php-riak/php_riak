--TEST--
Test bucket properties
--FILE--
<?php
include_once "connect.inc";
$client = new RiakClient($host, $port);
$bucket = new RiakBucket($client, "test_bucket_props");
// TODO Get back here and set some properties we can read back!
$props = $bucket->fetchProperties();
if (isset($props)) {
	echo "success!";
}
?>
--EXPECT--
success!