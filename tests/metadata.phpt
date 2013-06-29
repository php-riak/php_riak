--TEST--
Test metadata gets read and written
--FILE--
<?php
use \Riak\BucketPropertyList;
use \Riak\Object;
include_once "connect.inc";
$client = new RiakClient($host, $port);
$bucket = new RiakBucket($client, "test_bucket");
$props = new BucketPropertyList(3, false);
$bucket->applyProperties($props);

$obj = new Object("get_test");
try {
    $obj->setContent("test-get plap")
        ->addMetadata("ost", 7);
    $bucket->put($obj);

    $output = $bucket->get("get_test");
    $objs = $output->getObjectList();
    $obj0 = $objs[0];
    $meta0 = $obj0->getMetadataMap();
    if (isset($meta0["ost"]) && $meta0["ost"] == 7) {
        echo "success!";
    } else {
        var_dump($meta0);
    }
} catch (Exception $e) {
    var_dump($e);
}
?>
--EXPECT--
success!
