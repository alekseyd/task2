#include <list>
#include <new>

template <int PAGE_SIZE>
class BulkAllocator
{
    struct MemPage{
        size_t used;        //bytes used
        char data[PAGE_SIZE];
        size_t available() { return (PAGE_SIZE - used);}
        MemPage() : used(0){}
    };
    typedef MemPage* MemPagePtr;

    std::list<MemPage*> chunks;
    size_t allocated;   //bytes allocated
    size_t used;        //bytes used
    size_t page_count;
    void allocate_new_page() {
        chunks.push_back(new MemPage());
        allocated += PAGE_SIZE;
        ++page_count;
    }
public:
    BulkAllocator ()
        : allocated(0), used(0), page_count(0)
    {
        allocate_new_page();
    }
    ~BulkAllocator () {
        for (auto p: chunks) {
            delete p;
        }
    }
    void* get_memory_chunk(const size_t size)
    {
        if (size >=PAGE_SIZE) {
            return nullptr;
        }
        if (size > chunks.back()->available())  {
            allocate_new_page();
        }
        auto offset = chunks.back()->used;
        chunks.back()->used += size;
        used += size;
        return chunks.back()->data + offset;
    }
    void release_last_chunk(const size_t size)
    {
        if (size >=PAGE_SIZE || chunks.back()->used < size) {
            return;
        }
        chunks.back()->used -= size;
        used -= size;
    }
};
