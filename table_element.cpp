#include "table_element.hpp"
#include "bulk_allocator.hpp"

//switching to multihreading requires implementing bulk_allocator per thread
BulkAllocator<2*1024*1024> bulk_allocator;


void* table_element::operator new (size_t header_size, size_t key_size)
{
       return bulk_allocator.get_memory_chunk(header_size + key_size);
}

void table_element::operator delete(void* memory)
{
    table_element *p = static_cast<table_element *>(memory);

    bulk_allocator.release_last_chunk(sizeof(table_element) + p->length);
}
