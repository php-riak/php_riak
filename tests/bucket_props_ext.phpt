--TEST--
Test bucket properties
--FILE--
<?php
use \Riak\BucketPropertyList;
use \Riak\Property\ReplicationMode as RM;
include_once "connect.inc";
$client = new \Riak\Connection($host, $port);
$bucket = new \Riak\Bucket($client, "test_bucket_asdf_props_ext");
$oldProps = $bucket->getPropertyList();

$newProps = new BucketPropertyList();
$postCommitHooks = new \Riak\Property\CommitHookList();
$postCommitHooks[] = new \Riak\Property\CommitHook('erl_module', 'erl_function');
$postCommitHooks[] = new \Riak\Property\CommitHook('js_name');
$newProps->setSearchEnabled(true)
         ->setR(1)
         ->setNValue(1)
         ->setW(1)
         ->setRW(1)
         ->setDW(1)
         ->setPostCommitHookList($postCommitHooks)
         ->setBigVClock(5000)
         ->setReplicationMode(new RM\FullSyncOnly());

$bucket->setPropertyList($newProps);
$currentProps = $bucket->getPropertyList();
if ($currentProps->getNValue() === 1 &&
    $currentProps->getAllowMult() === false &&
    $currentProps->getSearchEnabled() === true &&
    $currentProps->getR() === 1 &&
    $currentProps->getW() === 1 &&
    $currentProps->getDW() === 1 &&
    $currentProps->getRW() === 1 &&
    $currentProps->getBigVClock() === 5000) {
    $postHooks = $currentProps->getPostCommitHookList();
    $foundJs = false;
    $foundErl = false;
    $replMode = $currentProps->getReplicationMode();
    if (!is_a($replMode, 'Riak\Property\ReplicationMode\FullSyncOnly')) {
        var_dump($replMode);
    }
    foreach ($postHooks as $hook) {
        if ($hook->isJavascript() && $hook->getJsName() == "js_name") {
            $foundJs = true;
        }
        if ($hook->isErlang() && $hook->getErlModule() == "erl_module" && $hook->getErlFunction() == "erl_function") {
            $foundErl = true;
        }
    }
    if ($foundErl && $foundJs) {
        echo "success!";
    } else {
        echo "could not verify hooks".PHP_EOL;
        var_dump($postHooks);
    }
} else {
    var_dump($currentProps);
}
$bucket->setPropertyList($oldProps);
?>
--EXPECT--
success!
