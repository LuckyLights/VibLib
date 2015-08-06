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

#include "VibEffect.h"
#include "VibDevice.h"

#include <CoreFoundation/CFUUID.h>
#include <stdlib.h>

CFUUIDRef effectTypeToNative(VibEffectType vibType) {
	switch (vibType) {
		case EFFECT_TYPE_NONE:
			VibLibError("VibEffect: can't match EFFECT_TYPE_NONE to native ID");
			break;
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
		VibLibError("VibEffect: Failed to create effect", ret);
	}
}

VibEffect::~VibEffect() {
	freeFFEffect();
}

void VibEffect::start() {
	HRESULT ret = FFEffectStart(ffEffectRef, 1, 0);
	if (ret != FF_OK) {
		VibLibError("VibEffect: Failed to start effect", ret);
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
			VibLibError("VibEffect: Unknown direction type.");
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
		VibLibError("VibEffect: Unable to update effect.", ret);
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
		case EFFECT_TYPE_NONE:
			VibLibError("VibEffect: can't update effect with type EFFECT_TYPE_NONE");
			return;
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