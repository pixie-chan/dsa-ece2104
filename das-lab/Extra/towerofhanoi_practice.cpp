#include <iostream>
using namespace std;

void hanoi(int n, char a, char b, char c) {
  if (n == 1) {
    cout << "Move disk 1 from " << a << " to " << c << endl;
    return;
  }
  hanoi(n - 1, a, c, b);
  cout << "Move disk " << n << " from " << a << " to " << c << endl;
  hanoi(n - 1, b, a, c);
}

int main() {
  int n;
  cout << "Enter disks: ";
  if (!(cin >> n) || n <= 0) {
    cout << "Enter a positive number" << endl;
    return 1;
  }
  cout << "Solving for " << n << " disks:" << endl;
  hanoi(n, 'A', 'B', 'C');
  return 0;
}
