#include "generic.h"

namespace SandboxEvasion {

VEDetection* Generic::create_instance(const json_tiny &j) {
	return new Generic(j);
}

void Generic::CheckAllCustom() {
	bool d;
	std::pair<std::string, std::string> report;
	std::string ce_name;

	ce_name = Config::cgen2s[Config::ConfigGeneric::SLEEP_DUMMY];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = CheckSleepDummyPatch();
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}

	ce_name = Config::cgen2s[Config::ConfigGeneric::PERFORMANCE_COUNTER];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = CheckPerformanceCounter();
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}

	ce_name = Config::cgen2s[Config::ConfigGeneric::DISK_SIZE];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = CheckDiskSize();
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}

	ce_name = Config::cgen2s[Config::ConfigGeneric::DRIVE_SIZE];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = CheckDriveSize();
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}

	ce_name = Config::cgen2s[Config::ConfigGeneric::DEVICE_NPF_NDIS];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = CheckNDISFile();
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}

	ce_name = Config::cgen2s[Config::ConfigGeneric::MOUSE_ACTIVE];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = CheckMouseActive();
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}

	ce_name = Config::cgen2s[Config::ConfigGeneric::RAM];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = CheckRAM();
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}

	ce_name = Config::cgen2s[Config::ConfigGeneric::SYSTEM_UPTIME];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = CheckSystemUptime();
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}

	ce_name = Config::cgen2s[Config::ConfigGeneric::PROCESSORS_COUNT];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = CheckNumberOfProcessors();
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}

	ce_name = Config::cgen2s[Config::ConfigGeneric::DNS_RESPONSE];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = CheckDNSResponse();
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}

	ce_name = Config::cgen2s[Config::ConfigGeneric::TIME_TAMPERING];
	if (IsEnabled(ce_name, conf.get<std::string>(ce_name + std::string(".") + Config::cg2s[Config::ConfigGlobal::ENABLED], ""))) {
		d = CheckTimeTampering(ProcessWorkingMode::MASTER);
		report = GenerateReportEntry(ce_name, json_tiny(conf.get(ce_name, pt::ptree())), d);
		log_message(LogMessageLevel::INFO, module_name, report.second, d ? RED : GREEN);
	}
}

bool Generic::CheckSystemUptime() const {
	// FIXME: make it configurable ?
	const DWORD uptime = 1000 * 60 * 12; // 12 minutes
	
	return GetTickCount() < uptime;
}

bool Generic::CheckRAM() const {
	MEMORYSTATUSEX ms = {};
	ms.dwLength = sizeof(ms);

	// FIXME: make it configurable ?
	const DWORDLONG min_ram = 1024 * 1024 * 1024; // 1GB

	if (!GlobalMemoryStatusEx(&ms))
		return false;

	return ms.ullTotalPhys < min_ram;
}

bool Generic::CheckDiskSize() const {
	HANDLE hDrive;
	GET_LENGTH_INFORMATION gli = {};
	DWORD dwReturned;
	uint32_t min_disk_size_gb = 60; 	// FIXME: make it configurable ?

	hDrive = CreateFile("\\\\.\\PhysicalDrive0", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hDrive == INVALID_HANDLE_VALUE)
		return false;

	if (!DeviceIoControl(hDrive, IOCTL_DISK_GET_LENGTH_INFO, NULL, 0, &gli, sizeof(gli), &dwReturned, NULL)) {
		CloseHandle(hDrive);
		return false;
	}

	CloseHandle(hDrive);
	return (gli.Length.QuadPart / (1024 * 1024 * 1024)) < min_disk_size_gb;
}

bool Generic::CheckDriveSize() const {
	ULARGE_INTEGER drive_size = {};
	uint32_t min_disk_size_gb = 60; 	// FIXME: make it configurable ?

	if (GetDiskFreeSpaceExA("C:\\", NULL, &drive_size, NULL))
		return (drive_size.QuadPart / (1024 * 1024 * 1024)) < min_disk_size_gb;

	return false;
}

bool Generic::CheckNDISFile() const {
	HANDLE hFile;
	const wchar_t ndis_wan_ip_fname[] = L"\\\\.\\NPF_NdisWanIp";
	DWORD err;

	hFile = CreateFileW(ndis_wan_ip_fname, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		err = GetLastError();
		return err != ERROR_PATH_NOT_FOUND && err != ERROR_FILE_NOT_FOUND;
	}
	CloseHandle(hFile);

	return true;
}

bool Generic::CheckMouseActive() const {
	POINT pos_f, pos_s;
	const uint32_t timeout = 1000; // timeout in milliseconds
	const uint8_t tries = 5;

	for (uint8_t i = 0; i < tries; ++i) {
		GetCursorPos(&pos_f);
		Sleep(timeout);
		GetCursorPos(&pos_s);

		if ((pos_s.x - pos_f.x) || (pos_s.y - pos_f.y))
			return false;
	}

	return true;
}

bool Generic::CheckMouseRawActive() const {
	POINT pos_f, pos_s;
	const uint32_t timeout = 1000; // timeout in milliseconds
	const uint8_t tries = 5;

	// TODO: implement

	for (uint8_t i = 0; i < tries; ++i) {
		
	}

	return false;
}

