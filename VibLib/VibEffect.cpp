//
//  VibEffect.cpp
//  VibLib
//
//  Created by Lucas Duroj on 23/07/15.
//  Copyright (c) 2015 Lucas Duroj. All rights reserved.
//

#include "VibEffect.h"
#include "VibDevice.h"
#include <stdlib.h>
#include <CoreFoundation/CFUUID.h>

#define CCONVERT(x)   (((x) > 0x7FFF) ? 10000 : ((x)*10000) / 0x7FFF)
#define CONVERT(x)    (((x)*10000) / 0x7FFF)

const char* FFStrError(signed long err) {
	switch (err) {
		case FFERR_DEVICEFULL:
			return "device full";
		case FFERR_DEVICEPAUSED:
			return "device paused";
		case FFERR_DEVICERELEASED:
			return "device released";
		case FFERR_EFFECTPLAYING:
			return "effect playing";
		case FFERR_EFFECTTYPEMISMATCH:
			return "effect type mismatch";
		case FFERR_EFFECTTYPENOTSUPPORTED:
			return "effect type not supported";
		case FFERR_GENERIC:
			return "undetermined error";
		case FFERR_HASEFFECTS:
			return "device has effects";
		case FFERR_INCOMPLETEEFFECT:
			return "incomplete effect";
		case FFERR_INTERNAL:
			return "internal fault";
		case FFERR_INVALIDDOWNLOADID:
			return "invalid download id";
		case FFERR_INVALIDPARAM:
			return "invalid parameter";
		case FFERR_MOREDATA:
			return "more data";
		case FFERR_NOINTERFACE:
			return "interface not supported";
		case FFERR_NOTDOWNLOADED:
			return "effect is not downloaded";
		case FFERR_NOTINITIALIZED:
			return "object has not been initialized";
		case FFERR_OUTOFMEMORY:
			return "out of memory";
		case FFERR_UNPLUGGED:
			return "device is unplugged";
		case FFERR_UNSUPPORTED:
			return "function call unsupported";
		case FFERR_UNSUPPORTEDAXIS:
			return "axis unsupported";
			
		default:
			return "unknown error";
	}
}


CFUUIDRef effectTypeToNative(VibEffectType vibType) {
	switch (vibType) {
		case EFFECT_TYPE_SINE:
			return kFFEffectType_Sine_ID;
			break;
		case EFFECT_TYPE_SQUARE:
			return kFFEffectType_Square_ID;
			break;
		case EFFECT_TYPE_TRIANGLE:
			return kFFEffectType_Triangle_ID;
			break;
		case EFFECT_TYPE_SAWTOOTH_DOWN:
			return kFFEffectType_SawtoothDown_ID;
			break;
		case EFFECT_TYPE_SAWTOOTH_UP:
			return kFFEffectType_SawtoothUp_ID;
			break;
			
		case EFFECT_TYPE_CONSTANT:
			return kFFEffectType_ConstantForce_ID;
			break;
			
		case EFFECT_TYPE_CUSTOM:
			return kFFEffectType_CustomForce_ID;
			break;
	}
	return kFFEffectType_Sine_ID;
}

VibEffect::VibEffect(const VibDevice* device, string name, const VibEffectData &data) :
data(data),
device(device),
name(name),
ffEffectRef(NULL)
{
	createFFEffect();
	updateFFEffect();
	
	//Create FF effect
	HRESULT ret = FFDeviceCreateEffect(device->ffDevice, effectTypeToNative(data.type), &ffEffect, &ffEffectRef);
	if(ret != FF_OK) {
		printf("VibEffect: Failed to create effect");
	}
}

VibEffect::~VibEffect() {
	freeFFEffect();
}

void VibEffect::start() {
	HRESULT ret = FFEffectStart(ffEffectRef, 1, 0);
	if (ret != FF_OK) {
		printf("VibEffect: Failed to start effter");
	}
}

