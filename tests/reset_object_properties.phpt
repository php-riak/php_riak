--TEST--
Test we can set properties of an object to null
--FILE--
<?php
use \Riak\Object;
include_once "connect.inc";
$client = new \Riak\Connection($host, $port);
$bucket = new \Riak\Bucket($client, "test_bucket");

// No key provieded
$obj = new Object();
$obj->setContent("dummy");
$obj->setContentEncoding("encoding");
$obj->setContentType("type");
$output = $bucket->put($obj);
$key = $output->getKey();

$getOutput = $bucket->get($key);
$obj = $getOutput->getFirstObject();
if (($obj->getContentEncoding() != "encoding") || ($obj->getContentType() != "type")) {
    var_dump($obj);
}
$obj->setContent(null);
$obj->setContentEncoding(null);
$obj->setContentType(null);

$bucket->put($obj);
$getOutput = $bucket->get($key);
$obj = $getOutput->getFirstObject();
// It seems Riak set contentType to text/plain if it is not set
if (($obj->getContentEncoding() != null) ||
    ($obj->getContentType() != "text/plain") ||
    ($obj->getContent() != null)) {
    echo "Not all properties was NULL as expected".PHP_EOL;
    var_dump($obj);
}
$bucket->delete($key);
echo "done!";
?>
--EXPECT--
done!
