//sample code which we pasted in text section of web page and then it will collect and paste it here in this file for further analysis

#include <stdio.h>

int functionD(int x, int y) {
    int z=x-y;
    
    return z;
}

int functionC(int a, int b) {
    int resultD = functionD(a, b);
    return resultD * 2; 
}

int functionB(int x, int y) {
    int sum = x + y;
    int product = x * y;
    int resultC = functionC(sum, product);
    return sum + product + resultC;
}

int functionA(int a, int b, int c) {
    int resultB = functionB(b, c);
    return a + resultB;
}

int main() {
    int resultA = functionA(2, 3, 4);
    printf("Result of functionA is: %d\n", resultA);

    return 0;
}