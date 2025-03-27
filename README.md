# Native5 & Native5.2
Overview:
NativeFive.exe and NativeFive.2.exe are tools designed to emulate the logic of cd (change directory) and dir (list directory contents) binaries on Windows. These tools interact with the file system, allowing navigation between directories and listing directory contents while incorporating Windows API calls.
Operating System Compatibility

Tested on: Windows
Not tested on: Linux, macOS

NativeFive.exe Features:
* Allows changing directories, including moving multiple levels at once (e.g., Directory\Subdirectory).
* Searches for target directories both downward (subdirectories) and upward (parent directories).
* Uses Win32 API calls such as SetCurrentDirectoryW() to modify the current working directory.

NativeFive.2.exe Features
* Lists directory contents, including file attributes and optional metadata.
* Accepts full path arguments and supports the following switches:
o /A ? Show all files (including hidden/system files).
o /S ? Recursively list all subdirectories.
o /Q ? Display file ownership information.
* Uses Win32 API calls such as GetFileAttributesW(), FindFirstFileW(), and LookupAccountSidW() to retrieve directory and file information.

*  Important Note on Changing Directories (NativeFive.exe)
When using NativeFive.exe to change directories, the process immediately exits after execution. This means:
 The current working directory of the process changes while it is running
 The console window (cmd or PowerShell) does NOT reflect this change once the process exits
 The console title does NOT update to reflect the new directory
 New commands cannot be executed within the context of NativeFive.exe once it has exited
This behavior occurs because NativeFive.exe is a separate process from the command-line session. Once it exists, its changes are lost since it does not persist in the shell session.


Example of Behavior
sh
Copy
C:\Users> NativeFive.exe C:\Windows
C:\Users>  # Still shows "C:\Users" because the process exited
Even though NativeFive.exe successfully changes its own working directory, it cannot modify the shell session's directory once it exits.
Workaround:
If you need persistent navigation like the built-in cd command:
1. Modify the code to implement a persistent interactive loop.

Exit Behavior
* Both tools exit immediately after execution.
* If you require continuous execution, modify the source code to run in a loop

Win32 APIs Used: 

NativeFive.exe
GetFullPathNameW Converts a relative path to an absolute path

SetCurrentDirectory Changes the current working directory

NativeFive.2.exe : 
FindFirstFile Begins searching for files in a directory

FindNextFileW Retrieves the next file in a directory search

GetFileAttributesW Retrieves file attributes (hidden, system, etc.)

GetFileSizeEx Retrieves the size of a file

FileTimeToLocalFileTime Converts file timestamps to local time

FileTimeToSystemTime Converts file timestamps to a human-readable format

GetNamedSecurityInfoW Retrieves security information (file owner)

LookupAccountSidW Resolves a security identifier (SID) to a username

LocalFree Frees memory allocated by security-related functions


Future Enhancements
* Add support for filtering files by extension
* Improve logging and error handling

Contributors
* Kyle H.

