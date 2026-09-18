/*
 * Experiment 4.2 : Max-priority queue using a SORTED SINGLY LINKED LIST
 * Course          : ELC2132 Data Structures & Algorithms Lab
 *
 * Aim             : Implement a priority queue as an abstract data type in C
 *                   using a singly linked list kept in descending order of
 *                   priority, supporting insert, extractMax, peek and isEmpty.
 *
 * Idea            : the list is always sorted, so the maximum is the head node
 *                   and extractMax / peek are O(1); insert has to walk the list
 *                   to find the correct position, so it is O(n).
 *
 * Operation       Time complexity
 *   insert        O(n) - traverse to the insertion point and relink
 *   peek          O(1) - the head always holds the maximum
 *   extractMax    O(1) - unlink the head node
 *   isEmpty       O(1)
 *
 * Compile : gcc -Wall -Wextra -std=c11 -o pq_list exp4_2_priority_queue_linkedlist.c
 */

#include <stdio.h>
#include <stdlib.h>

/* One node of the list: payload + priority + link to the next node. */
typedef struct Node {
    int data;
    int priority;
    struct Node *next;
} Node;

static Node *head = NULL;   /* highest priority element, or NULL when empty */
static int size = 0;

int isEmpty(void)
{
    return head == NULL;
}

/* Insert in descending order of priority: O(n). */
void insertItem(int data, int priority)
{
    Node *node = malloc(sizeof(Node));
    if (node == NULL) {
        printf("Memory allocation failed: cannot insert (%d, priority %d)\n",
               data, priority);
        return;
    }
    node->data = data;
    node->priority = priority;
    node->next = NULL;

    /* Case 1: the new element outranks the head, so it becomes the new head. */
    if (head == NULL || priority > head->priority) {
        node->next = head;
        head = node;
    } else {
        /* Case 2: walk until the next node has a strictly lower priority. */
        Node *current = head;
        while (current->next != NULL && current->next->priority >= priority) {
            current = current->next;
        }
        node->next = current->next;
        current->next = node;
    }

    size++;
    printf("Inserted: data = %d, priority = %d\n", data, priority);
}

/* Peek: O(1) - the head is the maximum because the list stays sorted. */
void peek(void)
{
    if (isEmpty()) {
        printf("Queue is empty: nothing to peek\n");
        return;
    }
    printf("Highest priority element: data = %d, priority = %d\n",
           head->data, head->priority);
}

/* Extract max: O(1) - unlink and free the head node. */
void extractMax(void)
{
    if (isEmpty()) {
        printf("Queue is empty: nothing to extract\n");
        return;
    }

    Node *temp = head;
    printf("Extracted: data = %d, priority = %d\n", temp->data, temp->priority);
    head = head->next;
    free(temp);
    size--;
}

void display(void)
{
    if (isEmpty()) {
        printf("Queue is empty\n");
        return;
    }
    printf("Queue contents (descending priority, size = %d):\n", size);
    for (Node *current = head; current != NULL; current = current->next) {
        printf("  data = %d, priority = %d", current->data, current->priority);
        printf("%s\n", current == head ? "  <- head (max)" : "");
    }
}

/* Frees every remaining node before the program exits. */
void freeQueue(void)
{
    while (head != NULL) {
        Node *temp = head;
        head = head->next;
        free(temp);
    }
    size = 0;
}

int main(void)
{
    int choice, data, priority;

    do {
        printf("\n--- Max Priority Queue (sorted linked list) ---\n");
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
                freeQueue();
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

    freeQueue();
    return 0;
}
