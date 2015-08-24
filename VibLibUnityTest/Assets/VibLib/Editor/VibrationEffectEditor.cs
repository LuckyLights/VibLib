using System;
using UnityEditor;
using UnityEngine;

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
