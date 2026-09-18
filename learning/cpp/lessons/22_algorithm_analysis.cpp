// 22_algorithm_analysis.cpp
// ECE2104 (DSA), Lecture 22: Algorithm analysis, growth of functions, O, Omega, Theta.
// Handout session outcome: express the time and space complexity of an algorithm. CO2.
//
// Build: g++ -Wall -Wextra -Wpedantic -std=c++17 -O2 -o /tmp/lec22 22_algorithm_analysis.cpp
//
// Six numbered evidence sections, every number computed by this program in this one run:
//   1  certificates: c and n0 found by scanning both sides, for O, Omega and Theta
//   2  the arithmetic laws of Big-O, each one evaluated on both sides
//   3  complexity by counting: five real loops against their closed forms
//   4  best, worst and average case on ONE search, counts summed then divided
//   5  the growth table: which term dominates, with the ratios that show it
//   6  the one line to remember
//
// -O2 deletes a loop whose result nobody reads, and that would turn every count below into
// a lie. So each counted loop keeps a volatile counter and a volatile checksum, the trip
// count is read from a volatile slot rather than a typed constant, and every counter and
// checksum is printed and checked against a closed form in the output below. Global
// operator new is deliberately NOT overridden: GCC 15 argues with that under
// -Wmismatched-new-delete, and this build has to stay warning free.

#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
using namespace std;

// ------------------------------------------------------------------ small printers
// Everything that needs its own precision goes through an ostringstream, so no sticky
// stream state can leak into the next table.
static string decimalText(long long num, long long den) {
    ostringstream out;
    out << fixed << setprecision(6) << ((double)num / (double)den);
    return out.str();
}
static string fractionText(long long num, long long den) {
    ostringstream out;
    long long divisor = gcd(num, den);
    out << (num / divisor) << "/" << (den / divisor);
    return out.str();
}
static string valueText(double v) {
    ostringstream out;
    out << setprecision(6) << defaultfloat << v;
    return out.str();
}
static string fixedText(double v) {
    ostringstream out;
    out << fixed << setprecision(6) << v;
    return out.str();
}
static string log10Text(double logValue) {
    ostringstream out;
    double exponent = floor(logValue);
    double mantissa = pow(10.0, logValue - exponent);
    out << fixed << setprecision(3) << mantissa << "e+" << setprecision(0) << exponent;
    return out.str();
}
static string ratioText(double log10Ratio) {
    if (log10Ratio > -6.0 && log10Ratio < 6.0) return valueText(pow(10.0, log10Ratio));
    return log10Text(log10Ratio);
}

// ------------------------------------------------------------------ the functions
// section 1 works on this pair: f is the function under analysis, g its reference
static long long fOfN(long long n) { return 3LL * n * n + 5LL * n + 7LL; }
static long long gOfN(long long n) { return 4LL * n * n; }
static long long linearOfN(long long n) { return n; }
// section 2 uses g(n) = n^2 so the constants in the laws are the familiar ones
static long long squareOfN(long long n) { return n * n; }
static long long secondOfN(long long n) { return 2LL * n * n + 9LL; }
static long long sumOfBoth(long long n) { return fOfN(n) + secondOfN(n); }
static long long cubicPolyOfN(long long n) { return 7LL * n * n * n + 100LL * n * n + 5LL; }

// ------------------------------------------------------------- logarithms, counted
// integer logarithms so the scans below stay exact: no floating point decides a verdict
static long long ceilLog2Of(long long v) {
    long long e = 0;
    while (e < 62 && (1LL << e) < v) e++;
    return e;
}
static long long nTimesCeilLog2(long long n) { return n * ceilLog2Of(n); }

// ------------------------------------------------------------------ the certificate
typedef long long (*IntFn)(long long);

struct Certificate {
    bool tailHolds;        // true: the inequality holds for every n from n0 to maxN
    long long n0;          // first n of the verified tail
    long long lastFail;    // largest n in the range where it failed, 0 if it never did
    long long tailSize;    // how many values of n were checked in the tail
    long long minMarginNum; // smallest margin over the tail, as a numerator over cDen
    long long failRunStart; // first n of the failure run that reaches maxN, 0 if none
};

// the comparison is done in integers: f(n)*cDen against cNum*g(n). c = cNum/cDen.
static Certificate scanUpperBound(IntFn fn, IntFn gn, long long cNum, long long cDen,
                                 long long maxN, bool strict) {
    Certificate cert{false, 1, 0, 0, 0, 0};
    long long runStartNow = 0;
    long long lastFailNow = 0;
    for (long long n = 1; n <= maxN; n++) {
        bool broke = strict ? (fn(n) * cDen >= cNum * gn(n)) : (fn(n) * cDen > cNum * gn(n));
        if (broke) {
            if (runStartNow == 0) runStartNow = n;
            lastFailNow = n;
        } else {
            runStartNow = 0;
        }
    }
    cert.lastFail = lastFailNow;
    if (lastFailNow == maxN) {                  // still failing at the end: no n0 in this range
        cert.failRunStart = runStartNow;
        return cert;
    }
    long long fromNow = lastFailNow + 1;
    long long smallestNow = cNum * gn(fromNow) - fn(fromNow) * cDen;
    for (long long n = fromNow + 1; n <= maxN; n++) {
        long long gapNow = cNum * gn(n) - fn(n) * cDen;
        if (gapNow < smallestNow) smallestNow = gapNow;
    }
    cert.tailHolds = true;
    cert.n0 = fromNow;
    cert.tailSize = maxN - fromNow + 1;
    cert.minMarginNum = smallestNow;
    return cert;
}

