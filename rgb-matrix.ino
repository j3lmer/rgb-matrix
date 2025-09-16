const int MATRIX_SIZE = 8;
const unsigned long SERIAL_CHECK_INTERVAL = 100;

const int rows[MATRIX_SIZE] = {
  2, 7, 19, 5, 13, 18, 12, 16
};

const int columns[MATRIX_SIZE] = {
  6, 11, 10, 3, 17, 4, 8, 9
};

typedef bool matrixOutput[MATRIX_SIZE][MATRIX_SIZE];

enum WeatherType {
  SUN,
  CLOUD,
  RAIN,
  SNOW,
  WIND,
  UNKNOWN,
  LIGHTNING,
  CLEAR
};

int pixels[MATRIX_SIZE][MATRIX_SIZE];
WeatherType currentWeatherType = UNKNOWN;
unsigned long lastSerialCheck = 0;

const matrixOutput sun = {
  { 1, 0, 0, 1, 1, 0, 0, 1 },
  { 0, 1, 0, 1, 1, 0, 1, 0 },
  { 0, 0, 1, 1, 1, 1, 0, 0 },
  { 1, 1, 1, 1, 1, 1, 1, 1 },
  { 1, 1, 1, 1, 1, 1, 1, 1 },
  { 0, 0, 1, 1, 1, 1, 0, 0 },
  { 0, 1, 0, 1, 1, 0, 1, 0 },
  { 1, 0, 0, 1, 1, 0, 0, 1 }
};

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

const matrixOutput lightning = {
  { 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 1, 0, 0, 0 },
  { 0, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

void setup() {
  Serial.begin(9600);

  initializePins();
  setWeatherType(SUN);

  Serial.println("=== Weather Display System ===");
  Serial.println("Commands: SUN, CLOUD, RAIN, SNOW, WIND, UNKNOWN, LIGHTNING, CLEAR");
  Serial.println("System ready!");
}

void loop() {
  checkSerialInput();
  displayLedMatrix();
}

void initializePins() {
  for (int pin = 0; pin < MATRIX_SIZE; pin++) {
    pinMode(columns[pin], OUTPUT);
    pinMode(rows[pin], OUTPUT);
    // Initialize all pins to HIGH (off state)
    digitalWrite(columns[pin], HIGH);
    digitalWrite(rows[pin], LOW);
  }
  Serial.println("Pins initialized");
}

void loadPattern(const matrixOutput pattern) {
  for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      // Convert pattern (1 = on, 0 = off) to LED state (LOW = on, HIGH = off)
      pixels[row][col] = pattern[row][col] ? LOW : HIGH;
    }
  }
}

void clearMatrix() {
  for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      pixels[row][col] = HIGH;
    }
  }
}

const matrixOutput* getWeatherPattern(WeatherType type) {
  switch (type) {
    case SUN: return &sun;
    case CLOUD: return &cloud;
    case RAIN: return &rain;
    case SNOW: return &snow;
    case WIND: return &wind;
    case LIGHTNING: return &lightning;
    case UNKNOWN: return &unknown;
    default: return nullptr;
  }
}

void setWeatherType(WeatherType newType) {
  if (currentWeatherType == newType) {
    return;
  }

  WeatherType oldType = currentWeatherType;
  currentWeatherType = newType;

  const matrixOutput* pattern = getWeatherPattern(newType);

  if (pattern != nullptr) {
    loadPattern(*pattern);
    Serial.print("Weather changed: ");
    Serial.print(weatherTypeToString(oldType));
    Serial.print(" -> ");
    Serial.println(weatherTypeToString(newType));
  } else {
    clearMatrix();
    Serial.println("Matrix cleared");
  }
}

void checkSerialInput() {
  if (millis() - lastSerialCheck < SERIAL_CHECK_INTERVAL) {
    return;
  }

  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase();

    processSerialCommand(command);
  }

  lastSerialCheck = millis();
}



void processSerialCommand(const String& command) {
  WeatherType newType = stringToWeatherType(command);

  if (newType != currentWeatherType) {
    setWeatherType(newType);
  } else {
    Serial.print("Already displaying: ");
    Serial.println(command);
  }
}

WeatherType stringToWeatherType(const String& str) {
  if (str == "SUN") return SUN;
  if (str == "CLOUD") return CLOUD;
  if (str == "RAIN") return RAIN;
  if (str == "SNOW") return SNOW;
  if (str == "WIND") return WIND;
  if (str == "LIGHTNING") return LIGHTNING;
  if (str == "UNKNOWN") return UNKNOWN;

  Serial.print("Unknown command: ");
  Serial.println(str);
  return currentWeatherType;
}

const char* weatherTypeToString(WeatherType type) {
  switch (type) {
    case SUN: return "SUN";
    case CLOUD: return "CLOUD";
    case RAIN: return "RAIN";
    case SNOW: return "SNOW";
    case WIND: return "WIND";
    case LIGHTNING: return "LIGHTNING";
    case UNKNOWN: return "UNKNOWN";
    default: return "INVALID";
  }
}


void displayLedMatrix() {
  for (int row = 0; row < MATRIX_SIZE; row++) {
    // Turn on current row
    digitalWrite(rows[row], HIGH);

    // Set column states for this row
    for (int col = 0; col < MATRIX_SIZE; col++) {
      digitalWrite(columns[col], pixels[row][col]);

      // Brief pulse for LED activation
      if (pixels[row][col] == LOW) {
        delayMicroseconds(1);
        digitalWrite(columns[col], HIGH);
      }
    }

    // Turn off current row
    digitalWrite(rows[row], LOW);
  }
}

void printCurrentPattern() {
  Serial.print("Current weather: ");
  Serial.println(weatherTypeToString(currentWeatherType));

  Serial.println("Pattern:");
  for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      Serial.print(pixels[row][col] == LOW ? "█" : "·");
      Serial.print(" ");
    }
    Serial.println();
  }
}
