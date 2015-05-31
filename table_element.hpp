#include <atomic>
#include <string>

class table_element{
    private:
        static const uint16_t zero = 0;
        size_t bucket_id;
        uint16_t offset;
        std::atomic<uint16_t> count;
        uint16_t length;
        char key[];

    public:
        table_element(const size_t _bucket_id, const uint16_t _offset,
                const std::string _key)
            : bucket_id(_bucket_id), offset(_offset),
            length(_key.size())
        {
            std::strncpy(this->key, _key.c_str(), length);
            std::atomic_init(&count, zero);
        }
        void* operator new (size_t header_size, size_t key_size);
        void operator delete(void* memory);

        auto get_counter() -> decltype(&count) {return &count;}
        void load_counter(uint16_t &var) {var = static_cast<uint16_t>(count);}
        uint16_t get_key_length() const { return length;}
        char* get_key() {return key;}
};
