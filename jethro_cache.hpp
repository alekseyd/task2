#include <unordered_map>
#include <vector>
#include <iterator>
#include <atomic>
#include <array>
#include <string>
#include "table_element.hpp"

template <typename T>
class _jethro_hash_iterator : public std::iterator<std::forward_iterator_tag, typename T::value_type>
{
    private:
        typename T::iterator p;

    public:
        _jethro_hash_iterator(typename T::iterator x) : p(x) {}
        _jethro_hash_iterator(const _jethro_hash_iterator& mit) : p(mit.p) {}
        _jethro_hash_iterator& operator++() {++p; return *this;}
        _jethro_hash_iterator operator++(int) {_jethro_hash_iterator tmp(*this); operator++(); return tmp;}
        bool operator==(const _jethro_hash_iterator& rhs) const {return p==rhs.p;}
        bool operator!=(const _jethro_hash_iterator& rhs) const {return not(p==rhs.p);}
        typename T::value_type& operator*() {return *p;}
};


template <typename T>
class _jethro_hash_const_iterator : public std::iterator<std::output_iterator_tag, typename T::value_type>
{
    private:
        typename T::const_iterator p;

    public:
        _jethro_hash_const_iterator(typename T::const_iterator x) : p(x) {}
        _jethro_hash_const_iterator( _jethro_hash_const_iterator& mit) : p(mit.p) {}
        _jethro_hash_const_iterator& operator++() {++p; return *this;}
        _jethro_hash_const_iterator operator++(int) {_jethro_hash_const_iterator tmp(*this); operator++(); return tmp;}
        bool operator==(const _jethro_hash_const_iterator& rhs) const {return p==rhs.p;}
        bool operator!=(const _jethro_hash_const_iterator& rhs) const {return not(p==rhs.p);}
        typename T::value_type& operator*() const {return *p;}
};

//taken from STL
size_t __constrain_hash(size_t h, size_t bc)
{
    return !(bc & (bc - 1)) ? h & (bc - 1) : h % bc;
};

template <class T>
class JethroHash
{
    private:
        static const uint16_t ONE=1;
        //array based solution is a simpliest implementation of buckets in hash map.
        //TODO: implement extencible solution, that can grow dynamically as more
        //key collisons occur
        static const uint16_t BUCKET_SIZE=4;

        std::hash<T> t_hash;
        size_t constrain_hash (const T& key)
        {
            return __constrain_hash(t_hash(key), bucket_count());
        }

    public:
        static const uint16_t zero = 0;
        typedef std::vector<std::array<std::atomic<table_element*>, BUCKET_SIZE>> Table;
        Table table;

        typedef table_element value_type;
        //typedef _jethro_hash_iterator<table_element, BulkAllocator> iterator;
        //typedef _jethro_hash_const_iterator<table_element, BulkAllocator> const_iterator;

        JethroHash(size_t bucket_count=10000000) : table(bucket_count) {}

        size_t inc(const T&);
        size_t get(const T&);
        void clear();

        //iterator begin() {return iterator(vec.begin());}
        //iterator end() {return iterator(vec.end());}

        typename Table::size_type bucket_count () const {return table.size();}

        //decltype(auto) begin() const {return const_iterator(vec.begin());}
        //auto end() const -> decltype(const_iterator(vec.end())) {return const_iterator(vec.end());} //C++11 style
};

