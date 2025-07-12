/*
 * 24GHz Microwave Radar Sensor - Distance Detection
 * 
 * This sketch reads distance data from the SEN0306 24GHz radar sensor
 * Note: This sensor provides distance but not angle information
 * 
 * Connections:
 * Pin 1 (VCC) -> Arduino 5V
 * Pin 2 (GND) -> Arduino GND  
 * Pin 3 (3.3V) -> Not connected (auxiliary power output)
 * Pin 4 (TX)  -> Arduino Digital Pin 4 (RX)
 * Pin 5 (RX)  -> Arduino Digital Pin 5 (TX) 
 * Pin 6 (Mode)-> Leave floating for distance only, or connect to GND for distance + spectral data
 */

#include <SoftwareSerial.h>

// Create SoftwareSerial object for communication with radar
SoftwareSerial radarSerial(4, 5); // RX=4, TX=5

// Variables for data processing
char receivedByte;
unsigned char dataBuffer[134] = {}; // Buffer for received data
int distanceHigh = 0, distanceLow = 0, targetDistance = 0;

// Multiple target detection variables
int targetCount = 1;
double spectralDistance = 0;
bool multiTargetMode = true; // Set to true if Pin 6 is connected to GND

void setup() {
  // Initialize serial communications
  Serial.begin(115200);
  radarSerial.begin(57600);
  
  Serial.println("24GHz Radar Sensor - Distance Detection");
  Serial.println("======================================");
  Serial.println("Note: This sensor provides distance information only.");
  Serial.println("Angle information is not available from this sensor.");
  Serial.println();
  
  // Detect if we're in multi-target mode by checking for spectral data
  delay(1000);
}

void loop() {
  // Check if data is available from radar
  if (radarSerial.available()) {
    // Look for start of frame (0xFF)
    if (radarSerial.read() == 0xFF) {
      
      // Read the data packet
      int bufferSize = multiTargetMode ? 134 : 8;
      for (int i = 0; i < bufferSize; i++) {
        while (!radarSerial.available()) {
          delay(1); // Wait for data
        }
        receivedByte = radarSerial.read();
        dataBuffer[i] = (unsigned char)receivedByte;
      }
      
      // Clear the serial buffer
      radarSerial.flush();
      
      // Verify frame header (should be 0xFF, 0xFF, 0xFF)
      if (dataBuffer[0] == 0xFF && dataBuffer[1] == 0xFF) {
        
        // Extract distance data (bytes 2 and 3)
        distanceHigh = dataBuffer[2];
        distanceLow = dataBuffer[3];
        targetDistance = (distanceHigh << 8) + distanceLow;
        
        // Display primary target distance
        Serial.print("Primary Target Distance: ");
        Serial.print(targetDistance);
        Serial.println(" cm");
        
        // Convert to other units
        Serial.print("Distance: ");
        Serial.print(targetDistance / 100.0, 2);
        Serial.println(" meters");
        
        // If in multi-target mode, process spectral data
        if (multiTargetMode && bufferSize == 134) {
          processSpectralData();
        }
        
        Serial.println("---");
      }
    }
  }
  
  delay(100); // Small delay between readings
}

void processSpectralData() {
  Serial.println("Additional Targets (from spectral analysis):");
  
  targetCount = 1;
  int peakStart = -1;
  int peakEnd = -1;
  int peakValue = 0;
  
  // Analyze spectral data (126 spectral lines starting from index 5)
  for (int i = 5; i < 131; i++) {
    int currentAmplitude = dataBuffer[i];
    
    // Look for peaks in spectral data (amplitude >= 10)
    if (currentAmplitude >= 10) {
      if (peakStart == -1) {
        peakStart = i;
        peakValue = currentAmplitude;
      }
      
      // Track the end of the peak
      if (currentAmplitude >= peakValue) {
        peakValue = currentAmplitude;
        peakEnd = i;
      }
    } else {
      // End of peak detected
      if (peakStart != -1) {
        // Calculate distance for this peak
        double peakCenter = peakStart + (peakEnd - peakStart) / 2.0;
        double spectralIndex = peakCenter - 5; // Adjust for spectral data start
        spectralDistance = spectralIndex * 0.126; // Convert to meters
        
        Serial.print("Target ");
        Serial.print(targetCount);
        Serial.print(": ");
        Serial.print(spectralDistance * 100, 1);
        Serial.print(" cm (");
        Serial.print(spectralDistance, 3);
        Serial.println(" m)");
        
        targetCount++;
        
        // Reset for next peak
        peakStart = -1;
        peakEnd = -1;
        peakValue = 0;
        
        // Limit to reasonable number of targets
        if (targetCount > 5) break;
      }
    }
  }
}

// Function to estimate angle sector based on detection strength
// This is a theoretical approach with very limited accuracy
String estimateAngleSector(int signalStrength, int maxStrength) {
  if (maxStrength == 0) return "Unknown";
  
  float strengthRatio = (float)signalStrength / maxStrength;
  
  if (strengthRatio > 0.9) {
    return "Center (±10°)";
  } else if (strengthRatio > 0.7) {
    return "Near-Center (±20°)";
  } else if (strengthRatio > 0.5) {
    return "Mid-Beam (±30°)";
  } else if (strengthRatio > 0.3) {
    return "Beam-Edge (±39°)";
  } else {
    return "Weak/Edge";
  }
}

