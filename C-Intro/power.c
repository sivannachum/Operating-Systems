#include <assert.h>
#include <stdio.h>

// This method takes a base number and an exponent you want to raise it to
// It returns base ** exp
int power(int base, int exp) {
  // Make sure we have nonnegative powers
  assert(exp >= 0);
  // If the exponent is 0, any number to the 0 power is equal to 1
  if (exp == 0){
    return 1;
  }
  // If the exponent is 1, any number to the 1 power is itself
  else if (exp == 1){
    return base;
  }
  // Otherwise, divide the work
  else {
    // If the exponent is even,
    // multiply the base to the power of half the exponent twice
    if (exp % 2 == 0){
      return power(base, exp/2) * power(base, exp/2);
    }
    // If the exponent is odd,
    // multiply the base times
    // the base to the power of  half the exponent twice
    else{
      return base * power(base, exp/2) * power(base, exp/2);
    }
  }
}

int main(void) {
	// Make sure it actually works:
	assert(power(2, 0) == 1);
	assert(power(2, 1) == 2);
	assert(power(2, 2) == 4);
	assert(power(2, 3) == 8);
	assert(power(2, 10) == 1024);
	assert(power(4, 3) == 4*4*4);
	assert(power(-4, 3) == -4*4*4);

	// Interactive:
	int base = 0;
	int exp = 0;

	printf("type <base> <exp>: ");
	scanf("%d %d", &base, &exp);
	printf("%d ** %d => %d\n", base, exp, power(base, exp));
   
	return 0;
}
