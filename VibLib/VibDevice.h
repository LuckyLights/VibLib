/*
 VibLib
 Copyright (c) 2015 Lucas Kampmann Duroj <lucasduroj@gmail.com>
 
 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.
 
 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:
 
 1. The origin of this software must not be misrepresented; you must not
 claim that you wrote the original software. If you use this software
 in a product, an acknowledgement in the product documentation would be
 appreciated but is not required.
 
 2. Altered source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 
 3. This notice may not be removed or altered from any source distribution.
 
 */

#ifndef __VibLib__VibDevice__
#define __VibLib__VibDevice__

#include "VibEffect.h"

#include <IOKit/IOKitLib.h>
#include <ForceFeedback/ForceFeedback.h>

#include <stdio.h>
#include <map>
#include <string>
using namespace std;

class VibDeviceInfo {
public:
	VibDeviceInfo(const io_service_t ioService);
	
	bool compare(const VibDeviceInfo& b) const  {
		return (strcmp(name, b.name) == 0 && strcmp(serial, b.serial) == 0);
	}
	
	const io_service_t ioService;
	char* name;
	char* serial;
};

class VibDevice {
public:
	VibDevice(const VibDeviceInfo info);
	~VibDevice();
	
	//Info
	const VibDeviceInfo info;
	
	//FF ref
	FFDeviceObjectReference ffDevice;
	
	//Usage page
	long usage;
	long usagePage;
	
	//Capabilities
	UInt32 storageCapacity;
	UInt32 playbackCapacity;
	UInt32 axisCount;
	UInt8 axes[3];
	
	VibEffect* createEffect(string name, const VibEffectData& data);
	VibEffect* getEffect(string name);
	void deleteEffect(string name);
	void deleteEffect(VibEffect* effect);
	
private:
	void setUsagePage();
	void setForceFeedback();
	void setCapabilities();
	
	map<string, VibEffect*> effects;
};


#endif /* defined(__VibLib__VibDevice__) */
