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

import java.util.Objects;

public class MainActivity extends AppCompatActivity {
    Button speakButton;
    TextView commandTextView;
    TextView receivedTextView;
    EditText ipAddressEditText;
    EditText portAddressEditText;
    EditText languageEditText;
    EditText commandEdit;

    ImageView cameraDrone;

    String voiceCommand;

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
        commandEdit = (EditText) findViewById(R.id.commandEdit);

        cameraDrone = (ImageView) findViewById(R.id.cameraDrone);

        new ConnectTask().execute("");

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
                    tcpClient.sendMessage(commandEdit.getText().toString());

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
            Bitmap bitmap = BitmapFactory.decodeByteArray(values[0], 0, values[0].length);
            if (bitmap != null) {
                cameraDrone.setImageBitmap(bitmap);
            }
            else if (Objects.equals(values[0].toString(), "ERROR")) {
                receivedTextView.setText("ERROR");
            }
        }
    }

}
