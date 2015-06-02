#include <map>
#include <thread>
#include <mutex>
#include <memory>
#include "bulk_allocator.hpp"

using namespace std;
map<thread::id, BulkAllocatorType> allocator_map;
mutex allocator_init_mutex;

void  init_allocator()
{
    std::lock_guard<std::mutex> lock(allocator_init_mutex);

    thread::id this_id = this_thread::get_id();
    allocator_map.emplace(make_pair(this_id, BulkAllocator<2*1024*1024>()));

}

BulkAllocatorType& get_allocator()
{
    /*thread_local*/ thread::id this_id = this_thread::get_id();
    return allocator_map.at(this_id);
}
