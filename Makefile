all:
	$(CC) -g -O0 -lstdc++ -std=c++11 ./jethro_cache.cpp ./table_element.cpp ./bulk_allocator.cpp -o jethro_cache
