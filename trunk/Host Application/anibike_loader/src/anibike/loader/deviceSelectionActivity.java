package anibike.loader;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import anibike.widget.ActionBar;
import anibike.widget.ActionBar.AbstractAction;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Typeface;
import android.os.Bundle;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class deviceSelectionActivity extends Activity
{

	// Member fields
	private BluetoothAdapter mBtAdapter;
	private ListView mDevicesListView;
	private ImageAdapter mDevicesImageAdapter;
	private static String mAboutActivityPath = null;
	private static String mPreferencesActivityPath = null;
	private static String mHelpActivityPath = null;
	private static String mSelectedActivityPath = null;
	private static int mDrawableID = 0;

	// Intent request codes
	private static final int REQUEST_ENABLE_BT = 2;

	// Passing Intent extra
	public static String EXTRA_DEVICE_ADDRESS = "device_address";

	// Accepting Intent extra's
	final public static String EXTRA_SELECTION_INTENT_NAME = "selected_intent_name";
	final public static String EXTRA_ABOUT_INTENT_NAME = "about_intent_name";
	final public static String EXTRA_PREFERENCES_INTENT_NAME = "preferences_intent_name";
	final public static String EXTRA_HELP_INTENT_NAME = "help_intent_name";
	final public static String EXTRA_DRAWABLE_ID = "drawable_id";

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);

		// get extras
		Bundle extras = getIntent().getExtras();
		if (extras != null)
		{
			mAboutActivityPath = extras.getString(EXTRA_ABOUT_INTENT_NAME);
			mPreferencesActivityPath = extras.getString(EXTRA_PREFERENCES_INTENT_NAME);
			mHelpActivityPath = extras.getString(EXTRA_HELP_INTENT_NAME);
			mSelectedActivityPath = extras.getString(EXTRA_SELECTION_INTENT_NAME);
			mDrawableID = extras.getInt(EXTRA_DRAWABLE_ID);
		}
		
		// Setup the window
		setContentView(R.layout.device_selection);

		// Setup the action-bar
		final ActionBar actionBar = (ActionBar) findViewById(R.id.actionbar);
		actionBar.setTitle("Select A Device...");
		actionBar.addAction(new refreshAction());

		mDevicesListView = (ListView) findViewById(R.id.device_list_view);
		mDevicesImageAdapter = new ImageAdapter(this);
		mDevicesListView.setAdapter(mDevicesImageAdapter);
		mDevicesListView.setOnItemClickListener(mDeviceClickListener);

		// Register for broadcasts when a device is discovered
		IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
		this.registerReceiver(m_Receiver, filter);

		// Register for broadcasts when discovery has finished
		filter = new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
		this.registerReceiver(m_Receiver, filter);

		// Get the local Bluetooth adapter
		mBtAdapter = BluetoothAdapter.getDefaultAdapter();
		if (mBtAdapter == null)
		{
			Toast.makeText(this, "Bluetooth hardware not detected.", Toast.LENGTH_LONG).show();
			finish();
			return;
		}

		// If BT is not on, request that it be enabled.
		if (!mBtAdapter.isEnabled())
		{
			Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
			startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
		}

		// Get a set of currently paired devices
		Set<BluetoothDevice> pairedDevices = mBtAdapter.getBondedDevices();

		// If there are paired devices, add each one to the ArrayAdapter
		if (pairedDevices.size() > 0)
		{
			for (BluetoothDevice device : pairedDevices)
			{
				mDevicesImageAdapter.add(device.getName(), device.getAddress());
			}
		} else
		{
			mDevicesImageAdapter.add("No paired devices...", "");
		}

		refreshDeviceList();
	}

	private class refreshAction extends AbstractAction
	{

		public refreshAction()
		{
			super(R.drawable.ic_action_refresh);

		}

		public void performAction(View view)
		{
			refreshDeviceList();
		}

	}

	public class ImageAdapter extends BaseAdapter
	{
		Context MyContext;
		private List<String> mDeviceNames;
		private List<String> mDeviceAddresses;

		public ImageAdapter(Context _MyContext)
		{
			MyContext = _MyContext;
			mDeviceNames = new ArrayList<String>();
			mDeviceAddresses = new ArrayList<String>();
		}

		public void add(String deviceName, String deviceAddress)
		{
			mDeviceNames.add(deviceName);
			mDeviceAddresses.add(deviceAddress);
			notifyDataSetChanged();
		}

		public int getCount()
		{
			/* Set the number of element we want on the grid */
			return mDeviceNames.size();
		}

		public String getAddress(long arg3)
		{
			// TODO Auto-generated method stub
			if (arg3 >= 0 && arg3 < mDeviceAddresses.size())
			{
				return mDeviceAddresses.get((int) arg3);
			}

			return null;
		}

		public void clear()
		{
			mDeviceNames.clear();
			mDeviceAddresses.clear();
			notifyDataSetChanged();
		}

		public View getView(int position, View convertView, ViewGroup parent)
		{
			String deviceName = mDeviceNames.get(position);
			String deviceAddress = mDeviceAddresses.get(position);
			View v = new CustomAdapterView(this.MyContext, deviceName, deviceAddress, position);
			v.setBackgroundColor(0xffffff);
			return v;
		}

		public Object getItem(int arg0)
		{
			// TODO Auto-generated method stub
			if (arg0 >= 0 && arg0 < mDeviceAddresses.size())
			{
				return mDeviceAddresses.get(arg0);
			}

			return null;
		}

		public long getItemId(int arg0)
		{
			return arg0;
		}

		class CustomAdapterView extends LinearLayout
		{
			public CustomAdapterView(Context context, String deviceName, String deviceAddress, int id)
			{
				super(context);
				setId(id);

				// container is a horizontal layer
				setOrientation(LinearLayout.HORIZONTAL);
				setPadding(0, 6, 0, 6);

				// image:params
				LinearLayout.LayoutParams Params = new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
				Params.setMargins(6, 0, 6, 0);
				// image:itself
				ImageView ivLogo = new ImageView(context);
				// load image
				ivLogo.setImageDrawable(context.getResources().getDrawable(mDrawableID));

				// image:add
				addView(ivLogo, Params);

				// vertical layer for text
				Params = new LinearLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.WRAP_CONTENT);
				LinearLayout PanelV = new LinearLayout(context);
				PanelV.setOrientation(LinearLayout.VERTICAL);
				PanelV.setGravity(Gravity.BOTTOM);
				PanelV.setBackgroundColor(0xffffff);

				TextView textName = new TextView(context);
				textName.setTextSize(20);
				textName.setTypeface(Typeface.DEFAULT, Typeface.BOLD);
				textName.setText(deviceName);
				textName.setTextColor(0xFF000000);
				PanelV.addView(textName);

				TextView textAddress = new TextView(context);
				textAddress.setTextSize(16);
				textAddress.setText(deviceAddress);
				textAddress.setTextColor(0xFF000000);
				PanelV.addView(textAddress);
				
				addView(PanelV, Params);
			}
		}
	}

	@Override
	protected void onDestroy()
	{
		super.onDestroy();

		// Make sure we're not doing discovery anymore
		if (mBtAdapter != null)
		{
			mBtAdapter.cancelDiscovery();
		}

		// Unregister broadcast listeners
		this.unregisterReceiver(m_Receiver);
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
		if (mDevicesImageAdapter.getCount() == 0)
			return false;

		// Handle item selection
		switch (item.getItemId())
		{
		case R.id.preferences:
			if (mPreferencesActivityPath != null)
			{
				Intent prefPanel = new Intent(mPreferencesActivityPath);
				startActivity(prefPanel);
			}
			return super.onOptionsItemSelected(item);
		case R.id.help:
			if (mHelpActivityPath != null)
			{
				Intent helpPanel = new Intent(mHelpActivityPath);
				startActivity(helpPanel);
			}
			return super.onOptionsItemSelected(item);
		case R.id.about:
			if (mAboutActivityPath != null)
			{
				Intent aboutPanel = new Intent(mAboutActivityPath);
				startActivity(aboutPanel);
			}
			return super.onOptionsItemSelected(item);
		default:
			return super.onOptionsItemSelected(item);
		}
	}

	@Override
	public void onBackPressed()
	{
		super.onBackPressed();
		finish();
	}

	// The BroadcastReceiver that listens for discovered devices and
	// changes the title when discovery is finished
	private final BroadcastReceiver m_Receiver = new BroadcastReceiver()
	{
		@Override
		public void onReceive(Context context, Intent intent)
		{
			String action = intent.getAction();

			// When discovery finds a device
			if (BluetoothDevice.ACTION_FOUND.equals(action))
			{
				// Get the BluetoothDevice object from the Intent
				BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
				// If it's already paired, skip it, because it's been listed
				// already
				if (device.getBondState() != BluetoothDevice.BOND_BONDED)
				{
					mDevicesImageAdapter.add(device.getName(), device.getAddress());
				}
				// When discovery is finished, change the Activity title
			} else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action))
			{
				setProgressBarIndeterminateVisibility(false);
				if (mDevicesImageAdapter.getCount() == 0)
				{
					mDevicesImageAdapter.add("No devices...", "");
				}
			}
		}
	};

	private OnItemClickListener mDeviceClickListener = new OnItemClickListener()
	{
		public void onItemClick(AdapterView<?> arg0, View view, int arg2, long arg3)
		{
			// Cancel discovery because it's costly and we're about to connect
			mBtAdapter.cancelDiscovery();

			// Get the device MAC address, which is the last 17 chars in the
			// View
			String address = ((ImageAdapter) mDevicesListView.getAdapter()).getAddress(arg3);
			try
			{
				// Create the result Intent and include the MAC address
				if (mSelectedActivityPath != null)
				{
					Intent startMainPanel = new Intent(mSelectedActivityPath);
					startMainPanel.putExtra(EXTRA_DEVICE_ADDRESS, address);
					startActivity(startMainPanel);
				}
				finish();
			} catch (IndexOutOfBoundsException e)
			{
				// Extraction failed, set result and finish this Activity
				setResult(Activity.RESULT_CANCELED);
				finish();
			}
		}
	};

	/**
	 * Start device discover with the BluetoothAdapter
	 * 
	 * @return
	 */
	private void refreshDeviceList()
	{
		setProgressBarIndeterminateVisibility(true);

		// If we're already discovering, stop it
		if (mBtAdapter.isDiscovering())
		{
			mBtAdapter.cancelDiscovery();
		}

		mDevicesImageAdapter.clear();
		// Get a set of currently paired devices
		Set<BluetoothDevice> pairedDevices = mBtAdapter.getBondedDevices();

		// If there are paired devices, add each one to the ArrayAdapter
		if (pairedDevices.size() > 0)
		{
			for (BluetoothDevice device : pairedDevices)
			{
				mDevicesImageAdapter.add(device.getName(), device.getAddress());
			}
		} else
		{
			mDevicesImageAdapter.add("No paired devices...", "");
		}

		// Request discovery from BluetoothAdapter
		mBtAdapter.startDiscovery();
	}

};