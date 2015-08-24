using UnityEngine;

public class TestVibLib : MonoBehaviour {
	public float magnitude = 1.0f;
	public float duration = 1.0f;

	public VibrationEffect _VibrationEffect;

	void Start() {
		VibLib.Init();
		VibLib.UploadEffect(_VibrationEffect);
    }

	void Update() {
		if (Input.GetKeyDown(KeyCode.Space))
			VibLib.Rumble(0, magnitude, duration);

		if (Input.GetKeyDown(KeyCode.Return))
			VibLib.PlayEffect(0, _VibrationEffect);
	}
}
