#ifndef __MLEAK_TEST_H__
#define __MLEAK_TEST_H__


#include <iostream>
#include <cassert>
#include <mutex>
#include <cstring>

void* operator new(std::size_t);
void* operator new[](std::size_t);
void* operator new(std::size_t, const char*, int);
void* operator new[](std::size_t, const char*, int);
void operator delete(void*) noexcept;
void operator delete[](void*) noexcept;

int checkLeaks();

#endif