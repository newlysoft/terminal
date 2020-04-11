// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"

#include "ProcessHandle.h"

#include "..\host\globals.h"
#include "..\host\telemetry.hpp"

// Routine Description:
// - Constructs an instance of the ConsoleProcessHandle Class
// - NOTE: Can throw if allocation fails or if there is a console policy we do not understand.
// - NOTE: Not being able to open the process by ID isn't a failure. It will be logged and continued.
ConsoleProcessHandle::ConsoleProcessHandle(const DWORD dwProcessId,
                                           const DWORD dwThreadId,
                                           const ULONG ulProcessGroupId) :
    pWaitBlockQueue(std::make_unique<ConsoleWaitQueue>()),
    pInputHandle(nullptr),
    pOutputHandle(nullptr),
    fRootProcess(false),
    dwProcessId(dwProcessId),
    dwThreadId(dwThreadId),
    _ulTerminateCount(0),
    _ulProcessGroupId(ulProcessGroupId),
    _hProcess(LOG_LAST_ERROR_IF_NULL(OpenProcess(MAXIMUM_ALLOWED,
                                                 FALSE,
                                                 dwProcessId))),
    _policy(ConsoleProcessPolicy::s_CreateInstance(_hProcess.get()))
{
    if (nullptr != _hProcess.get())
    {
        Telemetry::Instance().LogProcessConnected(_hProcess.get());
    }
}

CD_CONNECTION_INFORMATION ConsoleProcessHandle::GetConnectionInformation(IDeviceComm* deviceComm) const
{
    CD_CONNECTION_INFORMATION result = { 0 };
    result.Process = deviceComm->PutHandle(this);
    result.Input = deviceComm->PutHandle(pInputHandle.get());
    result.Output = deviceComm->PutHandle(pOutputHandle.get());
    return result;
}

// Routine Description:
// - Retrieves the policies set on this particular process handle
// - This specifies restrictions that may apply to the calling console client application
const ConsoleProcessPolicy ConsoleProcessHandle::GetPolicy() const
{
    return _policy;
}