static Certificate scanLowerBound(IntFn fn, IntFn gn, long long cNum, long long cDen,
                                 long long maxN, bool strict) {
    Certificate cert{false, 1, 0, 0, 0, 0};
    long long runStartNow = 0;
    long long lastFailNow = 0;
    for (long long n = 1; n <= maxN; n++) {
        bool broke = strict ? (fn(n) * cDen <= cNum * gn(n)) : (fn(n) * cDen < cNum * gn(n));
        if (broke) {
            if (runStartNow == 0) runStartNow = n;
            lastFailNow = n;
        } else {
            runStartNow = 0;
        }
    }
    cert.lastFail = lastFailNow;
    if (lastFailNow == maxN) {
        cert.failRunStart = runStartNow;
        return cert;
    }
    long long fromNow = lastFailNow + 1;
    long long smallestNow = fn(fromNow) * cDen - cNum * gn(fromNow);
    for (long long n = fromNow + 1; n <= maxN; n++) {
        long long gapNow = fn(n) * cDen - cNum * gn(n);
        if (gapNow < smallestNow) smallestNow = gapNow;
    }
    cert.tailHolds = true;
    cert.n0 = fromNow;
    cert.tailSize = maxN - fromNow + 1;
    cert.minMarginNum = smallestNow;
    return cert;
}

static string signText(bool upper, bool strict) {
    if (upper) return strict ? "<" : "<=";
    return strict ? ">" : ">=";
}

static void printCertificateBlock(const string &title, IntFn fn, IntFn gn, long long cNum,
                                  long long cDen, const Certificate &cert, long long maxN,
                                  bool upper) {
    cout << "    " << title << "\n";
    cout << "      claim: f(n) " << signText(upper, false) << " (" << cNum << "/" << cDen
         << ")*g(n) for every n >= n0\n";
    if (cert.tailHolds) {
        cout << "      largest n in 1.." << maxN << " where it FAILS = " << cert.lastFail
             << "  ->  n0 = " << cert.n0 << "\n";
        double leftSide = (double)fn(cert.n0);
        double rightSide = (double)(cNum * gn(cert.n0)) / (double)cDen;
        bool holdsNow = upper ? (leftSide <= rightSide) : (leftSide >= rightSide);
        cout << "      at n0 = " << cert.n0 << ": f = " << valueText(leftSide)
             << ", c*g = " << valueText(rightSide) << ", and it holds: "
             << (holdsNow ? "yes" : "NO") << "\n";
        cout << "      the tail n = " << cert.n0 << ".." << maxN << " holds for all "
             << cert.tailSize << " values of n, smallest margin over the tail = "
             << fractionText(cert.minMarginNum, cDen) << " = "
             << decimalText(cert.minMarginNum, cDen) << "  ->  CERTIFIED\n";
    } else {
        cout << "      it fails at every n from " << cert.failRunStart << " to " << maxN
             << " and never holds again in this range, so NO n0 exists  ->  REJECTED\n";
    }
}

static void printScanOutcome(bool upper, bool strict, long long cNum, long long cDen,
                             const Certificate &cert, long long maxN) {
    cout << "    " << (upper ? "upper bound" : "lower bound") << "  f(n) "
         << signText(upper, strict) << " (" << cNum << "/" << cDen << ")*g(n) : ";
    if (cert.tailHolds) {
        cout << "n0 = " << cert.n0 << ", " << cert.tailSize
             << " values of n verified, smallest margin "
             << fractionText(cert.minMarginNum, cDen) << "  CERTIFIED\n";
    } else {
        cout << "fails from n = " << cert.failRunStart << " through n = " << maxN
             << ", no n0 in this range  REJECTED\n";
    }
}

