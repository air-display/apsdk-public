package com.medialab.airplay;

import com.medialab.nci.NciObject;

public abstract class AirPlayHandler extends NciObject implements IAirPlayHandler {
    /**
     * Creates the native class instance.
     * @return the handle of the native object which is the value of the memory address.
     *
     */
    @Override
    protected long newNci() {
        // Call JNI method
        return nciNew();
    }
    private native long nciNew();

    /**
     * Destroys the native class instance.
     */
    @Override
    protected void deleteNci() {
        // Call JIN method
        nciDelete();
    }
    private native void nciDelete();
}
