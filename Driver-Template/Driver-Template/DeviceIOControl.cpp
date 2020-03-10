#include <ntddk.h>

NTSTATUS
DTmpTestFunc(
    _In_ PIRP Irp,
    _In_ PIO_STACK_LOCATION IrpSp
)
{
    // Declare any vars we need
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    ULONG ReturnSize = 0;

    // You can access the user buffer like this
    //----
    // PVOID UserBuffer = IrpSp->Parameters.DeviceIoControl.Type3InputBuffer;
    // SIZE_T Size = IrpSp->Parameters.DeviceIoControl.InputBufferLength;

    // Kernel buffer to be replicated into the user buffer
    PVOID OutBuffer = Irp->AssociatedIrp.SystemBuffer;
    DbgPrint("[?] Kernel IOCTL Buffer: 0x%p\n", OutBuffer);

    // On request we will send a string back to the user application
    char StatusMsg[] = "DTemplate test call success!";

    // Is the user buffer large enough for our data?
    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength >= (unsigned)strlen(StatusMsg))
    {
        // Set the return data
        memcpy_s(Irp->AssociatedIrp.SystemBuffer, sizeof(StatusMsg), StatusMsg, sizeof(StatusMsg));
        ReturnSize = sizeof(StatusMsg);
        Status = STATUS_SUCCESS;
    }
    else {
        // User did not allocate enough space
        Status = STATUS_BUFFER_TOO_SMALL;
    }

    // Becaue we are sending data back we set the size, if not => 0
    Irp->IoStatus.Information = ReturnSize;

    // Return the call status to IrpDeviceControl
    return Status;
}