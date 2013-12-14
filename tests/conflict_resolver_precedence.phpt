--TEST--
Test conflict resolver precedence
--FILE--
<?php

include_once "connect.inc";

use Riak\ObjectList;
use Riak\Output\ConflictResolver;

class MyBuketResolver implements ConflictResolver 
{
    public function resolve(ObjectList $objects)
    {
        echo __CLASS__ . PHP_EOL;

        $winner = $objects->first();
        $value  = 0;

        foreach ($objects as $object) {
            $value += intval($object->getContent());
        }

        $winner->setContent($value);

        return $winner;
    }
}

class MyKeyResolver implements ConflictResolver 
{
    public function resolve(ObjectList $objects)
    {
        echo __CLASS__ . PHP_EOL;

        $winner = $objects->first();
        $value  = 0;

        foreach ($objects as $object) {
            $value += intval($object->getContent());
        }

        $winner->setContent($value);

        return $winner;
    }
}

$key            = uniqid();
$keyResolver    = new MyKeyResolver();
$buketResolver  = new MyBuketResolver();
$object1        = new \Riak\Object($key);
$object2        = new \Riak\Object($key);
$client         = new \Riak\Connection($host, $port);
$bucket         = new \Riak\Bucket($client, 'test_bucket');
$options        = new \Riak\Input\GetResolverInput($keyResolver);
$props          = new \Riak\BucketPropertyList($nVal = 1, $allowMult = true);

$bucket->delete($key);
$bucket->setPropertyList($props);
$bucket->setConflictResolver($buketResolver);

$object1->setContent(1);
$object2->setContent(2);

$bucket->put($object1);
$bucket->put($object2);

$output = $bucket->get($key, $options);
$object = $output->getObject();
// New put the winner back
$bucket->put($object);

echo "Siblings          : " . count($output->getObjectList()) . PHP_EOL;
echo "Read Winner       : " . $bucket->get($key)->getFirstObject()->getContent() . PHP_EOL;
echo "Has Siblings      : " . var_export($bucket->get($key)->hasSiblings(), true) . PHP_EOL;

$bucket->delete($key);

?>
--EXPECT--
MyKeyResolver
Siblings          : 2
Read Winner       : 3
Has Siblings      : false
