--TEST--
Test map reduce input objects
--FILE--
<?php
$buckeetInput1 = new RiakMrInputBucket("bucket_name");
if ($buckeetInput1->getValue() !== "bucket_name") {
    var_dump($buckeetInput1);
}

$listInput = new RiakMrInputKeyList(array("bucket1" => array("1", "2", "3")));

$listInput  ->addArray(array("bucket1" => array("4", "5")))
            ->addArray(array("bucket2" => array("0", "asdf")));

$val = $listInput->getValue();
// var_dump($val);
echo "done!";
?>
--EXPECT--
done!
