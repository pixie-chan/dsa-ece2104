// =====================================================================
// LESSON 3: CONTROL FLOW — MAKING DECISIONS AND REPEATING ACTIONS
// =====================================================================
// This is where programming gets INTERESTING. So far our programs
// run top-to-bottom doing the same thing every time. Control flow
// lets the program MAKE DECISIONS and REPEAT things.
//
// Two main types:
//   1. CONDITIONALS: if / else if / else — choose between paths
//   2. LOOPS: for / while / do-while — repeat code
// =====================================================================

#include <iostream>
#include <string>
using namespace std;

int main()
{
    // ===========================================================
    // PART 1: IF / ELSE IF / ELSE
    // ===========================================================
    // The most basic decision-making tool.
    //
    //   if (condition) {
    //       // runs if condition is true
    //   } else if (anotherCondition) {
    //       // runs if first was false AND this is true
    //   } else {
    //       // runs if ALL above were false
    //   }

    int score = 85;

    if (score >= 90) {
        cout << "Grade: A" << endl;
    } else if (score >= 80) {
        cout << "Grade: B" << endl;
    } else if (score >= 70) {
        cout << "Grade: C" << endl;
    } else if (score >= 60) {
        cout << "Grade: D" << endl;
    } else {
        cout << "Grade: F" << endl;
    }

    // Only the FIRST matching branch runs. The rest are skipped.
    // score = 85 → prints "Grade: B" and skips everything else.

    // --- NESTED IF ---
    bool hasKey = true;
    bool doorIsLocked = true;

    if (doorIsLocked) {
        if (hasKey) {
            cout << "You unlock the door and enter." << endl;
        } else {
            cout << "The door is locked and you have no key!" << endl;
        }
    } else {
        cout << "You open the unlocked door." << endl;
    }

    // --- TERNARY OPERATOR (shorthand if/else) ---
    // condition ? valueIfTrue : valueIfFalse
    int age = 19;
    string status = (age >= 18) ? "adult" : "minor";
    cout << "Status: " << status << endl;

    // Same as writing:
    // string status;
    // if (age >= 18) status = "adult";
    // else status = "minor";

    // --- SWITCH STATEMENT ---
    // Good for comparing ONE value against many possible exact matches.
    // Works with int, char, enum — NOT string or float.
    int day = 3;

    switch (day) {
        case 1:
            cout << "Monday" << endl;
            break;    // IMPORTANT: break exits the switch!
        case 2:
            cout << "Tuesday" << endl;
            break;
        case 3:
            cout << "Wednesday" << endl;
            break;
        case 4:
            cout << "Thursday" << endl;
            break;
        case 5:
            cout << "Friday" << endl;
            break;
        case 6:
        case 7:         // fall-through: 6 and 7 share the same code
            cout << "Weekend!" << endl;
            break;
        default:        // runs if no case matches (like "else")
            cout << "Invalid day" << endl;
    }
    // If you forget "break", execution falls through to the next case!
    // This is a famous source of bugs. Always use break (unless you want fall-through).

    // ===========================================================
    // PART 2: LOOPS — REPEATING CODE
    // ===========================================================

    // --- FOR LOOP ---
    // Used when you know HOW MANY TIMES to repeat.
    //
    //   for (init; condition; update) {
    //       // body
    //   }
    //
    // init       → runs ONCE before the loop starts
    // condition  → checked BEFORE each iteration (if false, loop ends)
    // update     → runs AFTER each iteration

    cout << "\n--- Counting 1 to 5 ---" << endl;
    for (int i = 1; i <= 5; i++) {
        cout << i << " ";
    }
    cout << endl;

    // Count down:
    cout << "\n--- Countdown ---" << endl;
    for (int i = 5; i >= 1; i--) {
        cout << i << " ";
    }
    cout << endl;

    // Sum of 1 to 10:
    int sum = 0;
    for (int i = 1; i <= 10; i++) {
        sum += i;     // sum = sum + i
    }
    cout << "Sum of 1-10: " << sum << endl;   // 55

    // --- WHILE LOOP ---
    // Used when you DON'T know how many times to repeat —
    // you just know WHEN TO STOP (the condition becomes false).
    //
    //   while (condition) {
    //       // body
    //   }

    cout << "\n--- While loop: counting down ---" << endl;
    int fuel = 5;
    while (fuel > 0) {
        cout << "Fuel: " << fuel << " — still running" << endl;
        fuel--;     // CRITICAL: must change the condition variable
                    // or the loop runs FOREVER (infinite loop!)
    }
    cout << "Out of fuel!" << endl;

    // --- DO-WHILE LOOP ---
    // Like while, but the condition is checked AFTER the body.
    // This means the body ALWAYS runs AT LEAST ONCE.
    //
    //   do {
    //       // body
    //   } while (condition);

    cout << "\n--- Do-while demo ---" << endl;
    int choice;
    do {
        cout << "Enter 1 to continue, 0 to quit: ";
        cin >> choice;
    } while (choice != 0 && choice != 1);
    cout << "You entered: " << choice << endl;

    // --- BREAK AND CONTINUE ---
    // break    → exits the loop IMMEDIATELY (skips rest of iterations)
    // continue → skips the REST of current iteration, goes to next

    cout << "\n--- Break demo (stop at 3) ---" << endl;
    for (int i = 1; i <= 10; i++) {
        if (i == 3) break;     // loop stops when i reaches 3
        cout << i << " ";
    }
    cout << endl;              // prints: 1 2

    cout << "\n--- Continue demo (skip even numbers) ---" << endl;
    for (int i = 1; i <= 10; i++) {
        if (i % 2 == 0) continue;  // skip even numbers
        cout << i << " ";
    }
    cout << endl;              // prints: 1 3 5 7 9

    // --- NESTED LOOPS ---
    // A loop inside a loop. The inner loop runs COMPLETELY
    // for EACH iteration of the outer loop.
    cout << "\n--- Multiplication table (3x3) ---" << endl;
    for (int row = 1; row <= 3; row++) {
        for (int col = 1; col <= 3; col++) {
            cout << (row * col) << "\t";
        }
        cout << endl;
    }

    // ===========================================================
    // PUTTING IT TOGETHER: A SIMPLE GUESSING GAME
    // ===========================================================
    cout << "\n--- Guessing Game ---" << endl;

    int secret = 7;
    int guess;
    int attempts = 0;

    cout << "Guess a number between 1 and 10!" << endl;

    while (true) {     // infinite loop until we break
        cout << "Your guess: ";
        cin >> guess;
        attempts++;

        if (guess == secret) {
            cout << "Correct! You got it in " << attempts << " tries!" << endl;
            break;
        } else if (guess < secret) {
            cout << "Too low! Try again." << endl;
        } else {
            cout << "Too high! Try again." << endl;
        }
    }

    return 0;
}

// =====================================================================
// WHAT YOU LEARNED:
//   - if / else if / else for branching
//   - Ternary: condition ? trueValue : falseValue
//   - switch for matching one value against many cases
//   - for loop: when you know the count
//   - while loop: when you know the stop condition
//   - do-while: runs at least once
//   - break: exit loop, continue: skip iteration
//   - Nested loops: inner runs fully for each outer iteration
//
// EXERCISE:
//   1. FizzBuzz: Print numbers 1-50. For multiples of 3 print "Fizz",
//      multiples of 5 print "Buzz", multiples of both print "FizzBuzz"
//   2. Write a program that prints a pyramid of stars:
//          *
//         ***
//        *****
//       *******
//   3. Write a simple calculator: ask for two numbers and an operator
//      (+, -, *, /), then print the result using if/else or switch
// =====================================================================
