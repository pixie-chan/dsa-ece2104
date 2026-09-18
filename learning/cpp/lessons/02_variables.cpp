// =====================================================================
// LESSON 2: VARIABLES, DATA TYPES, INPUT/OUTPUT
// =====================================================================
// Variables are NAMED BOXES that hold data.
// Before using a variable, you must DECLARE it:
//
//   type name;              // declare (uninitialized — DON'T do this)
//   type name = value;      // declare + initialize (ALWAYS do this)
//
// C++ is STATICALLY TYPED: you must declare the type upfront.
// Unlike Python where x = 5 then x = "hello" works, in C++ the type
// is fixed once declared. This catches errors at compile time.
// =====================================================================

#include <iostream>
#include <string>
using namespace std;

int main()
{
    // --- BASIC DATA TYPES ---

    // INTEGERS (whole numbers, no decimal point)
    int age = 19;              // typical int (at least 16 bits, usually 32)
    int year = 2026;
    int negative = -42;
    long bigNumber = 9000000000L;     // bigger range integer

    // FLOATING POINT (numbers with decimal points)
    double pi = 3.14159;       // double = double precision (64-bit) — USE THIS
    float gravity = 9.8f;      // float = single precision (32-bit) — rarely used
    double price = 19.99;

    // CHARACTER (single character, single quotes)
    char grade = 'A';
    char newline = '\n';

    // BOOLEAN (true/false)
    bool isStudent = true;
    bool isRaining = false;

    // STRING (sequence of characters, double quotes)
    string name = "Ayush";
    string city = "Raipur";

    // --- PRINTING VARIABLES ---
    cout << "Name: " << name << endl;
    cout << "Age: " << age << endl;
    cout << "City: " << city << endl;
    cout << "Pi: " << pi << endl;
    cout << "Grade: " << grade << endl;
    cout << "Is student: " << isStudent << endl;  // prints 1 for true, 0 for false
    cout << boolalpha;  // makes bools print as "true"/"false" instead of 1/0
    cout << "Is student: " << isStudent << endl;
    cout << "Is raining: " << isRaining << endl;

    // --- ARITHMETIC OPERATORS ---
    int a = 10, b = 3;
    cout << "\n--- Arithmetic ---" << endl;
    cout << "a + b = " << a + b << endl;     // addition       = 13
    cout << "a - b = " << a - b << endl;     // subtraction    = 7
    cout << "a * b = " << a * b << endl;     // multiplication = 30
    cout << "a / b = " << a / b << endl;     // INTEGER division = 3 (not 3.33!)
    cout << "a % b = " << a % b << endl;     // modulo (remainder) = 1

    // IMPORTANT: integer division truncates!
    // 10 / 3 = 3, NOT 3.333
    // To get decimal results, at least one operand must be a double:
    cout << "10.0 / 3 = " << 10.0 / 3 << endl;   // = 3.33333

    // --- COMPOUND ASSIGNMENT ---
    int x = 5;
    x += 3;    // x = x + 3 = 8
    x -= 2;    // x = x - 2 = 6
    x *= 4;    // x = x * 4 = 24
    x /= 5;    // x = x / 5 = 4
    x %= 3;    // x = x % 3 = 1
    cout << "\nx after operations: " << x << endl;

    // --- INCREMENT/DECREMENT ---
    int counter = 0;
    counter++;   // counter = 1
    counter++;   // counter = 2
    counter--;   // counter = 1
    cout << "Counter: " << counter << endl;

    // --- COMPARISON OPERATORS (return bool) ---
    cout << "\n--- Comparisons ---" << endl;
    cout << "(5 > 3) = " << (5 > 3) << endl;       // true
    cout << "(5 < 3) = " << (5 < 3) << endl;       // false
    cout << "(5 == 5) = " << (5 == 5) << endl;     // true
    cout << "(5 != 5) = " << (5 != 5) << endl;     // false
    cout << "(5 >= 5) = " << (5 >= 5) << endl;     // true
    cout << "(5 <= 4) = " << (5 <= 4) << endl;     // false

    // --- LOGICAL OPERATORS ---
    bool hasLicense = true;
    bool hasCar = false;
    cout << "\n--- Logical ---" << endl;
    cout << "Can drive: " << (hasLicense && hasCar) << endl;  // AND: both must be true
    cout << "Can rent:  " << (hasLicense || hasCar) << endl;  // OR: either is true
    cout << "No car:    " << (!hasCar) << endl;                // NOT: flips the bool

    // --- TAKING INPUT FROM THE USER ---
    // cin = "character input" — reads from keyboard
    // >>   = extraction operator (pulls data from cin into variable)

    cout << "\n--- INPUT DEMO ---" << endl;

    int userAge;
    string userName;

    cout << "Enter your name: ";
    cin >> userName;           // NOTE: cin >> stops at whitespace!
                              // "Ayush Kharwar" would only store "Ayush"

    cout << "Enter your age: ";
    cin >> userAge;

    cout << "Hi " << userName << "! You are " << userAge << " years old." << endl;

    // --- getline: reading a full line with spaces ---
    cin.ignore(10000, '\n');   // clear the leftover newline from cin >> userAge
    string fullName;
    cout << "Enter your FULL name: ";
    getline(cin, fullName);   // getline reads the ENTIRE line including spaces
    cout << "Your full name is: " << fullName << endl;

    // --- const: variables that can't change ---
    const int DAYS_IN_WEEK = 7;
    const double GRAVITY = 9.8;
    // DAYS_IN_WEEK = 8;  // ERROR: can't modify a const

    cout << "Days in a week: " << DAYS_IN_WEEK << endl;

    // --- TYPE INFERENCE (auto) ---
    // Modern C++ lets the compiler figure out the type:
    auto score = 95;           // compiler deduces int
    auto gpa = 3.7;            // compiler deduces double
    auto school = "MIT";       // compiler deduces const char* (NOT string!)
    auto message = string("hi"); // explicitly make it a string

    cout << "score = " << score << ", gpa = " << gpa << endl;

    return 0;
}

// =====================================================================
// WHAT YOU LEARNED:
//   - Declare: type name = value;
//   - Types: int, double, float, char, bool, string, long
//   - ALWAYS initialize variables (uninitialized = garbage value)
//   - Integer division truncates (10/3 = 3, not 3.33)
//   - Operators: + - * / % += -= *= /= ++ -- == != > < >= <= && || !
//   - cin >> reads input, getline reads full lines with spaces
//   - const = can't change after initialization
//   - auto = compiler figures out the type
//
// EXERCISE:
//   1. Write a program that asks the user for length and width,
//      then calculates and prints the area of a rectangle
//   2. Write a program that converts Celsius to Fahrenheit
//      (F = C * 9/5 + 32)
//   3. Write a program that asks for your name, age, and favorite
//      number, then prints them all in a nice sentence
// =====================================================================
