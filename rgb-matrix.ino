// 2-dimensional array of row pin numbers:
const int rows[8] = {
  2, 7, 19, 5, 13, 18, 12, 16
};
// 2-dimensional array of column pin numbers:
const int columns[8] = {
  6, 11, 10, 3, 17, 4, 8, 9
};
// 2-dimensional array of pixels:
int pixels[8][8];

typedef bool matrixOutput[8][8];

enum WeatherType {
  SUN,
  CLOUD,
  RAIN,
  SNOW,
  WIND,
  UNKNOWN
};

WeatherType currentWeatherType = UNKNOWN;

const matrixOutput sun = {
  { 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 1, 1, 0, 1, 0 },
  { 0, 0, 0, 1, 1, 0, 0, 0 },
  { 1, 1, 1, 0, 0, 1, 1, 1 },
  { 1, 1, 1, 0, 0, 1, 1, 1 },
  { 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 1, 1, 0, 1, 0 },
  { 0, 0, 0, 1, 1, 0, 0, 0 }
};

// Improved cloud with better shape
const matrixOutput cloud = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 1, 1, 1, 0, 0 },
  { 0, 1, 1, 1, 1, 1, 1, 0 },
  { 1, 1, 1, 1, 1, 1, 1, 1 },
  { 1, 1, 1, 1, 1, 1, 1, 1 },
  { 1, 1, 1, 1, 1, 1, 1, 1 },
  { 0, 1, 1, 1, 1, 1, 1, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

// Enhanced rain with cloud and raindrops
const matrixOutput rain = {
  { 0, 1, 1, 1, 1, 1, 0, 0 },
  { 1, 1, 1, 1, 1, 1, 1, 0 },
  { 1, 1, 1, 1, 1, 1, 1, 1 },
  { 0, 1, 1, 1, 1, 1, 1, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 0, 1, 0, 1, 0, 1, 0 },
  { 0, 1, 0, 1, 0, 1, 0, 1 },
  { 1, 0, 1, 0, 1, 0, 1, 0 }
};

// New snow icon with cloud and snowflakes
const matrixOutput snow = {
  { 0, 1, 1, 1, 1, 1, 0, 0 },
  { 1, 1, 1, 1, 1, 1, 1, 0 },
  { 1, 1, 1, 1, 1, 1, 1, 1 },
  { 0, 1, 1, 1, 1, 1, 1, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 1, 0, 1, 0, 0 },
  { 1, 0, 1, 0, 1, 0, 1, 0 },
  { 0, 1, 0, 1, 0, 1, 0, 1 }
};

// New wind icon with flowing lines
const matrixOutput wind = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 1, 1, 1, 1, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 0 },
  { 1, 1, 1, 1, 1, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0 },
  { 1, 1, 1, 1, 1, 1, 1, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 1 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

// New unknown/question mark icon
const matrixOutput unknown = {
  { 0, 1, 1, 1, 1, 1, 0, 0 },
  { 1, 1, 0, 0, 0, 1, 1, 0 },
  { 0, 0, 0, 0, 1, 1, 0, 0 },
  { 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 0, 1, 1, 0, 0, 0, 0 },
  { 0, 0, 1, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 1, 0, 0, 0, 0 }
};

void loadPattern(const matrixOutput pattern) {
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      pixels[x][y] = pattern[x][y] ? LOW : HIGH;
    }
  }
}

void clearMatrix() {
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      // FIXED: HIGH = LED off (clear state)
      pixels[x][y] = HIGH;
    }
  }
}

void setup() {
  Serial.begin(9600);

  // Initialize all pins
  for (int thisPin = 0; thisPin < 8; thisPin++) {
    pinMode(columns[thisPin], OUTPUT);
    pinMode(rows[thisPin], OUTPUT);
  }


  currentWeatherType = SUN;
  loadWeather();

  Serial.println("Weather display ready!");
}

void loadWeather() {
  switch (currentWeatherType) {
    case SUN:
      loadPattern(sun);
      Serial.println("Loaded SUN pattern");
      break;
    case CLOUD:
      loadPattern(cloud);
      Serial.println("Loaded CLOUD pattern");
      break;
    case RAIN:
      loadPattern(rain);
      Serial.println("Loaded RAIN pattern");
      break;
    default:
      clearMatrix();
      Serial.println("Cleared matrix");
      break;
  }
}

void loop() {
  static unsigned long lastSerialCheck = 0;

  // Check serial less frequently to not interfere with display
  if (millis() - lastSerialCheck > 50) {
    if (Serial.available()) {
      String cmd = Serial.readStringUntil('\n');
      cmd.trim();

      WeatherType oldType = currentWeatherType;

      if (cmd == "SUN") currentWeatherType = SUN;
      else if (cmd == "CLOUD") currentWeatherType = CLOUD;
      else if (cmd == "RAIN") currentWeatherType = RAIN;
      else if (cmd == "CLEAR") currentWeatherType = UNKNOWN;

      Serial.print("Received: ");
      Serial.println(cmd);

      // Only reload if weather type changed
      if (currentWeatherType != oldType) {
        loadWeather();
      }
    }
    lastSerialCheck = millis();
  }

  // Continuously refresh the display
  displayLedPattern();
}



void displayLedPattern() {
  for (int thisRow = 0; thisRow < 8; thisRow++) {
    digitalWrite(rows[thisRow], HIGH);
    for (int thisCol = 0; thisCol < 8; thisCol++) {
      // get the state of the current pixel;
      int thisPixel = pixels[thisRow][thisCol];
      // when the row is HIGH and the col is LOW,
      // the LED where they meet turns on:
      digitalWrite(columns[thisCol], thisPixel);
      // turn the pixel off:
      if (thisPixel == LOW) {
        digitalWrite(columns[thisCol], HIGH);
      }
    }
    // take the row pin low to turn off the whole row:
    digitalWrite(rows[thisRow], LOW);
  }
}
