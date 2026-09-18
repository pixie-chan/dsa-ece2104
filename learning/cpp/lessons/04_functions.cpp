// =====================================================================
// LESSON 4: FUNCTIONS — THINKING IN BUILDING BLOCKS
// =====================================================================
// A function is a NAMED, REUSABLE block of code that does ONE job.
//
// Why functions?
//   - REUSABILITY: write once, call many times
//   - ORGANIZATION: break big problems into small pieces
//   - READABILITY: main() becomes a high-level outline
//   - TESTING: test each function independently
//
// Function anatomy:
//   returnType functionName(parameters) {
//       // body
//       return value;    // if returnType is not void
//   }
// =====================================================================

#include <iostream>
#include <string>
using namespace std;

// --- FUNCTION DECLARATIONS (prototypes) ---
// You can declare a function (tell the compiler it exists) and
// define it later. This lets main() use functions defined below it.
// The compiler reads top-to-bottom, so it needs to know about
// functions before they're called.

int add(int a, int b);              // returns int, takes two ints
double circleArea(double radius);   // returns double, takes double
void printGreeting(string name);    // returns nothing (void), takes string
bool isEven(int number);            // returns bool, takes int
int factorial(int n);               // recursive function (calls itself!)
double celsiusToFahrenheit(double celsius); // defined after main (below)

// =====================================================================
// MAIN FUNCTION — the "outline" of your program
// =====================================================================
int main()
{
    // Calling functions is simple: name(arguments)

    // --- BASIC FUNCTION CALLS ---
    int result = add(5, 3);
    cout << "5 + 3 = " << result << endl;
    cout << "10 + 20 = " << add(10, 20) << endl;   // call inline

    // --- VOID FUNCTION (doesn't return anything) ---
    printGreeting("Ayush");
    printGreeting("Kiki");

    // --- BOOLEAN FUNCTION ---
    cout << "Is 4 even? " << boolalpha << isEven(4) << endl;
    cout << "Is 7 even? " << isEven(7) << endl;

    // --- FUNCTION WITH CALCULATION ---
    double area = circleArea(5.0);
    cout << "Area of circle with radius 5: " << area << endl;

    // --- RECURSION (a function calling itself) ---
    cout << "\n--- Recursion: Factorial ---" << endl;
    cout << "0! = " << factorial(0) << endl;   // 1
    cout << "1! = " << factorial(1) << endl;   // 1
    cout << "5! = " << factorial(5) << endl;   // 120
    cout << "6! = " << factorial(6) << endl;   // 720

    // --- USING FUNCTIONS TO BUILD SOMETHING ---
    cout << "\n--- Temperature Converter ---" << endl;
    for (int c = 0; c <= 100; c += 20) {
        cout << c << "C = " << celsiusToFahrenheit(c) << "F" << endl;
    }

    return 0;
}

// =====================================================================
// FUNCTION DEFINITIONS — the actual implementation
// =====================================================================

// Simple: add two integers and return the sum
int add(int a, int b) {
    return a + b;
}

// Void function: does something but doesn't return a value
void printGreeting(string name) {
    cout << "Hello, " << name << "! Welcome to C++!" << endl;
    // no return statement needed for void
    // (or you can write "return;" to exit early)
}

// Boolean function: returns true or false
bool isEven(int number) {
    return (number % 2 == 0);
    // This is equivalent to:
    //   if (number % 2 == 0) return true;
    //   else return false;
    // But the short version is cleaner — the expression already IS a bool
}

// Function with a calculation (using M_PI from cmath would be better,
// but we'll hardcode pi here for simplicity)
double circleArea(double radius) {
    const double PI = 3.14159265359;
    return PI * radius * radius;
}

// RECURSION: a function that calls itself
// Factorial: n! = n × (n-1) × (n-2) × ... × 1
//   5! = 5 × 4 × 3 × 2 × 1 = 120
//
// Recursive thinking:
//   Base case:    0! = 1 (stop recursing)
//   Recursive case: n! = n × (n-1)!
//
// Every recursive function needs:
//   1. A BASE CASE (when to stop) — otherwise infinite recursion → crash
//   2. A RECURSIVE CASE (calls itself with a smaller problem)
int factorial(int n) {
    // Base case: if n is 0, factorial is 1
    if (n <= 1) {
        return 1;
    }
    // Recursive case: n! = n × (n-1)!
    return n * factorial(n - 1);
}

// HOW RECURSION WORKS (mental model for factorial(5)):
//
//   factorial(5)
//     = 5 * factorial(4)
//     = 5 * (4 * factorial(3))
//     = 5 * (4 * (3 * factorial(2)))
//     = 5 * (4 * (3 * (2 * factorial(1))))
//     = 5 * (4 * (3 * (2 * 1)))           ← base case reached!
//     = 5 * (4 * (3 * 2))
//     = 5 * (4 * 6)
//     = 5 * 24
//     = 120
//
// The calls STACK UP until the base case, then UNWIND.

// --- FUNCTION DECLARED AFTER main ---
// This works because we DECLARED it above main() as a prototype.
// If we hadn't, the compiler would error: "celsiusToFahrenheit not declared"
double celsiusToFahrenheit(double celsius) {
    return celsius * 9.0 / 5.0 + 32.0;
}

// =====================================================================
// WHAT YOU LEARNED:
//   - Functions = named, reusable blocks that do one job
//   - returnType name(parameters) { body; return value; }
//   - void = no return value
//   - Prototypes let you define functions after main()
//   - Parameters are COPIES of the arguments (pass by value)
//   - Recursion: function calls itself (needs base case!)
//
// KEY MENTAL MODEL:
//   Think of each function as a MACHINE: you feed it inputs (parameters),
//   it does something, and gives you back an output (return value).
//   "add(5, 3)" → machine takes 5 and 3 → returns 8
//
// EXERCISE:
//   1. Write a function `int max(int a, int b)` that returns the larger number
//   2. Write a function `bool isPrime(int n)` that checks if a number is prime
//   3. Write a function `void printTriangle(int height)` that prints a star
//      triangle of the given height using loops
//   4. Write a recursive function to compute the nth Fibonacci number
//      Fib(0)=0, Fib(1)=1, Fib(n) = Fib(n-1) + Fib(n-2)
// =====================================================================
