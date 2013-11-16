--TEST--
Test key not found result in GetOutput with no objects
--FILE--
<?php
use \Riak\Exception\NotFoundException;
include_once "connect.inc";
try {
	$client = new \Riak\Connection($host, $port);
	$bucket = new \Riak\Bucket($client, "test_bucket");
        $readdenObj = $bucket->get("Long_key_that_should_never_exists_hopefully");
        if ($readdenObj->hasObject()) {
            echo "fail!".PHP_EOL;
        }
        if (count($readdenObj->getObjectList()) > 0) {
            echo "fail2!".PHP_EOL;
        }
} catch (Exception $e) {
	var_dump($e);
}
echo "done!".PHP_EOL;
?>
--EXPECT--
done!
