#include <pthread.h>
#include <iostream>
#include <string>
using namespace std;
const int MAX_SYMBOLS = 256;
string message;
char symbols[MAX_SYMBOLS];
int frequency[MAX_SYMBOLS];
int symbolCount = 0;
pthread_t threads[MAX_SYMBOLS];
int indexForThread[MAX_SYMBOLS];
// Converts a number to a base 10 string
string toBase10(int number) {
    if (number == 0) {
        return "0";
    }
    string result = "";
    while (number > 0) {
        int digit = number % 10;
        char digitChar = '0' + digit;
        result = digitChar + result;
        number = number / 10;
    }
    return result;
}
// Converts a 16-bit number to binary
string toBase2(int number) {
    string result = "";
    for (int i = 15; i >= 0; i--) {
        int bit = (number >> i) & 1;
        if (bit == 1) {
            result += "1";
        } else {
            result += "0";
        }
    }
    return result;
}
// Converts a 16-bit number to hexadecimal
string toBase16(int number) {
    string hex = "0123456789ABCDEF";
    string result = "";
    for (int i = 12; i >= 0; i -= 4) {
        int digit = (number >> i) & 15;
        result += hex[digit];
    }
    return result;
}
// Switches the two bytes for little endian format
int toLittleEndian(int number) {
    int firstByte = number & 255;
    int secondByte = (number >> 8) & 255;
    return (firstByte << 8) | secondByte;
}
// Returns the number in big endian format
int toBigEndian(int number) {
    return number;
}
// Finds each different symbol in the message
void findSymbols() {
    bool alreadySeen[MAX_SYMBOLS] = {false};
    for (int i = 0; i < message.length(); i++) {
        unsigned char currentChar = message[i];
        if (alreadySeen[currentChar] == false) {
            alreadySeen[currentChar] = true;
            symbols[symbolCount] = currentChar;
            symbolCount++;
        }
    }
}
// Counts how many times one symbol appears in the message
void* countOneSymbol(void* argument) {
    int* indexPointer = (int*)argument;
    int myIndex = *indexPointer;
    char mySymbol = symbols[myIndex];
    int count = 0;
    for (int i = 0; i < message.length(); i++) {
        if (message[i] == mySymbol) {
            count++;
        }
    }
    frequency[myIndex] = count;
    return NULL;
}
// Prints the frequency of one symbol in each format
void printResult(int index) {
    int rawValue = frequency[index];
    int littleValue = toLittleEndian(rawValue);
    int bigValue = toBigEndian(rawValue);
    cout << "Symbol: " << symbols[index] << endl;
    cout << "Frequency (raw) value: " << toBase10(rawValue) << endl;
    cout << "Frequency base 2 (raw) value: " << toBase2(rawValue) << endl;
    cout << "Frequency base 16 (raw) value: " << toBase16(rawValue) << endl;
    cout << "Frequency Little Endian base 10 value: "
         << toBase10(littleValue) << endl;
    cout << "Frequency Little Endian base 2 value: "
         << toBase2(littleValue) << endl;
    cout << "Frequency Little Endian base 16 value: "
         << toBase16(littleValue) << endl;
    cout << "Frequency Big Endian base 10 value: "
         << toBase10(bigValue) << endl;
    cout << "Frequency Big Endian base 2 value: "
         << toBase2(bigValue) << endl;
    cout << "Frequency Big Endian base 16 value: "
         << toBase16(bigValue) << endl;
}
int main() {
    // Reads the message from the user
    if (!getline(cin, message)) {
        return 1;
    }
    // Finds all unique symbols
    findSymbols();
    if (symbolCount == 0) {
        return 0;
    }
    // Creates one thread for each unique symbol
    for (int i = 0; i < symbolCount; i++) {
        indexForThread[i] = i;
        int status = pthread_create(
            &threads[i],
            NULL,
            countOneSymbol,
            &indexForThread[i]
        );
        if (status != 0) {
            cerr << "cannot create the child thread" << endl;
            return 1;
        }
    }
    // Waits for all threads to finish counting
    for (int i = 0; i < symbolCount; i++) {
        pthread_join(threads[i], NULL);
    }
    // Prints the result for every symbol
    for (int i = 0; i < symbolCount; i++) {
        printResult(i);
        if (i + 1 < symbolCount) {
            cout << endl;
        }
    }
    return 0;
}
