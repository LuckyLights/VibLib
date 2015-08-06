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

#include "VibLib.h"
#include "VibUtil.h"

#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <ForceFeedback/ForceFeedback.h>
#include <CoreFoundation/CoreFoundation.h>

VibLib::VibLib() {
	
}

VibLib::~VibLib() {
	
}

void VibLib::scanDevices() {
	vector<VibDevice*> prevDevices(vibDevices);
	
	vibDevices.clear();
	
	CFDictionaryRef match = IOServiceMatching(kIOHIDDeviceKey);
	if (match == NULL) {
		VibLibError("VibLib: Failed to get IOServiceMatching.");
	}
	
	io_iterator_t iter;
	IOReturn res = IOServiceGetMatchingServices(kIOMasterPortDefault, match, &iter);
	if (res != kIOReturnSuccess) {
		VibLibError("VibLib: Couldn't create a HID object iterator.");
	}

	if (!IOIteratorIsValid(iter)) {
		return;
	}
	
	io_service_t ioService;
	while((ioService = IOIteratorNext(iter)) != IO_OBJECT_NULL) {
		
		if (FFIsForceFeedback(ioService) == FF_OK) {
			const VibDeviceInfo info(ioService);
			
			bool found = false;
			
			int i = 0;
			for (auto & iter : prevDevices) {
				if(info.compare(iter->info)) {
					found = true;
					vibDevices.push_back(iter);
					prevDevices.erase(prevDevices.begin()+i);
					break;
				}
				++i;
				
			}
			if(!found)
				vibDevices.push_back(new VibDevice(info));
		} else {
			IOObjectRelease(ioService);
		}
	}
	IOObjectRelease(iter);
	prevDevices.clear();
}

UInt32 VibLib::getDeviceCount() {
	return (UInt32)vibDevices.size();
}

VibDevice* VibLib::getDevice(UInt32 index) {
	if(index >= getDeviceCount())
		return NULL;
	return vibDevices[index];
}