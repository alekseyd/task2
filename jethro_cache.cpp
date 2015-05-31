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

    //for (auto &x: h) {
    //    std::cout << x.first << " -> " << x.second << " ("<< str_hash(x.first) << ")" <<endl;
    //}

    for (auto &x: {"abc", "cdef", "1234akajlknsqlkdnslkfnmldksfmls","mmmmdmd"}) {
        std::cout << x << " -> " << h.get(x) << endl;
    }

    std::cout << "bucket_count = " << h.table.size() <<endl;

//----------------------------------
    std::cout << sizeof(typename JethroHash<string>::Container::value_type)<<endl;

    return 0;
}


BulkAllocator<2*1024*1024> bulk_allocator;

void* table_element::operator new (size_t header_size, size_t key_size)
{
       return bulk_allocator.get_memory_chunk(header_size + key_size);
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
            if (strncmp(p->key, key.c_str(), p->length) != 0) {
                continue;
            }
        }
        count = 1 + atomic_fetch_add(&(p->count), ONE);
        break;
    }
    return count;
}

template <typename T>
size_t JethroHash<T>::get(const T& key)
{
    size_t count = 0;

    auto hash_index = constrain_hash(key);
    for (int i = 0; i<BUCKET_SIZE; ++i) {
        table_element* p = atomic_load(&(table[hash_index][i]));
        if ( p == nullptr ) {
            break;
        }else{
            if (strncmp(p->key, key.c_str(), p->length) == 0) {
                count = p->count;
                break;
            }
        }
    }

    return count;
}

template <typename T>
void JethroHash<T>::clear()
{
    Container empty_table;
    table.swap(empty_table);
}

template <typename T>
bool JethroHash<T>::erase(const T& key)
{
    return true;
}

