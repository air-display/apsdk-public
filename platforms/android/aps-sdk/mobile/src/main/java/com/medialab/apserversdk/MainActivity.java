package com.medialab.apserversdk;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

import com.medialab.airplay.AirPlayServer;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);

        AirPlayServer aps = new AirPlayServer();
        aps.initialize();
        aps.start();
        aps.stop();
        aps.uninitialize();
    }
}
