//#include <hash>
#include <unordered_map>
#include <vector>
#include <iterator>

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


template <class T>
class JethroHash
{
    private:
        typedef std::unordered_map<T, int>  Container;
        Container vec;

    public:
        typedef std::pair<T, size_t> value_type;
        typedef _jethro_hash_iterator<Container> iterator;
        typedef _jethro_hash_const_iterator<Container> const_iterator;

        JethroHash(size_t n=10000000) : vec(n) {}

        virtual size_t inc(const T&);
        virtual size_t get(const T&);
        virtual bool clear(const T&);

        iterator begin() {return iterator(vec.begin());}
        iterator end() {return iterator(vec.end());}

        decltype(auto) begin() const {return const_iterator(vec.begin());}
        auto end() const -> decltype(const_iterator(vec.end())) {return const_iterator(vec.end());}
};
