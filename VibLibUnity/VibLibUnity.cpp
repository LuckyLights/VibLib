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

#include <stdio.h>
#include "VibLib.h"
#define STD_RUMBLE "std_rumble"

extern "C" {
	
	
	VibLib* vibLib = NULL;
	VibEffectData rumbleData;
	
	void Init( void ) {
		if(vibLib != NULL) {
			delete vibLib;
			vibLib = NULL;
		}
		
		vibLib = new VibLib();
		vibLib->scanDevices();
		
		rumbleData = VibEffectData();
		rumbleData.periodic.period = 1000;
		rumbleData.periodic.magnitude = 10000;
		rumbleData.effect.duration = 500;
		
		for (int i = 0; i < vibLib->getDeviceCount(); ++i) {
			auto device = vibLib->getDevice(i);
			device->createEffect(STD_RUMBLE, rumbleData);
		}
	}
	
	void ScanDevices( void ) {
		vibLib->scanDevices();
	}
	
	void Rumble(int controllerIndex, float magnitude, float duration) {
		rumbleData.periodic.magnitude = (UInt32)(magnitude*32000.0f);
		rumbleData.effect.duration = (UInt32)(duration*1000.0f);
		
		int i = controllerIndex;
		if(i < 0 || i >= vibLib->getDeviceCount())
			return;
		
		auto device = vibLib->getDevice(i);
		auto effect = device->getEffect(STD_RUMBLE);
		effect->updateEffect(rumbleData);
		if(effect != NULL)
			effect->start();
	}
	
	
	VibEffectType typeTable[] = {EFFECT_TYPE_SINE, EFFECT_TYPE_SINE, EFFECT_TYPE_TRIANGLE, EFFECT_TYPE_SAWTOOTH_UP, EFFECT_TYPE_SAWTOOTH_DOWN};
	
	void UploadPeriodicEffect(const char* name, float magnitude, float duration, int periodicType, float offset, int period, float phase) {
		VibEffectData newData = VibEffectData();
		newData.type = typeTable[periodicType];
		newData.effect.duration = (UInt32)(duration*1000.0f);
		
		newData.periodic.magnitude = (UInt32)(magnitude*32000.0f);
		newData.periodic.period = period;
		newData.periodic.offset = offset;
		newData.periodic.phase = phase;
		
		for (int i = 0; i < vibLib->getDeviceCount(); ++i) {
			auto device = vibLib->getDevice(i);
			device->createEffect(name, newData);
		}
	}
	
	void UploadConstantEffect(const char* name, float magnitude, float duration) {
		VibEffectData newData = VibEffectData();
		newData.type = EFFECT_TYPE_CONSTANT;
		newData.constant.magnitude = (LONG)(magnitude*32000.0f);
		newData.effect.duration = (UInt32)(duration*1000.0f);
		
		for (int i = 0; i < vibLib->getDeviceCount(); ++i) {
			auto device = vibLib->getDevice(i);
			device->createEffect(name, newData);
		}
	}
	
	void _PlayEffect(int controllerIndex, const char* name) {
		int i = controllerIndex;
		if(i < 0 || i >= vibLib->getDeviceCount())
			return;
		
		auto device = vibLib->getDevice(i);
		auto effect = device->getEffect(name);
		if(effect != NULL)
			effect->start();
	}
	
}