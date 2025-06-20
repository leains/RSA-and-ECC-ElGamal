#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

/*Mencari PBB*/
int pbb(int a, int b){
    int mod;
    while (b != 0){
        mod = a % b;
        a = b;
        b = mod; 
    }
    return a;
}

/*Menghitung mod inverse dari e*/
int inverse(int e, int phi){
    int a = 0, na = 1;
    int b = phi, nb = e;
    int q,temp;
    while (nb != 0) {
        q = b / nb;
        temp = na;
        na = a - q * na;
        a = temp;

        temp = nb;
        nb = b - q * nb;
        b = temp;
    }
    if (a < 0) a += phi;
    return a;
}

int calculation(int base, int exp, int mod) {
    int result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1) result = (result * base) % mod;
        base = (base * base) % mod;
        exp = exp / 2;
    }
    return result;
}

int main(){
    int p,q,phi,e,d,n;
    /*p dan q dipilih dari awal karena hanya simulasi.*/
    p = 61;
    q = 47;
    n = p * q;
    phi = (p - 1) * (q - 1);
    /*Menentukan e, yaitu kunci publik*/
    e = 13; //Relatif prima dengan Φ(n)

    /*Menentukan d, yaitu kunci privat*/
    d = inverse(e, phi);

    char input[256]; 
    printf("Masukkan pesan: ");
    fgets(input, sizeof(input), stdin);

    int ascii[256];
    //Merubah huruf ke ascii
    for (int i=0;i<strlen(input);i++){
        ascii[i] = (int)input[i];
    }
    /*Enkripsi*/
    clock_t start_enc = clock();
    int encrypted[256];
    for (int i=0;i<strlen(input);i++){
        encrypted[i] = calculation(ascii[i],e,n);
        printf("Encrypted %d : %d",i,encrypted[i]);
        printf("\n");
    }
    clock_t end_enc = clock();
    clock_t start_dec = clock();
    /*Dekripsi*/
    int decrypted[256];
    for (int i=0;i<strlen(input);i++){
        decrypted[i] = calculation(encrypted[i],d,n);
    }
    clock_t end_dec = clock();
    printf("Pesan baru: ");
    for (int i=0;i<strlen(input);i++){
        printf("%c",(char)decrypted[i]);
    }

    printf("Waktu Enkripsi: %.10f detik\n", (double)(end_enc - start_enc) / CLOCKS_PER_SEC);
    printf("Waktu Dekripsi: %.10f detik\n", (double)(end_dec - start_dec) / CLOCKS_PER_SEC);
}