# Zero and Random Device Driver

A Windows kernel driver providing zero-filled and random byte streams through two character devices. The implementation is in [zero.cpp](zero.cpp); the current source version is **1.1.0.4**, last updated in September 2021.

| Win32 device path | Native device | Read behavior |
| --- | --- | --- |
| `\\?\Zero` | `\Device\Zero` | Fills the requested buffer with zero bytes. |
| `\\?\Random` | `\Device\Random` | Fills the requested buffer with random bytes. |

Both devices accept writes and discard their contents. They have no backing file or disk to overwrite. Reads can continue indefinitely, so callers should request a bounded amount of data rather than copy until end-of-file; the reported file size is zero even though reads return data.

## Reading a device

After the driver is installed and loaded, open a device with the Win32 file APIs. This C example reads one 4 KiB block of zeros into memory:

```c
#include <windows.h>

int main(void)
{
    HANDLE device = CreateFileW(L"\\\\?\\Zero", GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (device == INVALID_HANDLE_VALUE)
        return 1;

    BYTE buffer[4096];
    DWORD bytesRead = 0;
    BOOL ok = ReadFile(device, buffer, sizeof(buffer), &bytesRead, NULL);
    CloseHandle(device);
    return ok && bytesRead == sizeof(buffer) ? 0 : 1;
}
```

Use `L"\\\\?\\Random"` to read random bytes instead.

For builds targeting Vista or later, the random device calls `SystemPrng` through `ksecdd.lib`. Older-target builds use a separate `RtlRandomEx`/`RtlRandom` fallback seeded from driver-load time; that fallback is not suitable for cryptographic use. Selection is made at compile time, not by detecting the OS when a device is opened.

## Building and installation

Open [zero.sln](zero.sln) with Visual C++ and the Windows Driver Kit installed. [zero.vcxproj](zero.vcxproj) selects Windows SDK 10.0.17763.0 and the `WindowsKernelModeDriver10.0` toolset, with Debug/Release configurations for Win32, x64, ARM and ARM64. It builds `zero.cpp` and links `ksecdd.lib`. These are historical project settings, not a current Windows compatibility matrix.

Installation requires administrative access and a driver package suitable for the target Windows version, architecture and signing requirements. [install.cmd](install.cmd) installs and starts the `zero` service, selecting an INF under `win2k/`, `winnet/` or `win7/`. It expects staged driver binaries and an external `w32verc.exe`; the package recipe also expects `run64.exe`. These binaries are not included in this source checkout.

[mkcab.cmd](mkcab.cmd) contains the historical catalog/CAB packaging and signing workflow. Its certificate paths and timestamp settings require adaptation to the build environment.

## License

Licensed under the [MIT License](LICENSE).

Copyright © 2005–2021 Olof Lagerkvist, LTR Data.
