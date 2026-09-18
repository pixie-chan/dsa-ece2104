/*
 * Experiment 4.1 : Max-priority queue using an UNSORTED ARRAY
 * Course          : ELC2132 Data Structures & Algorithms Lab
 *
 * Aim             : Implement a priority queue as an abstract data type in C
 *                   using an unsorted array, supporting insert, extractMax,
 *                   peek and isEmpty.
 *
 * Idea            : new elements are appended at the end without any ordering
 *                   (insert is O(1)); the maximum is found by a linear scan of
 *                   the whole array, so peek and extractMax are O(n).
 *
 * Operation       Time complexity
 *   insert        O(1)  - append at pq[n]
 *   peek          O(n)  - linear scan for the highest priority
 *   extractMax    O(n)  - linear scan + shifting the hole left
 *   isEmpty       O(1)
 *
 * Compile : gcc -Wall -Wextra -std=c11 -o pq_array exp4_1_priority_queue_array.c
 */

#include <stdio.h>

#define MAX 100

/* One queue entry: the payload and the priority it is served with. */
typedef struct {
    int data;
    int priority;
} Element;

static Element pq[MAX];   /* unsorted storage */
static int size = 0;      /* number of elements currently in the queue */

int isEmpty(void)
{
    return size == 0;
}

int isFull(void)
{
    return size == MAX;
}

/* Insert: O(1) - the element is simply appended at the end. */
void insertItem(int data, int priority)
{
    if (isFull()) {
        printf("Queue overflow: cannot insert (%d, priority %d)\n", data, priority);
        return;
    }
    pq[size].data = data;
    pq[size].priority = priority;
    size++;
    printf("Inserted: data = %d, priority = %d\n", data, priority);
}

/* Returns the index of the highest-priority element. Ties: earliest wins. */
int maxIndex(void)
{
    int idx = 0;
    for (int i = 1; i < size; i++) {
        if (pq[i].priority > pq[idx].priority) {
            idx = i;
        }
    }
    return idx;
}

/* Peek: O(n) - report the maximum without removing it. */
void peek(void)
{
    if (isEmpty()) {
        printf("Queue is empty: nothing to peek\n");
        return;
    }
    int idx = maxIndex();
    printf("Highest priority element: data = %d, priority = %d\n",
           pq[idx].data, pq[idx].priority);
}

/* Extract max: O(n) - find the maximum, then close the gap by shifting. */
void extractMax(void)
{
    if (isEmpty()) {
        printf("Queue is empty: nothing to extract\n");
        return;
    }

    int idx = maxIndex();
    printf("Extracted: data = %d, priority = %d\n", pq[idx].data, pq[idx].priority);

    for (int i = idx; i < size - 1; i++) {
        pq[i] = pq[i + 1];
    }
    size--;
}

void display(void)
{
    if (isEmpty()) {
        printf("Queue is empty\n");
        return;
    }
    printf("Queue contents (insertion order, size = %d):\n", size);
    for (int i = 0; i < size; i++) {
        printf("  [%d] data = %d, priority = %d\n", i, pq[i].data, pq[i].priority);
    }
}

int main(void)
{
    int choice, data, priority;

    do {
        printf("\n--- Max Priority Queue (unsorted array) ---\n");
        printf("1. Insert\n2. Extract max\n3. Peek\n4. Display\n5. isEmpty\n6. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input: expected a menu number.\n");
            break;
        }

        switch (choice) {
        case 1:
            printf("Enter data and priority: ");
            if (scanf("%d %d", &data, &priority) != 2) {
                printf("Invalid input: expected two integers.\n");
                return 1;
            }
            insertItem(data, priority);
            break;
        case 2:
            extractMax();
            break;
        case 3:
            peek();
            break;
        case 4:
            display();
            break;
        case 5:
            printf("Queue is %s\n", isEmpty() ? "empty" : "not empty");
            break;
        case 6:
            printf("Exiting.\n");
            break;
        default:
            printf("Invalid choice: enter a number from 1 to 6.\n");
        }
    } while (choice != 6);

    return 0;
}
