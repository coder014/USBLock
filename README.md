# USBLock

A lightweight USB locker that prevent file operations (Ring3) of a given USB volume identified by its serial number.

The application uses Microsoft [Detours](https://github.com/microsoft/Detours) hook library to hook Windows *CreateFileA* & *CreateFileW* API call.

## HowTo

Create a text file named *vol.inf*, paste in the serial number (Hex format) of USB volume you want to lock and move it to the same directory as the program in.
