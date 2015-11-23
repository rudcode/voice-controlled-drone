package studio.android.art.artdrone;

import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
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

import java.util.ArrayList;
import java.util.Objects;

public class MainActivity extends AppCompatActivity {
    Button speakButton;
    TextView commandTextView;
    TextView receivedTextView;
    EditText ipAddressEditText;
    EditText portAddressEditText;
    EditText languageEditText;
    EditText commandEdit;
    Button sendCommandButton;
    Button connectButton;
    Button stopButton;

    ImageView cameraDrone;
    Bitmap bitmapCameraDrone;

    String voiceCommand;

    ConnectTask connectTCP = null;
    TCPClient tcpClient = null;

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
        commandEdit = (EditText) findViewById(R.id.commandEdit);
        sendCommandButton = (Button) findViewById(R.id.sendCommandButton);
        connectButton =  (Button) findViewById(R.id.connectButton);
        stopButton =  (Button) findViewById(R.id.stopButton);

        cameraDrone = (ImageView) findViewById(R.id.cameraDrone);

        speakButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);

                intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);

                String languageUsed = languageEditText.getText().toString();

                intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE, languageUsed);

                try {
                    startActivityForResult(intent, 1);

                    commandTextView.setText("");
                    receivedTextView.setText("");

                } catch (ActivityNotFoundException a) {
                    Toast.makeText(getApplicationContext(), "Perangkat Tidak Mendukung", Toast.LENGTH_SHORT).show();
                }

            }
        });

        sendCommandButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String commandEditString = commandEdit.getText().toString();
                commandTextView.setText(commandEditString);
                receivedTextView.setText("");
                if(tcpClient != null) {
                    tcpClient.sendMessage(commandEditString);
                }
                else {
                    receivedTextView.setText("Not connected");
                }
            }
        });

        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(tcpClient != null) {
                    tcpClient.stopClient();
                    tcpClient = null;
                    connectTCP = null;
                }
                if (connectTCP == null) {
                    connectTCP = new ConnectTask();
                    connectTCP.execute("");
                }
            }
        });

        stopButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(tcpClient != null) {
                    tcpClient.stopClient();
                    tcpClient = null;
                    connectTCP = null;
                }
            }
        });
    }

    @Override
    protected void onActivityResult (int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        switch (requestCode) {
            case 1: {
                if (resultCode == RESULT_OK && data != null) {

                    ArrayList<String> text = data.getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS);

                    voiceCommand = text.get(0);
                    commandTextView.setText(voiceCommand);

                    if(tcpClient != null) {
                        tcpClient.sendMessage("sc" + voiceCommand);
                    }
                    else {
                        receivedTextView.setText("Not connected");
                    }

                }
                break;
            }
        }
    }


    public class ConnectTask extends AsyncTask<String, byte[], TCPClient> {
        @Override
        protected TCPClient doInBackground(String... message) {

            tcpClient = new TCPClient(new TCPClient.OnMessageReceived() {

                @Override
                public void messageReceived(byte[] message, byte[] isImage) {
                    publishProgress(message, isImage);
                }
            }, ipAddressEditText.getText().toString(), Integer.parseInt(portAddressEditText.getText().toString()));
            tcpClient.run();
            return null;
        }

        @Override
        protected void onProgressUpdate(byte[]... values) {
            super.onProgressUpdate(values);
            //receivedTextView.setText(values[0].length + "");
            bitmapCameraDrone = null;
            if (Objects.equals(new String(values[1]), "I")) {
                bitmapCameraDrone = BitmapFactory.decodeByteArray(values[0], 0, values[0].length);
            }
            if (bitmapCameraDrone != null) {
                cameraDrone.setImageBitmap(bitmapCameraDrone);
            }
            else if (Objects.equals(new String(values[0]), "ERROR")) {
                receivedTextView.setText("ERROR");
            }
        }
    }

}