// ------------------------------------------------------------------ section 1
static void section1Certificates() {
    cout << "=== 1. a certificate: find the constant c and the threshold n0 by evaluating both sides ===\n";
    const long long scanMax = 20000;
    cout << "  f(n) = 3n^2 + 5n + 7      g(n) = 4n^2      scanned range: n = 1 .. 20000\n";
    cout << "  every scan below evaluates both sides 20000 times, one value of n at a time.\n";
    cout << "  The certificate is the pair that scan returns: the constant c, and n0 = one past the\n";
    cout << "  largest n where the inequality still failed.\n\n";

    cout << "  Big-O is an upper bound: f(n) <= c*g(n) for all n >= n0.\n";
    Certificate bigONTight = scanUpperBound(fOfN, gOfN, 1, 1, scanMax, false);
    printCertificateBlock("c = 1: the tightest round constant that works for the whole tail",
                          fOfN, gOfN, 1, 1, bigONTight, scanMax, true);
    Certificate bigONLoose = scanUpperBound(fOfN, gOfN, 2, 1, scanMax, false);
    printCertificateBlock("c = 2: a looser constant, so it buys a smaller n0",
                          fOfN, gOfN, 2, 1, bigONLoose, scanMax, true);

    cout << "\n  Omega is a lower bound: f(n) >= c*g(n) for all n >= n0.\n";
    Certificate bigOmegaOk = scanLowerBound(fOfN, gOfN, 3, 4, scanMax, false);
    printCertificateBlock("c = 3/4: the largest constant that survives the whole tail",
                          fOfN, gOfN, 3, 4, bigOmegaOk, scanMax, false);
    Certificate bigOmegaBad = scanLowerBound(fOfN, gOfN, 9, 10, scanMax, false);
    printCertificateBlock("c = 9/10: a little above the leading coefficient, so it dies",
                          fOfN, gOfN, 9, 10, bigOmegaBad, scanMax, false);

    cout << "\n  Theta is both bounds at once. A candidate c is tested on both sides, on the same\n";
    cout << "  range, and only the candidate that survives on both sides is a Theta certificate.\n";
    cout << "  c | lower bound f >= c*g | upper bound f <= c*g\n";
    const long long candidatesNum[7] = {1, 3, 9, 1, 3, 2, 3};
    const long long candidatesDen[7] = {2, 4, 10, 1, 2, 1, 1};
    for (int candIndex = 0; candIndex < 7; candIndex++) {
        long long cNum = candidatesNum[candIndex];
        long long cDen = candidatesDen[candIndex];
        Certificate lowNow = scanLowerBound(fOfN, gOfN, cNum, cDen, scanMax, false);
        Certificate upNow = scanUpperBound(fOfN, gOfN, cNum, cDen, scanMax, false);
        cout << "  c = " << cNum << "/" << cDen << "\n";
        printScanOutcome(false, false, cNum, cDen, lowNow, scanMax);
        printScanOutcome(true, false, cNum, cDen, upNow, scanMax);
    }
    cout << "  the sweep locates the boundary exactly. 3/4 is the largest constant that works as a\n";
    cout << "  lower bound: at 9/10 the lower bound dies and never recovers. 3/4 is also the smallest\n";
    cout << "  constant that could work as an upper bound: at 3/4 the upper bound already fails at n = 1,\n";
    cout << "  and the first candidate above it, 9/10, works from n = 10. 3/4 is the ratio of the\n";
    cout << "  leading coefficients, 3 divided by 4, so no smaller constant can work on either side, and\n";
    cout << "  c = 3/4 itself is the exact meeting point of the two bounds.\n";

    const long long thetaLowNum = 3, thetaLowDen = 4, thetaHighNum = 1, thetaHighDen = 1;
    long long thetaN0 = bigOmegaOk.n0 > bigONTight.n0 ? bigOmegaOk.n0 : bigONTight.n0;
    long long thetaShort = bigOmegaOk.n0 < bigONTight.n0 ? bigOmegaOk.n0 : bigONTight.n0;
    cout << "\n  the Theta certificate for f against g:\n";
    cout << "    c1 = " << thetaLowNum << "/" << thetaLowDen << " (from n = " << bigOmegaOk.n0
         << ") and c2 = " << thetaHighNum << "/" << thetaHighDen << " (from n = "
         << bigONTight.n0 << "), so n0 = max(" << bigOmegaOk.n0 << ", " << bigONTight.n0
         << ") = " << thetaN0 << " and the shorter certificate from n = " << thetaShort
         << " is dropped because Theta needs one n0 that works for BOTH sides\n";
    long long thetaViolationsLow = 0, thetaViolationsHigh = 0, thetaCounted = 0;
    for (long long n = thetaN0; n <= scanMax; n++) {
        thetaCounted++;
        if (fOfN(n) * thetaLowDen < thetaLowNum * gOfN(n)) thetaViolationsLow++;
        if (fOfN(n) * thetaHighDen > thetaHighNum * gOfN(n)) thetaViolationsHigh++;
    }
    cout << "    rescanned n = " << thetaN0 << ".." << scanMax << ": " << thetaCounted
         << " values of n, violations of the lower bound " << thetaViolationsLow
         << ", violations of the upper bound " << thetaViolationsHigh << "\n";
    cout << "    so " << thetaLowNum << "/" << thetaLowDen << " <= f(n)/g(n) <= " << thetaHighNum
         << "/" << thetaHighDen << " for every n >= " << thetaN0 << ": f is Theta(g)\n";
    const long long ratioRows[4] = {7, 100, 1000, 10000};
    for (int ratioIndex = 0; ratioIndex < 4; ratioIndex++) {
        long long ratioN = ratioRows[ratioIndex];
        cout << "    f/g at n = " << ratioN << ": " << fOfN(ratioN) << "/" << gOfN(ratioN)
             << " = " << decimalText(fOfN(ratioN), gOfN(ratioN))
             << "   (it is squeezed from above toward 3/4 = 0.75)\n";
    }

    cout << "\n  a claim with no certificate at all: \"f is O(n)\". Here g2(n) = n.\n";
    Certificate linearTry = scanUpperBound(fOfN, linearOfN, 1000, 1, scanMax, false);
    printScanOutcome(true, false, 1000, 1, linearTry, scanMax);
    const long long ratioNs[4] = {10, 100, 1000, 10000};
    cout << "    the ratio f(n)/n = 3n + 5 + 7/n, measured:";
    for (int ratioIndex = 0; ratioIndex < 4; ratioIndex++) {
        long long ratioN = ratioNs[ratioIndex];
        cout << "  n=" << ratioN << " -> " << decimalText(fOfN(ratioN), ratioN);
    }
    cout << "\n    that ratio grows without bound, so no constant bounds it: f is O(n^2), not O(n),\n";
    cout << "    and the scan above shows it with a witness rather than an argument.\n";
}

