//
//  VibLib.cpp
//  VibLib
//
//  Created by Lucas Duroj on 23/07/15.
//  Copyright (c) 2015 Lucas Duroj. All rights reserved.
//

#include "VibLib.h"
#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <ForceFeedback/ForceFeedback.h>
#include <CoreFoundation/CoreFoundation.h>

VibLib::VibLib() {
	
}

VibLib::~VibLib() {
	
}

void VibLib::scanDevices() {
	
	vibDevices.clear();
	
	CFDictionaryRef match = IOServiceMatching(kIOHIDDeviceKey);
	if (match == NULL) {
		printf("VibLib: Failed to get IOServiceMatching.");
	}
	
	io_iterator_t iter;
	IOReturn result = IOServiceGetMatchingServices(kIOMasterPortDefault, match, &iter);
	if (result != kIOReturnSuccess) {
		printf("VibLib: Couldn't create a HID object iterator.");
	}

	if (!IOIteratorIsValid(iter)) {
		return;
	}
	
	io_service_t ioService;
	while ((ioService = IOIteratorNext(iter)) != IO_OBJECT_NULL) {
		if (FFIsForceFeedback(ioService) == FF_OK) {
			vibDevices.push_back(new VibDevice(ioService));
		} else {
			IOObjectRelease(ioService);
		}
	}
	
	IOObjectRelease(iter);
}

UInt32 VibLib::getDeviceCount() {
	return (UInt32)vibDevices.size();
}

VibDevice* VibLib::getDevice(UInt32 index) {
	if(index >= getDeviceCount())
		return NULL;
	return vibDevices[index];
}