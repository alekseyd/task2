#include "jethro_cache.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <memory>

using namespace std;

int main(int argc, char* argv[])
{
    JethroHash<string> h;
    //TESTING is done here so far

    std::cout << "bucket_count = " << h.table.size() <<endl;

    h.inc("abc");
    h.inc("cdef");
    h.inc("abc");
    h.inc("akajlknsqlkdnslkfnmldksfmls");
    h.inc("1234akajlknsqlkdnslkfnmldksfmls");

    //for (auto &x: h) {
    //    std::cout << x.first << " -> " << x.second << " ("<< str_hash(x.first) << ")" <<endl;
    //}

    for (auto &x: {"abc", "cdef", "1234akajlknsqlkdnslkfnmldksfmls","mmmmdmd"}) {
        std::cout << x << " -> " << h.get(x) << endl;
    }

    std::cout << "bucket_count = " << h.table.size() <<endl;

//----------------------------------
    std::cout << sizeof(typename JethroHash<string>::Table::value_type)<<endl;

    return 0;
}

//JethroHash
template <class T>
size_t JethroHash<T>::inc(const T& key)
{
    auto hash_index = constrain_hash(key);
    cout << key << " -> " << constrain_hash(key) << endl;
    auto &bucket = table[hash_index];

    size_t count = 0;
    for (int i = 0; i<BUCKET_SIZE; ++i) {
        table_element* p = atomic_load(&(table[hash_index][i]));
        if ( p == nullptr ) {
            //void* vp = operator new (sizeof(table_element) + key.size());
            //unique_ptr<table_element> np(new (vp) table_element(hash_index, i, key));
            unique_ptr<table_element> np(new (key.size()) table_element(hash_index, i, key));
            table_element* my_null_ptr(nullptr);
            if (not atomic_compare_exchange_strong(&(table[hash_index][i]), &my_null_ptr, np.get())) {
                return inc(key);    //try to count once again, should a slot in a bucket is already taken
                                    //memory acquired for np will be free'd automatically thanks to unique_ptr
            }
            p = np.release();
        }else{
            //if we happen to get different keys within the same bucket,
            //let's try next slot
            if (strncmp(p->get_key(), key.c_str(), p->get_key_length()) != 0) {
                continue;
            }
        }
        count = 1 + atomic_fetch_add(p->get_counter(), ONE);
        break;
    }
    return count;
}

template <typename T>
size_t JethroHash<T>::get(const T& key)
{
    uint16_t count = 0;

    auto hash_index = constrain_hash(key);
    for (int i = 0; i<BUCKET_SIZE; ++i) {
        table_element* p = atomic_load(&(table[hash_index][i]));
        if ( p == nullptr ) {
            break;
        }else{
            if (strncmp(p->get_key(), key.c_str(), p->get_key_length()) == 0) {
                p->load_counter(count);
                break;
            }
        }
    }

    return count;
}

template <typename T>
void JethroHash<T>::clear()
{
    Table empty_table;
    table.swap(empty_table);
}

