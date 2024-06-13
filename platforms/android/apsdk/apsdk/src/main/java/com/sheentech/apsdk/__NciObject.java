/* 
 *  File: __NciObject.java
 *  Project: apsdk
 *  Created: Oct 25, 2018
 *  Author: Sheen Tian
 *  
 *  This file is part of apsdk (https://github.com/air-display/apsdk-public) 
 *  Copyright (C) 2018-2024 Sheen Tian 
 *  
 *  apsdk is free software: you can redistribute it and/or modify it under the terms 
 *  of the GNU General Public License as published by the Free Software Foundation, 
 *  either version 3 of the License, or (at your option) any later version.
 *  
 *  apsdk is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *  See the GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License along with Foobar. 
 *  If not, see <https://www.gnu.org/licenses/>.
 */

package com.sheentech.apsdk;

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
