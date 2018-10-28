package com.medialab.apserversdk;

import android.os.Bundle;
import android.app.Activity;

import com.medialab.airplay.AirPlayServer;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        AirPlayServer aps = new AirPlayServer();
        aps.initialize();
        aps.start();
        aps.stop();
        aps.uninitialize();
    }

}
