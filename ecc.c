#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define PRIME 751
#define AA 2
#define BB 3

typedef struct {
    int x;
    int y;
    int is_infinity;
} Point;

/*Mod Inverse untuk ECC*/
int inverse(int e, int phi){
    int t = 0, newt = 1;
    int r = phi, newr = e;
    while (newr != 0) {
        int quotient = r / newr;
        int temp = newt;
        newt = t - quotient * newt;
        t = temp;

        temp = newr;
        newr = r - quotient * newr;
        r = temp;
    }
    if (t < 0)
        t += phi;
    return t;
}

/*Mengecek apakah titik pt ada di kurva*/
int isOnCurve(Point pt) {
    if (pt.is_infinity) return 1;
    int lhs = (pt.y * pt.y) % PRIME;
    int rhs = (pt.x * pt.x * pt.x + AA * pt.x + BB) % PRIME;
    return lhs == rhs;
}

/*Penambahan titik A dan titik B*/
Point addPoint(Point A, Point B) {
    Point R;
    if (A.is_infinity) return B;
    if (B.is_infinity) return A;
    if (A.x == B.x && (A.y + B.y) % PRIME == 0) {
        R.is_infinity = 1;
        return R;
    }
    int m;
    if (A.x == B.x && A.y == B.y) {
        m = (3 * A.x * A.x + AA) * inverse(2 * A.y, PRIME) % PRIME;
    } else {
        m = (B.y - A.y) * inverse(B.x - A.x, PRIME) % PRIME;
    }
    if (m < 0) m += PRIME;
    R.x = (m * m - A.x - B.x) % PRIME;
    if (R.x < 0) R.x += PRIME;
    R.y = (m * (A.x - R.x) - A.y) % PRIME;
    if (R.y < 0) R.y += PRIME;
    R.is_infinity = 0;
    return R;
}

Point kaliSkalar(int k, Point pt) {
    Point R = {0, 0, 1};
    while (k > 0) {
        if (k % 2 == 1) R = addPoint(R, pt);
        pt = addPoint(pt, pt);
        k = k / 2;
    }
    return R;
}

Point encoding(int num, int *offset_out) {
    for (int offset = 0; offset < 50; offset++) {
        int try_x = (num + offset) % PRIME;
        for (int y = 0; y < PRIME; y++) {
            Point p = { try_x, y, 0 };
            if (isOnCurve(p)) {
                if (offset_out) *offset_out = offset;
                return p;
            }
        }
    }
    Point inf = {0, 0, 1};
    return inf;
}

int main() {
    /*Generator, d, dan Q*/
    Point G = {3, 6, 0};
    int d = 5;
    Point Q = kaliSkalar(d, G);
    
    /*Input string*/
    char input[256];
    int ascii[256];
    printf("Enter the word : ");
    fgets(input, sizeof(input), stdin);
    int len = strlen(input);
    for (int i=0; i < len; i++) {
        ascii[i] = (int)input[i];
    }

    // Encryption
    clock_t start_enc = clock();
    Point C1[256],C2[256],kQ[256];
    int k = 7; /*Kasus nyata : K nya akan digenerate secara acak.*/
    int offset[256];
    for (int i=0;i<len;i++) {
        offset[i] = 0;
        Point M = encoding(ascii[i], &offset[i]);
        if (M.is_infinity) {
            printf("Failed to encode number to curve.\n");
            return 1;
        }
        C1[i] = kaliSkalar(k, G);
        kQ[i] = kaliSkalar(k, Q);
        C2[i] = addPoint(M, kQ[i]);
        printf("Encrypted C(%d): (%d, %d), C2: (%d, %d)\n", i, C1[i].x, C1[i].y, C2[i].x, C2[i].y);
    }
    clock_t end_enc = clock();

    clock_t start_dec = clock();
    // Decryption
    int recovered[256];
    for (int i=0;i<len;i++) {
        Point dkC1 = kaliSkalar(d, C1[i]);
        dkC1.y = PRIME - dkC1.y;
        Point decrypted = addPoint(C2[i], dkC1);
        recovered[i] = (decrypted.x - offset[i] + PRIME) % PRIME;
    }
    
    for (int i=0;i<len;i++) {
        printf("%c", (char)recovered[i]);
    }
    clock_t end_dec = clock();

    printf("Waktu Enkripsi: %.10f detik\n", (double)(end_enc - start_enc) / CLOCKS_PER_SEC);
    printf("Waktu Dekripsi: %.10f detik\n", (double)(end_dec - start_dec) / CLOCKS_PER_SEC);
    return 0;
}

