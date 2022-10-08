#pragma once

namespace aquaback
{

struct BackendData
{
    mem::MemoryArena memory;

    command::Queue commands;

    bool run = true;
};

BackendData* init(void* mem, mem_size size)
{
    mem::MemoryArena temp;
    mem::arena_init(&temp, mem, size);

    BackendData* data = mem::arena_permanent_alloc<BackendData>(&temp);
    data->memory = temp;

    command::init_queue(&data->commands);

    return data;
}

int run_threaded(void* threadData)
{ 
    BackendData* data = (BackendData*)threadData;

    while(data->run)
    {
        SDL_Delay(50);
    }

    return 0;
}

void stop_thread(BackendData* data)
{ 
    data->run = false;
}

}
