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
#include <string>

using namespace std;

class VibDevice;

enum VibEffectType : UInt32 {
	EFFECT_TYPE_NONE = 0,
	//Periodics
	EFFECT_TYPE_SINE = 1,
	EFFECT_TYPE_SQUARE = 2,
	EFFECT_TYPE_TRIANGLE = 3,
	EFFECT_TYPE_SAWTOOTH_UP = 4,
	EFFECT_TYPE_SAWTOOTH_DOWN = 5,
	//Constant
	EFFECT_TYPE_CONSTANT = 6,
	//Custom
	EFFECT_TYPE_CUSTOM = 7,
};

struct VibEffectData {
	VibEffectType type = EFFECT_TYPE_SINE;
	
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
	} envelope;
	
	struct {
		SInt16 magnitude;
		SInt16 offset;
		UInt32 phase;
		UInt32 period;
	} periodic;
	
	struct {
		LONG magnitude;
	} constant;
	
	struct {
		UInt32 channelCount;
		UInt32 samplePeriod;
		UInt32 sampelCount;
		LONG* forceData;
	} custom;
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
	void createFFEffect();
	void updateFFEffect();
	void freeFFEffect();
	
	void updateEnvelope();
	void updateCord();
	
	VibEffectType lastType = EFFECT_TYPE_NONE;
	const VibDevice* device;
	FFEFFECT ffEffect;
	FFEffectObjectReference ffEffectRef;
};

#endif /* defined(__TestVibration__VibEffect__) */
