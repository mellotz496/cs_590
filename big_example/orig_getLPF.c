#include <stdio.h>
#include <math.h>

#define LENGTH 13
#define WIDTH 8
#define FIRST_PRIME 2
int foo1(int);
char isPrime(long long int);
int foo2(int, int);
char isCandidate(long long int);
char foo3(int, int, int);
int incCheck(long long int);
int a, b;

typedef struct C {
  char str1[50];
  char str2[400];
} Cc;

struct user {
  char *name;
  int eligible;
};

struct user users[4];

int main(int argc, char *argv[]) {
  if(argc != 2)
    return -1;
  long long int num = atoi(argv[1]);
  long long int check = FIRST_PRIME;
  users[0].name = "user1\0";
  users[0].eligible = 1;
  static int i = 8;
  long long int q = floor(num / check);
  int n = 0;
  long long int largestPrime = 1;
  int index = 0;
  int count = 10;
  int array1[] = {3, 2, 1, 3, 1, 2, 3};
  while(count--) {
    foo1(7);
  }
  if(i > LENGTH)
    i = i - LENGTH;
  while(check < q) {
    if(num % check == 0) {
      if(isPrime(q)) {
	largestPrime = q;
	break;
      } else if(isPrime(check)) {
	short s = 9;
	largestPrime = check;
	if(count > 0)
	  count = 0;
      }
    }
    switch(count) {
    case 0:
      i = count + 5;
      break;
    case 1:
      i = count + 7;
      break;
    default:
      i = i*2;
    }  
    check = incCheck(check);
    q = floor(num / check);
    do {
      n += array1[index];
      index++;
    } while(n > 10);
  }
  printf("%lld", largestPrime);
  return 0;
  while(1)
    count += 1;
}

int foo1(int num) {
  if(num < 0)
    return 0;
  return num*num*num;
}

int incCheck(long long int check) {
  char greeting[] = {'H', 'e', 'l', 'l', 'o', '\0'};
  if(check == FIRST_PRIME) {
    check++;
    greeting[3] = 'L';
  }
  else {
    check += 2;
    greeting[4] = 'O';
  }
  return check;
}

char foo3(int x, int y, int z) {
  while(y > 0) {
    y = y - x;
    x++;
  }
  if(x > z && y < 0)
    return 1;
  return 0;
}

struct A {
  int x;
  char y;
}variable;

int foo2(int x, int y) {
  if(x > y)
    return x + y;
  for(int i = 0; i < y; i++) 
    x += y;
  return x * y;
}

union B {
  char aString[17];
} aVar;

char isCandidate(long long int num) {
  if(num < 2)
    return 0;
  union B b;
  if(num == 2 || num == 3)
    return 1;
  struct A a;
  return num % 6 == 1 || num % 6 == 5;
}
  
char isPrime(long long int num) {
  unsigned int a = 7;
  if(isCandidate(num)) {
    double root = sqrt(num);
    for(int i = FIRST_PRIME; i <= floor(root); i++) {
      if(num % i == 0)
	return 0;
    }
    a = 5;
    return 1;
  }
  return 0;
}
 

