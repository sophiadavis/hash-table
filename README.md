## Customizable HashTables

----------

Sophia Davis  
October 2014

----------
### C Program
This is a hashtable implementation in C that allows users to experiment with how hashtable parameters impact performance. The hashtable consists of an array of "bins". Key-value pairs are stored based on the hash of the key -- this hash is used to determine which bin the key-value pair should be assigned to. Each bin stores a linked list of all key-value pairs assigned to that bin.  

In the initialization function, the user can specify the initial number of bins and the maximum load proportion. The maximum load proportion is a ratio of number of key-value pairs to total number of bins. When this ratio is reached, the hashtable will "resize" itself -- creating a new bin array with double the number of bins in the original hashtable. All key-value pairs will be reassigned based on this new bin-size. The user also controls the hash function used to hash each key, because the hash associated with each key must be passed in to functions for adding to, searching, or removing from the hashtable. If no hash is specified (or rather, HUGE_VAL is passed in for the hash value), a very pathetic hash function is used. Keys and values can be strings, integers, or floats. 

### Usage (C API)

I've put some examples of how to interact with the C interface in the `main` function of `hashtable.c`. To run this program, just compile and run `hashtable.c`. For example: 
 
```
clang hashtable.c -o hash   
./hash
```

----------
### Python (2) Bindings
So that's cool, I guess. However, the main purpose of this project was to learn a bit about how to write a C extension for Python (see the awesome [docs](https://docs.python.org/2/c-api/) and [tutorial](https://docs.python.org/2/extending/extending.html)). That's in `hashtablemodule.c` (and also `hashtablemodule_helpers.c`). In order to use the Python extension, run the `setup.py` file -- which is kind of like a Makefile for Python modules. This will output a `hashtable.so` binary file inside a a `build/lib(/Python Version/)` subdirectory. If you're in the same directory as this `hashtable.so` file, your Python programs can use my C hashtables!  

### Usage (Python API)  
```
python setup.py build
```
Grab the new `hashtable.so` file from the `build/lib(/Python Version/)` subdirectory. You can look at the unittests in the `hashtable_tests.py` to get an idea of what it can do. Here's a summary:  
``` python  
import hashtable  
my_hashtable = hashtable.HashTable()
my_hashtable.size ## => 4 (default)
my_hashtable.max_load ## => 0.5 (default)
my_hashtable.hash_func ## => <built-in function hash> (defaults to Python's built in hash function)
my_hashtable.load ## => 0
my_hashtable.set("hello", 3.14159) ## updates my_hashtable and returns None
my_hashtable.load ## => 1
print my_hashtable ## => *beautiful textual representation of a bin array with linked lists*

	## We can also specify a different initial bin size, maximum load proportion, 
	##		and hash function:  
	## This is a hash function of my own creation. If you pass in an int or a float, 
	## it does this one thing I found on StackOverflow. If you pass in a string, it 
	## does this other thing I found on SO. Clearly an awesome hash function.
def my_hash(obj):
    if isinstance(obj, int):
        # http://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
        return obj*2654435761 % 2**32
    if isinstance(obj, float):
        return int(math.ceil(obj*2654435761 % 2**32))
    else:
        # http://stackoverflow.com/questions/2511058/persistent-hashing-of-strings-in-python
        ord3 = lambda x : '%.3d' % ord(x)
        return int(''.join(map(ord3, obj)))
        
	## If we instantiate a hashtable with this custom hash function, 
	##		my C program will call the Python function!
h = hashtable.HashTable(size = 8, max_load = 0.90, hash_func = my_hash)

h.set("hello", "world") ## updates h and returns None
h.load ## => 1
print h
h.get("hello") ## => "world"
h.pop("hello") ## => "world"
h.get("hello") ## => None 
``` 	
I'd still like to explore how size, maximum load proportion, and hash function impact hashtable performance, but it is guaranteed to be worse than Python's native Dictionary ([source](http://svn.python.org/projects/python/trunk/Objects/dictobject.c)). 