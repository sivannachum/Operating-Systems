#include "assoc_list.h"

int main(void) {
	AssocList_t* dictionary = assoc_new();
    
    assert(assoc_size(dictionary) == 0);
    
    assoc_put(dictionary, bytes_copy_str("A"), bytes_copy_str("a"));
    assert(assoc_get(dictionary, bytes_copy_str("a")) == NULL);
    assert(assoc_get(dictionary, bytes_copy_str("B")) == NULL);
    assert(bytes_eqc(assoc_get(dictionary, bytes_copy_str("A")), "a"));
    assert(1 == assoc_size(dictionary));

    assoc_put(dictionary, bytes_copy_str("B"), bytes_copy_str("b"));
    assert(assoc_get(dictionary, bytes_copy_str("a")) == NULL);
    assert(assoc_get(dictionary, bytes_copy_str("b")) == NULL);
    assert(bytes_eqc(assoc_get(dictionary, bytes_copy_str("A")), "a"));
    assert(bytes_eqc(assoc_get(dictionary, bytes_copy_str("B")), "b"));
    assert(2 == assoc_size(dictionary));

    assoc_clear(dictionary);
    assert(assoc_size(dictionary) == 0);
    
    assoc_put(dictionary, bytes_copy_str("A"), bytes_copy_str("a"));
    assoc_put(dictionary, bytes_copy_str("B"), bytes_copy_str("b"));
    assoc_remove(dictionary, bytes_copy_str("B"));
    
    assert(assoc_get(dictionary, bytes_copy_str("a")) == NULL);
    assert(assoc_get(dictionary, bytes_copy_str("B")) == NULL);
    assert(bytes_eqc(assoc_get(dictionary, bytes_copy_str("A")), "a"));
    assert(1 == assoc_size(dictionary));
    
    assoc_remove(dictionary, bytes_copy_str("A"));
    assert(0 == assoc_size(dictionary));

    return 0;
}