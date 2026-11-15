#include "InputValidation.h"

#include <cctype>
#include <sstream>

bool tryParseNumber(const string& text, int& number) {
    stringstream stream(text);
    int value = 0;
    stream >> value;
    if (stream.fail() || value < 0) {
        return false;   // the text does not start with a number, the number is too big or negative
    }
    string rest;
    stream >> rest;
    if (!rest.empty()) {
        return false;   // something follows the number, like in "12a" or "1.5"
    }
    number = value;
    return true;
}

bool isValidPlayerName(const string& name) {
    if (name.empty() || name.size() > 10) {
        return false;
    }
    for (char letter : name) {
        if (!isalnum((unsigned char)letter) && letter != '_') {
            return false;
        }
    }
    return true;
}
