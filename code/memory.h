#pragma once

#include <new>

namespace mem
{

struct MemoryArena
{
    void* base = nullptr;
    mem_size size = 0;
    mem_size used = 0;
};

void arena_init(MemoryArena* mem, void* memory, mem_size size)
{
    mem->base = memory;
    mem->size = size;
}

/*
void arena_reset(MemoryArena* mem)
{
    MY_ASSERT(mem != get_memory());
    mem->used = 0;
}
*/

template<typename T, typename... Rest>
T* arena_permanent_alloc(MemoryArena* mem, Rest&&... args)
{
    void* loc = arena_raw_alloc(mem, sizeof(T), alignof(T));
    return new(loc) T(std::forward<Rest>(args)...);
}

/*
template<typename T, typename... Rest>
T* engine_permanent_alloc(Rest&&... args)
{
    return engine_permanent_alloc<T>(get_memory(), std::forward<Rest>(args)...);
}
*/

/*
template<typename T>
T* engine_permanent_alloc_array_no_init(u32 count)
{
    return engine_permanent_alloc_array_no_init<T>(get_memory(), count);
}
*/

template<typename T>
T* arena_permanent_alloc_array_no_init(MemoryArena* engineMem, u32 count)
{
    void* loc = arena_raw_alloc(engineMem, sizeof(T) * count, alignof(T[]));

    //TODO: this is theoretically bad if T is anything other than char/byte (i think)
    //object lifetimes don't actually start without constructor calls

    /*
    this is better
    for (T* tp = reinterpret_cast<T*>(loc); count--; tp++) {
    ::new(reinterpret_cast<void*>(tp)) T; // placement-new
    }
    return reinterpret_cast<T*>(p + 1);
    */

    return static_cast<T*>(loc);
}

/*
template<typename T>
T* engine_scratch_alloc_array_no_init(u32 count)
{
    void* loc = engine_raw_alloc(get_scratch_memory(), sizeof(T) * count, alignof(T[]));

    //TODO: this is bad if T is anything other than char/byte (i think)
    //object lifetimes don't actually start without constructor calls

    //this is better
    //for (T* tp = reinterpret_cast<T*>(loc); count--; tp++) {
    //::new(reinterpret_cast<void*>(tp)) T; // placement-new
    //}
    //return reinterpret_cast<T*>(p + 1);

    return static_cast<T*>(loc);
}
*/

void* arena_raw_alloc(MemoryArena* engineMem, mem_size sizeInBytes, uint8_t alignment)
{
    mem_size alignMinus1 = alignment - 1;
    mem_size oldLoc = (mem_size)engineMem->base + engineMem->used;
    mem_size loc = (oldLoc + alignMinus1) & ~(alignMinus1);
    mem_size sizeChange = sizeInBytes + loc - oldLoc;

    //TODO: if instead of assert?
    MY_ASSERT(engineMem->used + sizeChange < engineMem->size);

    engineMem->used += sizeChange;

    return (void*)loc;
}

}