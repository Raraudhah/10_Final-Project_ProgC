#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auth.h"
#include "dashboard.h"

struct User {
    char email[50];
    char password[11];
};

// ----------------------------------------------------------------
//                            P A R T  1
//                        1.1 TARGET KALORI
// ----------------------------------------------------------------
// Tujuan: Menghitung target kalori harian berdasarkan BMR, tingkat aktivitas, dan goals user
int hitungKaloriTarget(char *gender, int umur, int tinggi, int berat, int aktivitasLevel, int *goals) {
    double bmr, faktorAktivitas;

    // Rumus BMR untuk pria dan wanita berbeda
    if (strcmp(gender, "Male") == 0) {
        bmr = 10 * berat + 6.25 * tinggi - 5 * umur + 5;
    } else {
        bmr = 10 * berat + 6.25 * tinggi - 5 * umur - 161;
    }

    // Menentukan faktor aktivitas berdasarkan input
    switch (aktivitasLevel) {
        case 1: faktorAktivitas = 1.2; break;
        case 2: faktorAktivitas = 1.375; break;
        case 3: faktorAktivitas = 1.55; break;
        case 4: faktorAktivitas = 1.725; break;
        default: faktorAktivitas = 1.2;
    }

    double kebutuhanKalori = bmr * faktorAktivitas;

    // Menyesuaikan kalori berdasarkan goals
    if (goals[0]) kebutuhanKalori -= 500;        // Lose weight
    else if (goals[1]) kebutuhanKalori += 500;   // Gain weight
    else if (goals[2]) kebutuhanKalori += 250;   // Grow taller
    else if (goals[3]) kebutuhanKalori += 300;   // Build muscle

    return (int)kebutuhanKalori;
}

// ----------------------------------------------------------------
//                         1.2 BUAT AKUN BARU
// ----------------------------------------------------------------
// Tujuan: Meminta input untuk membuat akun baru dan memanggil function 1.1
void createAccount() {
    struct User user;
    char savedEmail[50], savedPassword[11];
    int emailExists = 0;
    int selectedGoals[6] = {0};
    int totalSelected = 0;
    char goalsInput[100];
    int activityChoice;
    char gender[10];
    int age, height, weight;

    printf("\n===== Create Your FitNite Account =====\n\n");

    // ===== Goals =====
    const char *goalsList[] = {
        "Lose Weight",
        "Gain Weight",
        "Grow Taller",
        "Build Muscle",
        "Meal Planning",
        "Stress Management\n"
    };
    
    printf("What are your fitness goals? (Max 3 choices, e.g., 1 4 6):\n");
    for (int i = 0; i < 6; i++) {
        printf("%d. %s\n", i + 1, goalsList[i]);
    }

    printf("Your choice: ");
    fgets(goalsInput, sizeof(goalsInput), stdin);

    // Parsing input goals
    char *token = strtok(goalsInput, " ");
    while (token != NULL && totalSelected < 6) {
        int index = atoi(token);
        if (index >= 1 && index <= 6) {
            if (!selectedGoals[index - 1]) {
                selectedGoals[index - 1] = 1;
                totalSelected++;
            }
        }
        token = strtok(NULL, " ");
    }

    // Validasi jumlah dan jenis goals
    int bodyGoals = selectedGoals[0] + selectedGoals[1] + selectedGoals[2];
        
    if (totalSelected == 0) {
        printf("You must select at least one goal.\n");
        return;
    }
    if (totalSelected > 3) {
        printf("You can only choose up to 3 goals.\n");
        return;
    }
    if (bodyGoals > 1) {
        printf("You can only select one body transformation goal (lose/gain weight/height).\n");
        return;
    }

    // ===== Activity Level =====
    printf("\nHow active are you?\n");
    printf("1. Not very active\n2. Moderately active\n3. Active\n4. Very active\n\n");
    printf("Your choice (1/2/3/4): ");
    scanf("%d", &activityChoice);
    getchar(); // Menghapus karakter newline dari buffer

    if (activityChoice < 1 || activityChoice > 4) {
        printf("Invalid selection. Please choose between 1–4.\n");
        return;
    } 

    // ===== Data Diri =====
    do {
        printf("\nWhats is your gender?\n");
        printf("1. Male\n2. Female\n\n");
        printf("Your choice (1/2): ");
        int genderChoice;
        scanf("%d", &genderChoice);
        getchar();
        
        if (genderChoice == 1) {
            strcpy(gender, "Male");
            break;
        } else if (genderChoice == 2) {
            strcpy(gender, "Female");
            break;
        } else {
            printf("Invalid input. Please enter 1 or 2.\n");
        }
    } while(1);

    printf("\nHow old are you? (years): ");
    scanf("%d", &age);

    printf("\nYour height (cm): ");
    scanf("%d", &height);

    printf("\nYour weight (kg): ");
    scanf("%d", &weight);
    getchar(); // Menghapus karakter newline dari buffer
   
    // ===== Email dan Password =====
    FILE *file;

    do {
        emailExists = 0;

        printf("\nEnter your email (@gmail.com): ");
        scanf("%s", user.email);

        // Memvalidasi domain email
        if (strstr(user.email, "@gmail.com") == NULL) {
            printf("Email must be a @gmail.com address.\n");
            continue;
        }

        // Mengecek apakah email sudah digunakan
        file = fopen("akun.txt", "r");
        if (file != NULL) {
            while (fscanf(file, "%s %s", savedEmail, savedPassword) != EOF) {
                if (strcmp(user.email, savedEmail) == 0) {
                    emailExists = 1;
                    printf("Oh noo! This email is already registered. Try another one.\n");
                    break;
                }
            }
            fclose(file);
        }
        
    } while (strstr(user.email, "@gmail.com") == NULL || emailExists);

    do {
        printf("Set your password (max 10 characters): ");
        scanf("%s", user.password);
        if (strlen(user.password) > 10) {
            printf("Password is too long! Maximum 10 characters allowed.\n");
        }
    } while (strlen(user.password) > 10);

    // ===== Hitung target kalori =====
    int targetKalori = hitungKaloriTarget(gender, age, height, weight, activityChoice, selectedGoals);
    printf("\nYour daily calorie target is: %d kcal\n", targetKalori);

    // Menyimpan data akun ke file
    file = fopen("akun.txt", "a"); // Mode append agar tidak menimpa data lama
    if (file != NULL) {
        fprintf(file, "%s %s %d\n", user.email, user.password, targetKalori);
        fclose(file);
        printf("Your account has been successfully created and saved!\n");
    } else {
        printf("Failed to save account. Please try again later.\n");
    }
}

