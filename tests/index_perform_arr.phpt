--TEST--
Test index with multiple values
--FILE--
<?php
use \Riak\BucketPropertyList;
include_once "connect.inc";
$client = new \Riak\Connection($host, $port);
$bucket = new \Riak\Bucket($client, "index_test_bucket2");
$props = new BucketPropertyList(3, false);
$bucket->setPropertyList($props);
try {
    $obj = new \Riak\Object("obj");
    $obj->setContent("test-idx");
    $obj->addIndex("tal_int", 0)->addIndex("tekst_bin", "text0")
        ->addIndex("tal_int", 1)->addIndex("tekst_bin", "text1")
        ->addIndex("tal_int", 2)->addIndex("tekst_bin", "text3")
        ->addIndex("tal_int", 3)->addIndex("tekst_bin", "text4");
    $in = $obj->getIndexMap("tal_int");
    $bucket->put($obj);
    $getOutput = $bucket->get("obj");
    $getObj = $getOutput->getObject();
    $out = $obj->getIndexMap("tal_int");
    if ($in !== $out) {
        echo "Different 1".PHP_EOL;
        var_dump($in);
        var_dump($out);
    }
    $bucket->put($getObj);
    $getOutput = $bucket->get("obj");
    $getObj = $getOutput->getObject();
    $out2 = $obj->getIndexMap("tal_int");
    if ($in !== $out2) {
        echo "Different 2".PHP_EOL;
        var_dump($in);
        var_dump($out2);
    }
    echo "done!";
} catch (Exception $e) {
    var_dump($e);
}
?>
--EXPECT--
done!
