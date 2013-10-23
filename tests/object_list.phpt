--TEST--
Test Riak\Output\ObjectList works
--FILE--
<?php
$obj1 = new \Riak\Object("1");
$obj2 = new \Riak\Object("2");
$objList = new \Riak\ObjectList();
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

$objList2 = new \Riak\ObjectList();
$objList2["plappe"] = $obj2;
if ($objList2->isEmpty()) {
    echo "List2 is not supposed to be empty".PHP_EOL;
}
if ($objList2->first()->getKey() !== $obj2->getKey()) {
    echo "List2 keys should equal".PHP_EOL;
}

//////
// Try inserting a non riak object
try {
    $objList2["fail"] = 3;
} catch (\Riak\Exception\BadArgumentsException $e) {
    echo "done!".PHP_EOL;
}
?>
--EXPECT--
Warning: Riak\ObjectList::offsetSet() expects parameter 2 to be Riak\Object, integer given in /Users/kaspar/Documents/workspace/php_riak/tests/object_list.php on line 35
done!
