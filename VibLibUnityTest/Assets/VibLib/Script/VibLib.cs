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

using System.Runtime.InteropServices;public class VibLib {	private const string DLL = "VibLibUnity";

	[DllImport(DLL)]	public static extern void Init();

	[DllImport(DLL)]	public static extern void ScanDevices();	[DllImport(DLL)]	public static extern void Rumble(int controllerIndex, float magnitude, float duration);	public static void UploadEffect(VibrationEffect effect) {		switch (effect.EffectType) {			case VibrationEffectType.Constant:				UploadConstantEffect(effect.UnityName, effect.Magnitude, effect.Duration);                break;
			case VibrationEffectType.Periodic:				UploadPeriodicEffect(effect.UnityName, effect.Magnitude, effect.Duration, (int)effect.PeriodicType, effect.Offset, effect.Period, effect.Phase);
				break;		}	}

	[DllImport(DLL)]	private static extern void UploadPeriodicEffect(string name, float magnitude, float duration, int periodicType,		float offset, int period, float phase);

	[DllImport(DLL)]	private static extern void UploadConstantEffect(string name, float magnitude, float duration);	public static void PlayEffect(int controllerIndex, VibrationEffect effect) {		_PlayEffect(controllerIndex, effect.UnityName);	}

	[DllImport(DLL)]	private static extern void _PlayEffect(int controllerIndex, string name);}