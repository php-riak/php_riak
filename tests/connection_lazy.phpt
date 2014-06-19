--TEST--
Test Lazy Riak\Connection
--FILE--
<?php
include_once "connect.inc";

use Riak\BucketPropertyList;
use Riak\Connection;
use Riak\Bucket;

class RiakConnectionProxy extends Connection
{
    private $valueHolder = null;

    private $initializer = null;

    public function __construct(\Closure $initializer)
    {
        $this->initializer = $initializer;
    }

    public function setValueHolder($valueHolder)
    {
        $this->valueHolder = $valueHolder;
    }

    public function getBucket($name)
    {
        $this->initializer->__invoke($this->valueHolder);

        return $this->valueHolder->getBucket($name);
    }

    public function getHost()
    {
        $this->initializer->__invoke($this->valueHolder);

        return $this->valueHolder->getHost();
    }

    public function getPort()
    {
        $this->initializer->__invoke($this->valueHolder);

        return $this->valueHolder->getPort();
    }
}

class RiakBucketProxy extends Bucket
{
    private $valueHolder = null;

    private $initializer = null;

    public function __construct(\Closure $initializer)
    {
        $this->initializer = $initializer;
    }

    public function setValueHolder($valueHolder)
    {
        $this->valueHolder = $valueHolder;
    }

    public function setPropertyList($bucketPropertyList)
    {
       $this->initializer->__invoke($this->valueHolder);

        return $this->valueHolder->setPropertyList($bucketPropertyList);
    }

    public function getPropertyList()
    {
        $this->initializer->__invoke($this->valueHolder);

        return $this->valueHolder->getPropertyList();
    }

    public function getConnection()
    {
        $this->initializer->__invoke($this->valueHolder);

        return $this->valueHolder->getConnection();
    }

    public function getKeyList()
    {
        $this->initializer->__invoke($this->valueHolder);

        return $this->valueHolder->getKeyList();
    }
}

$connectionInitializer = function(&$valueHolder) use ($host, $port) {
    $valueHolder = $valueHolder ?: new Connection($host, $port);
};

$bucketInitializer = function(&$valueHolder) use ($connectionInitializer) {
    $valueHolder = $valueHolder ?: new \Riak\Bucket(new RiakConnectionProxy($connectionInitializer), "test_bucket");
};

$bucket       = new RiakBucketProxy($bucketInitializer);
$propList     = new BucketPropertyList(1, false);

$bucket->setPropertyList($propList);

printf("bucket property : %s\n", json_encode($bucket->getPropertyList() instanceof BucketPropertyList));
printf("bucket keys     : %s\n", json_encode(is_array($bucket->getKeyList())));
?>
--EXPECT--
bucket property : true
bucket keys     : true
