// NtAlpc.h: NT ALPC Port privates

#pragma once

#include <winternl.h>

#define ALPC_MSGFLG_SYNC_REQUEST 0x20000 

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
			  // used by C++ source code
#endif

	typedef struct _QUAD
	{
		double DoNotUseThisField;
	} QUAD, * PQUAD, UQUAD, * PUQUAD;

	typedef struct _ALPC_MESSAGE_ATTRIBUTES
	{
		ULONG AllocatedAttributes;
		ULONG ValidAttributes;
	} ALPC_MESSAGE_ATTRIBUTES, * PALPC_MESSAGE_ATTRIBUTES;

	typedef struct _ALPC_PORT_ATTRIBUTES
	{
		ULONG Flags;
		SECURITY_QUALITY_OF_SERVICE SecurityQos;
		SIZE_T MaxMessageLength;
		SIZE_T MemoryBandwidth;
		SIZE_T MaxPoolUsage;
		SIZE_T MaxSectionSize;
		SIZE_T MaxViewSize;
		SIZE_T MaxTotalSectionSize;
		ULONG DupObjectTypes;
#ifdef _M_X64
		ULONG Reserved;
#endif
	} ALPC_PORT_ATTRIBUTES, * PALPC_PORT_ATTRIBUTES;

	typedef PVOID PSID;

	extern NTSYSCALLAPI
		NTSTATUS
		NTAPI
		NtAlpcConnectPort(
			__out PHANDLE PortHandle,
			__in PUNICODE_STRING PortName,
			__in POBJECT_ATTRIBUTES ObjectAttributes,
			__in_opt PALPC_PORT_ATTRIBUTES PortAttributes,
			__in ULONG Flags,
			__in_opt PSID RequiredServerSid,
			__inout PVOID ConnectionMessage,
			__inout_opt PULONG BufferLength,
			__inout_opt PALPC_MESSAGE_ATTRIBUTES OutMessageAttributes,
			__inout_opt PALPC_MESSAGE_ATTRIBUTES InMessageAttributes,
			__in_opt PLARGE_INTEGER Timeout
		);

	extern NTSYSCALLAPI
		NTSTATUS
		NTAPI
		NtAlpcSendWaitReceivePort(
			__in HANDLE PortHandle,
			__in ULONG Flags,
			__in_opt PVOID SendMessage_,
			__in_opt PALPC_MESSAGE_ATTRIBUTES SendMessageAttributes,
			__inout_opt PVOID ReceiveMessage,
			__inout_opt PULONG BufferLength,
			__inout_opt PALPC_MESSAGE_ATTRIBUTES ReceiveMessageAttributes,
			__in_opt PLARGE_INTEGER Timeout
		);


#ifdef __cplusplus
}
#endif
