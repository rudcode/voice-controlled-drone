package studio.android.art.artdrone3;

/**
 * Created by hp on 25/11/15.
 */
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

/**
 * Created by hp1 on 21-01-2015.
 */
public class Tab2 extends Fragment {
    public static ImageView cameraDrone;

    Handler handler;
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.tab_2,container,false);
        cameraDrone = (ImageView) v.findViewById(R.id.cameraDrone);

        handler = new Handler();

        handler.post(new Runnable() {
            @Override
            public void run() {
                Log.e("video", MainActivity.pager.getCurrentItem()+"");
                if (MainActivity.pager.getCurrentItem() == 1) {
                    if (MainActivity.tcpClient != null) {
                        MainActivity.tcpClient.sendMessage("vf");
                    }
                }
                handler.postDelayed(this, 50); // set time here to refresh textView
            }
        });

        return v;
    }
}