// old offsets 
#pragma once

#include <lua.h>
#include <Windows.h>
#include <cstdint>
#include <memory>

#define Rebase(Address) (Address + reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr)))
struct LookupResult
{
    std::uint32_t index;
    std::uint8_t missing;
    std::uint8_t reserved[3];
};

struct lua_State;
struct YieldState;
struct YieldingLuaThread;

namespace Offsets
{
    const uintptr_t Print = Rebase(0x1CFA0D0);
    const uintptr_t OpcodeLookupTable = Rebase(0x6F1BF50);
    const uintptr_t TaskDefer = Rebase(0x435A060);


    namespace Luau
    {
        const uintptr_t LuaO_NilObject = Rebase(0x64BA248);
        const uintptr_t LuaH_DummyNode = Rebase(0x64B68D8);
        const uintptr_t LuaD_Throw = Rebase(0x266ea40);
        const uintptr_t Luau_Execute = Rebase(0x269E520);
        const uintptr_t LuaVM_Load = Rebase(0x41E8BD0);
    }
    namespace Signals
    {
        const uintptr_t FireTouchInterest = Rebase(0x1584BC0);
        const uintptr_t StrictFilter = 0x30B8;
        const uintptr_t EventWhitelist = 0x38;
        const uintptr_t DisconnectConnect = Rebase(0x41B40E0);
    }

    namespace Flags
    {
        inline const uintptr_t FireMouseClick = Rebase(0x3C3EDC0);
        inline const uintptr_t FireRightMouseClick = Rebase(0x3C3EF60);
        inline const uintptr_t FireMouseHoverEnter = Rebase(0x3C403B0);
        inline const uintptr_t FireMouseHoverLeave = Rebase(0x3C40550);
        inline const uintptr_t FireProximityPrompt = Rebase(0x31BD800);
        const uintptr_t TaskSchedulerTargetFps = Rebase(0x7EF44B8);
    }
    namespace DataModel
    {
        const uintptr_t ScriptContext = 0x440;
        const uintptr_t Children = 0x78;
        const uintptr_t GameLoaded = 0x5e0;
        const uintptr_t ClassDescriptor = 0x18;

        const uintptr_t FakeDataModelToDataModel = 0x1f8;
        const uintptr_t FakeDataModelPointer = Rebase(0x8EE1728);
    }

    namespace ExtraSpace
    {
        const uintptr_t ScriptContextResume = Rebase(0x429B530);
        const uintptr_t ScriptContextToResume = 0xA38;
    }

    namespace Identity
    {
        const uintptr_t Identity = Rebase(0x828CA38);
        const uintptr_t GetIdentityStruct = Rebase(0x7240);
    }

    namespace Capabilities
    {
        const uintptr_t GetCapabilities = Rebase(0x1D34B70);
        const uintptr_t Capabilities = 0x30;
    }

    namespace Scripts
    {
        const uintptr_t LocalScriptByteCode = 0x180;
        const uintptr_t ModuleScriptByteCode = 0x120;
        const uintptr_t BytecodeStringOffset = 0x18;
        const uintptr_t ServerContextFlag = 0x130;

        const uintptr_t Pointer = 0x18;
        const uintptr_t Size = 0x28;
    }

    namespace InstanceBridge
    {
        const uintptr_t Push = Rebase(0x41D9F50);
    }

    namespace TaskScheduler
    {
        const uintptr_t TaskSchedulerPtr = Rebase(0x8C8D108); // TODO: fill in TaskScheduler pointer
        const uintptr_t JobStart = 0xC8;
        const uintptr_t JobName = 0x18;
        const uintptr_t Fps = 0xB0;
    }

    namespace Renderer
    {
        const uintptr_t ViewBase = 0x1d8;
        const uintptr_t Device = 0x8;
        const uintptr_t SwapChain = 0xC8;
    }

    namespace ScriptContext
    {
        const uintptr_t RequireBypass = 0xAAE;
    }
    namespace ClassDescriptor
    {
        const uintptr_t GetPropertyData = Rebase(0x2A86550);
        const uintptr_t GetProperty = Rebase(0x1D32FF0);
    }
    namespace Properties
    {
        const uintptr_t CallbackOffsetInDescriptor = 0x80;
        const uintptr_t HolderSelfOffset = 0x38;
        const uintptr_t CallbackOwnerOffset = 0x18;
        const uintptr_t CallbackFunctionOffset = 0x38;
        const uintptr_t LiveThreadRefOffset = 0x18;
        const uintptr_t CallbackThreadOffset = 0x28;
        const uintptr_t LuaStateGlobalOffset = 0x18;
        const uintptr_t FunctionRefIdOffset = 0x34;
        const uintptr_t AlternateHolderAdjustment = 0x1F0;
    }



    namespace signal_offsets
    {
        constexpr uintptr_t signature = 0x48;

        namespace connection
        {
            constexpr uintptr_t next = 0x10;
            constexpr uintptr_t enabled = 0x20;
            constexpr uintptr_t slot_wrapper = 0x30;
            constexpr uintptr_t slot_wrapper_self = 0x38;
        }

        namespace slot_wrapper
        {
            constexpr uintptr_t thread = 0x28;
            constexpr uintptr_t function_id = 0x34;
            constexpr uintptr_t weak_object_ref_fallback = 0x38;
            constexpr uintptr_t weak_object_ref = 0x68;
            constexpr uintptr_t weak_object_ref_legacy = 0x48;
        }

        namespace getter
        {
            constexpr uintptr_t chain = 0x68;
            constexpr uintptr_t self = 0x18;
            constexpr uintptr_t container = 0x50;
            constexpr uintptr_t container_thread = 0x28;
        }
    }

