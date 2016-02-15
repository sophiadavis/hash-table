import hashtable

import string
import unittest

def my_hash(obj):
    print "now hashing " + str(obj)
    if isinstance(obj, int):
        # http://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
        return obj*2654435761 % 2**32
    if isinstance(obj, float):
        return int(math.ceil(obj*2654435761 % 2**32))
    else:
        # http://stackoverflow.com/questions/2511058/persistent-hashing-of-strings-in-python
        ord3 = lambda x : '%.3d' % ord(x)
        return int(''.join(map(ord3, obj)))

class TestHashTable(unittest.TestCase):

    def setUp(self):
        self.h = hashtable.HashTable()

    def test_initialization_with_invalid_size(self):
        with self.assertRaisesRegexp(TypeError, "size parameter must be a positive integer."):
            h = hashtable.HashTable(size = -1)
        with self.assertRaisesRegexp(TypeError, "size parameter must be a positive integer."):
            h = hashtable.HashTable(size = 0)

    def test_initialization_with_invalid_max_load(self):
        with self.assertRaisesRegexp(TypeError, "max_load parameter must be a float between 0.0 and 1.0."):
            h = hashtable.HashTable(max_load = 2)
        with self.assertRaisesRegexp(TypeError, "max_load parameter must be a float between 0.0 and 1.0."):
            h = hashtable.HashTable(max_load = -0.5)

    def test_initialization_with_invalid_hash_function(self):
        h = hashtable.HashTable(hash_func = my_hash)
        self.assertRegexpMatches(str(h.hash_func), r"<function my_hash at .*>")

    def test_valid_initialization(self):
        h = hashtable.HashTable(hash_func = my_hash)
        self.assertRegexpMatches(str(h.hash_func), r"<function my_hash at .*>")
        self.assertEqual(h.size, 4) # defaults
        self.assertEqual(h.max_load, 0.5) # defaults

        h = hashtable.HashTable(size = 8)
        self.assertRegexpMatches(str(h.hash_func), r"<built-in function hash>") # defaults
        self.assertEqual(h.size, 8)
        self.assertEqual(h.max_load, 0.5) # defaults

        h = hashtable.HashTable(max_load = 0.25)
        self.assertRegexpMatches(str(h.hash_func), r"<built-in function hash>") # defaults
        self.assertEqual(h.size, 4) # defaults
        self.assertEqual(h.max_load, 0.25)

    def test_set_and_get(self):
        self.assertEqual(self.h.load, 0)
        for i in range(10):
            self.h.set(i, i)
            self.assertEqual(self.h.load, i + 1)

        for i in range(10):
            self.assertEqual(self.h.get(i), i)
            self.assertEqual(self.h.load, 10)

        for i in range(10):
            self.h.set(float(i)/2, float(i)/2)
            self.assertEqual(self.h.load, 10 + i + 1)

        for i in range(10):
            self.assertEqual(self.h.get(float(i)/2), float(i)/2)
            self.assertEqual(self.h.load, 20)

        for c in string.ascii_lowercase:
            self.h.set(c, c)
            self.assertEqual(self.h.load, 10 + 10 + string.ascii_lowercase.find(c) + 1)

        for c in string.ascii_lowercase:
            self.assertEqual(self.h.get(c), c)
            self.assertEqual(self.h.load, 46)

    def test_set_pop_and_load(self):
        self.assertEqual(self.h.load, 0)
        for i in range(10):
            self.h.set(i, i)
            self.assertEqual(self.h.load, i + 1)

        for i in range(10):
            self.assertEqual(self.h.pop(i), i)
            self.assertEqual(self.h.load, 10 - (i + 1))

        for i in range(10):
            self.h.set(float(i)/2, float(i)/2)
            self.assertEqual(self.h.load, i + 1)

        for i in range(10):
            self.assertEqual(self.h.pop(float(i)/2), float(i)/2)
            self.assertEqual(self.h.load, 10 - (i + 1))

        for c in string.ascii_lowercase:
            self.h.set(c, c)
            self.assertEqual(self.h.load, string.ascii_lowercase.find(c) + 1)

        for c in string.ascii_lowercase:
            self.assertEqual(self.h.pop(c), c)
            self.assertEqual(self.h.load, 26 - (string.ascii_lowercase.find(c) + 1))

    def test_updating_values(self):
        self.h.set(1, 2)
        self.h.set(1, "hello")
        self.assertEqual(self.h.get(1), "hello")

        self.h.set(1, 3.3)
        self.assertEqual(self.h.get(1), 3.3)

        self.h.set("astring", "astring")
        self.h.set("astring", 49)
        self.assertEqual(self.h.get("astring"), 49)

        self.h.set("astring", 3.3)
        self.assertEqual(self.h.get("astring"), 3.3)

if __name__ == '__main__':
    unittest.main()
