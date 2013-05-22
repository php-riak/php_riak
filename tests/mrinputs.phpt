--TEST--
Test map reduce input objects
--FILE--
<?php
$buckeetInput1 = new RiakMrInputBucket("bucket_name");
if ($buckeetInput1->getValue() !== "bucket_name") {
    var_dump($buckeetInput1);
}

$listInput = new RiakMrInputKeyList(array("bucket1" => array("1", "2", "3")));

$listInput  ->add(array("bucket1" => array("4", "5")))
            ->add(array("bucket2" => array("0", "asdf")));

$val = $listInput->getValue();
// TODO This should actually be [["bucket1", "1"],["bucket1", "2"] ....
if ( (count($val["bucket1"]) != 5) ||
     (count($val["bucket2"]) != 2) ) {
    var_dump($val);
}
echo "done!";
?>
--EXPECT--
done!
