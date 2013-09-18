--TEST--
CommitHook test
--FILE--
<?php
$hook = new \Riak\Property\CommitHook('Name', null);
$modFun = new \Riak\Property\ModuleFunction('Module', 'function');
$hook = new \Riak\Property\CommitHook('Name', $modFun);
$hookList = new \Riak\Property\CommitHookList();
$hookList["not_possible"] = "1234";
if (isset($hookList["not_possible"])) {
    echo "Should not be set..".PHP_EOL;
}
echo "done!".PHP_EOL;
?>
--EXPECTF--
Warning: Riak\Property\CommitHook::__construct() expects parameter 2 to be Riak\Property\ModuleFunction, %S

Warning: Riak\Property\CommitHookList::offsetSet() expects parameter 2 to be Riak\Property\CommitHook, %S
done!
