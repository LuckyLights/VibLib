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

using System.IO;
using UnityEditor;
using UnityEngine;

public enum VibrationEffectType {
	Periodic = 0,
	Constant = 1,
}

public enum VibrationEffectPeriodicType {
	Sine = 0,
	Square = 1,
	Triangle = 2,
	SawtoothUp = 3,
	SawtoothDown = 4,
}

public class VibrationEffect : ScriptableObject {

	public string UnityName;
	public VibrationEffectType EffectType;
	public VibrationEffectPeriodicType PeriodicType;
	public float Duration = 0.5f;
	public float Magnitude = 0.5f;

	//Periodic data
	public int Offset = 0;
	public int Phase = 0;
	public int Period = 1000;

	//Static
	[MenuItem("Assets/Create/VibrationEffect")]
	public static void CreateAsset() {
		VibrationEffect asset = ScriptableObject.CreateInstance<VibrationEffect>();

		string path = AssetDatabase.GetAssetPath(Selection.activeObject);
		if (path == "") {
			path = "Assets";
		} else if (Path.GetExtension(path) != "") {
			path = path.Replace(Path.GetFileName(AssetDatabase.GetAssetPath(Selection.activeObject)), "");
		}
		string assetPathAndName = AssetDatabase.GenerateUniqueAssetPath(path + "/VibrationEffect.asset");

		AssetDatabase.CreateAsset(asset, assetPathAndName);

		AssetDatabase.SaveAssets();
		AssetDatabase.Refresh();
		EditorUtility.FocusProjectWindow();
		Selection.activeObject = asset;
	}


}
