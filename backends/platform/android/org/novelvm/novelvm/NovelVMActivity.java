package org.novelvm.novelvm;

import android.Manifest;
import android.content.pm.PackageManager;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.media.AudioManager;
import android.net.Uri;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiInfo;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.text.ClipboardManager;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.MotionEvent;
import android.view.inputmethod.InputMethodManager;
import android.widget.ImageView;
import android.widget.Toast;
import android.widget.Button;
import android.widget.HorizontalScrollView;
import android.widget.ImageView;

import java.io.File;
import java.util.List;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

public class NovelVMActivity extends Activity {

	/* Establish whether the hover events are available */
	private static boolean _hoverAvailable;

	private ClipboardManager _clipboard;

	/**
	 * Id to identify an external storage read request.
	 */
	private static final int MY_PERMISSIONS_REQUEST_READ_EXT_STORAGE = 100; // is an app-defined int constant. The callback method gets the result of the request.

	static {
		try {
			MouseHelper.checkHoverAvailable(); // this throws exception if we're on too old version
			_hoverAvailable = true;
		} catch (Throwable t) {
			_hoverAvailable = false;
		}
	}

	public View.OnClickListener keyboardBtnOnClickListener = new View.OnClickListener() {
		@Override
		public void onClick(View v) {
			runOnUiThread(new Runnable() {
					public void run() {
						toggleKeyboard();
					}
				});
		}
	};

	// NovelVM specific code start
	// The callbacks below implement the action buttons for Grim and EMI.
	// TODO: Replace by a more generic "touch controls" mechanism
	private boolean isBtnsShowing = false;

	public View.OnClickListener optionsBtnOnClickListener = new View.OnClickListener() {
		@Override
		public void onClick(View v) {

			if(!isBtnsShowing)
				((HorizontalScrollView)findViewById(R.id.btns_scrollview)).setVisibility(View.VISIBLE);
			else
				((HorizontalScrollView)findViewById(R.id.btns_scrollview)).setVisibility(View.GONE);

			isBtnsShowing = !isBtnsShowing;

		}
	};

	private void emulateKeyPress(int keyCode){
		_novelvm.pushEvent(NovelVMEvents.JE_KEY, KeyEvent.ACTION_DOWN, keyCode, 0, 0, 0, 0);
		_novelvm.pushEvent(NovelVMEvents.JE_KEY, KeyEvent.ACTION_UP, keyCode, 0, 0, 0, 0);
	}

	public View.OnClickListener menuBtnOnClickListener = new View.OnClickListener() {
		@Override
		public void onClick(View v) {
			emulateKeyPress(KeyEvent.KEYCODE_F1);
		}
	};

	public View.OnClickListener inventoryBtnOnClickListener = new View.OnClickListener() {
		@Override
		public void onClick(View v) {
			emulateKeyPress(KeyEvent.KEYCODE_I);
		}
	};

	public View.OnClickListener lookAtBtnOnClickListener = new View.OnClickListener() {
		@Override
		public void onClick(View v) {
			emulateKeyPress(KeyEvent.KEYCODE_E);
		}
	};

	public View.OnClickListener useBtnOnClickListener = new View.OnClickListener() {
		@Override
		public void onClick(View v) {
			emulateKeyPress(KeyEvent.KEYCODE_ENTER);
		}
	};

	public View.OnClickListener pickUpBtnOnClickListener = new View.OnClickListener() {
		@Override
		public void onClick(View v) {
			emulateKeyPress(KeyEvent.KEYCODE_P);
		}
	};
	// NovelVM specific code end


	private class MyNovelVM extends NovelVM {

		public MyNovelVM(SurfaceHolder holder) {
			super(NovelVMActivity.this.getAssets(), holder);
		}

		@Override
		protected void getDPI(float[] values) {
			DisplayMetrics metrics = new DisplayMetrics();
			getWindowManager().getDefaultDisplay().getMetrics(metrics);

			values[0] = metrics.xdpi;
			values[1] = metrics.ydpi;
		}

		@Override
		protected void displayMessageOnOSD(final String msg) {
			if (msg != null) {
				Log.i(LOG_TAG, "MessageOnOSD: " + msg + " " + getCurrentCharset());
				runOnUiThread(new Runnable() {
					public void run() {
						Toast.makeText(NovelVMActivity.this, msg, Toast.LENGTH_SHORT).show();
					}
				});
			}
		}

