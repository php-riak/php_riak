--TEST--
Simple get test
--FILE--
<?php
use \Riak\BucketPropertyList;
use \Riak\Input\GetInput;
include_once "connect.inc";
$client = new \Riak\Connection($host, $port);
$bucket = new \Riak\Bucket($client, "test_bucket");
$props = new BucketPropertyList(3, false);
$bucket->setPropertyList($props);

try {
        $obj = new \Riak\Object("get_head");
        $obj->setContent("test-get plap");
        $bucket->put($obj);
        $cfg = new GetInput();
        $cfg->setReturnHead(true);
        $output = $bucket->get("get_head", $cfg);
        if (!$output->hasObject()) {
            echo "Output did not contain an object".PHP_EOL;
            var_dump($output);
        }
        $obj = $output->getFirstObject();
        if (!is_null($obj->getContent()) && strlen($obj->getContent() > 0)) {
            echo "Did not expect returned object to have data, when return_head is set".PHP_EOL;
            var_dump($obj);
        }
        $obj = new \Riak\Object("get_test");
        $obj->setContentType("text/plain");
        $obj->setContent("test-get plap");
        $obj->addMetadata("test", "test");
        $bucket->put($obj);
        $output = $bucket->get("get_test");
        if ($output->hasSiblings()) {
            echo "Did not expect siblings".PHP_EOL;
            var_dump($output);
        }
        $readdenObj = $output->getFirstObject();
        $meta = $readdenObj->getMetadataMap();
        if (strcmp($readdenObj->getContent(), $obj->getContent()) == 0 || strcmp($meta["test"], "test") !== 0 ) {
		echo "success!";
	}
} catch (Exception $e) {
	var_dump($e);
}
?>
--EXPECT--
success!
