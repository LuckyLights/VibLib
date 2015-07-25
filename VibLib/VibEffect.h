//
//  VibEffect.h
//  VibLib
//
//  Created by Lucas Duroj on 23/07/15.
//  Copyright (c) 2015 Lucas Duroj. All rights reserved.
//

#ifndef __TestVibration__VibEffect__
#define __TestVibration__VibEffect__

#include <stdio.h>
#include <ForceFeedback/ForceFeedback.h>
#include <CoreFoundation/CFUUID.h>
#include <string>

using namespace std;

class VibDevice;

struct VibEffectData {
	CFUUIDRef type = kFFEffectType_Sine_ID;
	
	struct {
		UInt32 duration = 1000;
		UInt32 delay = 0;
		UInt32 gain = 10000;
		UInt32 samplePeriod = 0;
		UInt32 axesCount = 0;
		FFCoordinateSystemFlag axisCord = FFEFF_POLAR;
		SInt32 axes[3];
	} effect;

	
	struct {
		UInt32 attackLevel;
		UInt32 attackTime;
		UInt32 fadeLevel;
		UInt32 fadeTime;
	} envelop;
	
	struct {
		SInt16 magnitude;
		SInt16 offset;
		UInt32 phase;
		UInt32 period;
	} periodic;
	
};

class VibEffect {
public:
	VibEffect(const VibDevice* device, string name, const VibEffectData& data);
	~VibEffect();
	
	string name;
	VibEffectData data;
	
	void start();
	
	void updateEffect(const VibEffectData& data);
	
private:
	void updateFFEffect();
	void updateCord();
	const VibDevice* device;
	FFEFFECT ffEffect;
	FFEffectObjectReference ffEffectRef;
};

#endif /* defined(__TestVibration__VibEffect__) */
