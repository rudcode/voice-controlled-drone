package studio.android.art.artdrone3;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.widget.Toolbar;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;

import java.util.Objects;


public class MainActivity extends AppCompatActivity {

    // Declaring Your View and Variables
    Toolbar toolbar;
    public static ViewPager pager;
    ViewPagerAdapter adapter;
    SlidingTabLayout tabs;
    CharSequence Titles[]={"Speech","Video","Status"};
    int Numboftabs = 3;

    public static ConnectTask connectTCP = null;
    public static TCPClient tcpClient = null;
    static Bitmap bitmapCameraDrone;

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
                if ((values[0][0] == (byte)'s' && values[0][1] == (byte)'c') || (values[0][0] == (byte)'d' && values[0][1] == (byte)'s')) {
                    Tab1.receivedTextView.setText(new String(values[0]));
                }
            }
        }
    }
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}