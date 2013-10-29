--TEST--
Test index query
--FILE--
<?php
use \Riak\Query\IndexQuery;
$q = new IndexQuery("asdf");
if ($q->isRangeQuery() == true) {
    echo "Should not be ranged query at this time".PHP_EOL;
}
$q->setRangeValue("min", "max");
if ($q->isRangeQuery() != true) {
    echo "Should be ranged query now".PHP_EOL;
}
$rv = $q->getRangeValue();
if ($rv[0] !== "min" || $rv[1] !== "max") {
    echo "Rangedvalues does not match".PHP_EOL;
}
$q->setExactValue("exact");
if ($q->isRangeQuery() == true) {
    echo "Should not be ranged query now".PHP_EOL;
}
echo "done!".PHP_EOL;
?>
--EXPECT--
done!
