#include "types.h"
#include "debug.h"
#include "log.h"
#include "memory.h"

#include <windows.h>
#include <psapi.h>
//#include <wow64apiset.h>

#include "..\libs\SDL2-devel-2.24.0-VC\SDL2-2.24.0\include\SDL.h"
#include "..\libs\SDL2-devel-2.24.0-VC\SDL2-2.24.0\include\SDL_opengl.h"
#include <GL/gl.h>

#include "..\libs\dearimgui\imgui.h"
#include "..\libs\dearimgui\backends\imgui_impl_sdl.h"
#include "..\libs\dearimgui\backends\imgui_impl_opengl3.h"

#include "frontend.h"
#include "backend.h"

int main(int argc, char** argv)
{
    u32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    SDL_Window* window = SDL_CreateWindow("Aqua", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, windowFlags);
    
    if(!window)
    {
        LOG_ERROR("SDL_CreateWindow");
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GLContext context = SDL_GL_CreateContext(window);

    if(!context)
    {
        LOG_ERROR("SDL_GL_CreateContext");
        return 1;
    }

    SDL_GL_MakeCurrent(window, context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 22.0f);

    ImGui::StyleColorsDark();
    
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();

    mem_size frontendMemorySize = 1024 * 1024 * 100;
    void* frontendMemReservation = VirtualAlloc(0, frontendMemorySize, MEM_RESERVE, PAGE_READWRITE);
    if(!frontendMemReservation)
    {
        LOG_ERROR("MEM_RESERVE");
        return 1;
    }

    void* frontendMem = VirtualAlloc(frontendMemReservation, frontendMemorySize, MEM_COMMIT, PAGE_READWRITE);
    if(frontendMem != frontendMemReservation)
    {
        LOG_ERROR("MEM_COMMIT");
        return 1;
    }

    aquafront::FrontendData* frontendData = aquafront::init(frontendMem, frontendMemorySize);

    mem_size backendMemorySize = 1024 * 1024 * 1024;
    void* backendMemReservation = VirtualAlloc(0, backendMemorySize, MEM_RESERVE, PAGE_READWRITE);
    if(!backendMemReservation)
    {
        LOG_ERROR("MEM_RESERVE");
        return 1;
    }

    void* backendMem = VirtualAlloc(backendMemReservation, backendMemorySize, MEM_COMMIT, PAGE_READWRITE);
    if(backendMem != backendMemReservation)
    {
        LOG_ERROR("MEM_COMMIT");
        return 1;
    }

    aquaback::BackendData* backendData = aquaback::init(backendMem, backendMemorySize); (void*)backendData;

    //////////////////////////////////////////////////////////////////
    /// main loop

    bool running = 1;
    while(running)
    {
        SDL_Event sdlEvent;
        while(SDL_PollEvent(&sdlEvent))
        {
            ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

            if(sdlEvent.type == SDL_KEYDOWN)
            {
                switch(sdlEvent.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                default:
                    break;
                }
            }
            else if(sdlEvent.type == SDL_QUIT)
            {
                running = false;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        /////////////////////////

        aquafront::update(frontendData);

        /////////////////////////

        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(.15f, .27f, .43f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

#if 0
    DWORD processes[1024] = { };
    DWORD bytesReturned = 0;

    DWORD targetPID = 0;

    /*
    DWORD pID;
    HWND hwnd = FindWindowA(NULL, "*Untitled - Notepad"); // the window title

    if(hwnd)
    {
        if(!GetWindowThreadProcessId(hwnd, &pID))
        {
            int a = 1;
            a = a;
        }
    }
    */

    if(!EnumProcesses(processes, sizeof(processes), &bytesReturned))
    {
        return 1;
    }

    const s32 numProcesses = bytesReturned / sizeof(DWORD);

    for(s32 i = 0; i < numProcesses; ++i)
    {
        TCHAR processName[MAX_PATH] = TEXT("<unknown>");
        const TCHAR notepadName[MAX_PATH] = TEXT("notepad.exe");
        //const TCHAR notepadName[MAX_PATH] = TEXT("notepad++.exe");

        if(processes[i] != 0)
        {
            HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);

            if(nullptr != processHandle)
            {
#if 0
                HMODULE modules[1024];
                DWORD moduleBytesReturned;

                if(EnumProcessModules(processHandle, modules, sizeof(modules), &moduleBytesReturned))
                {
                    const s32 numModules = bytesReturned / sizeof(DWORD);

                    for(s32 moduleIndex = 0; moduleIndex < numModules; ++moduleIndex)
                    {
                        GetModuleBaseName(processHandle, modules[moduleIndex], processName, sizeof(processName) / sizeof(TCHAR));

                        if(moduleIndex == 0)
                        {
                            if(0 == strncmp(processName, notepadName, MAX_PATH))
                            {
                                targetPID = processes[i];
                            }
                            else
                            {
                                break;
                            }
                        }

                        LOG_INFO("process index %d, module index %d: %s", i, moduleIndex, processName);
                    }
                }

#else
                HMODULE moduleHandle;
                DWORD moduleBytesReturned;

                if(EnumProcessModules(processHandle, &moduleHandle, sizeof(moduleHandle), &moduleBytesReturned))
                {
                    GetModuleBaseName(processHandle, moduleHandle, processName, sizeof(processName) / sizeof(TCHAR));
                }
                else
                {
                    LOG_INFO("oops 99");
                    continue;
                }

                //LOG_INFO("process id %d: %ld -> %s", i, processes[i], processName);

                if(0 == strncmp(processName, notepadName, MAX_PATH))
                {
                    LOG_INFO("process id %d: %ld -> %s", i, processes[i], processName);
                    targetPID = processes[i];
                }
#endif

                CloseHandle(processHandle);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    if(targetPID != 0)
    {
        HANDLE targetHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetPID);

        MEMORY_BASIC_INFORMATION mbi;
        char* begin = 0;
        char* memReadBuffer = nullptr;
        BOOL isWow64Process = false;
        if(!IsWow64Process(targetHandle, &isWow64Process))
        {
            LOG_ERROR("oops 0");
        }

        const intptr_t size = isWow64Process ? 0x80000000 : 0x800000000000;

        if(nullptr != targetHandle)
        {
            PROCESS_MEMORY_COUNTERS pmc;
            if(GetProcessMemoryInfo(targetHandle, &pmc, sizeof(pmc)))
            {
                int a = 1;
                a = a;
            }

            FILE* file;
            file = fopen("mem.bin", "wb");

            /*
            VirtualQueryEx(targetHandle, (LPCVOID)begin, &mbi, sizeof(mbi));

            if(mbi.Protect != PAGE_NOACCESS)
            {
                LOG_ERROR("oops 1");
                return 1;
            }
            */

            SIZE_T totalSize = 0;
            SIZE_T totalSizeInclSkipped = 0;
            s32 failed = 0;
            s32 regionsRead = 0;

            for(char* curr = begin; curr < begin + size; curr += mbi.RegionSize)
            {
                if(!VirtualQueryEx(targetHandle, curr, &mbi, sizeof(mbi)))
                    continue;
                
                if(mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) 
                    continue;

                totalSizeInclSkipped += mbi.RegionSize;

                //TODO: delete buffer after last iteration
                delete[] memReadBuffer;
                memReadBuffer = new char[mbi.RegionSize];
                DWORD oldprotect;

                bool undoVirtualProtectEx = false;

                if(mbi.Protect != PAGE_READWRITE && mbi.Protect != PAGE_READONLY)
                {
                    if(VirtualProtectEx(targetHandle, mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &oldprotect))
                    {
                        undoVirtualProtectEx = true;
                    }
                    else
                    {
                        LOG_ERROR("oops 2, protect is %ld", mbi.Protect);
                        failed += 1;
                        //continue;
                    }
                }

                //if(true)
                //if(VirtualProtectEx(targetHandle, mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldprotect))
                //if(VirtualProtectEx(targetHandle, mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &oldprotect))
                //if(VirtualProtectEx(targetHandle, mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &oldprotect))
                {
                    SIZE_T bytesRead = 0;
                    ReadProcessMemory(targetHandle, mbi.BaseAddress, memReadBuffer, mbi.RegionSize, &bytesRead);
                    regionsRead += 1;

                    if(undoVirtualProtectEx)
                    {
                        VirtualProtectEx(targetHandle, mbi.BaseAddress, mbi.RegionSize, oldprotect, &oldprotect);
                    }

                    LOG_INFO("read %llu of %llu", bytesRead, mbi.RegionSize);
                    totalSize += bytesRead;

                    fwrite(memReadBuffer, 1, (s32)bytesRead, file);

                    //LOG_INFO("block: %.*s", (s32)bytesRead, memReadBuffer);

                    for(s32 ci = 0; ci < (s32)bytesRead; ++ci)
                    {
                        const char c = memReadBuffer[ci];

                        if(c == 'h')
                        {
                            //printf("block: %.*s \n", (s32)(bytesRead - ci), &memReadBuffer[ci]);
                            //printf("block: %.*s", (s32)bytesRead, memReadBuffer);

                            if((ci + 5) < (s32)bytesRead)
                            {
                                if(memReadBuffer[ci + 1] == 'u' &&
                                    memReadBuffer[ci + 2] == 'l' &&
                                    memReadBuffer[ci + 3] == 'l' &&
                                    memReadBuffer[ci + 4] == 'o')
                                {
                                    int a = 1;
                                    a = a;
                                }
                            }
                        }
                    }

                    //printf("\n");

                    //printf("block: %.*s", (s32)bytesRead, memReadBuffer);
                }


                //LOG_INFO("region size: %I64u", mbi.RegionSize);


            }

            float mbSize = totalSize / (float)(1024 * 1024);
            LOG_INFO("total size: %llu (%.2f) (incl skipped: %llu)", totalSize, mbSize, totalSizeInclSkipped);
            LOG_INFO("VirtualProtectEx failed: %d", failed);
            LOG_INFO("Regions read: %d", regionsRead);

            CloseHandle(targetHandle);

            fclose(file);
        }
    }
#endif 

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}