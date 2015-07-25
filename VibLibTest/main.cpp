//
//  main.cpp
//  VibLibTest
//
//  Created by Lucas Duroj on 23/07/15.
//  Copyright (c) 2015 Lucas Duroj. All rights reserved.
//

#include <stdio.h>
#include "vibLib/VibLib.h"

int main (void) {
	VibLib* vibLib = new VibLib();
	vibLib->scanDevices();
	int deviceCount = vibLib->getDeviceCount();
	if(deviceCount > 0) {
		for (int i = 0; i < deviceCount; ++i) {
			auto device = vibLib->getDevice(i);
			printf("Found device: %s\n", device->name);
		}
		
		printf("Test 1: Create effect \n");
		VibEffectData data = VibEffectData();
		data.type = kFFEffectType_Square_ID;
		data.periodic.period = 1000;
		data.periodic.magnitude = 10000;
		data.effect.duration = 1000;
		data.effect.axisCord = FFEFF_POLAR;
		
		for (int i = 0; i < deviceCount; ++i) {
			auto device = vibLib->getDevice(i);
			auto effect = device->createEffect("vibrate", data);
			effect->start();
			printf("Test 1: Play device num: %i \n", i);
			sleep(1);
		}
		
		sleep(1);
		
		printf("Test 2: Update effect \n");
		for (int i = 0; i < deviceCount; ++i) {
			auto device = vibLib->getDevice(i);
			
			data.effect.duration = 4000;
			data.periodic.magnitude = 30000;
			data.type = kFFEffectType_Square_ID;
			
			auto effect = device->getEffect("vibrate");
			if(effect != NULL)
				effect->updateEffect(data);
			
			effect->start();
		}
		sleep(4);
		
		printf("end");
	}
	
	delete vibLib;
}