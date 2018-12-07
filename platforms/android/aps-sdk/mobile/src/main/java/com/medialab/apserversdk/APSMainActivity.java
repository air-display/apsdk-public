package com.medialab.apserversdk;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

public class APSMainActivity extends AppCompatActivity implements APSDemoApplication.IAirPlayAcceptor {
    private static final String TAG = "APSMainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ((APSDemoApplication)getApplication()).setAirplayAcceptor(this);
    }

    protected void onStart() {
        Log.d(TAG, "onStart: ");
        super.onStart();
    }

    @Override
    protected void onPause() {
        Log.d(TAG, "onPause: ");
        super.onPause();
    }

    @Override
    protected void onResume() {
        Log.d(TAG, "onResume: ");
        super.onResume();
    }

    @Override
    protected void onStop() {
        Log.d(TAG, "onStop: ");
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        Log.d(TAG, "onDestroy: ");
        super.onDestroy();
        ((APSDemoApplication)getApplication()).stopAirPlayServer();
    }

    @Override
    public void preparePlayer(String location, float position) {
        Intent intent = new Intent(APSMainActivity.this, APSPlayerActivity.class);
        intent.putExtra(APSPlayerActivity.PREFER_EXTENSION_DECODERS_EXTRA, false);
        intent.putExtra(APSPlayerActivity.START_WINDOW_INDEX, 0);
        intent.putExtra(APSPlayerActivity.START_POSITION, position);
        intent.setAction(APSPlayerActivity.ACTION_VIEW);
        intent.setData(Uri.parse(location));
        startActivity(intent);
    }
}
