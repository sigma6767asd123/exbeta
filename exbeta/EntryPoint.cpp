#include <Roblox/Hyperion/Scheduler/Scheduler.hpp>
#include <Roblox/Shared/CrashHandler.hpp>
#include <thread>
#include <Roblox/Renderer/RenderHook.hpp>

void raknetwatcher() {


    uintptr_t RakNet = TaskScheduler::GetJobByName("Net Peer Send");

    if (!RakNet) {
        while (true) {

            uintptr_t RakNet = TaskScheduler::GetJobByName("Net Peer Send");
            if (RakNet) {
                break;
            }
        }
    }
    Raknet::InitializeHook(RakNet);
    while (true) {

        uintptr_t RakNet = TaskScheduler::GetJobByName("Net Peer Send");
        if (!RakNet) {
            std::thread(&raknetwatcher).detach();
            break;
        }
    }
}

void Thread(HMODULE hModule)
{
    std::thread(Communication::Initialize).detach();
    
    std::thread(Scheduler::TeleportHandler).detach();

    //std::thread(raknetwatcher).detach(); hate u
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);

        CrashHandler::Initialize(); // not working idk

        std::thread(Thread, hModule).detach();
    }

    return TRUE;
}
