--TEST--
Riak Search Input test
--FILE--
<?php
use \Riak\Search\Input\ParameterBag;
include_once "connect.inc";
$inp = new ParameterBag();

$inp->setRowLimit(10);
if ($inp->getRowLimit() != 10) {
    echo "rowlimit".PHP_EOL;
    var_dump($inp);
}
$inp->setStartOffset(15);
if ($inp->getStartOffset() != 15) {
    echo "startoffset".PHP_EOL;
    var_dump($inp);
}
$inp->setSort("plappe_sort");
if ($inp->getSort() != "plappe_sort") {
    echo "sort".PHP_EOL;
    var_dump($inp);
}
$inp->setFilter("plappe_filter");
if ($inp->getFilter() != "plappe_filter") {
    echo "filter".PHP_EOL;
    var_dump($inp);
}
$inp->setDefaultField("def_field");
if ($inp->getDefaultField() != "def_field") {
    echo "default_field".PHP_EOL;
    var_dump($inp);
}
$inp->setDefaultOperation("def_op");
if ($inp->getDefaultOperation() != "def_op") {
    echo "def_op".PHP_EOL;
    var_dump($inp);
}
$inp->setPresort("plappe_presort");
if ($inp->getPresort() != "plappe_presort") {
    echo "presort".PHP_EOL;
    var_dump($inp);
}
$inp->setFieldLimits(array("1", "2", "3"));
$fls = $inp->getFieldLimits();
if ($fls[0] != "1" || $fls[1] != "2" || $fls[2] != "3") {
    echo "fieldlimits".PHP_EOL;
    var_dump($inp);
}
echo "done!".PHP_EOL;
?>
--EXPECT--
done!
