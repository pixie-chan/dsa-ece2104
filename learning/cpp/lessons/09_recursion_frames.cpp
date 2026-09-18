// 09_recursion_frames.cpp
// Lecture 16 prep: what recursion actually costs in frames.
// Counts calls, measures the deepest stack depth, and prints the real frame
// addresses of your own lab exercises (factorial, Towers of Hanoi) so the
// "recursion is just a stack" claim has numbers behind it.
//
// Compile: g++ -Wall -Wextra -std=c++17 -O2 -o rec_frames 09_recursion_frames.cpp
#include <iostream>
#include <cstddef>
#include <cstdint>
#include <string>
using namespace std;

static long long g_calls = 0;
static int g_depth = 0, g_maxDepth = 0;
// addresses are kept as integers on purpose: storing a live frame's address in a
// global pointer is what -Wdangling-pointer exists to catch, and this probe only
// needs the NUMBER, printed while the frames are alive and gone afterwards.
static uintptr_t g_firstFrame = 0, g_deepestFrame = 0;

__attribute__((noinline)) static long long factorial(int n) {
    volatile int frameProbe = n;                 // keeps this frame identifiable
    g_calls++;
    g_depth++;
    if (g_depth == 1) g_firstFrame = (uintptr_t)&frameProbe;
    if (g_depth > g_maxDepth) { g_maxDepth = g_depth; g_deepestFrame = (uintptr_t)&frameProbe; }
    long long r = (n <= 1) ? 1 : n * factorial(n - 1);
    g_depth--;
    return r;
}

__attribute__((noinline)) static void hanoi(int n, char from, char to, char via, int &moves) {
    g_calls++;
    g_depth++;
    if (g_depth > g_maxDepth) g_maxDepth = g_depth;
    if (n == 1) { moves++; }
    else { hanoi(n - 1, from, via, to, moves); moves++; hanoi(n - 1, via, to, from, moves); }
    g_depth--;
}

int main() {
    cout << "=== factorial: calls, depth, frames ===\n";
    for (int n = 1; n <= 5; n++) {
        g_calls = 0; g_depth = 0; g_maxDepth = 0;
        long long v = factorial(n);
        cout << "  factorial(" << n << ") = " << v << " | recursive calls = " << g_calls
             << " | deepest nesting = " << g_maxDepth << " frames\n";
    }
    g_calls = 0; g_maxDepth = 0;
    long long v4 = factorial(4);
    (void)v4;
    cout << "  factorial(4) at its deepest: " << g_maxDepth << " calls on the stack at once\n";
    cout << "  its first frame at  0x" << hex << g_firstFrame << dec
         << " and the innermost at 0x" << hex << g_deepestFrame << dec << "\n";
    uintptr_t span = g_firstFrame - g_deepestFrame;
    cout << "  span between first and innermost frame = " << span << " bytes, about "
         << (span / (g_maxDepth ? g_maxDepth - 1 : 1)) << " bytes per frame\n";

    cout << "\n=== Towers of Hanoi: calls and moves ===\n";
    for (int n = 1; n <= 4; n++) {
        g_calls = 0; g_depth = 0; g_maxDepth = 0;
        int moves = 0;
        hanoi(n, 'A', 'C', 'B', moves);
        cout << "  n = " << n << " | moves = " << moves << " (2^" << n << " - 1 = " << ((1 << n) - 1)
             << ") | hanoi calls = " << g_calls << " | deepest nesting = " << g_maxDepth << " frames\n";
    }
    cout << "\n  measured on this machine, the same machine that runs your DAS-LAB experiments.\n";
    return 0;
}
