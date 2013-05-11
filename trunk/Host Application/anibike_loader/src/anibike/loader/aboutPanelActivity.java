package anibike.loader;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;

public class aboutPanelActivity extends Activity
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		Log.d("About Panel Activity", "Created a new 'aboutPanelActivity'");
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		// Setup the window
		setContentView(R.layout.about_panel);
	}
}