		@Override
		protected void openUrl(String url) {
			startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse(url)));
		}

		@Override
		protected boolean hasTextInClipboard() {
			return _clipboard.hasText();
		}

		@Override
		protected byte[] getTextFromClipboard() {
			CharSequence text = _clipboard.getText();
			if (text != null) {
				String encoding = getCurrentCharset();
				byte[] out;
				Log.d(LOG_TAG, String.format("Converting from UTF-8 to %s", encoding));
				try {
					out = text.toString().getBytes(encoding);
				} catch (java.io.UnsupportedEncodingException e) {
					out = text.toString().getBytes();
				}
				return out;
			}
			return null;
		}

		@Override
		protected boolean setTextInClipboard(byte[] text) {
			String encoding = getCurrentCharset();
			String out;
			Log.d(LOG_TAG, String.format("Converting from %s to UTF-8", encoding));
			try {
				out = new String(text, encoding);
			} catch (java.io.UnsupportedEncodingException e) {
				out = new String(text);
			}
			_clipboard.setText(out);
			return true;
		}

		@Override
		protected boolean isConnectionLimited() {
			WifiManager wifiMgr = (WifiManager)getSystemService(Context.WIFI_SERVICE);
			if (wifiMgr != null && wifiMgr.isWifiEnabled()) {
				WifiInfo wifiInfo = wifiMgr.getConnectionInfo();
				return (wifiInfo == null || wifiInfo.getNetworkId() == -1); //WiFi is on, but it's not connected to any network
			}
			return true;
		}

		@Override
		protected void setWindowCaption(final String caption) {
			runOnUiThread(new Runnable() {
					public void run() {
						setTitle(caption);
					}
				});
		}

		@Override
		protected void showVirtualKeyboard(final boolean enable) {
			runOnUiThread(new Runnable() {
					public void run() {
						showKeyboard(enable);
					}
				});
		}

		@Override
		protected void showKeyboardControl(final boolean enable) {
			runOnUiThread(new Runnable() {
					public void run() {
						showKeyboardView(enable);
					}
				});
		}

		@Override
		protected String[] getSysArchives() {
			return new String[0];
		}

		@Override
		protected String[] getAllStorageLocations() {
			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M
			    && checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED
			) {
				requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, MY_PERMISSIONS_REQUEST_READ_EXT_STORAGE);
			} else {
				return _externalStorage.getAllStorageLocations().toArray(new String[0]);
			}
			return new String[0]; // an array of zero length
		}

	}

	private MyNovelVM _novelvm;
	private NovelVMEvents _events;
	private MouseHelper _mouseHelper;
	private Thread _novelvm_thread;
	private ExternalStorage _externalStorage;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setVolumeControlStream(AudioManager.STREAM_MUSIC);

		setContentView(R.layout.main);
		takeKeyEvents(true);

		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M
			&& checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED
		) {
			requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, MY_PERMISSIONS_REQUEST_READ_EXT_STORAGE);
		}

		// This is a common enough error that we should warn about it
		// explicitly.
		if (!Environment.getExternalStorageDirectory().canRead()) {
			new AlertDialog.Builder(this)
				.setTitle(R.string.no_sdcard_title)
				.setIcon(android.R.drawable.ic_dialog_alert)
				.setMessage(R.string.no_sdcard)
				.setNegativeButton(R.string.quit,
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog,
												int which) {
								finish();
							}
						})
			.show();

			return;
		}

		SurfaceView main_surface = (SurfaceView)findViewById(R.id.main_surface);

		main_surface.requestFocus();

		getFilesDir().mkdirs();

		// Store savegames on external storage if we can, which means they're
		// world-readable and don't get deleted on uninstall.
		String savePath = Environment.getExternalStorageDirectory() + "/NovelVM/Saves/";
		File saveDir = new File(savePath);
		saveDir.mkdirs();
		if (!saveDir.isDirectory()) {
			// If it doesn't work, resort to the internal app path.
			savePath = getDir("saves", Context.MODE_PRIVATE).getPath();
		}

		_clipboard = (ClipboardManager)getSystemService(CLIPBOARD_SERVICE);

		// Start NovelVM
		_novelvm = new MyNovelVM(main_surface.getHolder());

		_novelvm.setArgs(new String[] {
				"NovelVM",
				"--config=" + getFileStreamPath("novelvmrc").getPath(),
				"--path=" + Environment.getExternalStorageDirectory().getPath(),
				"--gui-theme=modern",
				"--savepath=" + savePath
		});

		Log.d(NovelVM.LOG_TAG, "Hover available: " + _hoverAvailable);
		if (_hoverAvailable) {
			_mouseHelper = new MouseHelper(_novelvm);
			_mouseHelper.attach(main_surface);
		}

		_events = new NovelVMEvents(this, _novelvm, _mouseHelper);

		// On screen button listener
		((ImageView)findViewById(R.id.show_keyboard)).setOnClickListener(keyboardBtnOnClickListener);
		// NovelVM specific code start
		((ImageView)findViewById(R.id.options)).setOnClickListener(optionsBtnOnClickListener);
		((Button)findViewById(R.id.menu_btn)).setOnClickListener(menuBtnOnClickListener);
		((Button)findViewById(R.id.inventory_btn)).setOnClickListener(inventoryBtnOnClickListener);
		((Button)findViewById(R.id.use_btn)).setOnClickListener(useBtnOnClickListener);
		((Button)findViewById(R.id.pick_up_btn)).setOnClickListener(pickUpBtnOnClickListener);
		((Button)findViewById(R.id.look_at_btn)).setOnClickListener(lookAtBtnOnClickListener);
		// NovelVM specific code end

		main_surface.setOnKeyListener(_events);
		main_surface.setOnTouchListener(_events);

		_novelvm_thread = new Thread(_novelvm, "NovelVM");
		_novelvm_thread.start();
	}

	@Override
	public void onStart() {
		Log.d(NovelVM.LOG_TAG, "onStart");

		super.onStart();
	}

	@Override
	public void onResume() {
		Log.d(NovelVM.LOG_TAG, "onResume");

		super.onResume();

		if (_novelvm != null)
			_novelvm.setPause(false);
		showMouseCursor(false);
	}

	@Override
	public void onPause() {
		Log.d(NovelVM.LOG_TAG, "onPause");

		super.onPause();

		if (_novelvm != null)
			_novelvm.setPause(true);
		showMouseCursor(true);
	}

	@Override
	public void onStop() {
		Log.d(NovelVM.LOG_TAG, "onStop");

		super.onStop();
	}

	@Override
	public void onDestroy() {
		Log.d(NovelVM.LOG_TAG, "onDestroy");

		super.onDestroy();

		if (_events != null) {
			_events.sendQuitEvent();

			try {
				// 1s timeout
				_novelvm_thread.join(1000);
			} catch (InterruptedException e) {
				Log.i(NovelVM.LOG_TAG, "Error while joining NovelVM thread", e);
			}

			_novelvm = null;
		}
	}


	@Override
	public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
		switch (requestCode) {
		case MY_PERMISSIONS_REQUEST_READ_EXT_STORAGE:
			// If request is cancelled, the result arrays are empty.
			if (grantResults.length > 0
			   && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
				// permission was granted
				Log.i(NovelVM.LOG_TAG, "Read External Storage permission was granted at Runtime");
			} else {
				// permission denied! We won't be able to make use of functionality depending on this permission.
				Toast.makeText(this, "Until permission is granted, some storage locations may be inaccessible!", Toast.LENGTH_SHORT)
				              .show();
			}
			break;
		default:
			break;
		}
	}


	@Override
	public boolean onTrackballEvent(MotionEvent e) {
		if (_events != null)
			return _events.onTrackballEvent(e);

		return false;
	}

	@Override
	public boolean onGenericMotionEvent(final MotionEvent e) {
		if (_events != null)
			return _events.onGenericMotionEvent(e);

		return false;
	}

	private void showKeyboard(boolean show) {
		SurfaceView main_surface = (SurfaceView)findViewById(R.id.main_surface);
		InputMethodManager imm = (InputMethodManager)
			getSystemService(INPUT_METHOD_SERVICE);

		if (show)
			imm.showSoftInput(main_surface, InputMethodManager.SHOW_IMPLICIT);
		else
			imm.hideSoftInputFromWindow(main_surface.getWindowToken(),
										InputMethodManager.HIDE_IMPLICIT_ONLY);
	}

	private void toggleKeyboard() {
		SurfaceView main_surface = (SurfaceView)findViewById(R.id.main_surface);
		InputMethodManager imm = (InputMethodManager)
			getSystemService(INPUT_METHOD_SERVICE);

		imm.toggleSoftInputFromWindow(main_surface.getWindowToken(),
		                              InputMethodManager.SHOW_IMPLICIT,
		                              InputMethodManager.HIDE_IMPLICIT_ONLY);
	}

	private void showKeyboardView(boolean show) {
		ImageView keyboardBtn = (ImageView)findViewById(R.id.show_keyboard);

		if (show)
			keyboardBtn.setVisibility(View.VISIBLE);
		else
			keyboardBtn.setVisibility(View.GONE);
	}

	private void showMouseCursor(boolean show) {
		/* Currently hiding the system mouse cursor is only
		   supported on OUYA.  If other systems provide similar
		   intents, please add them here as well */
		Intent intent =
			new Intent(show?
				   "tv.ouya.controller.action.SHOW_CURSOR" :
				   "tv.ouya.controller.action.HIDE_CURSOR");
		sendBroadcast(intent);
	}
}
