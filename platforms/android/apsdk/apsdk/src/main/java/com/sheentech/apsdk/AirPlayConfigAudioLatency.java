/* 
 *  File: AirPlayConfigAudioLatency.java
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

public class AirPlayConfigAudioLatency {

  private int type = 0;

  private String audioType;

  private int inputLatencyMicros = 0;

  private int outputLatencyMicros = 0;

  public int getType() { return type; }

  public void setType(int type) { this.type = type; }

  public String getAudioType() { return audioType; }

  public void setAudioType(String audioType) { this.audioType = audioType; }

  public int getInputLatencyMicros() { return inputLatencyMicros; }

  public void setInputLatencyMicros(int inputLatencyMicros) {
    this.inputLatencyMicros = inputLatencyMicros;
  }

  public int getOutputLatencyMicros() { return outputLatencyMicros; }

  public void setOutputLatencyMicros(int outputLatencyMicros) {
    this.outputLatencyMicros = outputLatencyMicros;
  }
}
