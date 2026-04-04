// Collection of helper functions
#include "Arduino.h"

float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh);

// Helper: hex char (0-9a-f) to decimal value
static int hexchar_to_dec(char c);

// From esp-idf/examples/protocols/http_server/simple/main/main.c
// Decodes %xx to UTF-8 char (modifies in-place)
void uri_decode(char *decoded, const char *encoded, size_t len);

// Blinks endlessly
void error_blink(int times, int delay_ms);

// Function to write a string to a file, deletes file if empty string is provided
bool writeFile(const char *path, const char *data);

// Function to read a file and return the content as a string, does not touch buffer if file doesn't exist or is empty
size_t readFile(const char *path, char *buffer, size_t bufferSize);

void listDir(const char *dirname, uint8_t levels);

bool isUsablePin(int number);