void VibEffect::updateCord() {
	
	// if no direction as assigne default to SPHERICAL
	if (data.effect.axesCount == 0) {
		ffEffect.dwFlags |= FFEFF_SPHERICAL;
		ffEffect.rglDirection = NULL;
	}
	
	LONG* ffDir = ffEffect.rglDirection;
	if(ffDir == NULL) {
		ffDir = (LONG*)malloc(sizeof(LONG) * data.effect.axesCount);
	}
	memset(ffDir, 0, sizeof(LONG) * data.effect.axesCount);
	ffEffect.rglDirection = ffDir;
	
	switch (data.effect.axisCord) {
		case FFEFF_POLAR:
			ffEffect.dwFlags |= FFEFF_POLAR;
			ffDir[0] = data.effect.axes[0];
			break;
		case FFEFF_CARTESIAN:
			ffEffect.dwFlags |= FFEFF_CARTESIAN;
			ffDir[0] = data.effect.axes[0];
			if (data.effect.axesCount > 1)
				ffDir[1] = data.effect.axes[1];
			if (data.effect.axesCount > 2)
				ffDir[2] = data.effect.axes[2];
			break;
		case FFEFF_SPHERICAL:
			ffEffect.dwFlags |= FFEFF_SPHERICAL;
			ffDir[0] = data.effect.axes[0];
			if (data.effect.axesCount > 1)
				ffDir[1] = data.effect.axes[1];
			if (data.effect.axesCount > 2)
				ffDir[2] = data.effect.axes[2];
			break;
		default:
			printf("VibEffect: Unknown direction type.");
	}
}

void VibEffect::updateEnvelope() {
	
	if ((data.envelope.attackTime == 0) && (data.envelope.fadeTime == 0)) {
		if(ffEffect.lpEnvelope != NULL) {
			free(ffEffect.lpEnvelope);
			ffEffect.lpEnvelope = NULL;
		}
	} else {
		FFENVELOPE* ffEnvelope = ffEffect.lpEnvelope;
		if(ffEnvelope == NULL) {
			ffEnvelope = (FFENVELOPE*)malloc(sizeof(FFENVELOPE));
		}
		memset(ffEnvelope, 0, sizeof(FFENVELOPE));
		ffEffect.lpEnvelope = ffEnvelope;
		
		ffEnvelope->dwSize = sizeof(FFENVELOPE);
		ffEnvelope->dwAttackLevel = CCONVERT(data.envelope.attackLevel);
		ffEnvelope->dwAttackTime = data.envelope.attackTime * 1000;
		ffEnvelope->dwFadeLevel = CCONVERT(data.envelope.fadeLevel);
		ffEnvelope->dwFadeTime = data.envelope.fadeTime * 1000;
	}
}

void VibEffect::updateEffect(const VibEffectData& data) {
	this->data = data;
	if(lastType != EFFECT_TYPE_NONE && lastType != data.type) {
		freeFFEffect();
		createFFEffect();
	}
	updateFFEffect();
	lastType = data.type;
	
	static const FFEffectParameterFlag flags = FFEP_DIRECTION | FFEP_DURATION | FFEP_ENVELOPE | FFEP_STARTDELAY | FFEP_TYPESPECIFICPARAMS;
	HRESULT ret = FFEffectSetParameters(ffEffectRef, &ffEffect, flags);
	if (ret != FF_OK) {
		printf("VibEffect: Unable to update effect.");
	}
}

void VibEffect::createFFEffect() {
	memset(&ffEffect, 0, sizeof(FFEFFECT));
	ffEffect.rgdwAxes = NULL;
	ffEffect.rglDirection = NULL;
	ffEffect.lpvTypeSpecificParams = NULL;
	ffEffect.lpEnvelope = NULL;
}

