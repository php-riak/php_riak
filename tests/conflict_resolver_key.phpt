--TEST--
Test key conflict resolver
--FILE--
<?php

include_once "connect.inc";

use Riak\ObjectList;
use Riak\Output\ConflictResolver;

class MyGetResolver implements ConflictResolver 
{
    public function resolve(ObjectList $objects)
    {
        $winner = $objects->first();
        $value  = 0;

        foreach ($objects as $object) {
            $value += intval($object->getContent());
        }

        $winner->setContent($value);

        return $winner;
    }
}

$key      = uniqid();
$resolver = new MyGetResolver();
$object1  = new \Riak\Object($key);
$object2  = new \Riak\Object($key);
$client   = new \Riak\Connection($host, $port);
$bucket   = new \Riak\Bucket($client, 'test_bucket');
$options  = new \Riak\Input\GetResolverInput($resolver);
$props    = new \Riak\BucketPropertyList($nVal = 1, $allowMult = true);

$bucket->setPropertyList($props);
$bucket->delete($key);

$object1->setContent(1);
$object2->setContent(2);

$bucket->put($object1);
$bucket->put($object2);

$output = $bucket->get($key, $options);
$object = $output->getObject();
// New put the winner back
$bucket->put($object);

echo "Has object        : " . var_export($output->hasObject(), true) . PHP_EOL;
echo "Siblings          : " . count($output->getObjectList()) . PHP_EOL;
echo "Get/Set Resolver  : " . var_export($resolver === $options->getConflictResolver(), true) . PHP_EOL;
echo "Winner value      : " . $object->getContent() . PHP_EOL;
echo "Has Siblings      : " . var_export($bucket->get($key)->hasSiblings(), true) . PHP_EOL;
echo "Read Winner       : " . $bucket->get($key)->getFirstObject()->getContent() . PHP_EOL;

$bucket->delete($key);

?>
--EXPECT--
Has object        : true
Siblings          : 2
Get/Set Resolver  : true
Winner value      : 3
Has Siblings      : false
Read Winner       : 3
