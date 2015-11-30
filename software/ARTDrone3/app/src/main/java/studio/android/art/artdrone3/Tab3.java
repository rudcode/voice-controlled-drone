package studio.android.art.artdrone3;

/**
 * Created by hp on 25/11/15.
 */
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

/**
 * Created by hp1 on 21-01-2015.
 */
public class Tab3 extends Fragment {
    public static ARTDroneStatus droneStatus;

    public static TextView armMode;
    public static TextView flightMode;
    public static TextView altitude;
    public static TextView compass;
    public static TextView velocityX;
    public static TextView velocityY;
    public static TextView velocityZ;
    public static TextView airTemperature;
    public static TextView airPressure;
    public static TextView battery;

    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.tab_3,container,false);

        armMode = (TextView) v.findViewById(R.id.armMode);
        flightMode = (TextView) v.findViewById(R.id.flightMode);
        altitude = (TextView) v.findViewById(R.id.altitude);
        compass = (TextView) v.findViewById(R.id.compass);
        velocityX = (TextView) v.findViewById(R.id.velocityX);
        velocityY = (TextView) v.findViewById(R.id.velocityY);
        velocityZ = (TextView) v.findViewById(R.id.velocityZ);
        airTemperature = (TextView) v.findViewById(R.id.airTemperature);
        airPressure = (TextView) v.findViewById(R.id.airPressure);
        battery = (TextView) v.findViewById(R.id.battery);

        return v;
    }

    public class ARTDroneStatus {
        public boolean armMode = false;
        public String flightMode = "Land";
        public float altitude = 0;
        public float compass = 0;
        public float velocityX = 0;
        public float velocityY = 0;
        public float velocityZ = 0;
        public float airTemperature = 0;
        public float airPressure = 0;
        public float battery = 0;
    }

}