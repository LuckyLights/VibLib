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

#include "VibDevice.h"
#include <IOKit/hid/IOHIDKeys.h>
#include <CoreFoundation/CFNumber.h>
#include <assert.h>
#include "VibUtil.h"

VibDevice::VibDevice(io_service_t ioService) :
ioService(ioService),
name(NULL)
{
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
	
	if(name != NULL)
		delete name;
}

VibEffect* VibDevice::createEffect(const string name, const VibEffectData& data) {
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

void VibDevice::deleteEffect(const string name) {
	deleteEffect(getEffect(name));
}

void VibDevice::deleteEffect(VibEffect* effect) {
	if(effect == NULL)
		return;
	VibEffect* _effect = effects.at(effect->name);
	if(_effect == effect) {
		effects.erase(_effect->name);
		delete _effect;
	}
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
	if ((KERN_SUCCESS == IORegistryEntryGetParentEntry(ioService, kIOServicePlane, &parent1)) &&
		(KERN_SUCCESS == IORegistryEntryGetParentEntry(parent1, kIOServicePlane, &parent2)) &&
		(KERN_SUCCESS == IORegistryEntryCreateCFProperties(parent2, &usbProperties, kCFAllocatorDefault, kNilOptions))) {
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
			
			
			if (refCF) {
				if (!CFStringGetCString((CFStringRef)refCF, name, 256, CFStringGetSystemEncoding())) {
					VibLibError("VibDevice: CFStringGetCString error retrieving pDevice->product.");
				}
			}
			
			CFRelease(usbProperties);
		} else {
			VibLibError("VibDevice: IORegistryEntryCreateCFProperties failed to create usbProperties.");
		}
		
		/* Release stuff. */
		if (kIOReturnSuccess != IOObjectRelease(parent2)) {
			VibLibError("VibDevice: IOObjectRelease error with parent2.");
		}
		if (kIOReturnSuccess != IOObjectRelease(parent1)) {
			VibLibError("VibDevice: IOObjectRelease error with parent1.");
		}
	} else {
		VibLibError("VibDevice: Error getting registry entries.");
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
				VibLibError("VibDevice: Recieving device's usage page.");
			
			refCF = CFDictionaryGetValue(hidProperties, CFSTR(kIOHIDPrimaryUsageKey));
			
			if (refCF && !CFNumberGetValue((CFNumberRef)refCF, kCFNumberLongType, &usage))
				VibLibError("VibDevice: Recieving device's usage.");
		}
		CFRelease(hidProperties);
	}
}

void VibDevice::setForceFeedback() {
	HRESULT ret = FFCreateDevice(ioService, &ffDevice);
	if (ret != FF_OK) {
		VibLibError("VibDevice: Unable to create device from service", ret);
	}
	
	//Reset
	ret = FFDeviceSendForceFeedbackCommand(ffDevice, FFSFFC_RESET);
	if(ret != FF_OK) {
		VibLibError("VibDevice: Fialed to send command FFSFFC_RESET", ret);
	}
	ret = FFDeviceSendForceFeedbackCommand(ffDevice, FFSFFC_SETACTUATORSON);
	if(ret != FF_OK) {
		VibLibError("VibDevice: Fialed to FFSFFC_SETACTUATORSON", ret);
	}

}

void VibDevice::setCapabilities() {
	FFCAPABILITIES features;
	HRESULT ret = FFDeviceGetForceFeedbackCapabilities(ffDevice, &features);
	if (ret != FF_OK) {
		VibLibError("VibDevice: Unable to get FeedbackCapabilities", ret);
	}
	UInt32 val;
	ret = FFDeviceGetForceFeedbackProperty(ffDevice, FFPROP_FFGAIN, &val, sizeof(val));
	if (ret != FF_OK) {
		VibLibError("VibDevice: Device dos not support gain", ret);
	}
	storageCapacity = features.storageCapacity;
	playbackCapacity = features.playbackCapacity;
	axisCount = ((features.numFfAxes) > 3) ? 3 : features.numFfAxes;
	memcpy(axes, features.ffAxes, axisCount * sizeof(UInt8));
}