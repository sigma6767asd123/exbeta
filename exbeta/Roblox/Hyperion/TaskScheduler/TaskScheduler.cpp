#include "TaskScheduler.hpp"

#include <Roblox/Shared/Memory.hpp>

uintptr_t TaskScheduler::GetDatamodel()
{

	if (!Memory::CanRead(Offsets::DataModel::FakeDataModelPointer, sizeof(uintptr_t)))
		return 0;

	const uintptr_t cotch1 = *reinterpret_cast<uintptr_t*>(Offsets::DataModel::FakeDataModelPointer);
	if (!Memory::CanRead(cotch1 + Offsets::DataModel::FakeDataModelToDataModel, sizeof(uintptr_t)))
		return 0;

	return *reinterpret_cast<uintptr_t*>(cotch1 + Offsets::DataModel::FakeDataModelToDataModel);
}

uintptr_t TaskScheduler::GetScriptContext(uintptr_t DataModel)
{

	if (!DataModel || !Utils::CheckMemory(DataModel))
		return 0;
	
	__try
	{
		uintptr_t Children = *reinterpret_cast<uintptr_t*>(DataModel + Offsets::DataModel::Children);
		if (!Children)
			return 0;
		uintptr_t FirstChild = *reinterpret_cast<uintptr_t*>(Children);
		if (!FirstChild)
			return 0;
		return *reinterpret_cast<uintptr_t*>(FirstChild + Offsets::DataModel::ScriptContext);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return 0;
	}

	return 0;
}

inline bool CheckMemory(uintptr_t address) {
	if (address < 0x10000 || address > 0x7FFFFFFFFFFF)
		return false;

	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQuery(reinterpret_cast<void*>(address), &mbi, sizeof(mbi)) == 0)
		return false;

	if (mbi.Protect & PAGE_NOACCESS || mbi.State != MEM_COMMIT)
		return false;

	return true;
}

lua_State* TaskScheduler::GetLuaStateForInstance(uintptr_t sc)
{
	if (!sc)
		return nullptr;

	__try
	{
		*reinterpret_cast<BOOLEAN*>(sc + Offsets::ScriptContext::RequireBypass) = TRUE;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}

	auto vmval1 = [](uint32_t a, uint32_t b) -> uint32_t { return b - a; };
	auto vmval2 = [](uint32_t a, uint32_t b) -> uint32_t { return a + b; };
	auto vmval3 = [](uint32_t a, uint32_t b) -> uint32_t { return a ^ b; };
	auto vmval4 = [](uint32_t a, uint32_t b) -> uint32_t { return a - b; };

	auto enc_test = [&](uintptr_t address, uint32_t e0, uint32_t e1, auto&& op) -> uintptr_t
		{
			uint32_t low = op(static_cast<uint32_t>(address), e0);
			uint32_t high = op(static_cast<uint32_t>(address), e1);

			uintptr_t ptr =
				(static_cast<uint64_t>(high) << 32) | static_cast<uint64_t>(low);

			if (!ptr)
				return 0;

			if (!CheckMemory(ptr))
				return 0;

			__try
			{
				uintptr_t test = *reinterpret_cast<uintptr_t*>(ptr);
				if (!test)
					return 0;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return 0;
			}

			return ptr;
		};

	for (uintptr_t offset = 0; offset <= 0x1000; offset += 0x8)
	{
		uintptr_t address = sc + offset;

		if (!CheckMemory(address))
			continue;

		__try
		{
			uint32_t* encrypted = reinterpret_cast<uint32_t*>(address);
			uint32_t e0 = encrypted[0];
			uint32_t e1 = encrypted[1];

			if (e0 == 0 && e1 == 0)
				continue;

			if (uintptr_t p = enc_test(address, e0, e1, vmval1)) return reinterpret_cast<lua_State*>(p);
			if (uintptr_t p = enc_test(address, e0, e1, vmval2)) return reinterpret_cast<lua_State*>(p);
			if (uintptr_t p = enc_test(address, e0, e1, vmval3)) return reinterpret_cast<lua_State*>(p);
			if (uintptr_t p = enc_test(address, e0, e1, vmval4)) return reinterpret_cast<lua_State*>(p);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			continue;
		}
	}

	return nullptr;
}

lua_State* TaskScheduler::GetLuaState(uintptr_t ScriptContext, uintptr_t Identity, uintptr_t Script)
{
	if (!ScriptContext || !Utils::CheckMemory(ScriptContext))
		return nullptr;

	__try
	{
		return Hyperion::GetLuaStateForInstance(ScriptContext, &Identity, &Script);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return nullptr;
	}
}

int TaskScheduler::GetGameLoaded(uintptr_t Datamodel)
{

	if (!Datamodel || !Utils::CheckMemory(Datamodel + Offsets::DataModel::GameLoaded)) {
		return 0;
	}

	return *reinterpret_cast<int*>(Datamodel + Offsets::DataModel::GameLoaded);
}

