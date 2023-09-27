#include "mleak_test.h"
#include <new>

#define ALIGNED_LIST_ITEM_SIZE sizeof(new_ptr_list_t)
#define DEBUG_NEW_FILE_NAME_LEN 200
std::mutex new_output_lock;
std::mutex new_ptr_lock;
int total_mem_alloc = 0;

struct new_ptr_list_t
{
    new_ptr_list_t* next;
    new_ptr_list_t* prev;
    std::size_t size;
    union 
    {
        char file[DEBUG_NEW_FILE_NAME_LEN];
        void *addr;
    };
    unsigned line;
} new_ptr_list;


static void* alloc_mem(std::size_t size, const char* file, int line) {
    assert(line >= 0);
    if (new_ptr_list.prev == nullptr) {
        new_ptr_list.prev = &new_ptr_list;
        new_ptr_list.next = &new_ptr_list;
    }

    std::size_t new_size = size + ALIGNED_LIST_ITEM_SIZE;
    new_ptr_list_t *ptr = (new_ptr_list_t*) malloc(new_size);
    if (ptr == nullptr) {
        std::unique_lock<std::mutex> lock(new_output_lock);
        std::cout << "Out of memory when allocating " << new_size << " bytes" << std::endl;
        abort();
    }
    // * pointer move to the empty position.
    void *usr_ptr = (char*)ptr + ALIGNED_LIST_ITEM_SIZE;

    if (line) {
        strncpy_s(ptr->file, file, DEBUG_NEW_FILE_NAME_LEN);
    } else {
        ptr->addr = (void*) file;
    }
    ptr->line = line;
    ptr->size = size;
    {
        std::unique_lock<std::mutex> lock(new_ptr_lock);
        ptr->prev = new_ptr_list.prev;
        ptr->next = &new_ptr_list;
        new_ptr_list.prev->next = ptr;
        new_ptr_list.prev = ptr;
    }
    total_mem_alloc += size;
    return usr_ptr;
}

static void free_pointer(void* usr_ptr, void* addr) {
    if (usr_ptr == nullptr) {
        return;
    }
    new_ptr_list_t* ptr = (new_ptr_list_t*)((char*)usr_ptr - ALIGNED_LIST_ITEM_SIZE);
    {
        std::unique_lock<std::mutex> lock(new_ptr_lock);
        total_mem_alloc -= ptr->size;
        if (ptr->prev == nullptr) {
            new_ptr_list.prev = nullptr;
            ptr->next = nullptr;
        } else {
            ptr->prev->next = ptr->next;
            ptr->next->prev = ptr->prev;
        }
    }
    free(ptr);
}

void* operator new (std::size_t size, const char* file, int line) {
    return alloc_mem(size, file, line);
}

void* operator new[] (std::size_t size, const char* file, int line) {
    return alloc_mem(size, file, line);
}

void* operator new(std::size_t size) {
    return operator new(size, __FILE__, __LINE__);
}
void* operator new[](std::size_t size) {
    return operator new[](size, __FILE__, __LINE__);
}
#define new new (__FILE__, __LINE__)

void operator delete(void* ptr) noexcept {
    free_pointer(ptr, nullptr);
}

void operator delete[](void* ptr) noexcept {
    free_pointer(ptr, nullptr);
}

void print_position(char* file_name, int line) {
    std::cout << "file: " << file_name << " line: " << line;
}
void print_position(void* addr, int line) {
    std::cout << "file: " << addr << " line: " << line;
}

// Check memory leak main function.
int checkLeaks() {
    int leak_cnt = 0;
    new_ptr_list_t* ptr = new_ptr_list.next;

    while(ptr != &new_ptr_list) {
        const char* const usr_ptr = (char*)ptr + ALIGNED_LIST_ITEM_SIZE;
        std::cout << "Leaked object at " << std::hex << (void*)usr_ptr 
             << "(size " << ptr->size << " ";
        if (ptr->line != 0) {
            print_position(ptr->file, ptr->line);
        } else {
            print_position(ptr->addr, ptr->line);
        }
        std::cout << ")" << std::endl;
        ptr = ptr->next;
        ++leak_cnt;
    }
    return leak_cnt;
}
