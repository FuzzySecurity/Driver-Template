# Driver Template

This is a simple template for a Kernel Driver and User client. The driver has all of the boilerplate elements and implements a single demo IOCTL for reference. The client has all of the API defs and can call the demo IOCTL.

### DbgPrint

Just a small note on seeing debug messages in KD, you should break and enter the following command:
```
ed nt!Kd_DEFAULT_MASK 8
```