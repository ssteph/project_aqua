#pragma once

namespace aquafront
{


struct FrontendData
{
    mem::MemoryArena memory;

    bool someBoolean = false;
    float someFloat = 1.23f;
};

FrontendData* init(void* mem, mem_size size)
{
    mem::MemoryArena temp;
    mem::arena_init(&temp, mem, size);

    FrontendData* data = mem::arena_permanent_alloc<FrontendData>(&temp);
    
    data->memory = temp;

    return data;
}

void update(FrontendData* data)
{ 
    ImGui::Begin("MyWindow");
    ImGui::Checkbox("Boolean property", &data->someBoolean);
    if(ImGui::Button("Reset Speed"))
    {
        // This code is executed when the user clicks the button
        data->someFloat = 0;
    }
    ImGui::SliderFloat("Speed", &data->someFloat, 0.0f, 10.0f);
    ImGui::End();
}


}