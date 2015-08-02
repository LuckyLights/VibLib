//
//  VibDevice.h
//  VibLib
//
//  Created by Lucas Duroj on 23/07/15.
//  Copyright (c) 2015 Lucas Duroj. All rights reserved.
//

#ifndef __TestVibration__VibDevice__
#define __TestVibration__VibDevice__

#include "VibEffect.h"

#include <IOKit/IOKitLib.h>
#include <ForceFeedback/ForceFeedback.h>

#include <stdio.h>
#include <map>
#include <string>

using namespace std;

class VibDevice {
public:
	VibDevice(io_service_t ioService);
	~VibDevice();
	
	//IO ref
	io_service_t ioService;
	
	//FF ref
	FFDeviceObjectReference ffDevice;
	
	//Device name
	char* name;
	
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
	void setDeviceName();
	void setUsagePage();
	void setForceFeedback();
	void setCapabilities();
	
	bool sendCommand(FFCommandFlag flags);
	
	map<string, VibEffect*> effects;
};


#endif /* defined(__TestVibration__VibDevice__) */
