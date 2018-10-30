package com.medialab.apserversdk;

import android.content.Context;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.medialab.airplay.AirPlayConfig;
import com.medialab.airplay.AirPlayServer;

public class MainActivity extends AppCompatActivity {

    AirPlayServer airPlayServer = null;

    AirPlayConfig config = null;

    boolean isServerStarted = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        final Button switch_btn = (Button) findViewById(R.id.server_switch);
        switch_btn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Button btn = (Button)v;
                if (isServerStarted) {
                    airPlayServer.stop();
                    btn.setText("Start");
                    isServerStarted = false;
                } else {
                    isServerStarted = airPlayServer.start();
                    if (!isServerStarted) {
                        Context context = getApplicationContext();
                        CharSequence text = "Hello toast!";
                        int duration = Toast.LENGTH_SHORT;

                        Toast toast = Toast.makeText(context, text, duration);
                        toast.show();
                    } else {
                        btn.setText("Stop");
                    }
                }
            }
        });

        config = AirPlayConfig.defaultInstance();
        airPlayServer = new AirPlayServer(getApplicationContext(), config);
    }
}
