// =====================================================================
// LESSON 1: BASIC SYNTAX — THE STRUCTURE OF A C++ PROGRAM
// =====================================================================
// Every C++ program has the same skeleton. Learn this skeleton and
// everything else is just filling in the blanks.
//
// Here's the anatomy:
//
//   1. #include directives — "import" libraries (preprocessor commands)
//   2. using directives    — avoid typing std:: over and over
//   3. int main()          — the ENTRY POINT. every program starts here
//   4. statements          — each statement ends with a semicolon ;
//   5. return 0            — tells the OS "program ended successfully"
//
// COMPILE:   g++ -std=c++20 -o lesson1 lesson1_basics.cpp
// RUN:       ./lesson1
// =====================================================================

// --- 1. INCLUDE DIRECTIVES ---
// These pull in libraries. Think of them as "import" in Python or Java.
// <iostream>  = input/output stream (cin, cout, cerr)
// <string>    = string type (text)
// <vector>    = dynamic arrays (we'll use this later)
#include <iostream>
#include <string>

// --- 2. USING DIRECTIVE ---
// Everything in the C++ standard library is in the "std" namespace.
// Without this line, you'd have to write std::cout, std::endl, etc.
// Using it means you can just write cout, endl, string, etc.
using namespace std;

// --- 3. MAIN FUNCTION ---
// This is where execution begins. Every C++ program MUST have exactly
// one main() function. The OS calls it when you run the program.
//
// int    = the return type (returns an integer to the OS)
// main   = the function name (must be "main")
// ()     = parameters (empty here = no command-line args)
int main()
{
    // --- 4. STATEMENTS ---
    // Each statement does something and ends with a semicolon ;
    // Think of ; like a period at the end of a sentence.

    // cout = "character output" — prints to the screen
    // <<   = the insertion operator (pushes data into cout)
    // endl = "end line" — prints a newline AND flushes the buffer
    // "\n" also works for newline but doesn't flush (slightly faster)

    cout << "Hello, World!" << endl;

    // You can chain multiple << operations together:
    cout << "My name is " << "Kiki" << " and I'm teaching you C++." << endl;

    // --- COMMENTS ---
    // Single line comment (everything after // is ignored by compiler)

    /* Multi-line comment
       everything between slash-star and star-slash is ignored
       use this for longer explanations */

    // --- PRINTING DIFFERENT THINGS ---
    // Numbers (no quotes):
    cout << 42 << endl;           // integer
    cout << 3.14159 << endl;     // floating point

    // Text (must use double quotes):
    cout << "Text goes in quotes" << endl;

    // Single character (single quotes):
    cout << 'A' << endl;

    // --- MULTIPLE VALUES ON ONE LINE ---
    cout << "The answer is " << 42 << " and pi is " << 3.14159 << endl;

    // --- ESCAPE SEQUENCES ---
    // \n  = newline (alternative to endl, slightly faster, no flush)
    // \t  = tab
    // \\  = literal backslash
    // \"  = literal double quote
    // \'  = literal single quote
    cout << "Tab\tTab\n";
    cout << "She said \"hello\" to me\n";
    cout << "Path: C:\\Users\\zen\n";

    // --- 5. RETURN STATEMENT ---
    // Returns control to the OS. 0 means "success".
    // Non-zero means something went wrong (you choose what numbers mean).
    // Modern C++ allows you to omit return 0 in main() (it's implied),
    // but writing it explicitly is clearer for beginners.
    return 0;
}

// =====================================================================
// WHAT YOU LEARNED:
//   - Every C++ program: #include → using → int main() { statements }
//   - Statements end with semicolons
//   - cout << X prints X to the screen
//   - Comments: // single line, /* multi line */
//   - Escape sequences: \n \t \\ \" \'
//   - return 0 signals success to the OS
//
// EXERCISE:
//   1. Change the program to print YOUR name instead of "Kiki"
//   2. Add a line that prints your age
//   3. Print a box using cout and \n:
//      ***********
//      *         *
//      *  Hi!    *
//      *         *
//      ***********
//   4. Compile and run: g++ -std=c++20 -o lesson1 lesson1_basics.cpp && ./lesson1
// =====================================================================
