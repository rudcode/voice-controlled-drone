package studio.android.art.artdrone3;

/**
 * Created by hp on 25/11/15.
 */
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Bundle;
import android.speech.RecognizerIntent;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Objects;

/**
 * Created by hp1 on 21-01-2015.
 */
public class Tab2 extends Fragment {
    ImageView cameraDrone;
    Bitmap bitmapCameraDrone;
    TextView commandTextView;
    TextView receivedTextView;
    EditText ipAddressEditText;
    EditText portAddressEditText;
    String voiceCommand;

    ConnectTask connectTCP = null;
    TCPClient tcpClient = null;
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.tab_2,container,false);
        commandTextView = (TextView) v.findViewById(R.id.commandTextView);
        receivedTextView = (TextView) v.findViewById(R.id.receivedTextView);
        ipAddressEditText = (EditText) v.findViewById(R.id.ipAddressEditText);
        portAddressEditText = (EditText) v.findViewById(R.id.portAddressEditText);
        return v;
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

        @Override
        protected void onProgressUpdate(byte[]... values) {
            super.onProgressUpdate(values);
            //receivedTextView.setText(values[0].length + "");
            if (Objects.equals(new String(values[1]), "I")) {
                bitmapCameraDrone = BitmapFactory.decodeByteArray(values[0], 0, values[0].length);
                if (bitmapCameraDrone != null) {
                    cameraDrone.setImageBitmap(bitmapCameraDrone);
                    bitmapCameraDrone = null;
                }
            }
            else if (Objects.equals(new String(values[1]), "S") && values[0].length >= 2) {
                if ((values[0][0] == (byte)'s' && values[0][1] == (byte)'c') || (values[0][0] == (byte)'d' && values[0][1] == (byte)'s')) {
                    receivedTextView.setText(new String(values[0]));
                }
            }
        }
    }
}