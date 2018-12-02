package com.medialab.nci;

import android.util.Log;

public abstract class NciObject {
    /**
     * The Log tag.
     */
    private static final String TAG = "NciObject";

    /**
     * Loads the JNI module.
     */
    static {
        Log.d(TAG, "static initializer: ");
        System.loadLibrary("aps-jni");
    }

    /**
     * The NCI pointer.
     */
    private long nciPtr;

    /**
     * Creates the NCI resource. The class should implement this method to call the
     * JNI method for constructing the NCI object.
     *
     * @return
     */
    abstract protected long newNci();

    /**
     * Destroys the NCI resources. The class should implement this method to call
     * the JNI method for destroying the NCI object.
     */
    abstract protected void deleteNci();

    /**
     * Constructs the NciObject.
     */
    public NciObject() {
        this.nciPtr = newNci();
    }

    /**
     * Finalizes the NCI resource.
     *
     * @throws Throwable
     */
    @Override
    protected synchronized void finalize() throws Throwable {
        super.finalize();
        deleteNci();
    }
}
