#include "Scheduler.hpp"
#include <Roblox/Environment/Environment.hpp>
#include <Roblox/Environment/Yielding/Yielding.hpp>
#include <Roblox/Hyperion/Execution/Execution.hpp>
#include <Roblox/Hyperion/TaskScheduler/TaskScheduler.hpp>
#include <Roblox/Environment/Libraries/Drawing.hpp>
#include <Roblox/Environment/Libraries/Raknet.hpp>

void Scheduler::Reset()
{
    

    Shared::VoltState = nullptr;
    Shared::RobloxState = nullptr;
    Drawing::ClearAll();
}

static bool InitializeGameImpl()
{
    if (Shared::isInitializedOnGame)
        return true;

    uintptr_t ScriptContext =
        TaskScheduler::GetScriptContext(Shared::lastKnownDatamodel);

    if (!ScriptContext)
        return false;
    lua_State* MainThread = nullptr;
    __try { MainThread = TaskScheduler::GetLuaStateForInstance(ScriptContext); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }

    if (!MainThread)
        return false;

    Shared::RobloxState = MainThread;


    Shared::RobloxState = reinterpret_cast<lua_State*>(MainThread);
    if (!Shared::RobloxState)
        return false;

    Shared::VoltState = lua_newthread(Shared::RobloxState);
    if (!Shared::VoltState)
        return false;

    luaL_sandboxthread(Shared::VoltState);
    Environment::Initialize(Shared::VoltState);

    Shared::isInitializedOnGame = true;

    return true;
}

static bool InitializeGame()
{
    __try
    {
        return InitializeGameImpl();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        Shared::isInitializedOnGame = false;
        Shared::RobloxState = nullptr;
        Shared::VoltState = nullptr;
        return false;
    }
}

static bool ExecuteGameImpl()
{
    std::string Script;

    {
        std::lock_guard<std::mutex> lock(Shared::ExecutionRequestsMutex);

        if (Shared::VoltState && !Shared::ExecutionRequests.empty())
        {
            Script = std::move(Shared::ExecutionRequests.front());
            Shared::ExecutionRequests.erase(Shared::ExecutionRequests.begin());
        }
    }

    if (!Script.empty())
        Execution::ExecuteScript(Shared::VoltState, std::move(Script));

    Yielding::RunYield();
    return true;
}

static bool ExecuteGame()
{
    __try
    {
        return ExecuteGameImpl();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

static uintptr_t stepHook(uintptr_t Job, uintptr_t Stats)
{
    if (!Shared::originalStepFunction)
        return 0;

    if (Shared::lockedStep)
        return Shared::originalStepFunction(Job, Stats);

    if (!Shared::isInitializedOnGame)
    {
        if (!InitializeGame())
            return Shared::originalStepFunction(Job, Stats);
    }
    else if (!ExecuteGame())
    {
        return Shared::originalStepFunction(Job, Stats);
    }

    return Shared::originalStepFunction(Job, Stats);
}

LUAU_FASTFLAG(LuauCIProto)

void Scheduler::TeleportHandler()
{

    Shared::isInitializedOnGame = false;
    Scheduler::Reset();

    if (TaskScheduler::GetGameLoaded(TaskScheduler::GetDatamodel()) == 15) {
        while (TaskScheduler::GetGameLoaded(TaskScheduler::GetDatamodel()) == 15) {
            Sleep(100);
        }
    }

    if (!Shared::lastKnownDatamodel || Shared::lastKnownDatamodel != TaskScheduler::GetDatamodel()) {
        Shared::lastKnownDatamodel = TaskScheduler::GetDatamodel();
    }

    uintptr_t GameWHSJ = TaskScheduler::GetJobByTypeName("WaitingHybridScriptsJob(Default;Ugc)");
    if (!GameWHSJ) {
        Sleep(50);
        return TeleportHandler();
    }


    Shared::lastWHSJ = GameWHSJ;

    if (Shared::ourVFtable == nullptr)
    {
        const auto jobVFtable = *reinterpret_cast<uintptr_t**>(Shared::lastWHSJ);
        const auto newVFtable = new uintptr_t[9];
        memcpy(newVFtable, jobVFtable, sizeof(uintptr_t) * 9);

        Shared::originalStepFunction = reinterpret_cast<Shared::JobStep_t>(jobVFtable[8]);
        newVFtable[8] = reinterpret_cast<uintptr_t>(&stepHook);

        Shared::ourVFtable = newVFtable;
    }

    Shared::lockedStep = false;

    *reinterpret_cast<uintptr_t**>(Shared::lastWHSJ) = static_cast<uintptr_t*>(Shared::ourVFtable);

    while (true)
    {
        uintptr_t currentDatamodel = TaskScheduler::GetDatamodel();

        if (currentDatamodel != Shared::lastKnownDatamodel && TaskScheduler::GetGameLoaded(currentDatamodel) > 15) {
            std::thread(&TeleportHandler).detach();
            break;
        }
        else if (currentDatamodel != Shared::lastKnownDatamodel && TaskScheduler::GetGameLoaded(currentDatamodel) == 15) {
            std::thread(&TeleportHandler).detach();
            break;
        }

        Sleep(10);
    }

}