bool Generic::CheckSleepDummyPatch() const {
	DWORD tick_count_f, tick_count_s;
	DWORD tick_count_diff;
	const DWORD delay_ms = 900; // timeout in milliseconds

	tick_count_f = GetTickCount();
	SleepEx(delay_ms, FALSE);
	tick_count_s = GetTickCount();

	return (tick_count_s - tick_count_f) < (delay_ms - 50);
}

bool Generic::CheckPerformanceCounter() const {
	LARGE_INTEGER Frequency = { 0 };
	LARGE_INTEGER StartingTime = { 0 };
	LARGE_INTEGER EndingTime = { 0 };
	LARGE_INTEGER ElapsedTimeMs = { 0 };

	const DWORD delay_ms = 10000;	// timeout in milliseconds
	const DWORD max_delta = 50;		// delay in milliseconds

	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);

	// Activity to be timed
	Sleep(delay_ms);

	QueryPerformanceCounter(&EndingTime);

	ElapsedTimeMs.QuadPart = 1000ll * (EndingTime.QuadPart - StartingTime.QuadPart) / Frequency.QuadPart;

	// printf("Elapsed: %u. Waited: %u\n", ElapsedTimeMs.LowPart, delay_ms);

	return abs(static_cast<long>(ElapsedTimeMs.LowPart) - static_cast<long>(delay_ms)) > max_delta;
}

bool Generic::CheckNumberOfProcessors() const {
	const DWORD min_proc_count = 2;
	SYSTEM_INFO si = {};

	if (get_number_of_processors() < min_proc_count)
		return true;

	GetSystemInfo(&si);

	return si.dwNumberOfProcessors < min_proc_count;
}

bool Generic::CheckDNSResponse() const {
	// Calling function DnsQuery to query Host or PTR records   
	std::list<IP4_ADDRESS> ips;
	// FIXME: should it be configurable?
	std::map<std::string, size_t> dns_r = {
		{ "microsoft.com",	3 },
		{ "bbc.com",		3 },
		{ "amazon.com",		3 }
	};
	bool sandbox_detected = false;
	unsigned char ip_addr[4];

	for (auto &dns : dns_r) {
		ips = {};
		if (!perform_dns_request(dns.first, ips))
			continue;

		// perform check on number of received domains
		if (ips.size() < dns.second) {
			sandbox_detected = true;
			break;
		}

		// perform check on received IP-addresses
		for (auto &ip : ips) {
			memcpy(ip_addr, reinterpret_cast<void *>(&ip), sizeof(ip_addr));
			if (!(((ip_addr[0] != 10) && (((ip_addr[0] != 0xac) || (ip_addr[1] <= 15)) || (ip_addr[1] >= 0x20))) && ((ip_addr[0] != 0xc0) || (ip_addr[1] != 0xa8)))) {
				sandbox_detected = true;
				break;
			}
		}
	}

	return sandbox_detected;
}

bool Generic::CheckTimeTampering(ProcessWorkingMode wm) const {
	switch (wm) {
	case ProcessWorkingMode::MASTER:
		return CheckTimeTamperingMaster();
	case ProcessWorkingMode::SLAVE:
		return CheckTimeTamperingSlave();
	default:
		return false;
	}
}


bool Generic::CheckTimeTamperingMaster() const {
	wchar_t app_params[] = L"--action --dtt";
	PROCESS_INFORMATION pi = {};

	if (!run_self_susp(app_params, &pi))
		return false;

	// resume thread
	ResumeThread(pi.hThread);

	// wait process for finish
	DWORD ec;
	do {
		if (!GetExitCodeProcess(pi.hProcess, &ec)) {
			TerminateProcess(pi.hProcess, 0xFF);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return false;
		}
		Sleep(100);
	} while (ec == STILL_ACTIVE);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return !!ec;
}


bool Generic::CheckTimeTamperingSlave() const {
	const int delta = 5 * 1000; // 5 sec
	const int64_t k100NStoMSecs = 10000ll;
	bool sandboxDetected = false;
	const std::string host("google.com");	// FIXME: should be configurable?

	FILETIME ftLocalStart, ftLocalEnd, ftWebStart, ftWebEnd;

	GetSystemTimeAsFileTime(&ftLocalStart);
	if (!get_web_time(host, ftWebStart))
		return false;

	int64_t totalMSec = 0;
	for (int i = 0; i < 10; ++i) {
		const int sleepSec = 1 + (rand() % 10);
		totalMSec += sleepSec * 1000;
		SleepEx(sleepSec * 1000, FALSE);
	}

	GetSystemTimeAsFileTime(&ftLocalEnd);
	if (!get_web_time(host, ftWebEnd))
		return false;

	// PC's clock validation
	const int64_t localTimeDiff = std::abs(ftLocalEnd - ftLocalStart) / k100NStoMSecs;
	const int64_t webTimeDiff = std::abs(ftWebEnd - ftWebStart) / k100NStoMSecs;

	if (std::abs(localTimeDiff - webTimeDiff) > delta)
		sandboxDetected = true;

	// second check for proper sleep delay
	if (!sandboxDetected) {
		if (localTimeDiff < totalMSec)
			sandboxDetected = true;
		if (webTimeDiff < totalMSec)
			sandboxDetected = true;
	}
	return sandboxDetected;
}

} // SandboxEvasion
