#include <iostream>
using namespace std;
#define SIZE 7

void mergeSort(int A[], int low, int high);
void merge(int A[], int low, int high, int mid);

int main() {
  int arraySize = 7;
  int A[SIZE] = {28, 30, 24, 22, 24, 28, 32};
  int low = 0;
  int high = arraySize - 1;

  cout << "Unsorted Array: ";
  for (int i = 0; i < SIZE; i++) {
    cout << A[i] << " ";
  }

  cout << endl;

  mergeSort(A, low, high);

  cout << "Sorted Array: ";
  for (int i = 0; i < SIZE; i++) {
    cout << A[i] << " ";
  }

  return 0;
}

void mergeSort(int A[], int low, int high) {
  if (low < high) {
    int mid;
    mid = (low + high) / 2;
    mergeSort(A, low, mid);
    mergeSort(A, mid + 1, high);
    merge(A, low, high, mid);
  }
  return;
}

void merge(int A[], int low, int high, int mid) {
  int i, j, k, c[50];
  i = low;
  k = low;
  j = mid + 1;
  while (i <= mid && j <= high) {
    if (A[i] < A[j]) {
      c[k] = A[i];
      k++;
      i++;
    } else {
      c[k] = A[j];
      k++;
      j++;
    }
  }
  while (i <= mid) {
    c[k] = A[i];
    k++;
    i++;
  }
  while (j <= high) {
    c[k] = A[j];
    k++;
    j++;
  }
  for (i = low; i < k; i++) {
    A[i] = c[i];
  }
}
