// =====================================================================
// LESSON 5: ARRAYS, VECTORS, AND STRINGS — THE CONTAINERS YOU'LL LIVE IN
// =====================================================================
// In CP and LeetCode, 90% of problems are "read data into a container,
// do something to it." vector + string are your bread and butter.
//
// The family tree:
//
//   C-style array   int a[5];        fixed size, dumb, dangerous.
//                                    know it exists, then avoid it.
//   vector<int>     resizable array  ← this is "Python list" / "Java ArrayList"
//   string          array of chars with superpowers
//
// COMPILE:   g++ -std=c++20 -Wall -o l5 05_arrays_vectors_strings.cpp
// RUN:       ./l5
// =====================================================================

#include <bits/stdc++.h>
using namespace std;

int main() {

    // --- 1. C-STYLE ARRAY (the dinosaur) -----------------------------
    int arr[5] = {10, 20, 30, 40, 50};
    cout << "arr[2] = " << arr[2] << "\n";
    // TRAP: arr out of bounds is undefined behavior. No error, no crash,
    // just silent garbage or a random segfault hours later. vector fixes this.

    // --- 2. VECTOR: THE RESIZABLE ARRAY ------------------------------
    // three ways to create one:
    vector<int> v1 = {3, 1, 4, 1, 5};      // from a list
    vector<int> v2(5, 0);                  // {0,0,0,0,0}  (size, fill)
    vector<int> v3;                        // empty, grows with push_back

    v3.push_back(7);                       // append  → like Python's .append()
    v3.push_back(8);
    v3.push_back(9);

    cout << "v3 size = " << v3.size() << ", last = " << v3.back() << "\n";
    v3.pop_back();                         // remove last → like Python's .pop()
    cout << "after pop, last = " << v3.back() << "\n";

    // indexing works like arrays/Python lists (0-based):
    cout << "v1[0]=" << v1[0] << " v1[4]=" << v1[4] << "\n";

    // --- 3. LOOPING OVER A VECTOR ------------------------------------
    // way 1: index loop (when you NEED the index)
    for (int i = 0; i < (int)v1.size(); i++) cout << v1[i] << " ";
    cout << "\n";

    // way 2: range-for (when you just need values) — like Python's `for x in list`
    for (int x : v1) cout << x << " ";
    cout << "\n";

    // TRAP: the unsigned size problem.
    // v1.size() is unsigned. `i < v1.size() - 1` when v1 is EMPTY:
    // 0u - 1 wraps around to a HUGE number → loop runs wild → crash.
    // Fix: cast, or compute the bound as a signed int first.
    // (int)v1.size()  ← do this whenever comparing with int i

    // --- 4. 2D VECTORS (grids / matrices) ----------------------------
    vector<vector<int>> grid(3, vector<int>(4, 0));  // 3 rows x 4 cols of zeros
    grid[1][2] = 99;
    cout << "grid[1][2] = " << grid[1][2] << "\n";
    // picture:
    //   row0: 0  0  0  0
    //   row1: 0  0 99  0
    //   row2: 0  0  0  0

    // --- 5. SORTING (free money) -------------------------------------
    vector<int> messy = {5, 2, 9, 1, 7};
    sort(messy.begin(), messy.end());              // ascending
    for (int x : messy) cout << x << " ";
    cout << "\n";
    sort(messy.rbegin(), messy.rend());            // descending (reversed)
    for (int x : messy) cout << x << " ";
    cout << "\n";

    // --- 6. STRINGS ---------------------------------------------------
    string s = "leetcode";
    cout << "len = " << s.length() << ", s[0] = " << s[0] << "\n";

    string t = "code" + string("forces");  // concat: string + string works
    cout << t << "\n";

    // substr(start, length) — like Python slicing but with (start, LENGTH)
    string sub = s.substr(0, 4);
    cout << "substr(0,4) = " << sub << "\n";

    // find: returns index of first occurrence, or string::npos if absent
    if (s.find("et") != string::npos) cout << "'et' at index " << s.find("et") << "\n";

    // comparing strings with == compares CONTENT (unlike C arrays)
    if (string("abc") == string("abc")) cout << "equal content\n";

    // strings are sortable too:
    string jumble = "dcba";
    sort(jumble.begin(), jumble.end());
    cout << "sorted jumble = " << jumble << "\n";

    // --- 7. WORKED EXAMPLE: PALINDROME CHECK --------------------------
    // The classic LeetCode warmup. Two pointers walk inward:
    //
    //    r  a  c  e  c  a  r
    //    L              R    → 'r'=='r' ✓ move both
    //       L        R       → 'a'=='a' ✓
    //          L  R          → 'c'=='c' ✓
    //          L>R? stop     → palindrome!
    string word = "racecar";
    int L = 0, R = (int)word.size() - 1;
    bool isPal = true;
    while (L < R) {
        if (word[L] != word[R]) { isPal = false; break; }
        L++; R--;
    }
    cout << word << " palindrome? " << (isPal ? "yes" : "no") << "\n";

    // =================================================================
    // CHEAT SHEET (pin this to your brain):
    //   push_back(x)  append        pop_back()   remove last
    //   v.size()      length        v.back()     last element
    //   v.front()     first         v.empty()    is it empty?
    //   sort(v.begin(), v.end())                ascending
    //   (int)v.size()  CAST IT when comparing with int
    //   s.substr(i, LEN)  s.find(x)  s[i]  s.length()
    // =================================================================
    return 0;
}
