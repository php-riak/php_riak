[![Build Status](https://travis-ci.org/php-riak/php_riak.png?branch=master)](https://travis-ci.org/php-riak/php_riak)

Discussion forum: http://ost.io/@php-riak/php_riak  
Stubs for editor autocompletion at https://github.com/php-riak/php_riak_stub  

# Riak PHP extension (Basho C client branch)
PHP extension for interfacing with Riak.  
## Features
- Protobuffer client
- Persistent connections
- Auto reconnects after errors or timeouts
- Riak PHP session store replacement, that can be enabled directly in php.ini no code needed.

## Building
In a terminal make sure you are in project root, then run the following commands.
Giving the riak-c path is mandatory at the moment.

	phpize
        ./configure --with-riak-c-dir=/some/path/riak-c-client/src
	sudo make install

Module is now installed but it needs to be added to the appropriate php.ini as well, this can be done by adding the following line to your php.ini  

	extension=riak.so
Migh vary depending on you php installation though.

## ini settings
You can override the following settings in your php.ini (settings shown below are the default settings if not specified in php.ini)

        ; How many persistent connections pr. host:port so if you are connecting to 5 differenct servers there will be 20 persistent connections to each.  
        ; If you do not want to reuse connections between request set this value to 0  
        riak.persistent.connections=20
        ; If a connection has not been active within this time frame, automatically reconnect before using it again
        ; (in seconds)
        riak.persistent.timeout=1800
        ; Keep sockets alive (recommended)
        riak.socket.keep_alive=1
        ; Socket receive timeout [ms]
        riak.socket.recv_timeout=10000
        ; Socket send timeout [ms]
        riak.socket.send_timeout=10000
        ; Number of retries if riak returns an error  
        riak.default.retries=1

## Usage

### Riak session
This extension includes a session module that can replace the default PHP session handler, no code changes needed just setup the following in your php.ini:

        session.save_handler=riak
        session.save_path=proto://HOST:PORT/SESSIONBUCKET

You can specify w, dw, pw, r, rw and pr values by adding them to the session.save_path.
For instance to use a local riak with w and r values of 2:

        proto://127.0.0.1:8087/session?w=2&r=2

Please note that the session handles does NOT set any bucket properties.
Bucket properties should either be set in app.config or applied manually on the bucket before using this session handler, conflicting writes are not handled so please disable siblings on the bucket.
Recommended bucket properties:

        n_val=3
        allow_mult=false
        last_write_wins=false

The session handler does not implement garbage collection, use riak´s key-expire setting instead
See http://docs.basho.com/riak/1.3.1/tutorials/choosing-a-backend/Bitcask/ for betails on how to enable automatic key expire.

### From PHP
####Riak connection
To create a connection to Riak you simply instantiate a \Riak\Connection class.  
If no connection could be made a \Riak\Exception\ConnectionException will be thrown.
```
$connection = new \Riak\Connection('localhost', 8087);
```
Remember php_riak uses to protobuffers interface which means you should use the riak probuffers port, which by default is set to 8087.  
  
####Get/put
Since all objects in Riak are stored in buckets you start by creating a \Riak\Bucket and then you can call put to store an object.  
Below is a simple example that show how to do a put an object and get it back from Riak, the example assumes siblings(allowMult) are turned of on the bucket, which it is be by default.  
```PHP
// Create a new bucket
$bucket = new \Riak\Bucket($connection, 'bucket_name');
 
// Create a new object
$obj = new \Riak\Object('object_name');
// Set the object data that will be saved to Riak
$obj->setContent("some content to save");
// Store the object in the bucket
$bucket->put($obj);
 
// Read back the object from Riak
$response = $bucket->get('object_name');
// Make sure we got an object back
if ($response->hasObject()) {
  // Get the first returned object
  $readdenObject = $response->getFirstObject();
  echo "Object content: ".$readdenObject->getContent();
}
```

####Bucket properties (Riak 1.4 and above)
Modifying bucket properties are easy, create a \Riak\BucketPropertyList object and set the desired properties and then call setPropertyList on a \Riak\Bucket.   

 ```PHP
use \Riak\Property\ReplicationMode as RM;

$bucket = new \Riak\Bucket($connection, 'bucket_props_ext');

// Create new bucket properties
$newProps = new \Riak\BucketPropertyList();
$newProps->setSearchEnabled(true) // Enable riak search on bucket
  ->setR(1) // Set R value
  ->setNValue(1) // Set N value
  ->setW(1) // Set W value
  ->setRW(1) // Set RW value
  ->setDW(1) // Set DW value
  ->setBigVClock(5000) // Set big vclock
  ->setReplicationMode(new RM\FullSyncOnly()); // Set replication mode to fullsync only
 
// Create some post commit hooks we can set on the bucket
$postCommitHooks = new \Riak\Property\CommitHookList();
$postCommitHooks[] = new \Riak\Property\CommitHook('module', 'function');
$postCommitHooks[] = new \Riak\Property\CommitHook('js_function_name');
$newProps->setPostCommitHookList($postCommitHooks);
 
// Apply the properties
$bucket->setPropertyList($newProps);
 
// Properties are now applied on the bucket
```

####Siblings
Siblings are an important feature in Riak, if you are unsure how they work I recommend reading about Vector Clocks here http://docs.basho.com/riak/latest/theory/concepts/Vector-Clocks/  
Sibling can occur when bucket properties have allowMult set to true and multiple values are written to the same key at the same time, or without vector clocks.  

When siblings occur they should be resolved, this should preferable happen when an object is updated, this is the normal workflow for sibling resolution:  
* An object value needs to be changed.  
* The object is fetched from riak using ->get()  
* getObject is called on the output (this makes php_riak automatically call the conflict resolver, which results in a single object)  
* Update the value like normal  
* Save back the object  

The conflict is now gone since you resolved it after the read and you put the resolved version back into riak.  

What about when you don't want to update the object but just read the value?  
* Either ignore the conflict, this means that your resolver will be called on every read until next time the object is updated, 
however your resolver function should always return a single merged object so you should not se any difference on the output.  
* Or put back your resolved object, you can make your conflict resolver do a put after it have resolved the object, remember to read back the vclock as well.  
  Be very carefull when doing it this ways as you increases the chance of multiple clients trying to resolve the same conflict at the same time 
which will result in even more conflicts (read more about it here http://docs.basho.com/riak/latest/theory/concepts/Vector-Clocks/ )  
  

####Merging siblings
To deal with the siblings your code needs to merge siblings, for this purpose php_riak have a Riak\Output\ConflictResolver interface, you implement this interface and tell the \Riak\Bucket object to use this resolver when it encounters conflicts, below is a simple example that will just merge the content of the siblings.  
 ```PHP
 // Make sure allowMult = true on your bucket
 // Create a conflict resolver
class SimpleMergeResolver implements \Riak\Output\ConflictResolver {

    /**
     * Resolve or merge the conflicting objects and return one that should be store back into riak.
     * @param \Riak\ObjectList $objects
     * @return Object|null
     */
    public function resolve(\Riak\ObjectList $objects)
    {
        $result = null;
        $mergedContent = "";
        foreach ($objects as $object) {
            if (!$object->isDeleted()) {
                if (is_null($result)) {
                    // We just take the first object that is not deleted and use as base for our result
                    // that way we don't need to create a new object and copy the vclock, metadata and indexes etc.
                    $result = $object;
                }
                $mergedContent .= $object->getContent();
            }
        }
        // If we actually found a result, set the content to the merged value.
        if (isset($result)) {
            $result->setContent($mergedContent);
        }
        return $result;
    }
}

$bucket = new \Riak\Bucket($connection, 'siblings');
// Set our resolver on the bucket, to have it invoked automatically on conflicts
$bucket->setConflictResolver(new SimpleMergeResolver());

// Create an object with some data
$obj = new \Riak\Object('conflicting');
$obj->setContent('some data');
$bucket->put($obj);
// Now create a new object on same key, without reading the value first
$obj = new \Riak\Object('conflicting');
$obj->setContent('some other data');
$bucket->put($obj);


$getOutput = $bucket->get('conflicting');
// To make sure the resolver is called you should use the getObject on the output
$resolvedObject = $getOutput->getObject();
// Save back the object
$bucket->put($resolvedObject);

// Read back and ensure the sibling is now gone.
$getOutput = $bucket->get('conflicting');
echo var_export($getOutput->hasSiblings(), true).PHP_EOL;
echo var_export($getOutput->getObject()->getContent(), true).PHP_EOL;
```

####Secondary Indexes
Secondary indexes can be added to \Riak\Object by calling addIndex.  
Querying secondary indexes is also very easy, simply call the index function on an \Riak\Bucket object, se the example below:  
 ```PHP
// Create 10 objects with indexes
for ($i=0; $i<10; $i++) {
   $obj = new \Riak\Object("obj$i");
   $obj->setContent('dummy data');
   // Set a integer index num_int and a binary index text_bin
   // remember secondary index should always end on _int or _bin
   $obj->addIndex('num_int', $i);
   $obj->addIndex('text_bin', "text$i");
   // Store object
   $bucket->put($obj);
}
 
// Query for all objects where num_int = 1
$result = $bucket->index("num_int", 1);
// Result is an array of keys, in this case obj1 should be the only entry
echo "First query returned key: ".$result[0].PHP_EOL;
 
// Now make a ranged query on the text_bin index
$result = $bucket->index("text_bin", "text4", "text6");
// This query will match objects which index is text4, text5 and text6
echo "Second query returned: ";
print_r($result);
```
 
####Search
Riak have builtin full text search which can be very usefull, I will only demonstrate 
how php_riak client can query Riak search not how it works in general, to get the 
full picture jump to the http://docs.basho.com/riak/latest/dev/using/search/  

The example below demonstrates how to do a simple search.  
```PHP
// Make sure search is enabled in both riak app.config and on the bucket

// Create some test data
$testDataArr[] = '{"name": "apple","price": 2.50, "tags": ["fruit"]}';
$testDataArr[] = '{"name": "potato","price": 1.50, "tags": ["veg", "something"]}';
$testDataArr[] = '{"name": "pineapple","price": 15, "tags": ["fruit"]}';
$testDataArr[] = '{"name": "cheese", "price": 45, "tags": ["cow", "dairy"]}';
$i = 0;
foreach ($testDataArr as $testData) {
   $i++;
   $obj = new \Riak\Object("id$i");
   $obj->setContentType("application/json");
   $obj->setContent($testData);
   $bucket->put($obj);
}
 
// Perform the search
$search = new \Riak\Search\Search($connection);
$searchInput = new \Riak\Search\Input\ParameterBag();
// Search on the name field
$searchInput->setDefaultField('name');
// Now search in our search_ex_bucket after documents with the name apple
$searchResult = $search->search('search_ex_bucket', 'apple', $searchInput);
// Did we find something?
 
// Number of found documents:
echo 'Search found '.$searchResult->getNumFound().' with the name apple'.PHP_EOL;
$foundDocuments = $searchResult->getDocuments();
foreach ($foundDocuments as $document) {
   var_dump($document);
}
```  

####Counters
In Riak 1.4 basho added support CRDT counters http://docs.basho.com/riak/latest/dev/references/http/counters/  
Counters are very simple to use, and they are very useful for a lot of use cases.  
The example below shows how to get and modify counters:  
```PHP
// Make sure allowMult is set to true on the bucket, as this is required for CRDT's to work.

// Counter can be constructed in two ways
// With new:
$counter1 = new \Riak\CRDT\Counter($bucket, "counter1");
// Or with Riak\Bucket's counter function
$counter2 = $bucket->counter("counter2");
 
// All counters start at 0
// All changes to the counter value is done using increment like this
$counter1->increment(10);
// Use negative values to decrement
$counter2->increment(-10);
 
// Increment can also return the updated value
echo "Counter1 value: ".$counter1->incrementAndGet(10).PHP_EOL;
 
// The Riak\Bucket->counter function can save some typing
// the counter function will always return a counter object or throw exception
$c2val = $bucket->counter("counter2")->get();
echo "Counter2 value: ".$c2val.PHP_EOL;
```  

####MapReduce  
php_riak is able to do advanced map reduce queries including streaming results etc.  
I have not had the time to do an example yet, 
however take a look in this php_riak unittest: https://github.com/php-riak/php_riak/blob/master/tests/mapreduce.phpt and the classes here https://github.com/php-riak/php_riak_stub/tree/master/Riak/MapReduce  

## Test requirements
To make all tests succeed you need a running riak server with eleveldb backend and riak search enabled in app.config.
You also need to have seach enabled on the ´testsearch´ bucket, this can be done using the riak search-cmd like this:
```
bin/search-cmd install testsearch
```

##Documentation
Generated documentation can be found at http://phpriak.bachpedersen.dk/docs  
More documentation and examples can be found at http://phpriak.bachpedersen.dk/  
