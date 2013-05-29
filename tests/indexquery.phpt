--TEST--
Test index queries
--FILE--
<?php
include_once "connect.inc";
$client = new RiakClient($host, $port);
$bucket = new RiakBucket($client, "index_test_bucket");
$props = new RiakBucketProperties(3, false);
$bucket->applyProperties($props);

try {
    for ($i=0; $i<10; $i++) {
        $obj = new RiakObject("obj$i");
        $obj->data = "test-idx";
        $obj->indexes = array("tal_int" => $i, "tekst_bin" => "text$i");
        $bucket->putObject($obj);
    }
    $result = $bucket->indexQuery("tal_int", 1);
    if ($result !== "obj1") {
        var_dump($result);
    }
} catch (Exception $e) {
    var_dump($e);
}
?>
--EXPECT--
success!
