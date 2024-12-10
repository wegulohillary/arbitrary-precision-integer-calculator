#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_DIGITS 1000  // Maximum digits for arbitrary precision

typedef struct {
    int digits[MAX_DIGITS];
    int length;
    int sign; // 1 for positive, -1 for negative
} BigInt;

// Helper functions
void initialize(BigInt *num) {
    memset(num->digits, 0, sizeof(num->digits));
    num->length = 0;
    num->sign = 1;
}

void parse(BigInt *num, const char *str) {
    initialize(num);
    int start = 0;
    if (str[0] == '-') {
        num->sign = -1;
        start = 1;
    }
    num->length = strlen(str) - start;
    for (int i = 0; i < num->length; i++) {
        num->digits[num->length - 1 - i] = str[start + i] - '0';
    }
}

void print(const BigInt *num) {
    if (num->sign == -1) printf("-");
    for (int i = num->length - 1; i >= 0; i--) {
        printf("%d", num->digits[i]);
    }
    printf("\n");
}

int compare(const BigInt *a, const BigInt *b) {
    if (a->length != b->length) return a->length - b->length;
    for (int i = a->length - 1; i >= 0; i--) {
        if (a->digits[i] != b->digits[i]) return a->digits[i] - b->digits[i];
    }
    return 0;
}

// Addition
void add(const BigInt *a, const BigInt *b, BigInt *result) {
    initialize(result);
    if (a->sign == b->sign) {
        result->sign = a->sign;
        int carry = 0, maxLength = (a->length > b->length ? a->length : b->length);
        for (int i = 0; i < maxLength || carry; i++) {
            int sum = carry;
            if (i < a->length) sum += a->digits[i];
            if (i < b->length) sum += b->digits[i];
            result->digits[result->length++] = sum % 10;
            carry = sum / 10;
        }
    } else {
        BigInt abs_a = *a, abs_b = *b;
        abs_a.sign = abs_b.sign = 1;
        if (compare(&abs_a, &abs_b) >= 0) {
            subtract(&abs_a, &abs_b, result);
            result->sign = a->sign;
        } else {
            subtract(&abs_b, &abs_a, result);
            result->sign = b->sign;
        }
    }
}

// Subtraction
void subtract(const BigInt *a, const BigInt *b, BigInt *result) {
    initialize(result);
    if (a->sign != b->sign) {
        BigInt temp = *b;
        temp.sign *= -1;
        add(a, &temp, result);
        return;
    }
    if (compare(a, b) < 0) {
        subtract(b, a, result);
        result->sign = -1;
        return;
    }
    int borrow = 0;
    for (int i = 0; i < a->length; i++) {
        int diff = a->digits[i] - (i < b->length ? b->digits[i] : 0) - borrow;
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result->digits[result->length++] = diff;
    }
    while (result->length > 0 && result->digits[result->length - 1] == 0) {
        result->length--;
    }
}

// Multiplication
void multiply(const BigInt *a, const BigInt *b, BigInt *result) {
    initialize(result);
    result->sign = a->sign * b->sign;
    for (int i = 0; i < a->length; i++) {
        int carry = 0;
        for (int j = 0; j < b->length || carry; j++) {
            int prod = result->digits[i + j] +
                       a->digits[i] * (j < b->length ? b->digits[j] : 0) + carry;
            result->digits[i + j] = prod % 10;
            carry = prod / 10;
        }
    }
    result->length = a->length + b->length;
    while (result->length > 0 && result->digits[result->length - 1] == 0) {
        result->length--;
    }
}

// REPL
void repl() {
    char input[1024];
    BigInt a, b, result;
    printf("Arbitrary Precision Calculator\n");
    printf("Supported operations: +, -, *, !, ^\n");
    while (1) {
        printf(">>> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        if (strcmp(input, "exit") == 0) break;

        char op;
        char num1[512], num2[512];
        if (sscanf(input, "%s %c %s", num1, &op, num2) == 3) {
            parse(&a, num1);
            parse(&b, num2);
            switch (op) {
                case '+': add(&a, &b, &result); break;
                case '*': multiply(&a, &b, &result); break;
            }
            print(&result);
        }
    }
}

int main() {
    repl();
    return 0;
}
