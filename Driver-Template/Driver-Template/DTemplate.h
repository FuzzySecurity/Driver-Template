#include <ntddk.h>

// Tag
#define DRIVER_TAG 'Dtmp'

// IOCTL Def
#define DTMP_DEVICE 0x8000

// METHOD_BUFFERED is used here so functions can set Irp->AssociatedIrp.SystemBuffer
#define IOCTL(Function) CTL_CODE(DTMP_DEVICE, Function, METHOD_BUFFERED, FILE_ANY_ACCESS)

// IOCTL's
// Find code: "{0:X}" -f $((0x00008000 -shl 16) -bor (0x00000000 -shl 14) -bor (FUNCTIONID -shl 2) -bor 0x00000000)
#define DTMP_TEST_FUNCTION   IOCTL(0x800) // 0x80002000
#define DTMP_FUNC_1          IOCTL(0x801) // 0x80002004
#define DTMP_FUNC_2          IOCTL(0x802) // 0x80002008