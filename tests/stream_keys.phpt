--TEST--
Test bucket stream keys
--FILE--
<?php
use \Riak\BucketPropertyList;
include_once "connect.inc";
$client = new RiakClient($host, $port);
$bucket = new RiakBucket($client, "test_stream_keys");
$props = new BucketPropertyList(3, false);
$bucket->applyProperties($props);

class KeyStreamer implements RiakKeyStreamer {
    public $cnt = 0;
    public function key($key) {
        if (strcmp(substr($key, 0, 6), "stream") == 0) {
            $this->cnt++;
        }
    }
}

try {
    // Make some objects we can list afterwards
    for ($i=0; $i<20; $i++) {
        $obj = new RiakObject("stream$i");
        $obj->data = "test-get plap";
        $bucket->put($obj);
    }
    $streamer = new KeyStreamer();
    $bucket->streamKeys($streamer);
    if ($streamer->cnt == 20) {
        echo "success!".PHP_EOL;
    } else {
        var_dump($streamer);
    }
    for ($i=0; $i<20; $i++) {
        $bucket->delete("stream$i");
    }
} catch (Exception $e) {
    var_dump($e);
}
?>
--EXPECT--
success!