    namespace Connection
    {
        inline constexpr uintptr_t enabled = 0x20;
        inline constexpr uintptr_t next = 0x10;
        inline constexpr uintptr_t slot_wrapper = 0x30;
        inline constexpr uintptr_t slot_wrapper_self = 0x38;
    }

    namespace SlotWrapper
    {
        inline constexpr uintptr_t function_id = 0x34;
        inline constexpr uintptr_t thread = 0x28;
        inline constexpr uintptr_t weak_object_ref = 0x68;
    }

    namespace connection = signal_offsets::connection;
    namespace slot_wrapper = signal_offsets::slot_wrapper;

    namespace signal_slot
    {
        inline constexpr uintptr_t is_once = 0xA5;
    }




    namespace reflections
    {
        const uintptr_t CastArgs = Rebase(0x414ECD0);
    }
    namespace Raknet
    {
        const uintptr_t RakPeer = 0x1D8;
    }
    namespace Instance
    {
        inline constexpr uintptr_t PropertyDescriptorBitFlags = 0x8C;
        inline constexpr uintptr_t Properties = 0x3C0;
        inline constexpr uintptr_t ClassDescriptor = 0x18;
        inline constexpr uintptr_t Name = 0x8;
        inline constexpr uintptr_t Overlap = 0x1F8;
    }
}

namespace Hyperion
{

    inline auto LuaVM_Load = (int(__fastcall*)(lua_State * thread, void* bytecode, const char* chunkName, int env))Offsets::Luau::LuaVM_Load;
    inline auto Luau_Execute = (void(__fastcall*)(lua_State*))Offsets::Luau::Luau_Execute;

    inline auto ScriptContextResume = (uint64_t(__fastcall*)(uint64_t, YieldState*, YieldingLuaThread**, uint32_t, uint8_t, uint64_t))Offsets::ExtraSpace::ScriptContextResume;
    inline auto LuaD_Throw = (void(__fastcall*)(lua_State*, int))Offsets::Luau::LuaD_Throw;
    inline auto GetIdentityStruct = (uintptr_t(__fastcall*)(uintptr_t))Offsets::Identity::GetIdentityStruct;
    inline auto GetCapabilities = (__int64(__fastcall*)(int*))Offsets::Capabilities::GetCapabilities;
    inline auto PushInstance = (void(__fastcall*)(lua_State*, void*))Offsets::InstanceBridge::Push;
    using InternT = std::uintptr_t(__fastcall*)(const char*);
    using FindBucketT = LookupResult * (__fastcall*)(std::uintptr_t, LookupResult*, const std::uintptr_t*, std::uint32_t);

    inline std::uintptr_t InternName(const char* name)
    {
        const auto function = reinterpret_cast<InternT>(Offsets::ClassDescriptor::GetPropertyData);
        return function(name);
    }

    namespace Signal
    {
        inline auto FireTouchInterest = (void(__fastcall*)(uintptr_t, uintptr_t, uintptr_t, bool, bool))Offsets::Signals::FireTouchInterest;
    }
    inline auto Print = (uintptr_t(*)(int, const char*, ...))Offsets::Print;

    inline LookupResult* FindBucket(std::uintptr_t table, LookupResult* output, const std::uintptr_t* key, std::uint32_t mode)
    {
        const auto function = reinterpret_cast<FindBucketT>(Offsets::ClassDescriptor::GetProperty);
        return function(table, output, key, mode);
    }
    using fireclick_t = void(__fastcall*)(void* clickDetector, float distance, void* player);
    inline auto FireMouseClick = reinterpret_cast<fireclick_t>(Offsets::Flags::FireMouseClick);
    inline auto FireRightMouseClick = reinterpret_cast<fireclick_t>(Offsets::Flags::FireRightMouseClick);

    using TaskDeferT = int(__fastcall*)(lua_State*);
    inline TaskDeferT TaskDefer = reinterpret_cast<TaskDeferT>(Offsets::TaskDefer);
    using firehover_t = void(__fastcall*)(void* clickDetector, void* player);
    inline auto FireMouseHover = reinterpret_cast<firehover_t>(Offsets::Flags::FireMouseHoverEnter);
    inline auto FireMouseHoverLeave = reinterpret_cast<firehover_t>(Offsets::Flags::FireMouseHoverLeave);
    inline auto FireProximityPrompt = (void(__thiscall*)(uintptr_t))Offsets::Flags::FireProximityPrompt;
    inline auto FireTouchInterest = (void(__fastcall*)(uintptr_t, uintptr_t, uintptr_t, bool, bool))Offsets::Signals::FireTouchInterest;

    inline auto CastArgs = reinterpret_cast<uintptr_t(__fastcall*)(lua_State*, int, void*, bool, int)>(Offsets::reflections::CastArgs);
    namespace InstanceBridge
    {
        inline auto UintPtr = (void(__fastcall*)(lua_State*, std::uintptr_t))Offsets::InstanceBridge::Push;
        inline auto Shared = (void(__fastcall*)(lua_State*, std::shared_ptr<std::uintptr_t*>))Offsets::InstanceBridge::Push;
        inline auto Void = (void(__fastcall*)(lua_State*, void**))Offsets::InstanceBridge::Push;
        inline auto Void2 = (void(__fastcall*)(lua_State*, void*))Offsets::InstanceBridge::Push;
        inline auto Weak = (void(__fastcall*)(lua_State*, std::weak_ptr<std::uintptr_t>))Offsets::InstanceBridge::Push;
    }
}