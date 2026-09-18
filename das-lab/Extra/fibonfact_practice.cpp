#include <iostream>
using namespace std;

void fibo(int n);
int fact(int f);

int main() {

  int n, f;
  cout << ("Enter a number to generate its n'th fibonacci number: ") << endl;
  cin >> n;
  fibo(n);
  cout << "Enter a number below to generate its factorial: " << endl;
  cin >> f;
  cout << fact(f);
  return 0;
}

void fibo(int n) {
  int a, b, next;
  a = 0;
  b = 1;
  for (int i = 0; i < n - 1; i++) {
    next = a + b;
    a = b;
    b = next;
  }
  cout << "n'th term of fibonacci series is: " << a << endl;
}

int fact(int f) {
  if (f == 0)
    return 1;
  return f * fact(f - 1);
}
