--TEST--
Test conflict resolver exceptions
--FILE--
<?php

include_once "connect.inc";

use Riak\ObjectList;
use Riak\Output\ConflictResolver;

class DummyResolver implements ConflictResolver 
{
    public $return;

    public function __constructor($return = null)
    {
        $this->return = $return;
    }

    public function resolve(ObjectList $objects)
    {
        return $this->return;
    }
}

$key      = uniqid();
$resolver = new DummyResolver();
$object1  = new \Riak\Object($key);
$object2  = new \Riak\Object($key);
$client   = new \Riak\Connection($host, $port);
$props    = new \Riak\BucketPropertyList($nVal = 1, $allowMult = true);
$bucket   = new \Riak\Bucket($client, 'test_bucket');

$bucket->setPropertyList($props);
$object1->setContent(1);
$object2->setContent(2);

$bucket->put($object1);
$bucket->put($object2);

try {
    $output = $bucket->get($key);
    $object = $output->getObject();
} catch (Exception $e) {
    echo get_class($e) . ' : ' . $e->getMessage() . PHP_EOL;
}


$bucket->setConflictResolver($resolver);

try {
    $resolver->return = new \Riak\Object();

    $output = $bucket->get($key);
    $object = $output->getObject();
} catch (Exception $e) {
    echo get_class($e) . ' : ' . $e->getMessage() . PHP_EOL;
}

try {
    $resolver->return = new \stdClass();

    $output = $bucket->get($key);
    $object = $output->getObject();
} catch (Exception $e) {
    echo get_class($e) . ' : ' . $e->getMessage() . PHP_EOL;
}

try {
    $resolver->return = 0;

    $output = $bucket->get($key);
    $object = $output->getObject();
} catch (Exception $e) {
    echo get_class($e) . ' : ' . $e->getMessage() . PHP_EOL;
}

try {
    $resolver->return = null;

    $output = $bucket->get($key);
    $object = $output->getObject();
} catch (Exception $e) {
    echo get_class($e) . ' : ' . $e->getMessage() . PHP_EOL;
}

$bucket->delete($key);

?>
--EXPECT--
Riak\Exception\NonUniqueException : GetOutput contains unresolved siblings
Riak\Exception\UnresolvedConflictException : The resolved Riak\Object does not contain a valid key.
Riak\Exception\UnresolvedConflictException : Conflict resolver should return a instance of Riak\Object
Riak\Exception\UnresolvedConflictException : Conflict resolver should return a instance of Riak\Object
Riak\Exception\UnresolvedConflictException : Conflict resolver should return a instance of Riak\Object
