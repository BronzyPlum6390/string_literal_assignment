#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

using namespace std;

// Helper function to check if a character is a digit
bool isDigit(char c) 
{
    return c >= '0' && c <= '9';
}

// Function to validate if a string is a valid numeric literal
bool isValidNumericLiteral(const std::string& str) 
{
    // Define states for the finite state machine
    enum State { START, INTEGER, DECIMAL, AFTER_DECIMAL, EXPONENT, EXPONENT_SIGN, EXPONENT_DIGITS };
    State state = START;
    bool hasDigit = false;

    // Iterate through each character in the string
    for (char c : str) 
    {
        switch (state) 
        {
            case START:
                // Initial state: accept sign, digit, or decimal point
                if (c == '+' || c == '-') state = INTEGER;
                else if (isDigit(c)) { state = INTEGER; hasDigit = true; }
                else if (c == '.') state = DECIMAL;
                else return false;
                break;
            case INTEGER:
                // After initial integer part: accept digits, decimal point, or exponent
                if (isDigit(c)) hasDigit = true;
                else if (c == '.') state = AFTER_DECIMAL;
                else if (c == 'e' || c == 'E') state = EXPONENT;
                else return false;
                break;
            case DECIMAL:
                // Immediately after decimal point: must be followed by a digit
                if (isDigit(c)) { state = AFTER_DECIMAL; hasDigit = true; }
                else return false;
                break;
            case AFTER_DECIMAL:
                // After decimal and at least one digit: accept more digits or exponent
                if (isDigit(c)) hasDigit = true;
                else if (c == 'e' || c == 'E') state = EXPONENT;
                else return false;
                break;
            case EXPONENT:
                // Start of exponent: accept sign or digit
                if (c == '+' || c == '-') state = EXPONENT_SIGN;
                else if (isDigit(c)) state = EXPONENT_DIGITS;
                else return false;
                break;
            case EXPONENT_SIGN:
                // After exponent sign: must be followed by a digit
                if (isDigit(c)) state = EXPONENT_DIGITS;
                else return false;
                break;
            case EXPONENT_DIGITS:
                // Exponent digits: only accept more digits
                if (!isDigit(c)) return false;
                break;
        }
    }

    // Valid if we have at least one digit and end in a valid state
    return hasDigit && (state == INTEGER || state == AFTER_DECIMAL || state == EXPONENT_DIGITS);
}

int main() {
    std::string inputFileName;
    std::cout << "Enter the input file name: ";
    std::cin >> inputFileName;

    // Open input file
    std::ifstream inputFile(inputFileName);
    if (!inputFile) {
        std::cerr << "Error opening input file." << std::endl;
        return 1;
    }

    // Create output file name and open output file
    std::string outputFileName = inputFileName.substr(0, inputFileName.find_last_of('.')) + "_output.txt";
    std::ofstream outputFile(outputFileName);
    if (!outputFile) {
        std::cerr << "Error creating output file." << std::endl;
        return 1;
    }

    std::string line;
    // Read input file line by line
    while (std::getline(inputFile, line)) {
        // Validate each line
        bool isValid = isValidNumericLiteral(line);
        std::string result = line + (isValid ? " is a valid numeric literal" : " is NOT a valid numeric literal");
        
        // Output result to console and file
        std::cout << result << std::endl;
        outputFile << result << std::endl;
    }

    // Close files
    inputFile.close();
    outputFile.close();

    std::cout << "Results have been written to " << outputFileName << std::endl;

    return 0;
}