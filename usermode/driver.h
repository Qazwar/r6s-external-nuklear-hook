#pragma once
#include "imports.h"

typedef struct _COPY_MEMORY
{
	void* bufferAddress;         // Buffer address   
	UINT_PTR  address;        // Target address
	ULONGLONG  size;             // Buffer size
	ULONG     pid;              // Target process id
	BOOLEAN   write;            // TRUE if write operation, FALSE if read
	BOOLEAN  ReqBase;           // TRUE if request base address, FALSE if not.
	ULONG64 BaseAddress;    // Base address of the game
	void* Output;
	BOOLEAN ClearPIDCache;
	BOOLEAN PIDCacheCleared;
	BOOLEAN Read;
	BOOLEAN ReadString;
	BOOLEAN WriteString;
	const char* moduleName;
	ULONG pid_ofSource;

	//UINT_PTR	process_id; already here 
	//PVOID		address; already here
	//SIZE_T		size;  already here

	BOOLEAN		change_protection;
	ULONG		protection;
	ULONG		protection_old;
}COPY_MEMORY;

static std::uint32_t process_id = 0;

struct HandleDisposer
{
	using pointer = HANDLE;
	void operator()(HANDLE handle) const
	{
		if (handle != NULL || handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
		}
	}
};

using unique_handle = std::unique_ptr<HANDLE, HandleDisposer>;

static std::uint32_t get_process_id(std::string_view process_name) {
	PROCESSENTRY32 processentry;
	const unique_handle snapshot_handle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));

	if (snapshot_handle.get() == INVALID_HANDLE_VALUE)
		return 0;

	processentry.dwSize = sizeof(MODULEENTRY32);

	while (Process32Next(snapshot_handle.get(), &processentry) == TRUE) {
		if (process_name.compare(processentry.szExeFile) == 0)
			return processentry.th32ProcessID;
	}
	return 0;
}

template<typename ... A>
uint64_t call_hook(const A ... arguments)
{
	void* control_function = GetProcAddress(LoadLibrary("win32u.dll"), "NtOpenCompositionSurfaceSectionInfo");

	const auto control = static_cast<uint64_t(__stdcall*)(A...)>(control_function);

	return control(arguments ...);
}

static ULONG64 change_protection(uint64_t pid, uint64_t address, uint32_t page_protection, std::size_t size)
{
	COPY_MEMORY m = { 0 };
	m.pid = process_id;
	m.address = address;
	m.protection = page_protection;
	m.size = size;
	m.change_protection = TRUE;

	m.ReqBase = FALSE;
	m.ClearPIDCache = FALSE;
	m.Read = FALSE;
	m.ReadString = FALSE;
	m.write = FALSE;
	m.WriteString = FALSE;

	return call_hook(&m);
}

static ULONG64 get_module_base_address(const char* moduleName) {

	COPY_MEMORY m = { 0 };
	m.pid = process_id;
	m.ReqBase = TRUE;
	m.ClearPIDCache = FALSE;
	m.Read = FALSE;
	m.ReadString = FALSE;
	m.write = FALSE;
	m.WriteString = FALSE;
	m.moduleName = moduleName;
	call_hook(&m);

	ULONG64 base = NULL;
	base = m.BaseAddress;
	return base;
}

template <class T>
T Read(UINT_PTR ReadAddress) {

	T response{};

	COPY_MEMORY m;
	m.pid = process_id;
	m.size = sizeof(T);
	m.address = ReadAddress;
	m.Read = TRUE;
	m.ReadString = FALSE;
	m.WriteString = FALSE;
	m.write = FALSE;
	m.ReqBase = FALSE;
	m.ClearPIDCache = FALSE;

	call_hook(&m);

	return *(T*)& m.Output;
}

static bool WriteVirtualMemoryRaw(UINT_PTR WriteAddress, UINT_PTR SourceAddress, SIZE_T WriteSize);

template<typename S>
bool Write(UINT_PTR WriteAddress, const S& value)
{
	return WriteVirtualMemoryRaw(WriteAddress, (UINT_PTR)& value, sizeof(S));
}
bool WriteVirtualMemoryRaw(UINT_PTR WriteAddress, UINT_PTR SourceAddress, SIZE_T WriteSize)
{
	COPY_MEMORY m;
	m.address = WriteAddress;
	m.pid = process_id;
	m.pid_ofSource = GetCurrentProcessId();
	m.write = TRUE;
	m.ClearPIDCache = FALSE;
	m.Read = FALSE;
	m.ReadString = FALSE;
	m.ReqBase = FALSE;
	m.WriteString = FALSE;
	m.bufferAddress = (void*)SourceAddress;
	m.size = WriteSize;

	call_hook(&m);

	return true;
}

static BOOLEAN CleanPIDDBCacheTable() {

	COPY_MEMORY m;
	m.ClearPIDCache = TRUE;
	m.Read = FALSE;
	m.WriteString = FALSE;
	m.write = FALSE;
	m.ReqBase = FALSE;
	m.ReadString = FALSE;
	call_hook(&m);

	return m.PIDCacheCleared;
}

static void ReadString(UINT_PTR String_address, void* buffer, SIZE_T size) {
	COPY_MEMORY m;
	m.pid = process_id;
	m.ReadString = TRUE;
	m.Read = FALSE;
	m.ClearPIDCache = FALSE;
	m.ReqBase = FALSE;
	m.write = FALSE;
	m.WriteString = FALSE;
	m.address = String_address;
	m.bufferAddress = buffer;
	m.size = size;

	call_hook(&m);
}

static void WriteString(UINT_PTR String_address, void* buffer, SIZE_T size) {
	COPY_MEMORY m;
	m.pid = process_id;
	m.WriteString = TRUE;
	m.ClearPIDCache = FALSE;
	m.Read = FALSE;
	m.ReadString = FALSE;
	m.ReqBase = FALSE;
	m.write = FALSE;
	m.address = String_address;
	m.bufferAddress = buffer;
	m.size = size;

	call_hook(&m);
}