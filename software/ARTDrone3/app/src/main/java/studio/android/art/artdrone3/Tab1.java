package studio.android.art.artdrone3;

/**
 * Created by hp on 25/11/15.
 */
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.os.Bundle;
import android.speech.RecognizerIntent;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.os.AsyncTask;

import java.util.ArrayList;
import java.util.Objects;

/**
 * Created by hp1 on 21-01-2015.
 */
public class Tab1 extends Fragment {
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
    String voiceCommand;

    ConnectTask connectTCP = null;
    TCPClient tcpClient = null;
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view =inflater.inflate(R.layout.tab_1,container,false);
        speakButton = (Button) view.findViewById(R.id.speakButton);
        commandTextView = (TextView) view.findViewById(R.id.commandTextView);
        receivedTextView = (TextView) view.findViewById(R.id.receivedTextView);
        languageEditText = (EditText) view.findViewById(R.id.languageEditText);
        ipAddressEditText = (EditText) view.findViewById(R.id.ipAddressEditText);
        portAddressEditText = (EditText) view.findViewById(R.id.portAddressEditText);
        commandEdit = (EditText) view.findViewById(R.id.commandEdit);
        sendCommandButton = (Button) view.findViewById(R.id.sendCommandButton);
        connectButton =  (Button) view.findViewById(R.id.connectButton);
        stopButton =  (Button) view.findViewById(R.id.stopButton);

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
                    Toast.makeText(getActivity().getApplicationContext(), "Perangkat Tidak Mendukung", Toast.LENGTH_SHORT).show();
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

        return view;
    }

    @Override
    public void onActivityResult (int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        switch (requestCode) {
            case 1: {
                if (resultCode == getActivity().RESULT_OK && data != null) {

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
    }
}