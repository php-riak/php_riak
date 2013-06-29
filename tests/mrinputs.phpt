--TEST--
Test map reduce input objects
--FILE--
<?php
use \Riak\Object;
use \Riak\MapReduce\Input\BucketInput;
use \Riak\MapReduce\Input\KeyDataListInput;
use \Riak\MapReduce\Input\KeyListInput;
include_once "connect.inc";
$client = new RiakClient($host, $port);
$bucket = new RiakBucket($client, "test_bucket");

$buckeetInput1 = new BucketInput("bucket_name");
$val = $buckeetInput1->getValue();
$val = $buckeetInput1->getValue();
if ($val !== "bucket_name") {
    var_dump($buckeetInput1);
}

$buckeetInput2 = new BucketInput("bucket_name2");
$buckeetInput2->setIndexFilter("test_bin", "s", "e");
$val = $buckeetInput2->getValue();
if ($val["bucket"] !== "bucket_name2" || $val["index"] !== "test_bin" || $val["start"] != "s" || $val["end"] !== "e") {
    var_dump($val);
}

$obj = new Object("object_key");
$obj2 = new Object("object_key2");
$obj3 = new Object("object_key3");

$keyDataList = new KeyDataListInput();
$keyDataList->add("bucket","key1","data")
            ->add("bucket","key2","data")
            ->add("bucket","key3","data");
$val = $keyDataList->getValue();
if (count($val) !== 3) {
    var_dump($val);
}

$listInput = new KeyListInput(array("bucket1" => array("key1", "key2", "key3")));
$listInput  ->addArray(array("bucket1" => array("key4", "key5")))
            ->addArray(array("bucket2" => array("0", "asdf")))
            ->addArray(array("bucket3" => $obj))
            ->addArray(array("bucket4" => array($obj2, $obj3)))
            ->addSingle("bucket5", "ost")
            ->addSingle("bucket6", "ost2")
            ->addSingle("bucket7", $obj)
            ->addSingle($bucket, $obj2);

$val = $listInput->getValue();

if (count($val) !== 14) {
    var_dump($val);
}
echo "done!";
?>
--EXPECT--
done!