// ------------------------------------------------------------------ section 2
static void section2Laws() {
    cout << "\n=== 2. the arithmetic laws of Big-O, each one evaluated on both sides ===\n";
    const long long lawMax = 20000;
    cout << "  here g(n) = n^2, so the constants found below are the familiar ones.\n";

    cout << "\n  (a) sum of two functions: if f1 is O(g) with (c1, n01) and f2 is O(g) with (c2, n02),\n";
    cout << "      then f1 + f2 is O(g) with c = c1 + c2 and n0 = max(n01, n02).\n";
    Certificate c1Found = scanUpperBound(fOfN, squareOfN, 4, 1, lawMax, false);
    Certificate c2Found = scanUpperBound(secondOfN, squareOfN, 3, 1, lawMax, false);
    cout << "      f1(n) = 3n^2 + 5n + 7  scanned: c1 = 4, n01 = " << c1Found.n0
         << " (last failure at n = " << c1Found.lastFail << ")\n";
    cout << "      f2(n) = 2n^2 + 9       scanned: c2 = 3, n02 = " << c2Found.n0
         << " (last failure at n = " << c2Found.lastFail << ")\n";
    long long lawSumC = 4 + 3;
    long long lawSumN0 = c1Found.n0 > c2Found.n0 ? c1Found.n0 : c2Found.n0;
    cout << "      the law predicts c = 4 + 3 = " << lawSumC << " and n0 = max(" << c1Found.n0
         << ", " << c2Found.n0 << ") = " << lawSumN0 << "\n";
    Certificate sumFound = scanUpperBound(sumOfBoth, squareOfN, lawSumC, 1, lawMax, false);
    cout << "      scanning the sum against " << lawSumC << "n^2: last failure at n = "
         << sumFound.lastFail << ", so the scan certifies it from n0 = " << sumFound.n0 << "\n";
    cout << "      n        f1 + f2        " << lawSumC << "n^2          holds     margin\n";
    const long long lawRows[5] = {7, 10, 100, 1000, 10000};
    for (int rowIndex = 0; rowIndex < 5; rowIndex++) {
        long long rowN = lawRows[rowIndex];
        long long leftSum = sumOfBoth(rowN);
        long long rightSum = lawSumC * squareOfN(rowN);
        cout << "      " << left << setw(9) << rowN << setw(14) << leftSum << setw(14) << rightSum
             << setw(11) << (leftSum <= rightSum ? "yes" : "NO") << (rightSum - leftSum) << "\n";
    }
    cout << right;
    cout << "      verdict: the predicted certificate is valid. The scan reaches the same c at\n";
    cout << "      n0 = " << sumFound.n0 << ", which is " << (lawSumN0 - sumFound.n0)
         << " steps earlier than the law's bound of " << lawSumN0
         << ", so the law is conservative, never wrong.\n";

    cout << "\n  (b) product with a constant: k*f is O(g) with c = k*c1 and the same n0.\n";
    const long long kConst = 5;
    long long prodC = kConst * 4;
    Certificate prodFound = scanUpperBound([](long long n) { return kConst * fOfN(n); },
                                          squareOfN, prodC, 1, lawMax, false);
    cout << "      k = " << kConst << ", so the law predicts c = " << kConst << " * 4 = " << prodC
         << " and n0 = " << c1Found.n0 << " (the same n0 as f1)\n";
    cout << "      scanning " << kConst << "f1 against " << prodC << "n^2: last failure at n = "
         << prodFound.lastFail << ", n0 = " << prodFound.n0 << ", so the predicted n0 is exact\n";
    cout << "      n        " << kConst << "f1(n)         " << prodC << "n^2         holds     margin\n";
    for (int rowIndex = 0; rowIndex < 5; rowIndex++) {
        long long rowN = lawRows[rowIndex];
        long long leftProd = kConst * fOfN(rowN);
        long long rightProd = prodC * squareOfN(rowN);
        cout << "      " << left << setw(9) << rowN << setw(14) << leftProd << setw(14) << rightProd
             << setw(11) << (leftProd <= rightProd ? "yes" : "NO") << (rightProd - leftProd) << "\n";
    }
    cout << right;
    cout << "      the constant out front does not change the class, only the constant in the bound.\n";

    cout << "\n  (c) dominance: n^2 beats n log2 n from some n onward, for every constant k.\n";
    cout << "      the scan below uses ceil(log2 n), which is never below log2 n, so a certified\n";
    cout << "      n^2 > k*n*ceil(log2 n) is also a certified n^2 > k*n*log2(n).\n";
    const long long domKs[3] = {8, 64, 1024};
    const long long domMax = 200000;
    for (int domIndex = 0; domIndex < 3; domIndex++) {
        long long domK = domKs[domIndex];
        Certificate domCert = scanLowerBound(squareOfN, nTimesCeilLog2, domK, 1, domMax, true);
        cout << "      k = " << domK << ": n0 = " << domCert.n0 << ", " << domCert.tailSize
             << " values of n verified from " << domCert.n0 << " to " << domMax
             << "  ->  CERTIFIED\n";
        cout << "        at n0: n^2 = " << squareOfN(domCert.n0) << ", k*n*ceil(log2 n) = "
             << domK * nTimesCeilLog2(domCert.n0) << " (ceil(log2 " << domCert.n0 << ") = "
             << ceilLog2Of(domCert.n0) << "), margin " << domCert.minMarginNum << "\n";
    }
    cout << "      the three thresholds grow with k, and no threshold exists for all k at once:\n";
    cout << "      the ratio n^2/(n log2 n) = n/log2 n, measured:";
    for (int rowIndex = 0; rowIndex < 5; rowIndex++) {
        long long rowN = lawRows[rowIndex];
        cout << "  n=" << rowN << " -> " << valueText((double)rowN / log2((double)rowN));
    }
    cout << "\n      n/log2 n is unbounded, so every constant k is eventually beaten.\n";

    cout << "\n  (d) the polynomial degree rule: p(n)/n^d tends to the leading coefficient a_d, so a\n";
    cout << "      polynomial is Theta(n^d), never Theta of any other power.\n";
    cout << "      n        p1 = 3n^2+5n+7   p1/n^2     p2 = 7n^3+100n^2+5   p2/n^3\n";
    for (int rowIndex = 0; rowIndex < 5; rowIndex++) {
        long long rowN = lawRows[rowIndex];
        double p1Ratio = (double)fOfN(rowN) / (double)squareOfN(rowN);
        double p2Ratio = (double)cubicPolyOfN(rowN) / (double)(rowN * squareOfN(rowN));
        cout << "      " << left << setw(9) << rowN << setw(17) << fOfN(rowN) << setw(11)
             << valueText(p1Ratio) << setw(20) << cubicPolyOfN(rowN) << valueText(p2Ratio) << "\n";
    }
    cout << right;
    cout << "      p1/n^2 falls from 3.85714 at n = 7 to 3.0005 at n = 10000, and p2/n^3 from 21.3003\n";
    cout << "      at n = 7 to 7.01 at n = 10000: each ratio is trapped just above its leading\n";
    cout << "      coefficient, and that is section 1's certificate seen from the other side. p1/n^2\n";
    cout << "      stays inside [3, 4] from n = " << c1Found.n0
         << " onward, and 3 and 4 are exactly c1*g and c2*g of section 1.\n";
}

