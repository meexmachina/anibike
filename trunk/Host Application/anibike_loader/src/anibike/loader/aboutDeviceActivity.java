package anibike.loader;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;

public class aboutDeviceActivity extends Activity
{
	private ListView mListView;
	private ArrayAdapter<String> mListAdapter;
	DeviceProperty[] mItems = new DeviceProperty[4];

	public class DeviceProperty
	{
		public DeviceProperty (String _name, String _value)
		{
			name = _name;
			value = _value;
		}
		public String name;
		public String value;
	}

	public abstract class TwoLineArrayAdapter<T> extends ArrayAdapter<T>
	{
		private int mListItemLayoutResId;

		public TwoLineArrayAdapter(Context context, T[] ts)
		{
			this(context, android.R.layout.two_line_list_item, ts);
		}

		public TwoLineArrayAdapter(Context context, int listItemLayoutResourceId, T[] ts)
		{
			super(context, listItemLayoutResourceId, ts);
			mListItemLayoutResId = listItemLayoutResourceId;
		}

		@Override
		public android.view.View getView(int position, View convertView, ViewGroup parent)
		{

			LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);

			View listItemView = convertView;
			if (null == convertView)
			{
				listItemView = inflater.inflate(mListItemLayoutResId, parent, false);
			}

			// The ListItemLayout must use the standard text item IDs.
			TextView lineOneView = (TextView) listItemView.findViewById(android.R.id.text1);
			TextView lineTwoView = (TextView) listItemView.findViewById(android.R.id.text2);
			lineOneView.setTextSize((float) 25.0);
			lineTwoView.setTextSize((float) 18.0);

			T t = (T) getItem(position);
			lineOneView.setText(lineOneText(t));
			lineTwoView.setText(lineTwoText(t));

			return listItemView;
		}

		public abstract String lineOneText(T t);

		public abstract String lineTwoText(T t);
	}

	public class DevicePropertyArrayAdapter extends TwoLineArrayAdapter<DeviceProperty>
	{
		public DevicePropertyArrayAdapter(Context context, DeviceProperty[] proplist)
		{
			super(context, proplist);
		}

		@Override
		public String lineOneText(DeviceProperty e)
		{
			return e.name;
		}

		@Override
		public String lineTwoText(DeviceProperty e)
		{
			return e.value;
		}
	}

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		Log.d("About Camera Activity", "Created a new 'aboutCameraActivity'");
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		// Setup the window
		setContentView(R.layout.about_device_panel);

		Intent infoIntent = getIntent();
		Bundle infoBundle = infoIntent.getExtras();

		mItems[0] = new DeviceProperty("Manufacturer", infoBundle.getString("Manufacturer"));
		mItems[1] = new DeviceProperty("Model", infoBundle.getString("Model"));
		mItems[2] = new DeviceProperty("Version", infoBundle.getString("Version"));
		mItems[3] = new DeviceProperty("Serial Number", infoBundle.getString("SerialNumber"));
				
		mListView = (ListView)findViewById(R.id.about_device_list_view);
		mListView.setAdapter(new DevicePropertyArrayAdapter(this, mItems));

	}
}
