--TEST--
Test bucket list keys
--FILE--
<?php
use \Riak\BucketPropertyList;
use \Riak\Object;
include_once "connect.inc";
$client = new \Riak\Connection($host, $port);
$bucket = new RiakBucket($client, "test_list_keys");
$props = new BucketPropertyList(3, false);
$bucket->applyProperties($props);
try {
    // Make some objects we can list afterwards
    for ($i=0; $i<20; $i++) {
        $obj = new Object("list$i");
        $obj->setContent("test-get plap");
        $bucket->put($obj);
    }
    $keys = $bucket->listKeys();
    $foundcnt = 0;
    foreach ($keys as $key) {
        $keystart = substr($key, 0, 4);
        if (strcmp($keystart, "list") == 0) {
            $foundcnt++;
        }
    }
    if ($foundcnt == 20) {
        echo "success!".PHP_EOL;
    } else {
        var_dump($keys);
    }
    for ($i=0; $i<20; $i++) {
        $bucket->delete("list$i");
    }
} catch (Exception $e) {
    var_dump($e);
}
?>
--EXPECT--
success!
