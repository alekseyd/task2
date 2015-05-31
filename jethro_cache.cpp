#include "jethro_cache.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <memory>

using namespace std;

int main(int argc, char* argv[])
{
    JethroHash<string> h;

    std::cout << "bucket_count = " << h.table.size() <<endl;

    h.inc("abc");
    h.inc("cdef");
    h.inc("abc");
    h.inc("akajlknsqlkdnslkfnmldksfmls");
    h.inc("1234akajlknsqlkdnslkfnmldksfmls");

    std::hash<string> str_hash;
    //for (auto &x: h) {
    //    std::cout << x.first << " -> " << x.second << " ("<< str_hash(x.first) << ")" <<endl;
    //}

    std::cout << "bucket_count = " << h.table.size() <<endl;

//----------------------------------
    std::cout << sizeof(typename JethroHash<string>::Container::value_type)<<endl;

    return 0;
}

//JethroHash
static const uint16_t one=1;
template <class T>
size_t JethroHash<T>::inc(const T& key)
{

    auto hash_index = constrain_hash(key);
    cout << key << " -> " << constrain_hash(key) << endl;
    auto &bucket = table[hash_index];

    size_t count = 0;
    for (int i = 0; i<3; ++i) {
        table_element_16* p = atomic_load(&(table[hash_index][i]));
        if ( p == nullptr ) {
            unique_ptr<table_element_16> np = make_unique<table_element_16>(); //make_unique_ptr
            np->length = key.size();
            strncpy(np->key, key.c_str(), np->length);
            table_element_16* my_null_ptr(nullptr);
            if (not atomic_compare_exchange_strong(&(table[hash_index][i]), &my_null_ptr, np.get())) {
                count = inc(key);   //try to count once again, should a slot in a bucker is already taken
                                    //memory acquired for np will be free'd automatically
            }else{
                p = np.release();
                p->bucket_id = hash_index;
                p->offset = i;
            }
        }
        count = 1 + atomic_fetch_add(&(p->count), one);
    }
    return count;
}

template <typename T>
size_t JethroHash<T>::get(const T& key)
{
    size_t count = 0;

    auto hash_index = constrain_hash(key);
    for (int i = 0; i<3; ++i) {
        table_element_16* p = atomic_load(&(table[hash_index][i]));
        if ( p == nullptr ) {
            break;
        }else{
            if (strncpy(p->key, key.c_str(), p->length) == 0) {
                count = p->count;
                break;
            }
        }
    }

    return count;
}

template <typename T>
bool JethroHash<T>::clear(const T& key)
{
    return true; //vec.erase(key);
}

