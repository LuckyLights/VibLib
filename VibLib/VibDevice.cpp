//
//  VibDevice.cpp
//  VibLib
//
//  Created by Lucas Duroj on 23/07/15.
//  Copyright (c) 2015 Lucas Duroj. All rights reserved.
//

#include "VibDevice.h"
#include <IOKit/hid/IOHIDKeys.h>
#include <CoreFoundation/CFNumber.h>
#include <assert.h>

VibDevice::VibDevice(io_service_t ioService) {
	this->ioService = ioService;
	this->setDeviceName();
	this->setUsagePage();
	this->setForceFeedback();
	this->setCapabilities();
}

VibDevice::~VibDevice() {
	for (auto iter = effects.begin(); iter != effects.end(); ++iter) {
		delete iter->second;
	}
	effects.clear();
}

VibEffect* VibDevice::createEffect(string name, const VibEffectData& data) {
	if(effects.size() >= storageCapacity) {
		printf("VibDevice: Max amount of effect has been reached.");
		return NULL;
	}
	
	auto newEffect = new VibEffect(this, name, data);
	effects.insert(pair<string, VibEffect*>(newEffect->name, newEffect));
	
	return newEffect;
}

VibEffect* VibDevice::getEffect(string name) {
	return effects.at(name);
}


bool VibDevice::sendCommand(FFCommandFlag flags) {
	HRESULT ret = FFDeviceSendForceFeedbackCommand(ffDevice, flags);
	return ret == FF_OK;
}

void VibDevice::setDeviceName() {
	name = new char[256];
	
	CFMutableDictionaryRef hidProperties, usbProperties;
	io_registry_entry_t parent1, parent2;
	kern_return_t ret;
	
	hidProperties = usbProperties = 0;
	
	ret = IORegistryEntryCreateCFProperties(ioService, &hidProperties,
											kCFAllocatorDefault, kNilOptions);
	if ((ret != KERN_SUCCESS) || !hidProperties) {
		 printf("VibDevice: Unable to create CFProperties.");
	}
	
	/* Mac OS X currently is not mirroring all USB properties to HID page so need to look at USB device page also
	 * get dictionary for usb properties: step up two levels and get CF dictionary for USB properties
	 */
	if ((KERN_SUCCESS ==
		 IORegistryEntryGetParentEntry(ioService, kIOServicePlane, &parent1))
		&& (KERN_SUCCESS ==
			IORegistryEntryGetParentEntry(parent1, kIOServicePlane, &parent2))
		&& (KERN_SUCCESS ==
			IORegistryEntryCreateCFProperties(parent2, &usbProperties,
											  kCFAllocatorDefault,
											  kNilOptions))) {
				if (usbProperties) {
					CFTypeRef refCF = 0;
					/* get device info
					 * try hid dictionary first, if fail then go to usb dictionary
					 */
					
					
					/* Get product name */
					refCF = CFDictionaryGetValue(hidProperties, CFSTR(kIOHIDProductKey));
					if (!refCF) {
						refCF = CFDictionaryGetValue(usbProperties, CFSTR("USB Product Name"));
					}
					//					printf("refCF: %s", CFStringGetCStringPtr(refCF, CFStringGetSystemEncoding()));
					if (refCF) {
						if (!CFStringGetCString((CFStringRef)refCF, name, 256, CFStringGetSystemEncoding())) {
							 printf("VibDevice: CFStringGetCString error retrieving pDevice->product.");
						}
					}
					
					CFRelease(usbProperties);
				} else {
					printf("VibDevice: IORegistryEntryCreateCFProperties failed to create usbProperties.");
				}
				
				/* Release stuff. */
				if (kIOReturnSuccess != IOObjectRelease(parent2)) {
					printf("VibDevice: IOObjectRelease error with parent2.");
				}
				if (kIOReturnSuccess != IOObjectRelease(parent1)) {
					printf("VibDevice: IOObjectRelease error with parent1.");
				}
			} else {
				printf("VibDevice: Error getting registry entries.");
			}
}


void VibDevice::setUsagePage() {
	CFMutableDictionaryRef hidProperties = 0;
	CFTypeRef refCF = 0;
	IOReturn result = IORegistryEntryCreateCFProperties(ioService, &hidProperties, kCFAllocatorDefault, kNilOptions);
	
	if ((result == KERN_SUCCESS) && hidProperties) {
		refCF = CFDictionaryGetValue(hidProperties, CFSTR(kIOHIDPrimaryUsagePageKey));
		if (refCF) {
			if (!CFNumberGetValue((CFNumberRef)refCF, kCFNumberLongType,  &usagePage))
				printf("VibDevice: Recieving device's usage page.");
			
			refCF = CFDictionaryGetValue(hidProperties, CFSTR(kIOHIDPrimaryUsageKey));
			
			if (refCF && !CFNumberGetValue((CFNumberRef)refCF, kCFNumberLongType, &usage))
				printf("VibDevice: Recieving device's usage.");
		}
		CFRelease(hidProperties);
	}
}

void VibDevice::setForceFeedback() {
	HRESULT ret = FFCreateDevice(ioService, &ffDevice);
	if (ret != FF_OK) {
		printf("VibDevice: Unable to create device from service");
	}
	
	//Reset
	if(!sendCommand(FFSFFC_RESET))
		printf("VibDevice: Fialed to RESET!");
	if(!sendCommand(FFSFFC_SETACTUATORSON))
		printf("VibDevice: Failed to SETACTUATORSON!");

}

void VibDevice::setCapabilities() {
	FFCAPABILITIES features;
	HRESULT ret = FFDeviceGetForceFeedbackCapabilities(ffDevice, &features);
	if (ret != FF_OK) {
		printf("VibDevice: Unable to get FeedbackCapabilities");
	}
	UInt32 val;
	ret = FFDeviceGetForceFeedbackProperty(ffDevice, FFPROP_FFGAIN, &val, sizeof(val));
	if (ret != FF_OK) {
		printf("VibDevice: Device dos nit support gain");
	}
	storageCapacity = features.storageCapacity;
	playbackCapacity = features.playbackCapacity;
	axisCount = ((features.numFfAxes) > 3) ? 3 : features.numFfAxes;
	memcpy(axes, features.ffAxes, axisCount * sizeof(UInt8));
}