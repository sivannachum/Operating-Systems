#include "hashtable.h"

int main(void) {
    HashTable_t* dictionary = hash_new();    
    assert(hash_size(dictionary) == 0);
    assert(dictionary->num_buckets == 0);
    
    hash_put(dictionary, bytes_copy_str("A"), bytes_copy_str("a"));
    assert(dictionary->num_buckets == 4);
    assert(hash_get(dictionary, bytes_copy_str("a")) == NULL);
    assert(hash_get(dictionary, bytes_copy_str("B")) == NULL);
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("A")), "a"));
    assert(1 == hash_size(dictionary));

    hash_put(dictionary, bytes_copy_str("B"), bytes_copy_str("b"));
    assert(hash_get(dictionary, bytes_copy_str("a")) == NULL);
    assert(hash_get(dictionary, bytes_copy_str("b")) == NULL);
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("A")), "a"));
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("B")), "b"));
    hash_remove(dictionary, bytes_copy_str("Nonexistant string"));
    assert(2 == hash_size(dictionary));

    hash_put(dictionary, bytes_copy_str("hello"), bytes_copy_str("world"));
    hash_put(dictionary, bytes_copy_str("Hello"), bytes_copy_str("World"));
    hash_put(dictionary, bytes_copy_str("3455"), bytes_copy_str("324"));
    hash_put(dictionary, bytes_copy_str("123"), bytes_copy_str("456"));
    hash_put(dictionary, bytes_copy_str("where"), bytes_copy_str("here"));
    hash_put(dictionary, bytes_copy_str("when"), bytes_copy_str("now"));
    hash_put(dictionary, bytes_copy_str("why"), bytes_copy_str("because"));
    hash_put(dictionary, bytes_copy_str("how"), bytes_copy_str("some"));
    assert(dictionary->num_buckets == 7);
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("A")), "a"));
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("hello")), "world"));
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("Hello")), "World"));
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("3455")), "324"));
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("123")), "456"));
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("where")), "here"));
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("when")), "now"));
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("why")), "because"));
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("how")), "some"));

    hash_remove(dictionary, bytes_copy_str("A"));
    hash_remove(dictionary, bytes_copy_str("hello"));
    hash_remove(dictionary, bytes_copy_str("123"));
    hash_remove(dictionary, bytes_copy_str("when"));
    hash_remove(dictionary, bytes_copy_str("why"));
    hash_remove(dictionary, bytes_copy_str("how"));
    hash_remove(dictionary, bytes_copy_str("where"));
    hash_remove(dictionary, bytes_copy_str("B"));
    assert(hash_get(dictionary, bytes_copy_str("A")) == NULL);
    assert(hash_get(dictionary, bytes_copy_str("hello")) == NULL);
    assert(hash_get(dictionary, bytes_copy_str("123")) == NULL);
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("Hello")), "World"));
    assert(dictionary->num_buckets == 5);
    hash_put(dictionary, bytes_copy_str("Hello"), bytes_copy_str("aab"));
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("Hello")), "aab"));
    assert(2 == hash_size(dictionary));


    hash_clear(dictionary);
    assert(hash_size(dictionary) == 0);
    assert(dictionary->num_buckets == 0);
    
    hash_put(dictionary, bytes_copy_str("A"), bytes_copy_str("a"));
    hash_put(dictionary, bytes_copy_str("B"), bytes_copy_str("b"));
    hash_remove(dictionary, bytes_copy_str("B"));
    assert(dictionary->num_buckets == 4);
    
    assert(hash_get(dictionary, bytes_copy_str("a")) == NULL);
    assert(hash_get(dictionary, bytes_copy_str("B")) == NULL);
    assert(bytes_eqc(hash_get(dictionary, bytes_copy_str("A")), "a"));
    assert(1 == hash_size(dictionary));
    
    hash_remove(dictionary, bytes_copy_str("A"));
    assert(hash_get(dictionary, bytes_copy_str("A")) == NULL);
    assert(0 == hash_size(dictionary));

    hash_clear(dictionary);

    return 0;
}
