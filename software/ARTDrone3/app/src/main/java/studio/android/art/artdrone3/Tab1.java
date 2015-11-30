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
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.PopupWindow;

import java.util.ArrayList;

/**
 * Created by hp1 on 21-01-2015.
 */
public class Tab1 extends Fragment {
    ImageButton speakButton;
    TextView commandTextView;
    public static TextView receivedTextView;
    public static EditText ipAddressEditText;
    public static EditText portAddressEditText;
    EditText languageEditText;
    EditText commandEdit;
    ImageButton sendCommandButton;
    Button connectButton;
    Button stopButton;
    String voiceCommand;
    ImageButton FAB;
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view =inflater.inflate(R.layout.tab_1,container,false);
        speakButton = (ImageButton) view.findViewById(R.id.speakButton);
        commandTextView = (TextView) view.findViewById(R.id.commandTextView);
        receivedTextView = (TextView) view.findViewById(R.id.receivedTextView);
        languageEditText = (EditText) view.findViewById(R.id.languageEditText);
        ipAddressEditText = (EditText) view.findViewById(R.id.ipAddressEditText);
        portAddressEditText = (EditText) view.findViewById(R.id.portAddressEditText);
        commandEdit = (EditText) view.findViewById(R.id.commandEdit);
        sendCommandButton = (ImageButton) view.findViewById(R.id.sendCommandButton);
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
                if(MainActivity.tcpClient != null) {
                    MainActivity.tcpClient.sendMessage(commandEditString);
                }
                else {
                    receivedTextView.setText("Not connected");
                }
            }
        });

        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(MainActivity.tcpClient != null) {
                    MainActivity.tcpClient.stopClient();
                    MainActivity.tcpClient = null;
                    MainActivity.connectTCP = null;
                }
                if (MainActivity.connectTCP == null) {
                    MainActivity.connectTCP = new MainActivity.ConnectTask();
                    MainActivity.connectTCP.execute("");
                }
            }
        });

        stopButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (MainActivity.tcpClient != null) {
                    MainActivity.tcpClient.stopClient();
                    MainActivity.tcpClient = null;
                    MainActivity.connectTCP = null;
                }
            }
        });
        final ImageButton btnOpenPopup = (ImageButton) view.findViewById(R.id.imageButton);
        btnOpenPopup.setOnClickListener(new Button.OnClickListener() {

            @Override
            public void onClick(View arg0) {
                LayoutInflater layoutInflater = (LayoutInflater) getActivity().getBaseContext()
                        .getSystemService(getContext().LAYOUT_INFLATER_SERVICE);
                View popupView = layoutInflater.inflate(R.layout.activity_second,
                        null);
                final PopupWindow popupWindow = new PopupWindow(popupView,
                        ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);

                Button btnDismiss = (Button) popupView
                        .findViewById(R.id.dismiss);
                btnDismiss.setOnClickListener(new Button.OnClickListener() {

                    @Override
                    public void onClick(View v) {
                        // TODO Auto-generated method stub
                        popupWindow.dismiss();
                    }
                });

                popupWindow.showAtLocation(btnOpenPopup, Gravity.CENTER, 0, 0);

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

                    if(MainActivity.tcpClient != null) {
                        MainActivity.tcpClient.sendMessage("sc" + voiceCommand);
                    }
                    else {
                        receivedTextView.setText("Not connected");
                    }

                }
                break;
            }
        }
    }

}