#pragma once
#include <ntddk.h>

extern HANDLE g_hTargetId;

VOID InstallHooks();

VOID UnInstallHooks();