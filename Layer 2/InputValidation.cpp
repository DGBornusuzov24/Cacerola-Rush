#include "logic/InputValidation.h"

#include <cctype>
#include <cstdlib>

bool tryParseNumber(const string& text, int& number) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return false;
    }
    string trimmed = text.substr(first, text.find_last_not_of(" \t\r\n") - first + 1);
    if (trimmed.size() > 6 || trimmed.find_first_not_of("0123456789") != string::npos) {
        return false;
    }
    number = atoi(trimmed.c_str());
    return true;
}

bool isValidPlayerName(const string& name) {
    if (name.empty() || name.size() > 10) {
        return false;
    }
    for (char letter : name) {
        if (!isalnum(static_cast<unsigned char>(letter)) && letter != '_') {
            return false;
        }
    }
    return true;
}