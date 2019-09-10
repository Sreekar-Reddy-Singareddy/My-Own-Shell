#include <stdio.h>

int main () {
  int * arr;
  int i=0;
  size_t s = sizeof(int);
  printf("Size: %zu\n", s);

  arr = (int *) calloc(10, s); // Assign the pointer to the created memory to the pointer '*arr'
  
  while (i<10){
    scanf("%d",arr);
    arr++;
    i++;
  }

  arr -= 10;
  i=0;
  while (i<10) {
    printf("Current Array Element: %8d | Address: %p\n", *arr, arr); // Printing the current element in the array
    i++;
    arr++;
  }
  arr -=10;

  int j=0;
  printf("Which element do you want to see? ");
  scanf("%d",&j);
  printf("Element at %d is ::: %d\n",j,*(arr+j));

}
