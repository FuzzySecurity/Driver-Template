using System;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;

namespace Client_Template
{
    class Program
    {
        // Get driver handle
        public static IntPtr GetDriverHandle()
        {
            // Prep API parameters
            APIDef.UNICODE_STRING ObjectName = new APIDef.UNICODE_STRING();
            APIDef.RtlInitUnicodeString(ref ObjectName, ("\\DosDevices\\DTemplate")); // hard-coded for template
            IntPtr pObjectName = Marshal.AllocHGlobal(Marshal.SizeOf(ObjectName));
            Marshal.StructureToPtr(ObjectName, pObjectName, true);

            APIDef.OBJECT_ATTRIBUTES objectAttributes = new APIDef.OBJECT_ATTRIBUTES();
            objectAttributes.Length = Marshal.SizeOf(objectAttributes);
            objectAttributes.ObjectName = pObjectName;
            objectAttributes.Attributes = 0x40; // OBJ_CASE_INSENSITIVE

            APIDef.IO_STATUS_BLOCK ioStatusBlock = new APIDef.IO_STATUS_BLOCK();

            IntPtr hDriver = IntPtr.Zero;

            UInt32 CallRes = APIDef.NtCreateFile(ref hDriver, (UInt32)(APIDef.FileAccessFlags.WRITE_DAC | APIDef.FileAccessFlags.FILE_GENERIC_READ | APIDef.FileAccessFlags.FILE_GENERIC_WRITE), ref objectAttributes, ref ioStatusBlock, IntPtr.Zero, 0, 0, 0x1, 0, IntPtr.Zero, 0);
            if (CallRes == APIDef.NTSTATUS_STATUS_ACCESS_DENIED)
            {
                Console.WriteLine("[!] STATUS_ACCESS_DENIED, Administrative privileges required..");
                return IntPtr.Zero;
            }
            else
            {
                if (CallRes == APIDef.NTSTATUS_STATUS_SUCCESS)
                {
                    Console.WriteLine("[+] Driver handle success..");
                    return hDriver;
                }
                else
                {
                    Console.WriteLine("[!] Failed to get device handle : " + string.Format("{0:X}", CallRes));
                    Console.WriteLine("[?] Is the driver loaded..");
                    return IntPtr.Zero;
                }
            }
        }

        // Call driver test function
        public static void DTemplateTestFunc(IntPtr hDriver)
        {
            // Call driver
            IntPtr OutBuff = Marshal.AllocHGlobal(500); // We just alloc large enough here
            APIDef.RtlZeroMemory(OutBuff, 100);

            APIDef.IO_STATUS_BLOCK isb = new APIDef.IO_STATUS_BLOCK();
            UInt32 CallRes = APIDef.NtDeviceIoControlFile(
                hDriver,
                IntPtr.Zero,
                IntPtr.Zero,
                IntPtr.Zero,
                ref isb,
                APIDef.IOCTL_DTMP_TEST_FUNCTION,
                IntPtr.Zero,
                0,
                OutBuff,
                500);

            // Process result
            if (CallRes != APIDef.NTSTATUS_STATUS_SUCCESS)
            {
                Console.WriteLine("[>] Fail STATUS: " + string.Format("{0:X}", CallRes));
            }
            else
            {
                Console.WriteLine("[>] SUCCESS");
                Console.WriteLine("[?] DTemplate returned -> " + Marshal.PtrToStringAnsi(OutBuff));

                Marshal.FreeHGlobal(OutBuff);
            }
        }

        static void Main(string[] args)
        {
            // Read args
            if (args.Length == 0)
            {
                // Print help
                Console.WriteLine("[!] Args not provided..");
            }
            else
            {
                // Regex the args
                int Test = Array.FindIndex(args, s => new Regex(@"(?i)(-|--|/)(t|Test)$").Match(s).Success);

                // arg switch case
                if (Test != -1)
                {
                    // Get driver handle
                    IntPtr hDriver = GetDriverHandle();
                    if (hDriver == IntPtr.Zero)
                    {
                        return;
                    }
                    DTemplateTestFunc(hDriver);
                }
            }
        }
    }
}
