#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <iomanip>
#include <TlHelp32.h>
#include <Psapi.h>
#include <sstream>
#include <shlobj_core.h>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Shell32.lib")

bool setPrevilegue() {
	
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		return false;
	}

	LUID luid;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		CloseHandle(hToken);
		return false;
	}

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
		CloseHandle(hToken);
		return false;
	}

	CloseHandle(hToken);
	return true;
}

int main() {

	if (!IsUserAnAdmin()) {
		std::cout << "RUN THIS PROGRAM IN ADMINISTRATOR!!\n";
		std::system("pause");
		return 1;
	}

	if (!setPrevilegue()) {
		return 1;
	}
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	PROCESS_MEMORY_COUNTERS_EX pmc;
	ZeroMemory(&pmc, sizeof(pmc));

	HANDLE hProcess;
	std::wstringstream ssFile;
	std::size_t totalMemoryUse = 0;
	MEMORYSTATUSEX mstat;
	mstat.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&mstat);

	Process32First(hSnap, &pe);
	do {
		ssFile << std::setw(25) << std::left << pe.szExeFile
			<< std::right << pe.th32ProcessID;
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ProcessID);
		if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
			totalMemoryUse += (pmc.PrivateUsage / (1024 * 1024));
			ssFile << std::setw(25) << pmc.PrivateUsage / (1024 * 1024) << "MB"
				<< '\n' << std::wstring(70, '-') << std::endl;
		}
		else {
			ssFile << std::setw(25) << "NO ACCES"
				<< '\n' << std::wstring(70, '-') << std::endl;
		}
		CloseHandle(hProcess);

	} while (Process32Next(hSnap, &pe));
	DWORDLONG ttalSys = (mstat.ullTotalPhys - mstat.ullAvailPhys);
	ssFile << std::setw(25) << std::left << "Name" << std::right << "Process ID"
		<< std::setw(25) << "Memory Usage" << std::right
		<< std::endl << "Total Procces memory used: " << totalMemoryUse << "MB" << '\n'
		<< "Total System Memory used: " << (mstat.ullTotalPhys - mstat.ullAvailPhys) / (1024 * 1024) << "MB";
			
	std::wofstream file("ProcessMemorySnapshot.txt");
	file << ssFile.str();

	file.close();
	CloseHandle(hSnap);
}