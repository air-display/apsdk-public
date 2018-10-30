package com.medialab.apserversdk;

import android.os.Bundle;
import android.app.Activity;

import com.medialab.airplay.AirPlayConfig;
import com.medialab.airplay.AirPlayServer;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        AirPlayConfig config = AirPlayConfig.defaultInstance();
        AirPlayServer airPlayServer = new AirPlayServer(getApplicationContext(), config);

        airPlayServer.start();
    }

}
