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
import android.widget.ImageView;

/**
 * Created by hp1 on 21-01-2015.
 */
public class Tab2 extends Fragment {
    public static ImageView cameraDrone;

    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.tab_2,container,false);
        cameraDrone = (ImageView) v.findViewById(R.id.cameraDrone);

        return v;
    }
}