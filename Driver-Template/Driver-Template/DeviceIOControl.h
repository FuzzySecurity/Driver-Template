#pragma once

// IOCTL functions
NTSTATUS
DTmpTestFunc(
    _In_ PIRP Irp,
    _In_ PIO_STACK_LOCATION IrpSp
);