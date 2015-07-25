//
//  VibLib.h
//  VibLib
//
//  Created by Lucas Duroj on 23/07/15.
//  Copyright (c) 2015 Lucas Duroj. All rights reserved.
//

#ifndef __TestVibration__VibLib__
#define __TestVibration__VibLib__

#include <stdio.h>
#include <vector>
#include "VibDevice.h"

using namespace std;

class VibLib {
	vector<VibDevice*> vibDevices;
public:
	VibLib();
	~VibLib();
	
	void scanDevices();
	UInt32 getDeviceCount();
	VibDevice* getDevice(UInt32 index);
	
private:
	
};

#endif /* defined(__TestVibration__VibLib__) */
