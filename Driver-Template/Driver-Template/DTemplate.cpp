#include <ntddk.h>
#include "DTemplate.h"
#include "DeviceIOControl.h"

// IrpHandlers
//=========================

NTSTATUS
IrpDeviceControl(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp
)
{
	ULONG IoControlCode = 0;
	PIO_STACK_LOCATION IrpSp = NULL;
	NTSTATUS Status = STATUS_NOT_SUPPORTED;

	UNREFERENCED_PARAMETER(DeviceObject);

	IrpSp = IoGetCurrentIrpStackLocation(Irp);
	IoControlCode = IrpSp->Parameters.DeviceIoControl.IoControlCode;

	if (IrpSp)
	{
		switch (IoControlCode)
		{
		case DTMP_TEST_FUNCTION:
			DbgPrint("[?] DTemplate Called -> DTMP_TEST_FUNCTION\n");
			Status = DTmpTestFunc(Irp, IrpSp);
			DbgPrint("[?] Exited -> DTMP_TEST_FUNCTION\n");
			break;
		case DTMP_FUNC_1:
			DbgPrint("[?] DTemplate Called -> DTMP_FUNC_1\n");
			Status = STATUS_SUCCESS; // Add your own
			DbgPrint("[?] Exited -> DTMP_FUNC_1\n");
			break;
		case DTMP_FUNC_2:
			DbgPrint("[?] DTemplate Called -> DTMP_FUNC_2\n");
			Status = STATUS_SUCCESS; // Add your own
			DbgPrint("[?] Exited -> DTMP_FUNC_2\n");
			break;
		default:
			DbgPrint("[-] Invalid IOCTL Code: 0x%X\n", IoControlCode);
			Status = STATUS_INVALID_DEVICE_REQUEST;
			break;
		}
	}

	// Your functions return the NTSTATUS & and handle the optional ret buffer
	Irp->IoStatus.Status = Status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}

NTSTATUS
IrpNotImplementedHandler(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp
)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_NOT_SUPPORTED;
}

NTSTATUS
IrpCreateClose(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp
)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

// Driver unload
//=========================

void UnloadDTemplate(_In_ PDRIVER_OBJECT DriverObject) {

	UNICODE_STRING DosDeviceName = { 0 };
	RtlInitUnicodeString(&DosDeviceName, L"\\DosDevices\\DTemplate");
	IoDeleteSymbolicLink(&DosDeviceName);
	IoDeleteDevice(DriverObject->DeviceObject);

	DbgPrint("[+] DTemplate unloaded..\n");
}

// Driver load
//=========================

extern "C"
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	// Vars
	UNICODE_STRING DeviceName, DosDeviceName = { 0 };
	PDEVICE_OBJECT DeviceObject = NULL;
	NTSTATUS CallRes;

	// Set up device
	RtlInitUnicodeString(&DeviceName, L"\\Device\\DTemplate");
	RtlInitUnicodeString(&DosDeviceName, L"\\DosDevices\\DTemplate");
	CallRes = IoCreateDevice(
		DriverObject,
		0,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&DeviceObject
	);

	if (!NT_SUCCESS(CallRes)) {
		if (DeviceObject != NULL) {
			IoDeleteDevice(DeviceObject);
		}

		return CallRes;
	}

	// IRP handlers
	for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		DriverObject->MajorFunction[i] = IrpNotImplementedHandler;
	}

	DriverObject->DriverUnload = UnloadDTemplate;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = IrpCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = IrpCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IrpDeviceControl;

	// Create Symlink
	CallRes = IoCreateSymbolicLink(&DosDeviceName, &DeviceName);

	// Debug status
	DbgPrint("[+] DTemplate loaded..\n");

	return STATUS_SUCCESS;
}