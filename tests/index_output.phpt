--TEST--
Test index output
--FILE--
<?php
use \Riak\Output\IndexResult;
use \Riak\Output\IndexResultList;
$r1 = new IndexResult("key1");
$r2 = new IndexResult("key2");
$resultList = new IndexResultList();
$resultList[] = $r1;
$resultList[] = $r2;
if ($resultList[0] !== $r1) {
    echo "Error".PHP_EOL;
}
echo "done!".PHP_EOL;
?>
--EXPECT--
done!