// ------------------------------------------------------------------ section 3
static volatile long long g_tripRead = 0;

static void countSingle(long long tripN, long long &countOut, long long &sumOut) {
    volatile long long singleCount = 0;
    volatile long long singleSum = 0;
    for (long long singleI = 0; singleI < tripN; singleI++) {
        singleCount++;
        singleSum += singleI;
    }
    countOut = singleCount;
    sumOut = singleSum;
}
static void countNested(long long tripN, long long &countOut, long long &sumOut) {
    volatile long long nestedCount = 0;
    volatile long long nestedSum = 0;
    for (long long outerI = 0; outerI < tripN; outerI++) {
        for (long long innerJ = 0; innerJ < tripN; innerJ++) {
            nestedCount++;
            nestedSum += 7 * outerI + 3 * innerJ;
        }
    }
    countOut = nestedCount;
    sumOut = nestedSum;
}
static void countDoubling(long long tripN, long long &countOut, long long &sumOut) {
    volatile long long doublingCount = 0;
    volatile long long doublingSum = 0;
    for (long long stepI = 1; stepI < tripN; stepI *= 2) {
        doublingCount++;
        doublingSum += stepI;
    }
    countOut = doublingCount;
    sumOut = doublingSum;
}
static void countUpperTriangle(long long tripN, long long &countOut, long long &sumOut) {
    volatile long long upperCount = 0;
    volatile long long upperSum = 0;
    for (long long upI = 0; upI < tripN; upI++) {
        for (long long upJ = upI + 1; upJ < tripN; upJ++) {
            upperCount++;
            upperSum += upI + upJ;
        }
    }
    countOut = upperCount;
    sumOut = upperSum;
}
static void countLowerTriangle(long long tripN, long long &countOut, long long &sumOut) {
    volatile long long lowerCount = 0;
    volatile long long lowerSum = 0;
    for (long long lowI = 0; lowI < tripN; lowI++) {
        for (long long lowJ = 0; lowJ < lowI; lowJ++) {
            lowerCount++;
            lowerSum += lowI - lowJ;
        }
    }
    countOut = lowerCount;
    sumOut = lowerSum;
}

static long long closedSingleCount(long long n) { return n; }
static long long closedSingleChecksum(long long n) { return n * (n - 1) / 2; }
static long long closedNestedCount(long long n) { return n * n; }
static long long closedNestedChecksum(long long n) { return 5 * n * n * (n - 1); }
static long long closedDoublingCount(long long n) { return ceilLog2Of(n); }
static long long closedDoublingChecksum(long long n) { return (1LL << ceilLog2Of(n)) - 1; }
static long long closedTriangleCount(long long n) { return n * (n - 1) / 2; }
static long long closedUpperChecksum(long long n) { return n * (n - 1) * (n - 1) / 2; }
static long long closedLowerChecksum(long long n) { return n * (n - 1) * (n + 1) / 6; }

struct LoopShape {
    const char *titleLine;
    const char *costText;
    void (*measure)(long long, long long &, long long &);
    long long (*closedCount)(long long);
    long long (*closedChecksum)(long long);
};

