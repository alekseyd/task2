#include <unordered_map>
#include <vector>
#include <iterator>
#include <atomic>
#include <array>

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

decltype(auto) __constrain_hash(size_t h, size_t bc)
{
    return !(bc & (bc - 1)) ? h & (bc - 1) : h % bc;
};


template <int N>
struct table_element{
    size_t bucket_id;
    uint16_t offset;
    std::atomic<uint16_t> count;
    uint16_t length;
    char key[N];
};
typedef table_element<16> table_element_16;

template <class T>
class JethroHash
{
    private:
        std::hash<T> t_hash;
        decltype(auto) constrain_hash (const T& key)
        {
            return __constrain_hash(t_hash(key), bucket_count());
        }


    public:
        typedef std::vector<std::array<std::atomic<table_element_16*>, 4>> Container;
        //typedef std::unordered_map<T, int>  SetContainer;
    public:
        //SetContainer vec;
        Container table;

    public:
        typedef std::pair<T, size_t> value_type;
        //typedef _jethro_hash_iterator<SetContainer> iterator;
        //typedef _jethro_hash_const_iterator<SetContainer> const_iterator;

        JethroHash(size_t bucket_count=10000000) : table(bucket_count) {}

        virtual size_t inc(const T&);
        virtual size_t get(const T&);
        virtual bool clear(const T&);

        //iterator begin() {return iterator(vec.begin());}
        //iterator end() {return iterator(vec.end());}

        decltype(auto) bucket_count () const {return table.size();}

        //decltype(auto) begin() const {return const_iterator(vec.begin());}
        //auto end() const -> decltype(const_iterator(vec.end())) {return const_iterator(vec.end());} //C++11 style
};

