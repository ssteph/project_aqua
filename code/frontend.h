#pragma once

namespace aquafront
{
    void init()
    {

    }

    void update(bool& someBoolean, float& someFloat)
    { 
        ImGui::Begin("MyWindow");
        ImGui::Checkbox("Boolean property", &someBoolean);
        if(ImGui::Button("Reset Speed"))
        {
            // This code is executed when the user clicks the button
            someFloat = 0;
        }
        ImGui::SliderFloat("Speed", &someFloat, 0.0f, 10.0f);
        ImGui::End();
    }
}