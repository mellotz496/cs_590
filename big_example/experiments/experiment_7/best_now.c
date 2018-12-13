#include <stdio.h>
#include <math.h>

#define LENGTH 13
#define WIDTH 8
#define FIRST_PRIME 2
int foo1(int);
char isPrime(long long int);
                  ;
                               ;
                        ;
int incCheck(long long int);
        ;

                  
                
                 
    ;

struct user {
  char *name;
  int eligible;
};

struct user users[4];

int main(int argc, char *argv[]) {
               
             ;
  long long int num = atoi(argv[1]);
  long long int check = FIRST_PRIME;
                           ;
                       ;
  static int i = 8;
  long long int q = floor(num / check);
  int n = 0;
  long long int largestPrime = 1;
  int index = 0;
  int count = 10;
  int array1[] = {3, 2, 1, 3, 1, 2, 3};
                  
            
   
                
                  ;
  while(check < q) {
    if(num % check == 0) {
      if(isPrime(q)) {
	largestPrime = q;
	     ;
      } else if(isPrime(check)) {
	            
	largestPrime = check;
	             
            ;
      }
     
                   
           
                    
            
           
                    
            
            
              
    }  
    check = incCheck(check);
    q = floor(num / check);
        
                         
              
                   ;
  }
  printf("%lld", largestPrime);
           
          
               
}

                   
             
             
                     
 

int incCheck(long long int check) {
  char greeting[] = {'H', 'e', 'l', 'l', 'o', '\0'};
  if(check == FIRST_PRIME) {
    check++;
                     ;
  }
  else {
    check += 2;
                     ;
  }
  return check;
}

                                
                
              
        
   
                    
             
           
 

          
        
         
          

                        
           
                 
                             
           
               
 

         
                   
      ;

char isCandidate(long long int num) {
             
            ;
            
  if(num == 2 || num == 3)
    return 1;
             
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
          
    return 1;
  }
  return 0;
}
 

