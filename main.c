#include <stdio.h>
#include "auth.h"
#include "utils.h"

// ----------------------------------------------------------------
//                            M A I N
// ----------------------------------------------------------------
// Tujuan: Memungkinkan user mengakses sistem pelacak kesehatan dan kebugaran diri dengan akun pribadi
int main() {
    int choice;

    // Membuat folder/direktori yang dibutuhkan jika belum ada
    createDirectories();

    printf("=========================================\n");
    printf("           WELCOME TO FitNite            \n");
    printf("Your ultimate buddy for a healthier life!\n");
    printf("=========================================\n\n");

    printf("What would you like to do today?\n");
    printf("1. Create a new account\n");
    printf("2. Log in to your account\n\n");
    printf("Your choice (1/2): ");
    scanf("%d", &choice);
    getchar(); // Menghapus karakter newline dari buffer

    // Mengecek pilihan user
    switch(choice) {
        case 1:
            createAccount();
            break;
        case 2:
            login();
            break;
        default:
            printf("Oops! That's not a valid choice. Please try again.\n");
    }

    return 0;
}