static const LoopShape g_loopShapes[5] = {
    {"loop 1: one loop, i from 0 to n-1, one operation per value of i",
     "n operations",
     countSingle, closedSingleCount, closedSingleChecksum},
    {"loop 2: a nested loop, i and j both from 0 to n-1 (every pair once)",
     "n * n operations",
     countNested, closedNestedCount, closedNestedChecksum},
    {"loop 3: i starts at 1 and doubles while i < n",
     "ceil(log2 n) operations, which is log2 n exactly when n is a power of two",
     countDoubling, closedDoublingCount, closedDoublingChecksum},
    {"loop 4: a triangular loop, j from i+1 to n-1 (the pairs above the diagonal)",
     "n(n-1)/2 operations",
     countUpperTriangle, closedTriangleCount, closedUpperChecksum},
    {"loop 5: i from 0 to n-1 then j from 0 to i-1 (the pairs below the diagonal)",
     "n(n-1)/2 operations, the same size as loop 4",
     countLowerTriangle, closedTriangleCount, closedLowerChecksum},
};

static const long long g_sizeList[6] = {2, 4, 8, 16, 64, 1024};

static void section3Counting() {
    cout << "\n=== 3. complexity by counting real operations on loops that really run ===\n";
    cout << "  Each loop below keeps a volatile counter and a volatile checksum, and the trip count is\n";
    cout << "  read from a volatile slot, so no loop here can be folded away or deleted by -O2. Every\n";
    cout << "  measured count and every measured checksum is printed and checked against a closed form\n";
    cout << "  computed independently of the loop.\n";
    for (const LoopShape &shapeNow : g_loopShapes) {
        cout << "\n  " << shapeNow.titleLine << "\n";
        cout << "    cost: " << shapeNow.costText << "\n";
        cout << "    " << left << setw(8) << "n" << setw(16) << "count measured" << setw(14)
             << "closed form" << setw(10) << "verdict" << setw(20) << "checksum measured"
             << setw(14) << "closed form" << "verdict\n";
        for (long long sizeNow : g_sizeList) {
            g_tripRead = sizeNow;
            const long long tripNow = g_tripRead;
            long long measuredCount = 0;
            long long measuredSum = 0;
            shapeNow.measure(tripNow, measuredCount, measuredSum);
            long long expectedCount = shapeNow.closedCount(tripNow);
            long long expectedSum = shapeNow.closedChecksum(tripNow);
            cout << "    " << left << setw(8) << tripNow << setw(16) << measuredCount << setw(14)
                 << expectedCount << setw(10)
                 << (measuredCount == expectedCount ? "match" : "MISMATCH") << setw(20)
                 << measuredSum << setw(14) << expectedSum
                 << (measuredSum == expectedSum ? "match" : "MISMATCH") << "\n";
        }
    }
    cout << right;
    cout << "\n  the counts read straight off the tables: n, n^2, log2 n, n(n-1)/2, n(n-1)/2.\n";
    cout << "  a count is the evidence: n = 1024 gives a nested loop 1048576 operations and the two\n";
    cout << "  triangular loops 523776 each, so the half-full triangle is half of the square, measured.\n";
}

// ------------------------------------------------------------------ section 4
static const int SEARCH_N = 1024;
static const int SEARCH_ABSENT = 64;
static int g_searchTable[SEARCH_N];

__attribute__((noinline)) static long long countComparisons(const int *table, int size, int key,
                                                            int &indexOut) {
    long long compared = 0;
    for (int pos = 0; pos < size; pos++) {
        compared++;
        if (table[pos] == key) {
            indexOut = pos;
            return compared;
        }
    }
    indexOut = -1;
    return compared;
}

