package com.virtable.airplay;

import android.util.Log;

/**
 * Represents the NciObject
 */
abstract class NciObject {
  /**
   * The Log tag.
   */
  private static final String TAG = "NciObject";

  static {
    Log.d(TAG, "static initializer: ");
    System.loadLibrary("aps-jni");
  }

  /**
   * The NCI pointer.
   */
  private long nci_obj_;

  /**
   * Constructs the NciObject.
   */
  NciObject() { newNci(); }
  /**
   * Creates the NCI resource. The class should implement this method to call
   * the JNI method for constructing the NCI object.
   *
   */
  abstract protected void newNci();
  /**
   * Destroys the NCI resources. The class should implement this method to call
   * the JNI method for destroying the NCI object.
   */
  abstract protected void deleteNci();
  /**
   * Finalizes the NCI resource.
   *
   * @throws Throwable **
   */
  @Override
  protected synchronized void finalize() throws Throwable {
    super.finalize();
    deleteNci();
  }
}
