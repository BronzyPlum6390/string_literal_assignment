#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

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

void processFile(const fs::path& inputPath) {
    // Open input file
    std::ifstream inputFile(inputPath);
    if (!inputFile) {
        std::cerr << "Error opening input file: " << inputPath << std::endl;
        return;
    }

    // Create output file in the same directory as the input file
    fs::path outputPath = inputPath.parent_path() / (inputPath.stem().string() + "_output.txt");
    std::ofstream outputFile(outputPath);
    if (!outputFile) {
        std::cerr << "Error creating output file: " << outputPath << std::endl;
        return;
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

    std::cout << "Results for " << inputPath.filename() << " have been written to " << outputPath << std::endl;
}

int main() 
{
    std::string inputFileName;
    
    // Display current working directory
    std::cout << "Current working directory: " << fs::current_path() << std::endl;
    
    // Loop to get and process multiple files
    while (true) {
        std::cout << "\nEnter an input file name (or full path), or type 'done' to finish: ";
        std::getline(std::cin, inputFileName);
        
        if (inputFileName == "done") break;
        
        // Convert to absolute path if it's not already
        fs::path inputPath = fs::absolute(inputFileName);
        
        std::cout << "Processing file: " << inputPath << std::endl;
        processFile(inputPath);
    }

    std::cout << "\nAll files have been processed." << std::endl;
    return 0;
}