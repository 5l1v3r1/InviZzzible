#ifndef _GENERIC_H
#define _GENERIC_H

#include "ve_detection.h"
#include "json.h"

namespace SandboxEvasion {

class Generic : VEDetection {
public:
	Generic(const json_tiny &j) : VEDetection(j) {
		module_name = std::string("GENERIC");
	}
	virtual ~Generic() {}

	static VEDetection* create_instance(const json_tiny &j);

	// overriden
	virtual void CheckAllCustom();

protected:
	bool CheckSystemUptime() const;
	bool CheckRAM() const;
	bool CheckDiskSize() const;
	bool CheckDriveSize() const;
	bool CheckMouseActive() const;
	bool CheckSleepDummyPatch() const;
	bool CheckNumberOfProcessors() const;
	bool CheckDNSResponse() const;
};

} // SandboxEvasion

#endif // !_GENERIC_H
