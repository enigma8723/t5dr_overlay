
#include <iostream>
//#include "stdafx.hpp"

#include "Helpers.h"
#include "Memory.hpp"

using std::string;

namespace ByteswapHelpers
{
	void SWAP_INT16(void* x)
	{
		auto val = DEREF_INT16(x);
		DEREF_INT16(x) = BYTESWAP_INT16(val);
	}

	void SWAP_INT32(void* x)
	{
		auto val = DEREF_INT32(x);
		DEREF_INT32(x) = BYTESWAP_INT32(val);
	}

	void SWAP_INT64(void* x)
	{
		auto val = DEREF_INT64(x);
		DEREF_INT64(x) = BYTESWAP_INT64(val);
	}
};


namespace ProcessHandling {

	HANDLE AttachToProcess(std::string processName, Memory* memory) {

		memory->GetDebugPrivileges();
		int processId = memory->GetProcessId(processName);
		HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, processId);

		return processHandle;
	}
};