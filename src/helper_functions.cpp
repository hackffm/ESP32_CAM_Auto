#include "helper_functions.h"
#include "FS.h"
#include <LittleFS.h> 
#include "PwmThing.h"
#include <stdio.h>   
#include <string.h>  

extern PwmThing WhiteLED;
extern fs::FS &filesystem;

// these pins should be safe to use, avoid pin 1+3 as used for serial, avoid pin 4 as used for flash light
// pin 0 is camera clk, pin 16 is PSRAM CS - both can not be used!
// pin 33 is internal RedLED, pin 4 is WhiteLED, need extra wires if use need (and disable LED Things in code)
// Serial RX is disabled if GPIO3 is used
const int usablePins[]     = {-1,2,3,12,13,14,15 /* ,33 ,1 ,3, 4 */};
const char availablePins[] = "-1,2,3,12,13,14,15";  // For Web Interface
const size_t usablePinsCount = sizeof(usablePins) / sizeof(usablePins[0]);



// Global static array to hold the compile time string in "YYYYMMDD HH:mm:ss" format
char CompileTime[20] = {0}; // Buffer size: 17 chars + null terminator

/**
 * @brief Retrieves the compile date and time in the format "YYYYMMDD HH:mm:ss"
 *        and stores it in the global static array CompileTime.
 *
 * This function uses the predefined macros __DATE__ and __TIME__ to obtain the
 * compilation date and time. It parses these macros, converts the month name
 * to a numerical value (1-12), and formats the result into the CompileTime array.
 * The format follows ISO 8601 conventions for date and time representation.
 *
 * @note The CompileTime array is static and initialized only once. It will retain
 *       its value for the duration of the program execution.
 */
void getCompileTime() {
    // Extract date components from __DATE__ (format: "Mmm dd yyyy")
    const char* date = __DATE__;
    char month_str[4] = {0}; // Buffer for 3-letter month abbreviation + null terminator
    int day, year;

    // Parse the date string using sscanf. Expected format: "Dec 25 2023"
    // Note: The space before %d handles single-digit days (e.g., "Dec  5 2023")
    if (sscanf(date, "%3s %d %d", month_str, &day, &year) != 3) {
        // In case of parsing failure, clear the buffer and return
        CompileTime[0] = '\0';
        return;
    }

    // Map the 3-letter month abbreviation to a numerical value (Jan=1, Feb=2, ..., Dec=12)
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    int month = 0;
    for (int i = 0; i < 12; ++i) {
        if (strcmp(month_str, months[i]) == 0) {
            month = i + 1; // Months are 1-based
            break;
        }
    }

    // Extract time components from __TIME__ (format: "hh:mm:ss")
    const char* time = __TIME__;
    int hour, minute, second;

    // Parse the time string using sscanf. Expected format: "14:30:45"
    if (sscanf(time, "%d:%d:%d", &hour, &minute, &second) != 3) {
        CompileTime[0] = '\0';
        return;
    }

    // Format the result into the CompileTime array as "YYYYMMDD HH:mm:ss"
    // Using snprintf ensures buffer safety and proper formatting with leading zeros
    snprintf(CompileTime, sizeof(CompileTime), "%04d%02d%02d %02d:%02d:%02d",
             year, month, day, hour, minute, second);
}


float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh) 
{ return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow; }

// Helper: hex char (0-9a-f) to decimal value
static int hexchar_to_dec(char c) {
    if ('0' <= c && c <= '9') return c - '0';
    if ('A' <= c && c <= 'F') return c - 'A' + 10;
    if ('a' <= c && c <= 'f') return c - 'a' + 10;
    return 0;
}

// From esp-idf/examples/protocols/http_server/simple/main/main.c
// Decodes %xx to UTF-8 char (modifies in-place)
void uri_decode(char *decoded, const char *encoded, size_t len) {
    char *p = decoded;
    const char *e = encoded;
    while (*e && (p - decoded < len)) {
        if (*e == '%') {
            if (e[1] && e[2]) {
                *p++ = (hexchar_to_dec(e[1]) << 4) + hexchar_to_dec(e[2]);
                e += 3;
            } else {
                *p++ = '%';
                e++;
            }
        } else if (*e == '+') {
            *p++ = ' ';
            e++;
        } else {
            *p++ = *e++;
        }
    }
    *p = 0;
}

void error_blink(int times, int delay_ms) {
  while(true) { for (int i = 0; i < times; i++) {
      WhiteLED.set(32); delay(delay_ms);
      WhiteLED.set(0); delay(delay_ms);
    } delay(3000); }
}


// Function to write a string to a file
bool writeFile(const char *path, const char *data) {
  if(strlen(data) == 0) {
    Serial.printf("Empty string, deleting %s\n", path);
    filesystem.remove(path);
    return true;
  }
  File file = filesystem.open(path, "w", true); // "w" overwrite file
  if (!file) {
    Serial.printf("Can't open file %s to write.\n", path);
    return false;
  }
  file.print(data);
  file.close();
  Serial.printf("File written: %s\n", path);
  return true;
}

// Function to read a file and return the content as a string
size_t readFile(const char *path, char *buffer, size_t bufferSize) {
  if(filesystem.exists(path) == false) {
    Serial.printf("File %s does not exist\r\n", path);
    return 0;
  }
  File file = filesystem.open(path, "r");  
  if (!file) {
    Serial.println("Can't open file for reading");
    return 0;
  }
  size_t bytesRead = 0;
  if(file.size() > 0) {
    bytesRead = file.readBytes(buffer, bufferSize - 1); // Leave space for null terminator
    buffer[bytesRead] = '\0'; // Null-terminate the string
  }
  file.close();
  return bytesRead;
}

void listDir(const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  fs::File root = filesystem.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }

  size_t t = LittleFS.totalBytes();
  size_t u = LittleFS.usedBytes();
  
  Serial.printf("Total space: %d, used space: %d, free space: %d\r\n", t, u, t - u);
}

bool isUsablePin(int number) {
    // Check if the number is in the list
    for (int i = 0; i < usablePinsCount; i++) if (usablePins[i] == number) return true;
    return false;
}