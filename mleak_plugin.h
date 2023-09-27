#ifndef __MEMORY_LEAK_TOOLS_H__
#define __MEMORY_LEAK_TOOLS_H__

#include <iostream>
#include <cassert>
#include <mutex>
#include <cstring>

void* operator new(std::size_t, const char*, int);
void* operator new[](std::size_t, const char*, int);
void operator delete(void*) noexcept;
void operator delete[](void*) noexcept;

int checkLeaks();

#endif