void VibEffect::updateFFEffect() {
	ffEffect.dwSize = sizeof(FFEFFECT);
	ffEffect.dwSamplePeriod = data.effect.samplePeriod;
	ffEffect.dwGain = data.effect.gain;
	ffEffect.dwFlags = FFEFF_OBJECTOFFSETS;
	
	ffEffect.dwDuration = data.effect.duration * 1000;
	ffEffect.dwStartDelay = data.effect.delay * 1000;    /* In microseconds. */
	
	ffEffect.cAxes = device->axisCount;
	if (ffEffect.cAxes > 0) {
		DWORD* axes = ffEffect.rgdwAxes;
		if(!axes)
			axes = (DWORD*)malloc(sizeof(DWORD) * ffEffect.cAxes);
		axes[0] = device->axes[0];
		if (ffEffect.cAxes > 1) {
			axes[1] = device->axes[1];
		}
		if (ffEffect.cAxes > 2) {
			axes[2] = device->axes[2];
		}
		ffEffect.rgdwAxes = axes;
	}
	
	switch (data.type) {
		case EFFECT_TYPE_SINE:
		case EFFECT_TYPE_TRIANGLE:
		case EFFECT_TYPE_SQUARE:
		case EFFECT_TYPE_SAWTOOTH_DOWN:
		case EFFECT_TYPE_SAWTOOTH_UP: { 	//update periodic data
			FFPERIODIC* periodic = (FFPERIODIC*)ffEffect.lpvTypeSpecificParams;
			if(periodic == NULL) {
				periodic = (FFPERIODIC*)malloc(sizeof(FFPERIODIC));
			}
			memset(periodic, 0, sizeof(FFPERIODIC));
			periodic->dwMagnitude = CONVERT(data.periodic.magnitude);
			periodic->lOffset = CONVERT(data.periodic.offset);
			periodic->dwPhase = data.periodic.phase;
			periodic->dwPeriod = data.periodic.period * 1000;
			
			ffEffect.cbTypeSpecificParams = sizeof(FFPERIODIC);
			ffEffect.lpvTypeSpecificParams = periodic;
			
			updateEnvelope();
			updateCord();
		}
			break;
		case EFFECT_TYPE_CONSTANT: {
			FFCONSTANTFORCE* constant = (FFCONSTANTFORCE*)ffEffect.lpvTypeSpecificParams;
			if(constant == NULL) {
				constant = (FFCONSTANTFORCE*)malloc(sizeof(FFCONSTANTFORCE));
			}
			memset(constant, 0, sizeof(FFCONSTANTFORCE));
			constant->lMagnitude = CONVERT(data.constant.magnitude);
			
			ffEffect.cbTypeSpecificParams = sizeof(FFCONSTANTFORCE);
			ffEffect.lpvTypeSpecificParams = constant;
			
			updateEnvelope();
			updateCord();
		}
			break;
		case EFFECT_TYPE_CUSTOM: {
			FFCUSTOMFORCE* custom = (FFCUSTOMFORCE*)ffEffect.lpvTypeSpecificParams;
			if(custom == NULL) {
				custom = (FFCUSTOMFORCE*)malloc(sizeof(FFCUSTOMFORCE));
			}
			memset(custom, 0, sizeof(FFCUSTOMFORCE));
			
			custom->cChannels = data.custom.channelCount;
			custom->dwSamplePeriod = data.custom.samplePeriod * 1000;
			custom->cSamples = data.custom.sampelCount;
			
			custom->rglForceData = (LONG*)malloc(sizeof(LONG) * custom->cSamples * custom->cChannels);
			for (int i = 0; i < custom->cSamples * custom->cChannels; ++i) {
				custom->rglForceData[i] = CCONVERT(data.custom.forceData[i]);
			}
			
			ffEffect.cbTypeSpecificParams = sizeof(FFCUSTOMFORCE);
			ffEffect.lpvTypeSpecificParams = custom;
			
			updateEnvelope();
			updateCord();
		}
			break;
	}
	
}

void VibEffect::freeFFEffect() {
	
	if(ffEffect.rgdwAxes)
		free(ffEffect.rgdwAxes);
	if(ffEffect.lpvTypeSpecificParams)
		free(ffEffect.lpvTypeSpecificParams);
	if(ffEffect.rglDirection)
		free(ffEffect.rglDirection);
	
	if(ffEffectRef != NULL) {
		FFEffectUnload(ffEffectRef);
		ffEffectRef = NULL;
	}
}