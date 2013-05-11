package anibike.loader;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import anibike.widget.ActionBar;
import anibike.widget.ActionBar.AbstractAction;

public class deviceControlActivity extends Activity
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);

		// Setup the window
		setContentView(R.layout.device_control);

		// Setup the action-bar
		final ActionBar actionBar = (ActionBar) findViewById(R.id.dev_control_actionbar);
		actionBar.setTitle("Connected");
		actionBar.addAction(new preferencesAction());
		actionBar.setOnTitleClickListener(new OnClickListener()
		{
			public void onClick(View v)
			{
				Intent aboutDeviceIntent = new Intent("anibike.loader.ABOUTDEVICEACTIVITY");

				aboutDeviceIntent.putExtra("Manufacturer", "n/a");
				aboutDeviceIntent.putExtra("Model", "n/a");
				aboutDeviceIntent.putExtra("Version", "n/a");
				aboutDeviceIntent.putExtra("SerialNumber", "n/a");
				startActivity(aboutDeviceIntent);
			}
		});
	}

	private class preferencesAction extends AbstractAction
	{

		public preferencesAction()
		{
			super(R.drawable.ic_menu_preferences);

		}

		public void performAction(View view)
		{
			// refreshDeviceList();
		}

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.option_menu, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{

		// Handle item selection
		switch (item.getItemId())
		{
		case R.id.preferences:
			return super.onOptionsItemSelected(item);
		case R.id.help:
			return super.onOptionsItemSelected(item);
		case R.id.about:
				Intent aboutPanel = new Intent("anibike.loader.ABOUTPANELACTIVITY");
				startActivity(aboutPanel);
			return super.onOptionsItemSelected(item);
		default:
			return super.onOptionsItemSelected(item);
		}
	}
}