uintptr_t TaskScheduler::GetJobByName(const std::string& Name)
{

	uintptr_t Scheduler = *reinterpret_cast<uintptr_t*>(Offsets::TaskScheduler::TaskSchedulerPtr);
	if (!Scheduler || !Utils::CheckMemory(Scheduler + 0x200))
		return 0;

	uintptr_t JobsStart = *reinterpret_cast<uintptr_t*>(Scheduler + Offsets::TaskScheduler::JobStart);
	uintptr_t JobsEnd = *reinterpret_cast<uintptr_t*>(Scheduler + Offsets::TaskScheduler::JobStart + sizeof(uintptr_t));
	if (!JobsStart || JobsStart >= JobsEnd || JobsEnd - JobsStart > 0x100000)
		return 0;

	for (uintptr_t Job = JobsStart; Job < JobsEnd; Job += 0x10)
	{
		uintptr_t JobPtr = *reinterpret_cast<uintptr_t*>(Job);
		if (!JobPtr || !Utils::CheckMemory(JobPtr + Offsets::TaskScheduler::JobName + 0x20))
			continue;

		const uintptr_t nameAddr = JobPtr + Offsets::TaskScheduler::JobName;
		const size_t len = *reinterpret_cast<size_t*>(nameAddr + 0x10);
		if (len >= 0x100)
			continue;

		const char* data = len < 0x10
			? reinterpret_cast<const char*>(nameAddr)
			: *reinterpret_cast<const char**>(nameAddr);
		if (!data || !Memory::CanRead(reinterpret_cast<uintptr_t>(data), len ? len : 1))
			continue;

		if (len == Name.size() && memcmp(data, Name.data(), len) == 0)
			return JobPtr;
	}

	return 0;
}

uintptr_t TaskScheduler::GetJobByTypeName(const std::string& TypeName)
{
	

	uintptr_t TaskScheduler =
		*reinterpret_cast<uintptr_t*>(
			Offsets::TaskScheduler::TaskSchedulerPtr
			);

	if (!TaskScheduler)
		return 0;

	uintptr_t JobsStart =
		*reinterpret_cast<uintptr_t*>(
			TaskScheduler + Offsets::TaskScheduler::JobStart
			);

	uintptr_t JobsEnd =
		*reinterpret_cast<uintptr_t*>(
			TaskScheduler +
			Offsets::TaskScheduler::JobStart +
			sizeof(uintptr_t)
			);

	if (!JobsStart || !JobsEnd || JobsStart >= JobsEnd)
		return 0;

	for (uintptr_t Current = JobsStart;
		Current < JobsEnd;
		Current += 0x10)
	{
		uintptr_t Job =
			*reinterpret_cast<uintptr_t*>(Current);

		if (!Job)
			continue;

		const char* JobTypeName =
			*reinterpret_cast<const char**>(
				Job + 0xF8
				);

		if (!JobTypeName)
			continue;

		if (strcmp(JobTypeName, TypeName.c_str()) == 0)
			return Job;
	}

	return 0;
}

void TaskScheduler::SetProtoCapabilities(Proto* Root, uintptr_t* Capabilities)
{

	if (!Root)
		return;

	std::vector<Proto*> Pending{ Root };
	while (!Pending.empty())
	{
		Proto* Current = Pending.back();
		Pending.pop_back();
		if (!Current)
			continue;

		Current->userdata = Capabilities;
		if (Current->sizep < 0 || Current->sizep > 100000 || (!Current->p && Current->sizep != 0))
			continue;

		for (int Index = 0; Index < Current->sizep; ++Index)
			if (Current->p[Index])
				Pending.push_back(Current->p[Index]);
	}
}

void TaskScheduler::SetThreadCapabilities(lua_State* L, int Level, uintptr_t Capabilities)
{

	if (!L || !L->userdata)
		return;

	L->userdata->identity = Level;
	L->userdata->capabilities = Capabilities;
}

void TaskScheduler::SetIdentity(lua_State* Thread, int Identity, bool IsInstance)
{

	if (!Thread || !Thread->userdata)
		return;

	rbxextraspace* basedData = reinterpret_cast<rbxextraspace*>(Thread->userdata);
	basedData->identity = Identity;
	basedData->capabilities = Hyperion::GetCapabilities(&Identity);

	if (IsInstance)
	{
		uintptr_t IdentityPointer = *reinterpret_cast<uintptr_t*>(Offsets::Identity::Identity);
		if (!IdentityPointer || IdentityPointer > 0xFFFFFFFFFFFF)
			return;

		uintptr_t IdentityStructure = Hyperion::GetIdentityStruct(IdentityPointer);
		if (!IdentityStructure)
			return;

		*reinterpret_cast<int32_t*>(IdentityStructure) = Identity;
		*reinterpret_cast<uintptr_t*>(IdentityStructure + Offsets::Capabilities::Capabilities) = Hyperion::GetCapabilities(&Identity);
	}
}
void TaskScheduler::RequestExecution(std::string Script)
{
	std::lock_guard<std::mutex> lock(Shared::ExecutionRequestsMutex);
	Shared::ExecutionRequests.push_back(Script);
}
void TaskScheduler::SendScript(const std::string& Script)
{

	std::lock_guard<std::mutex> lock(Shared::ExecutionRequestsMutex);
	Shared::ExecutionRequests.push_back(Script);
}