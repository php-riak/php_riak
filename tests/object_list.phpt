--TEST--
Test Riak\Output\ObjectList works
--FILE--
<?php
$obj1 = new \Riak\Object("1");
$obj2 = new \Riak\Object("1");
$objList = new \Riak\Output\ObjectList();
if (!$objList->isEmpty()) {
    echo "List is supposed to be empty!".PHP_EOL;
}
if (!is_null($objList->first())) {
    echo "When list is empty null should be returned as first!".PHP_EOL;
}
$objList[] = $obj1;
$objList["2"] = $obj2;
if ($objList->count() !== 2) {
    echo "Count should be 2".PHP_EOL;
}
if (!$objList->offsetExists("2")) {
    echo "2 should exist in list".PHP_EOL;
}
if ($objList->first()->getKey() !== $obj1->getKey()) {
    echo "Keys should equal".PHP_EOL;
}
echo "done!".PHP_EOL;
?>
--EXPECT--
done!
