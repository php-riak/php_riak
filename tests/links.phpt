--TEST--
Test links gets written and read correctly
--FILE--
<?php
include_once "connect.inc";
$client = new RiakClient($host, $port);
$bucket = new RiakBucket($client, "test_bucket");
$props = new RiakBucketProperties(3, false);
$bucket->applyProperties($props);

$obj = new RiakObject("key1");
try {
    $obj->contentType = "text/plain";
    $obj->data = "test-get plap";

    for ($i=0; $i<10; $i++) {
        $obj2 = new RiakObject("key_with_link_$i");
        $obj2->contentType = "text/plain";
        $obj2->data = "dummy";
        $bucket->put($obj2);

        // add links to obj
        $obj->links[] = new RiakLink("link$i", "test_bucket", $obj2->key);
    }
    $bucket->put($obj);

    $readdenObj = $bucket->get("key1");
    $success = true;
    if (count($readdenObj->links) == 10) {
        $i = 0;
        foreach ($readdenObj->links as $link) {
            if (strcmp($link->tag, "link$i") !== 0) {
                $success = false;
                echo "expected tag name to be 'link$i' but was '".$link->tag."'".PHP_EOL;
            }
            if (strcmp($link->bucket, 'test_bucket') !== 0) {
                $success = false;
                echo "expected bucket name to be 'test_bucket' but was '".$link->bucket."'".PHP_EOL;
            }
            if (strcmp($link->key, "key_with_link_$i") !== 0) {
                $success = false;
                echo "expected key name to be 'key_with_link_$i' but was '".$link->key."'".PHP_EOL;
            }
            $i++;
        }
    } else {
        $success = false;
        var_dump($readdenObj->links);
    }
    if ($success) {
        echo "success!";
    }
} catch (Exception $e) {
        var_dump($e);
}
?>
--EXPECT--
success!
