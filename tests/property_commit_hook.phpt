--TEST--
CommitHook test
--FILE--
<?php
$jsHook = new \Riak\Property\CommitHook('Name');
$erlHook = new \Riak\Property\CommitHook('Module', 'Function');
if (!$jsHook->isJavascript() || $jsHook->isErlang()) {
    echo 'Javascript hook unexpected became something else'.PHP_EOL;
    var_dump($jsHook);
}
if ($erlHook->isJavascript() || !$erlHook->isErlang()) {
    echo 'Erlang hook unexpected became something else'.PHP_EOL;
    var_dump($erlHook);
}
echo "done!".PHP_EOL;
?>
--EXPECT--
done!
