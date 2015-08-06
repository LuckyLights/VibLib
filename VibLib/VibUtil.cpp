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

#include "VibUtil.h"

const char* ffErrorToString(signed long err) {
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

void VibLibError(const char* message, signed long err) {
	printf("%s - err: %s\n", message, ffErrorToString(err));
}

void VibLibError(const char* message) {
	printf("%s\n", message);
}
