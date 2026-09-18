/*
 * Kept for the record: this is the earlier practice version of Experiment 0,
 * Part 2, which combined the insertion and the linear search in a single
 * program and ignored the return value of scanf() (so gcc warned about it).
 *
 * It was split into the two files that the manual asks for:
 *   Exp0_Array_Operations/exp0_2.c                 (a) insertion
 *   Exp0_Array_Operations/exp0_2_linear_search.c   (b) linear search
 *
 * The code below is unchanged, exactly as it was written on 23 Aug 2026.
 */

#include <stdio.h>
#define SIZE 3

int main(void)
{
    int arr[SIZE] = {10, 20, 30};
    int a[10], insertIndex, newValue, item, flag, position;
    int i;

    printf("Elements of the array:\n");
    for (i = 0; i < SIZE; ++i) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    printf("Enter the index where you want to insert (0 to %d): ", SIZE - 1);
    scanf("%d", &insertIndex);

    if (insertIndex < 0 || insertIndex > SIZE - 1) {
        printf("Invalid index!\n");
        return 1;
    }

    printf("Enter the new value to insert: ");
    scanf("%d", &newValue);

    for (i = SIZE - 1; i > insertIndex; --i) {
        arr[i] = arr[i - 1];
    }
    arr[insertIndex] = newValue;

    printf("Array after insertion:\n");
    for (i = 0; i < SIZE; ++i) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    printf("\nEnter the data in the array:\n");
    for (i = 0; i < 10; i++) {
        scanf("%d", &a[i]);
    }

    printf("Enter the element to be searched: ");
    scanf("%d", &item);

    flag = 0;
    for (i = 0; i < 10; i++) {
        if (item == a[i]) {
            flag = 1;
            position = i;
            break;
        }
    }

    if (flag == 0) {
        printf("Element Not Found\n");
    } else {
        printf("Element Found at Position = %d\n", position);
    }

    return 0;
}