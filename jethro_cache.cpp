#include "jethro_cache.hpp"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    JethroHash<string> h;
    h.inc("abc");
    h.inc("cdef");
    h.inc("abc");
    h.inc("akajlknsqlkdnslkfnmldksfmls");
    h.inc("1234akajlknsqlkdnslkfnmldksfmls");

    for (auto &x: h) {
        std::cout << x.first << " -> " << x.second << std::endl;
    }
    return 0;
}


//JethroHash
template <class T>
size_t JethroHash<T>::inc(const T& key)
{
    size_t count = 1;

    typename Container::iterator it = vec.find(key);
    if (it != vec.end())  {
        count = ++(it->second);
    }else{
        vec.emplace(value_type(key, 1));
    }

    return count;
}

template <typename T>
size_t JethroHash<T>::get(const T& key)
{
    size_t count = 0;

    typename Container::iterator it = vec.find(key);
    if (it != vec.end()) {
        count = it->second;
    }

    return count;
}

template <typename T>
bool JethroHash<T>::clear(const T& key)
{
    return vec.erase(key);
}
