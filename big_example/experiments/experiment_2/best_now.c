#include <stdio.h>
#include <math.h>
#define LENGTH 13
#define WIDTH 8
#define FIRST_PRIME 2
char isPrime(long long int);
int incCheck(long long int);
int main(int argc, char *argv[]) {
  long long int num = atoi(argv[1]);
  long long int check = FIRST_PRIME;
  long long int q = floor(num / check);
  long long int largestPrime = 1;
  while(check < q) {
    if(num % check == 0) {
      if(isPrime(q)) {
	largestPrime = q;
      } else if(isPrime(check)) {
	largestPrime = check;
      }
    }
    check = incCheck(check);
    q = floor(num / check);
  }
  printf("%lld", largestPrime);
}
int incCheck(long long int check) {
  if(check == FIRST_PRIME) {
    check++;
  }
  else {
    check += 2;
  }
  return check;
}
char isPrime(long long int num) {
                       {
    double root = sqrt(num);
    for(int i = FIRST_PRIME; i <= floor(root); i++) {
      if(num % i == 0)
	return 0;
    }
    return 1;
  }
}