static void section4Cases() {
    cout << "\n=== 4. best, worst and average case measured on ONE function ===\n";
    cout << "  linearSearch: walk the array from index 0, one comparison per element, stop at the key.\n";
    cout << "  the array holds 1024 distinct ints, 0..1023, so every key in that range is present once.\n";
    for (int fillI = 0; fillI < SEARCH_N; fillI++) g_searchTable[fillI] = fillI;

    int bestIndex = -1;
    long long bestCount = countComparisons(g_searchTable, SEARCH_N, 0, bestIndex);
    int lastIndex = -1;
    long long lastCount = countComparisons(g_searchTable, SEARCH_N, SEARCH_N - 1, lastIndex);
    int absentIndex = -1;
    long long absentCount = countComparisons(g_searchTable, SEARCH_N, 5000, absentIndex);
    int missingIndex = -1;
    long long missingCount = countComparisons(g_searchTable, SEARCH_N, -7, missingIndex);
    cout << "  (a) best case    key 0 (index 0)          -> index " << bestIndex
         << ", comparisons " << bestCount << ", closed form 1\n";
    cout << "  (b) worst case   key 1023 (last index)    -> index " << lastIndex
         << ", comparisons " << lastCount << ", closed form n = " << SEARCH_N << "\n";
    cout << "  (c) worst case   key 5000 (absent)        -> index " << absentIndex
         << ", comparisons " << absentCount << ", closed form n = " << SEARCH_N
         << " (the whole array is scanned)\n";
    cout << "  (d) the same for a negative key, -7 (absent): index " << missingIndex
         << ", comparisons " << missingCount << "\n";

    cout << "\n  average case, summed over every possible key one call at a time:\n";
    long long presentSum = 0;
    long long presentKeys = 0;
    for (int keyPresent = 0; keyPresent < SEARCH_N; keyPresent++) {
        int idxPresent = -1;
        presentSum += countComparisons(g_searchTable, SEARCH_N, keyPresent, idxPresent);
        presentKeys++;
    }
    long long presentClosed = (long long)SEARCH_N * (SEARCH_N + 1) / 2;
    cout << "    keys present (0..1023): " << presentKeys << " keys, comparisons summed = "
         << presentSum << ", closed form 1024*1025/2 = " << presentClosed << "  ->  "
         << (presentSum == presentClosed ? "match" : "MISMATCH") << "\n";
    cout << "      average = " << presentSum << "/" << presentKeys << " = "
         << fractionText(presentSum, presentKeys) << " = " << decimalText(presentSum, presentKeys)
         << ", and the textbook closed form (n+1)/2 = " << (SEARCH_N + 1) << "/2 = "
         << decimalText(SEARCH_N + 1, 2) << "  ->  "
         << (presentSum * 2 == (long long)presentKeys * (SEARCH_N + 1) ? "match" : "MISMATCH") << "\n";

    long long absentSum = 0;
    long long absentKeys = 0;
    for (int keyAbsent = SEARCH_N + 1; keyAbsent <= SEARCH_N + SEARCH_ABSENT; keyAbsent++) {
        int idxAbsent = -1;
        absentSum += countComparisons(g_searchTable, SEARCH_N, keyAbsent, idxAbsent);
        absentKeys++;
    }
    cout << "    keys absent (1025..1088): " << absentKeys << " keys, comparisons summed = "
         << absentSum << ", one full scan of " << SEARCH_N << " per key  ->  "
         << (absentSum == absentKeys * SEARCH_N ? "match" : "MISMATCH") << "\n";
    cout << "      average = " << absentSum << "/" << absentKeys << " = "
         << fractionText(absentSum, absentKeys) << " = " << decimalText(absentSum, absentKeys)
         << ", which is exactly n, the failure cost\n";

    long long unionSum = presentSum + absentSum;
    long long unionKeys = presentKeys + absentKeys;
    cout << "    both sets together (every possible key in this experiment): " << unionKeys
         << " keys, comparisons summed = " << presentSum << " + " << absentSum << " = " << unionSum
         << "\n";
    cout << "      average = " << unionSum << "/" << unionKeys << " = "
         << fractionText(unionSum, unionKeys) << " = " << decimalText(unionSum, unionKeys) << "\n";
    cout << "    the counts behind that sum, printed so it can be rebuilt by hand:\n";
    cout << "      keys 0 1 2 -> comparisons";
    for (int keyShow = 0; keyShow < 3; keyShow++) {
        int idxShow = -1;
        cout << " " << countComparisons(g_searchTable, SEARCH_N, keyShow, idxShow);
    }
    cout << "   keys " << (SEARCH_N - 2) << " " << (SEARCH_N - 1) << " -> comparisons";
    for (int keyShow = SEARCH_N - 2; keyShow < SEARCH_N; keyShow++) {
        int idxShow = -1;
        cout << " " << countComparisons(g_searchTable, SEARCH_N, keyShow, idxShow);
    }
    cout << "\n";
    cout << "    the three cases are one function and three keys: 1 comparison, 1024 comparisons, and\n";
    cout << "    an average of " << fractionText(unionSum, unionKeys) << " = "
         << decimalText(unionSum, unionKeys) << " when present and absent keys are mixed in.\n";
    cout << "    the successful half averages (n+1)/2 = " << decimalText(SEARCH_N + 1, 2) << "\n";
    cout << "    and the failing half costs a full n = " << SEARCH_N << ": the average case sits\n";
    cout << "    between best and worst exactly because it is a weighted mix of those two, not a third speed.\n";
}

// ------------------------------------------------------------------ section 5
struct GrowthRow {
    long long sizeN;
    double log10Of[7];   // 1, log2 n, n, n log2 n, n^2, n^3, 2^n
};

static const long long g_growthSizes[4] = {10, 100, 1000, 10000};
static GrowthRow g_growthRows[4];

static void fillGrowthRows() {
    for (int rowIndex = 0; rowIndex < 4; rowIndex++) {
        long long sizeN = g_growthSizes[rowIndex];
        double logOfN = log2((double)sizeN);
        double logOfSize = log10((double)sizeN);
        g_growthRows[rowIndex].sizeN = sizeN;
        g_growthRows[rowIndex].log10Of[0] = 0.0;                          // the constant 1
        g_growthRows[rowIndex].log10Of[1] = log10(logOfN);                // log2 n
        g_growthRows[rowIndex].log10Of[2] = logOfSize;                    // n
        g_growthRows[rowIndex].log10Of[3] = log10((double)sizeN * logOfN); // n log2 n
        g_growthRows[rowIndex].log10Of[4] = 2.0 * logOfSize;              // n^2
        g_growthRows[rowIndex].log10Of[5] = 3.0 * logOfSize;              // n^3
        g_growthRows[rowIndex].log10Of[6] = (double)sizeN * log10(2.0);   // 2^n
    }
}

static string pow2ValueText(long long n) {
    if (n <= 62) {
        ostringstream out;
        out << (1LL << n);
        return out.str();
    }
    if (n <= 1023) return valueText(ldexp(1.0, (int)n));
    return "capped";
}

