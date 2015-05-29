Cache requirments:
------------------
- First of all, no matter what is the hash we are going to use, cache should be large enough to accomodate 100M records with as less as possible collisions amount
- We do not want to get into rehashing, i.e. we want the hash function to distribute its results evenly... let's use only well known  good string hashes, and start with **stdlib**  hash implementation for strings
- Size of hash table -- we want to allocate as much as available RAM permits:

Hash table and bucket structure
--------------------------------
Each bucket contains series of pointers to equivalent elements, i.e. elements with the same hash value
    * Initial bucket size:  3; then grow per rule:  8, 16, 32, 64, 128, 192, 256... We will take memory blocks for extending from memory pools of size 3, 5, 8, 16, 32, 64 words
    * Memory of single bucket  is constructed thru  linked list (previous mem block finishes with pointer to the next one in the bucket or *null_ptr*)

Every bucket is indexed inside the hash table by that common hash value.

Element structure
-----------------
Basically, our cache element is just  tuple of string **key** and its **counter**.
But in "real" life  the element is more complex structure with **key** and  **counter** beign main publicly accessible members.
Technically cache element is a class with following members

   * bucket's index in the hash table (that's a hash of element's key)
   * offset of given key in the bucket
   * key counter  (i.e. how many times we encountered given string so far)
   * key length
   * key itself (probably, without zero)

All the elements are allocated from series of memory pools with block size adjusted to key length.
We wiil use pools based the on following sizes

:NOTE: what I think requires tuning is:
    - what are the pool sizes we are going to  use for memory allocation  for cache elements;
    - how many blocks are allocated at once  by pool when no more free blocks are available. For time being let's stick to 64;
    - how should bucket grow; what are the pool associated with bucket growth mechanism

Multhreading issues
-------------------
  #. Reading doesn't change state of the cache, that is we have no probs running
  #. Access for changes -- it's going to be

Result output
------------------
  1. *find by key* operation seems pretty obvious
        * calculate hash from key
        * in a bucket indexed by hash find walk thru the relevant pointer and jump to the element we need.

  2. *bulk output* -- we can easily enumerate all the keys by walking thru ALL elements contained in the cache (they all reside inside the pools.)
