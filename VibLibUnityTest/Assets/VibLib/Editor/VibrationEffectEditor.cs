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

using UnityEditor;

[CustomEditor(typeof(VibrationEffect))]
class VibrationEffectEditor : Editor {
	public override void OnInspectorGUI() {
		serializedObject.Update();

		var effect = target as VibrationEffect;
		if (effect != null) {
			effect.UnityName = EditorGUILayout.TextField("Effect name", effect.UnityName);
			effect.EffectType = (VibrationEffectType)EditorGUILayout.EnumPopup("Type", effect.EffectType);

			EditorGUILayout.Space();

			switch (effect.EffectType) {
				case VibrationEffectType.Periodic:
					effect.PeriodicType = (VibrationEffectPeriodicType)EditorGUILayout.EnumPopup("Periodic", effect.PeriodicType);

					effect.Offset = EditorGUILayout.IntField("Offset (μs)", effect.Offset);
					effect.Phase = EditorGUILayout.IntField("Phase (μs)", effect.Phase);
					effect.Period = EditorGUILayout.IntField("Period (μs)", effect.Period);

					break;
				case VibrationEffectType.Constant:
					break;
			}
		}

		EditorGUILayout.Space();

		effect.Duration = EditorGUILayout.FloatField("Duration (sec)", effect.Duration);
		effect.Magnitude = EditorGUILayout.Slider("Magnitude", effect.Magnitude, 0.0f, 1.0f);
	}
}
