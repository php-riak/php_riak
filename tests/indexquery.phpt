--TEST--
Test index queries
--FILE--
<?php
use \Riak\BucketPropertyList;
include_once "connect.inc";
$client = new \Riak\Connection($host, $port);
$bucket = new RiakBucket($client, "index_test_bucket");
$props = new BucketPropertyList(3, false);
$bucket->applyProperties($props);

try {
    for ($i=0; $i<10; $i++) {
        $obj = new \Riak\Object("obj$i");
        $obj->setContent("test-idx");
        $obj->addIndex("tal_int", $i)->addIndex("tekst_bin", "text$i");
        $bucket->put($obj);
    }
    $result = $bucket->indexQuery("tal_int", 1);
    if ($result[0] !== "obj1") {
        var_dump($result);
    }
    $result = $bucket->indexQuery("tekst_bin", "text4", "text6");
    if (count($result) !== 3 ||
        !in_array("obj4", $result) ||
        !in_array("obj5", $result) ||
        !in_array("obj6", $result)) {
        var_dump($result);
    }
    $props = NULL;
    $bucket = NULL;
    $result = NULL;
    echo "done!";
} catch (Exception $e) {
    var_dump($e);
}
?>
--EXPECT--
done!
