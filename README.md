

# Process Memory Snapshot

```
⚠️!!WARNING, RUN PROGRAMM AS ADMINISTRATOR!!⚠️
```

Process Memory Snapshot is a small Windows console utility designed to collect information about running processes and the amount of memory they use during system execution.

When the program starts, it checks for administrator privileges. This is required to access system processes and retrieve extended memory information. After that, the application enables `SeDebugPrivilege`, allowing it to interact with processes that are normally restricted for standard users.

The program then creates a snapshot of all active processes in the system. Each process is processed one by one: the application reads its executable name, process identifier (PID), and attempts to open the process for memory inspection. If access is granted, the program retrieves private memory usage through WinAPI and converts the value to megabytes before adding it to the final report.

While iterating through processes, the utility also calculates the total amount of memory used by all accessible processes. In addition, the current physical memory state of the system is retrieved to display overall RAM usage at the moment of execution.

If a process cannot be accessed, the program continues execution without interruption and marks the entry as `NO ACCESS` inside the report.

After all processes are processed, the application generates a text report and saves it as `ProcessMemorySnapshot.txt`. The file contains the process list, process IDs, memory usage information, and overall system memory statistics.

The project is written in C++ and uses native Windows API functions. It is intended to be built on Windows using Visual Studio.

## Required Libraries

```cpp
Psapi.lib
Shell32.lib
