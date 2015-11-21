package studio.android.art.artdrone;

import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.graphics.Bitmap;
import android.speech.RecognizerIntent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.os.AsyncTask;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Objects;

import org.opencv.core.*;
import org.opencv.imgcodecs.*;
import org.opencv.android.*;
import org.opencv.imgproc.Imgproc;


public class MainActivity extends AppCompatActivity {

    static {
        // If you use opencv 2.4, System.loadLibrary("opencv_java")
        System.loadLibrary("opencv_java3");
    }

    private Socket socket = null;
    BufferedReader in = null;
    PrintWriter out = null;
    boolean clientRun = false;

    Button speakButton;
    TextView commandTextView;
    TextView receivedTextView;
    EditText ipAddressEditText;
    EditText portAddressEditText;
    EditText languageEditText;
    ImageView cameraDrone;

    String voiceCommand;
    Mat imgStream;
    String messageFromServer;

    TCPClient tcpClient;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        speakButton = (Button) findViewById(R.id.speakButton);
        commandTextView = (TextView) findViewById(R.id.commandTextView);
        receivedTextView = (TextView) findViewById(R.id.receivedTextView);
        ipAddressEditText = (EditText) findViewById(R.id.ipAddressEditText);
        portAddressEditText = (EditText) findViewById(R.id.portAddressEditText);
        languageEditText = (EditText) findViewById(R.id.languageEditText);
        cameraDrone = (ImageView) findViewById(R.id.cameraDrone);
        //cameraDrone.setImageResource(R.drawable.url);

        new ConnectTask().execute(messageFromServer);

        if (!OpenCVLoader.initDebug()) {
            receivedTextView.setText("OpenCV Gagal");
        }

        speakButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                Intent intent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);

                intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);

                String languageUsed = languageEditText.getText().toString();

                intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE, languageUsed);

                try {
//                    startActivityForResult(intent, 1);

                    commandTextView.setText("");
                    receivedTextView.setText("");
                    tcpClient.sendMessage("vf");

                } catch (ActivityNotFoundException a) {
                    Toast.makeText(getApplicationContext(), "Perangkat Tidak Mendukung", Toast.LENGTH_SHORT).show();
                }

            }
        });
    }

    @Override
    protected void onActivityResult (int requestCode, int resultCode, Intent data) {
//        super.onActivityResult(requestCode, resultCode, data);
//
//        switch (requestCode) {
//            case 1: {
//                if (resultCode == RESULT_OK && data != null) {
//
//                    ArrayList<String> text = data.getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS);
//
//                    voiceCommand = text.get(0);
//                    commandTextView.setText(voiceCommand);
//
//                    try {
//                        //new Thread(tcpClient().start());
//                        //socket.close();
//
//                    } catch (Exception e) {
//                        e.printStackTrace();
//                    }
//                }
//                break;
//            }
//
//        }
    }


    public class ConnectTask extends AsyncTask<String, byte[], TCPClient> {

        @Override
        protected TCPClient doInBackground(String... message) {

            tcpClient = new TCPClient(new TCPClient.OnMessageReceived() {

                @Override
                public void messageReceived(byte[] message) {
                    publishProgress(message);
                }
            }, ipAddressEditText.getText().toString(), Integer.parseInt(portAddressEditText.getText().toString()));
            tcpClient.run();
            return null;
        }

        @Override
        protected void onProgressUpdate(byte[]... values) {
            super.onProgressUpdate(values);

//            if (Objects.equals(values[0], "ARTStartImages")) {
//                messageFromServer = "";
//                receivedTextView.setText(values[0]);
//            }
//            else if (Objects.equals(values[0], "ARTEndImages")) {
           // messageFromServer = "";
                int width = 1280, height = 720;
            //messageFromServer = values[0];
                receivedTextView.setText(values[0].length + " ");
                //Mat img_buff = new Mat (width, height, CvType.CV_8U);
                //img_buff.put(0, 0, messageFromServer.getBytes());

                imgStream = Imgcodecs.imdecode(new MatOfByte(values[0]), Imgcodecs.CV_LOAD_IMAGE_COLOR);

                if (!imgStream.empty()) {
                    Bitmap bm = Bitmap.createBitmap(imgStream.cols(), imgStream.height(), Bitmap.Config.ARGB_8888);
                    Utils.matToBitmap(imgStream, bm);
                    cameraDrone.setImageBitmap(bm);
                    cameraDrone.invalidate();
                    //receivedTextView.setText("ok");
                }

//                messageFromServer = "";
//            }
//            else {
//
//                messageFromServer += values[0];
//                receivedTextView.setText(messageFromServer.length() + "");
//            }


        }
    }

}
