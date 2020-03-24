package com.virtable.apserversdk;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

public class APSMainActivity extends AppCompatActivity {
  private static final String TAG = "APSMainActivity";

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    Log.d(TAG, "onCreate: ");
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
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
  }
}
