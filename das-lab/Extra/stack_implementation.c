#include <stdio.h>
#define MAX 5

int stack[MAX], top = -1;

int isFull() { return top == MAX - 1; }
int isEmpty() { return top == -1; }

void push(int val) {
  if (isFull()) {
    printf("Stack overflow! cant push %d\n", val);
    return;
  }
  top++;
  stack[top] = val;
  printf("Pushed: %d\n", val);
}

int pop() {
  if (isEmpty()) {
    printf("Stack Underflow! Stack is empty! Nothing to pop!\n");
    return -1;
  }
  return stack[top--];
}

int peek() {
  if (isEmpty()) {
    printf("Stack is empty\n");
    return -1;
  }
  return stack[top];
}

void display() {
  if (isEmpty()) {
    printf("Stack is empty\n");
    return;
  }
  printf("Stack from top -> bottom: ");
  for (int i = top; i >= 0; i--) {
    printf("%d ", stack[i]);
  }
  printf("\n");
}

int main() {
  printf("This is stack implementation using by using array in C.\n");

  int n, val;
  printf("How many elements to push? (max %d): ", MAX);
  scanf("%d", &n);

  for (int i = 0; i < n; i++) {
    printf("Enter element %d: ", i + 1);
    scanf("%d", &val);
    push(val);
  }

  display();

  printf("Popped: %d\n", pop());
  printf("Top is: %d\n", peek());
  display();

  return 0;
}