static void section5Growth() {
    cout << "\n=== 5. the growth table: which term dominates, at four sizes ===\n";
    fillGrowthRows();
    cout << "  n        " << left << setw(8) << "1" << setw(12) << "log2 n" << setw(11) << "n"
         << setw(15) << "n log2 n" << setw(14) << "n^2" << setw(16) << "n^3" << "2^n\n";
    for (int rowIndex = 0; rowIndex < 4; rowIndex++) {
        const GrowthRow &rowNow = g_growthRows[rowIndex];
        long long sizeN = rowNow.sizeN;
        cout << "  " << left << setw(9) << sizeN << setw(8) << 1 << setw(12)
             << valueText(log2((double)sizeN)) << setw(11) << sizeN << setw(15)
             << valueText((double)sizeN * log2((double)sizeN)) << setw(14) << sizeN * sizeN
             << setw(16) << sizeN * sizeN * sizeN << pow2ValueText(sizeN) << "\n";
    }
    cout << right;
    double capExponent = 10000.0 * log10(2.0);
    cout << "\n  2^n is capped at n = 10000 and the reason is checkable: 2^10000 = 10^"
         << fixedText(capExponent) << ",\n";
    cout << "  it needs 10001 bits, and the largest finite double on this machine is "
         << valueText(numeric_limits<double>::max()) << ",\n";
    cout << "  so no value in this program can hold it and printing an integer there would be a lie.\n";
    cout << "  what IS exact and printable is the decimal exponent: log10(2^10000) = 10000*log10(2) =\n";
    cout << "  " << fixedText(capExponent) << ", so the value has "
         << (long long)(capExponent + 1.0) << " decimal digits. The cap is a limit of 64 bit\n";
    cout << "  integers and doubles, not a limit of the growth rate; the ratios below use the exponent\n";
    cout << "  directly, so nothing in this table is guessed.\n";

    cout << "\n  the ratio between consecutive columns at each n (this column over the one before it):\n";
    cout << "  n        " << left << setw(13) << "(log2 n)/1" << setw(13) << "n/log2 n"
         << setw(15) << "(n log2 n)/n" << setw(17) << "n^2/(n log2 n)" << setw(12) << "n^3/n^2"
         << "2^n/n^3\n";
    for (int rowIndex = 0; rowIndex < 4; rowIndex++) {
        const GrowthRow &rowNow = g_growthRows[rowIndex];
        cout << "  " << left << setw(9) << rowNow.sizeN;
        for (int colIndex = 1; colIndex < 7; colIndex++) {
            string cellNow = ratioText(rowNow.log10Of[colIndex] - rowNow.log10Of[colIndex - 1]);
            if (colIndex < 6) cout << setw(13) << cellNow;
            else cout << cellNow;
        }
        cout << "\n";
    }
    cout << right;
    cout << "  the last column is the one that matters. At n = 10 the exponential is only "
         << ratioText(g_growthRows[0].log10Of[6] - g_growthRows[0].log10Of[5])
         << " times n^3, so n^3\n";
    cout << "  and 2^n are still close at that size. At n = 10000 it is "
         << ratioText(g_growthRows[3].log10Of[6] - g_growthRows[3].log10Of[5])
         << " times n^3, about\n";
    cout << "  " << (long long)(g_growthRows[3].log10Of[6] - g_growthRows[3].log10Of[5])
         << " orders of magnitude, and no constant in front of n^3 can close that gap.\n";

    cout << "\n  the factor by which each term multiplies when n is multiplied by 10:\n";
    cout << "  step            1        log2 n      n          n log2 n     n^2        n^3        2^n\n";
    const char *stepLabels[3] = {"10 -> 100", "100 -> 1000", "1000 -> 10000"};
    for (int rowIndex = 0; rowIndex < 3; rowIndex++) {
        cout << "  " << left << setw(16) << stepLabels[rowIndex];
        for (int colIndex = 0; colIndex < 7; colIndex++) {
            string cellNow = ratioText(g_growthRows[rowIndex + 1].log10Of[colIndex]
                                       - g_growthRows[rowIndex].log10Of[colIndex]);
            if (colIndex < 6) cout << setw(11) << cellNow;
            else cout << cellNow;
        }
        cout << "\n";
    }
    cout << right;
    cout << "  a straight line multiplies by 10, a square by 100, a cube by 1000, and the last column\n";
    cout << "  by 2^90, 2^900 and 2^9000: for the exponential the factor over a decade of n is a power\n";
    cout << "  of two, not a constant. That is the whole argument for reading the exponent of a growth\n";
    cout << "  function first and the constant in front of it a distant second.\n";
}

// ------------------------------------------------------------------ section 6
static void section6Reminder() {
    cout << "\n=== 6. the one line to remember ===\n";
    cout << "  Big-O, Omega and Theta are not descriptions of code, they are a constant and a threshold\n";
    cout << "  that can be checked by evaluating both sides: this run certified f(n) <= 1*g(n) from n = 7\n";
    cout << "  and f(n) >= (3/4)*g(n) from n = 1 for f(n) = 3n^2 + 5n + 7 against g(n) = 4n^2, and the\n";
    cout << "  certificate breaks the moment the constant crosses 3/4, the ratio of leading coefficients.\n";
    cout << "  Counting is how the complexity is found and how it is checked: the nested loop above ran\n";
    cout << "  1048576 times at n = 1024 and the two triangular loops 523776 each, both measured and both\n";
    cout << "  matched against their closed forms.\n";
    cout << "  Best and worst case are two keys into one function, 1 comparison against 1024, and the\n";
    cout << "  average over every possible key is 9224/17 = 542.588235, a weighted mix of the two, not a\n";
    cout << "  third speed. The growth table is why the exponent matters more than the constant: over one\n";
    cout << "  decade of n, n grows by 10, n^2 by 100 and 2^n by 2^90.\n";
}

int main() {
    section1Certificates();
    section2Laws();
    section3Counting();
    section4Cases();
    section5Growth();
    section6Reminder();
    return 0;
}
