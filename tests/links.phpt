--TEST--
Test links gets written and read correctly
--FILE--
<?php
use \Riak\Link;
use \Riak\Object;
use \Riak\BucketPropertyList;

include_once "connect.inc";
$client = new \Riak\Connection($host, $port);
$bucket = new \Riak\Bucket($client, "test_bucket");
$props = new BucketPropertyList(3, false);
$bucket->setPropertyList($props);

$obj = new Object("key1");
try {
    for ($i=0; $i<10; $i++) {
        $obj2 = new Object("key_with_link_$i");
        $bucket->put($obj2);
        // add link to obj
        $obj->addLink(new Link("link$i", "test_bucket", $obj2->getKey()));
    }
    $bucket->put($obj);

    $output = $bucket->get("key1");
    $objs = $output->getObjectList();
    $readdenObj = $objs[0];
    $success = true;
    $readdenLinks = $readdenObj->getLinkList();
    if (count($readdenLinks) == 10) {
        $i = 0;
        foreach ($readdenLinks as $link) {
            if (strcmp($link->getTag(), "link$i") !== 0) {
                $success = false;
                echo "expected tag name to be 'link$i' but was '".$link->tag."'".PHP_EOL;
            }
            if (strcmp($link->getBucketName(), 'test_bucket') !== 0) {
                $success = false;
                echo "expected bucket name to be 'test_bucket' but was '".$link->bucket."'".PHP_EOL;
            }
            if (strcmp($link->getKey(), "key_with_link_$i") !== 0) {
                $success = false;
                echo "expected key name to be 'key_with_link_$i' but was '".$link->key."'".PHP_EOL;
            }
            $i++;
        }
    } else {
        $success = false;
        var_dump($readdenLinks);
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
