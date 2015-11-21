package studio.android.art.artdrone;

import android.util.Log;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.util.Objects;

/**
 * Description
 *
 * @author Catalin Prata
 *         Date: 2/12/13
 */
public class TCPClient {

    public static String SERVER_IP; //your computer IP address
    public static int SERVER_PORT;
    // message to send to the server
    private String mServerMessage;
    // sends message received notifications
    private OnMessageReceived mMessageListener = null;
    // while this is true, the server will continue running
    private boolean mRun = false;
    // used to send messages
    private PrintWriter mBufferOut;
    // used to read messages from the server
    private BufferedReader mBufferIn;

    /**
     * Constructor of the class. OnMessagedReceived listens for the messages received from server
     */
    public TCPClient(OnMessageReceived listener, String ipAddress, int portAddress) {
        mMessageListener = listener;
        SERVER_IP = ipAddress;
        SERVER_PORT = portAddress;
    }

    /**
     * Sends the message entered by client to the server
     *
     * @param message text entered by client
     */
    public void sendMessage(String message) {
        if (mBufferOut != null && !mBufferOut.checkError()) {
            mBufferOut.println(message);
            mBufferOut.flush();
        }
    }

    /**
     * Close the connection and release the members
     */
    public void stopClient() {
        Log.i("Debug", "stopClient");

        // send mesage that we are closing the connection
        //sendMessage(Constants.CLOSED_CONNECTION + "Kazy");

        mRun = false;

        if (mBufferOut != null) {
            mBufferOut.flush();
            mBufferOut.close();
        }

        mMessageListener = null;
        mBufferIn = null;
        mBufferOut = null;
        mServerMessage = null;
    }

    public void run() {

        mRun = true;

        try {
            //here you must put your computer's IP address.
            InetAddress serverAddr = InetAddress.getByName(SERVER_IP);

            Log.e("TCP Client", "C: Connecting...");

            //create a socket to make the connection with the server
            Socket socket = new Socket(serverAddr, SERVER_PORT);

            try {
                Log.i("Debug", "inside try catch");
                //sends the message to the server
                mBufferOut = new PrintWriter(new BufferedWriter(new OutputStreamWriter(socket.getOutputStream())), true);

                //receives the message which the server sends back
                InputStream mInputStream = socket.getInputStream();
                DataInputStream mDataInputStream = new DataInputStream(mInputStream);
                mBufferIn = new BufferedReader(new InputStreamReader(mInputStream));

                // send login name
                //sendMessage(Constants.LOGIN_NAME + PreferencesManager.getInstance().getUserName());
                //sendMessage("Hi");
                //in this while the client listens for the messages sent by the server
                while (mRun) {
                    mServerMessage = mBufferIn.readLine();
                    Log.e("Debug1", mServerMessage);
                    if (Objects.equals(mServerMessage, "*ARTS")) {
                        mServerMessage = "";
                        mServerMessage = mBufferIn.readLine();
                        int len = Integer.parseInt(mServerMessage);

                        if (len > 0) {
                            byte[] data = new byte[len];
//                            mDataInputStream.readFully(data, 0, len);
//                            char[] buffer = new char[len];
                            int n = 0;
                            while (len > 0) {
                                Log.e("Debug2", len + " " + n);
                                n = mDataInputStream.read(data, n, len);
                                len = len - n;
                            }
                            while (mMessageListener == null) ;
                            if (mMessageListener != null) {
                                //call the method messageReceived from MyActivity class
                                mMessageListener.messageReceived(data);
                            }
                        }
                    }
                            //mMessageListener.messageReceived(data);

//                        int n = mBufferIn.read(buffer);

//                        mServerMessage = "";
//                        mServerMessage = buffer.toString();
//                        Log.e("Debug", data.toString());

//                    if (Objects.equals(mServerMessage, "*ARTS")) {
//                        mServerMessage = "";
//                        boolean artImages = true;
//                        while(artImages) {
//                            value = mBufferIn.read();
//                            if ((char) value == 'A') {
//                                value = mBufferIn.read();
//                                if ((char) value == 'R') {
//                                    value = mBufferIn.read();
//                                    if ((char) value == 'T') {
//                                        value = mBufferIn.read();
//                                        if ((char) value == 'E') {
//                                            value = mBufferIn.read();
//                                            if ((char) value == '#') {
//                                                mServerMessage = buffer.toString();
//                                                artImages = false;
//                                            } else {
//                                                buffer += 'A' + 'R' + 'T' + 'E' + (char) value;
//                                            }
//                                        } else {
//                                            buffer += 'A' + 'R' + 'T' + (char) value;
//                                        }
//                                    } else {
//                                        buffer += 'A' + 'R' + (char) value;
//                                    }
//                                } else {
//                                    buffer += 'A' + (char) value;
//                                }
//                            } else buffer += (char) value;
//                        }
//                    }

//                    if (mServerMessage != null && mMessageListener != null) {
//                        //call the method messageReceived from MyActivity class
//                        mMessageListener.messageReceived(mServerMessage);
//                    }

                }
                Log.e("RESPONSE FROM SERVER", "S: Received Message: '" + mServerMessage + "'");

            } catch (Exception e) {

                Log.e("TCP", "S: Error", e);

            } finally {
                //the socket must be closed. It is not possible to reconnect to this socket
                // after it is closed, which means a new socket instance has to be created.
                socket.close();
            }

        } catch (Exception e) {

            Log.e("TCP", "C: Error", e);

        }

    }

    //Declare the interface. The method messageReceived(String message) will must be implemented in the MyActivity
    //class at on asynckTask doInBackground
    public interface OnMessageReceived {
        public void messageReceived(byte[] message);
    }
}