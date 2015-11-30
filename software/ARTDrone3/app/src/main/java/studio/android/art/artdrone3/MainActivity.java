package studio.android.art.artdrone3;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.widget.Toolbar;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import java.util.Objects;


public class MainActivity extends AppCompatActivity {

    // Declaring Your View and Variables
    Toolbar toolbar;
    public static ViewPager pager;
    ViewPagerAdapter adapter;
    SlidingTabLayout tabs;
    CharSequence Titles[] = {"Speech", "Video", "Status"};
    int Numboftabs = 3;

    public static ConnectTask connectTCP = null;
    public static TCPClient tcpClient = null;
    public static ARTDroneStatus droneStatus = null;
    static Bitmap bitmapCameraDrone;
    Handler handler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Creating The Toolbar and setting it as the Toolbar for the activity
        toolbar = (Toolbar) findViewById(R.id.tool_bar);
        setSupportActionBar(toolbar);

        // Creating The ViewPagerAdapter and Passing Fragment Manager, Titles fot the Tabs and Number Of Tabs.
        adapter = new ViewPagerAdapter(getSupportFragmentManager(),Titles,Numboftabs);

        // Assigning ViewPager View and setting the adapter
        pager = (ViewPager) findViewById(R.id.pager);
        pager.setAdapter(adapter);

        // Assiging the Sliding Tab Layout View
        tabs = (SlidingTabLayout) findViewById(R.id.tabs);
        tabs.setDistributeEvenly(true); // To make the Tabs Fixed set this true, This makes the tabs Space Evenly in Available width

        // Setting Custom Color for the Scroll bar indicator of the Tab View
        tabs.setCustomTabColorizer(new SlidingTabLayout.TabColorizer() {
            @Override
            public int getIndicatorColor(int position) {
                return getResources().getColor(R.color.colorAccent);
            }
        });

        // Setting the ViewPager For the SlidingTabsLayout
        tabs.setViewPager(pager);

        droneStatus = new ARTDroneStatus();

        handler = new Handler();
        handler.post(new Runnable() {
            @Override
            public void run() {
                if (pager.getCurrentItem() == 1) {
                    if (tcpClient != null) {
                        tcpClient.sendMessage("vf");
                    }
                    handler.postDelayed(this, 35);
                }
                else if (pager.getCurrentItem() == 2) {
                    if (tcpClient != null) {
                        tcpClient.sendMessage("ds");
                    }
                    handler.postDelayed(this, 250);
                }
                else {
                    handler.postDelayed(this, 500); // set time here to refresh textView
                }
            }
        });
    }


    public static class ARTDroneStatus {
        public static boolean armMode;
        public static String flightMode;
        public static float altitude;
        public static float compass;
        public static float velocityX;
        public static float velocityY;
        public static float velocityZ;
        public static float airTemperature;
        public static float airPressure;
        public static float battery;

        public ARTDroneStatus() {
            armMode = false;
            flightMode = "Land";
            altitude = 0;
            compass = 0;
            velocityX = 0;
            velocityY = 0;
            velocityZ = 0;
            airTemperature = 0;
            airPressure = 0;
            battery = 0;
        }
    }

    public static class ConnectTask extends AsyncTask<String, byte[], TCPClient> {
        @Override
        protected TCPClient doInBackground(String... message) {
            tcpClient = new TCPClient(new TCPClient.OnMessageReceived() {

                @Override
                public void messageReceived(byte[] message, byte[] isImage) {
                    publishProgress(message, isImage);
                }
            }, Tab1.ipAddressEditText.getText().toString(), Integer.parseInt(Tab1.portAddressEditText.getText().toString()));
            tcpClient.run();
            return null;
        }
        @Override
        protected void onProgressUpdate(byte[]... values) {
            super.onProgressUpdate(values);
            //receivedTextView.setText(values[0].length + "");
            if (Objects.equals(new String(values[1]), "I")) {
                bitmapCameraDrone = BitmapFactory.decodeByteArray(values[0], 0, values[0].length);
                if (bitmapCameraDrone != null) {
                    Tab2.cameraDrone.setImageBitmap(bitmapCameraDrone);
                    bitmapCameraDrone = null;
                }
            }
            if (Objects.equals(new String(values[1]), "S") && values[0].length >= 2) {
                if ((values[0][0] == (byte)'s' && values[0][1] == (byte)'c' && values[0][2] == (byte)':')) {
                    Tab1.receivedTextView.setText(new String(values[0], 3, values[0].length - 3));
                }
                else if (values[0][0] == (byte)'d' && values[0][1] == (byte)'s' && values[0][2] == (byte)':') {
                    Log.e("ds", "ds");
                    int j = 3;
                    int k = 0;

                    for (int i = 3; i < values[0].length; i++) {
                        if (values[0][i] == (byte)';') {
                            Log.e("ds", ";");
                            if (k == 0) {
                                droneStatus.armMode = (values[0][j] != (byte)'0');
                                if (droneStatus.armMode) {
                                    Tab3.armMode.setText("Arm");
                                }
                                else {
                                    Tab3.armMode.setText("Disarm");
                                }
                            }
                            else if (k == 1) {
                                droneStatus.flightMode = new String(values[0], j, i - j);
                                Tab3.flightMode.setText(droneStatus.flightMode);
                            }
                            else if (k == 2) {
                                droneStatus.altitude = Float.parseFloat(new String(values[0], j, i - j));
                                Tab3.altitude.setText(droneStatus.altitude + "");
                            }
                            else if (k == 3) {
                                droneStatus.compass = Float.parseFloat(new String(values[0], j, i - j));
                                Tab3.compass.setText(droneStatus.compass + "");
                            }
                            else if (k == 4) {
                                droneStatus.velocityX = Float.parseFloat(new String(values[0], j, i - j));
                                Tab3.velocityX.setText(droneStatus.velocityX + "");
                            }
                            else if (k == 5) {
                                droneStatus.velocityY = Float.parseFloat(new String(values[0], j, i - j));
                                Tab3.velocityY.setText(droneStatus.velocityY + "");
                            }
                            else if (k == 6) {
                                droneStatus.velocityZ = Float.parseFloat(new String(values[0], j, i - j));
                                Tab3.velocityZ.setText(droneStatus.velocityZ + "");
                            }
                            else if (k == 7) {
                                droneStatus.airTemperature = Float.parseFloat(new String(values[0], j, i - j));
                                Tab3.airTemperature.setText(droneStatus.airTemperature + "");
                            }
                            else if (k == 8) {
                                droneStatus.airPressure = Float.parseFloat(new String(values[0], j, i - j));
                                Tab3.airPressure.setText(droneStatus.airPressure + "");
                            }
                            else if (k == 9) {
                                droneStatus.battery = Float.parseFloat(new String(values[0], j, i - j));
                                Tab3.battery.setText(droneStatus.battery + "");
                                break;
                            }
                            k++;
                            j = i + 1;
                        }
                    }
                }
            }
        }
    }

}