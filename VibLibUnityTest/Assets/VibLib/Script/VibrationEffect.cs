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