// ----------------------------------------------------------------
//                            P A R T  2
//                          2.1 LOGIN AKUN
// ----------------------------------------------------------------
// Tujuan: Memverifikasi email dan password dari data akun yang tersimpan
void login() {
    char inputEmail[50], inputPassword[11];
    char savedEmail[50], savedPassword[11];
    int savedKalori;
    int found = 0;

    printf("\n===== Login =====\n");
    printf("Enter your email: ");
    scanf("%s", inputEmail);
    printf("Enter your password: ");
    scanf("%s", inputPassword);

    FILE *file = fopen("akun.txt", "r");
    if (file == NULL) {
        printf("No account found. Please create an account first.\n");
        return;
    }

    // Mengecek apakah email dan password cocok dengan yang tersimpan
    while (fscanf(file, "%s %s %d", savedEmail, savedPassword, &savedKalori) != EOF) {
        if (strcmp(inputEmail, savedEmail) == 0 && strcmp(inputPassword, savedPassword) == 0) {
            found = 1;
            break;
        }
    }
    fclose(file);

    if (found) {
        printf("Login successful! Welcome back, %s\n", inputEmail);
        printf("Your daily calorie target is: %d kcal\n", savedKalori);
        dashboard(inputEmail, savedKalori); // Mengarahkan user ke function 4.1 setelah login
    } else {
        printf("Login gagal. Email atau password salah.\n");
        int opsi;
        printf("1. Lupa Password\n2. Kembali\nPilihan: ");
        scanf("%d", &opsi);

        if (opsi == 1) {
            forgotPassword(); // Memanggil function 2.2 jika dipilih
        }
    }   
}

// ----------------------------------------------------------------
//                          2.2 LUPA PASSWORD
// ----------------------------------------------------------------
// Tujuan: Mengganti password jika email ditemukan
void forgotPassword() {
    char email[50], newPassword[11];
    char savedEmail[50], savedPassword[11];
    int found = 0;
    int count = 0;

    // Menyimpan data user sementara ke array struct
    struct {
        char email[50];
        char password[11];
        int kalori;
    } users[100];

    printf("\n===== Forgot Password =====\n");
    printf("Enter your registered email: ");
    scanf("%s", email);

    FILE *file = fopen("akun.txt", "r");
    if (file == NULL) {
        printf("No account data found.\n");
        return;
    }

    // Membaca semua akun dan cek apakah email ditemukan
    while (fscanf(file, "%s %s %d", users[count].email, users[count].password, &users[count].kalori) != EOF) {
        if (strcmp(email, users[count].email) == 0) {
            found = 1;
        }
        count++;
    }
    fclose(file);

    if (found) {
        // Minta password baru dan validasi panjangnya
        do {
            printf("Enter a new password (max 10 characters): ");
            scanf("%s", newPassword);
            if (strlen(newPassword) > 10) {
                printf("Password too long! Please try again.\n");
            }
        } while (strlen(newPassword) > 10);

        // Update password di array
        for (int i = 0; i < count; i++) {
            if (strcmp(email, users[i].email) == 0) {
                strcpy(users[i].password, newPassword);
            }
        }

        // Tulis ulang file dengan data yang telah diperbarui
        file = fopen("akun.txt", "w");
        for (int i = 0; i < count; i++) {
            fprintf(file, "%s %s %d\n", users[i].email, users[i].password, users[i].kalori);
        }
        fclose(file);

        printf("Password successfully updated!\n");
    } else {
        printf("Email not found. Please create an account first.\n");
    }
}