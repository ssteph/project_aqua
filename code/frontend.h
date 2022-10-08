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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::SetNextWindowPos({ 0.0f, 0.0f });

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowSize(io.DisplaySize);

    //ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoBackground*/;
        ImGui::Begin("MyWindow", nullptr, flags);

        ImGui::Checkbox("Boolean property", &data->someBoolean);
        if(ImGui::Button("Reset Speed"))
        {
            // This code is executed when the user clicks the button
            data->someFloat = 0;
        }
        ImGui::SliderFloat("Speed", &data->someFloat, 0.0f, 10.0f);
        ImGui::End();

    //ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}


}