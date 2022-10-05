#pragma once

namespace aquaback
{

struct BackendData
{
    mem::MemoryArena memory;
};

BackendData* init(void* mem, mem_size size)
{
    mem::MemoryArena temp;
    mem::arena_init(&temp, mem, size);

    BackendData* data = mem::arena_permanent_alloc<BackendData>(&temp);

    data->memory = temp;

    return data;
}


}
