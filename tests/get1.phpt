--TEST--
Simple get test
--FILE--
<?php
use \Riak\BucketPropertyList;
use \Riak\Input\GetInput;
include_once "connect.inc";
$client = new RiakClient($host, $port);
$bucket = new RiakBucket($client, "test_bucket");
$props = new BucketPropertyList(3, false);
$bucket->applyProperties($props);

try {
        $obj = new RiakObject("get_head");
        $obj->data = "test-get plap";
        $bucket->put($obj);
        $cfg = new GetInput();
        $cfg->setReturnHead(true);
        $output = $bucket->get("get_head", $cfg);
        $objs = $output->getObjectList();
        if (isset($objs[0]->data) && strlen($objs[0]->data > 0)) {
            echo "Did not expect returned object to have data, when return_head is set".PHP_EOL;
            var_dump($objs);
        }
        $obj = new RiakObject("get_test");
	$obj->contentType = "text/plain";
	$obj->data = "test-get plap";
        $obj->metadata["test"] = "test";
        $bucket->put($obj);
        $output = $bucket->get("get_test");
        if ($output->hasSiblings()) {
            echo "Did not expect siblings".PHP_EOL;
            var_dump($output);
        }
        $objs = $output->getObjectList();
        if (strcmp($objs[0]->data, $obj->data) == 0 || strcmp($objs[0]->metadata["test"], "test") !== 0 ) {
		echo "success!";
	}
} catch (Exception $e) {
	var_dump($e);
}
?>
--EXPECT--
success!
