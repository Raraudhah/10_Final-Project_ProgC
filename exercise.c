#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "exercise.h"
#include "utils.h"

#define MAX_LINE 256

// ----------------------------------------------------------------
//                             P A R T  5
//                       5.1 Menu Daily Exercise
// ----------------------------------------------------------------
// Tujuan: Menampilkan menu kepada user sehigga fitur daily exercise dapat diakses
void exerciseTrackMenu(const char *email) {
    int pilih;

    // Menampilkan menu fitur yang ada di daily exercise
    while (1) {
        printf("\n===== DAILY EXERCISE =====\n");
        printf("1. History\n");
        printf("2. Add New Exercise\n");
        printf("3. Back\n");
        printf("Your choice: ");
        scanf("%d", &pilih);
        getchar();

        // Mengksekusi fitur daily exercise  
        if (pilih == 1) {
            showExerciseHistory(email);
        } else if (pilih == 2) {
            addNewExercise(email);
        } else if (pilih == 3) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
}

// ----------------------------------------------------------------
//                      5.2 Riwayat Exercise
// ----------------------------------------------------------------
// Tujuan: Menampilkan riwayat exercise yang dilakukan user selama 7 hari terakhir
void showExerciseHistory(const char *email) {
    // Membuat nama file log berdasarkan email user
    char filename[100];
    sprintf(filename, "logs/%s_exercise_logs.txt", email);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("You haven't logged any exercises yet.\n");
        return;
    }

    char line[MAX_LINE];
    int found = 0; // Variabel untuk mengecek apakah data valid

    // Header tampilan history
    printf("\n--- 7-Day Exercise History ---\n");
    printf("Date        Time      Category    Description     Duration/Set-Reps    Calories Burned\n");

    // Membaca file baris demi baris lalu menyimpan datanya sementara 
    while (fgets(line, sizeof(line), fp)) {
        char user[50], tanggal[20], waktu[20], type[20];
        char desc[100] = "";
        int duration = 0, sets = 0, reps = 0, calories = 0;

        // Mengambil 4 elemen awal dari baris
        if (sscanf(line, "%s %s %s %s", user, tanggal, waktu, type) != 4)
            continue;

        // Mengarahkan pointer ke bagian deskripsi dan angka setelah 4 elemen awal
        char *ptr = line;
        for (int i = 0; i < 4; i++) {
            ptr = strchr(ptr, ' ');
            if (ptr) ptr++;
            else break;
        }

        if (!ptr) continue;

        // Parsing deskripsi + angka: durasi, sets, reps, kalori
        char tempDesc[100];
        int matched = sscanf(ptr, "%[^0-9] %d %d %d %d", tempDesc, &duration, &sets, &reps, &calories);
        if (matched >= 2) {
            strcpy(desc, tempDesc);
            desc[strcspn(desc, "\n")] = '\0';
            while (isspace(desc[strlen(desc) - 1])) desc[strlen(desc) - 1] = '\0';

            // Filtering data user saat ini dan menampilkan data selama 7 hari terakhir
            if (strcmp(user, email) == 0 && isWithin7Days(tanggal)) {
                found = 1;
                if (strcmp(type, "cardio") == 0) {
                    printf("%-11s %-9s %-11s %-15s %2d menit %15d kkal\n",
                           tanggal, waktu, "Cardio", desc, duration, calories);
                } else {
                    printf("%-11s %-9s %-11s %-15s %d sets x %d reps %5d kkal\n",
                           tanggal, waktu, "Strength", desc, sets, reps, calories);
                }
            }
        }
    }

    fclose(fp);
    if (!found) {
        printf("You haven't logged any exercise in the last 7 days.\n");
    }
} 

// ----------------------------------------------------------------
//                       5.3 Tambah Exercise
// ----------------------------------------------------------------
// Tujuan: Menambahkan data exercise baru dari user lalu menyimpannya ke file log pribadi user
void addNewExercise(const char *email) {
    int choice;
    printf("\n=== Add New Exercise ===\n");
    printf("1. Cardiovascular\n");
    printf("2. Strength\n");
    printf("Your choice: ");
    scanf("%d", &choice);

    char description[100];
    int kalori = 0, durasi = 0, beratkg = 0;
    int sets = 0, reps = 0;
    getchar(); // flush newline

    // Input yang perlu diberikan oleh user
    printf("Activity description: ");
    fgets(description, sizeof(description), stdin);
    description[strcspn(description, "\n")] = '\0';

    printf("Enter your current weight (kg): ");
    scanf("%d", &beratkg);
    int beratlb = beratkg * 2.20462;
    printf("Your body weight (lbs): %d lb\n", beratlb);

    // Input yang perlu diberikan oleh user berdasarkan kategori exercise
    if (choice == 1) {
        // Cardiovascular
        printf("Duration (minute): ");
        scanf("%d", &durasi);
    } else if (choice == 2) {
        // Strength
        printf("Number of sets: ");
        scanf("%d", &sets);
        printf("Reps/set: ");
        scanf("%d", &reps);
        durasi = sets; // Asumsi 1 set = ±1 menit
    } else {
        printf("Invalid choice.\n");
        return;
    }

    // Mencari data kalori exercise dari dataset
    kalori = lookupExerciseCalories(description, durasi, beratlb);
    if (kalori == -1) {
        printf("We can't found this activity in our dataset. Enter calories manually: ");
        scanf("%d", &kalori);
    } else {
        printf("Automatically calculated calories: %d kkal\n", kalori);
    }

    // Mengatur tanggal dan waktu sekarang
    char tanggal[20], waktu[20];
    getToday(tanggal, sizeof(tanggal));
    getCurrentTime(waktu, sizeof(waktu));

    // Menyimpan data ke file log user
    char filename[100];
    sprintf(filename, "logs/%s_exercise_logs.txt", email);
    FILE *fp = fopen(filename, "a");
    if (fp != NULL) {
        if (choice == 1) {
            // Cardio
            fprintf(fp, "%s %s %s cardio %s %d 0 0 %d\n", email, tanggal, waktu, description, durasi, kalori);
        } else {
            // Strength
            fprintf(fp, "%s %s %s strength %s %d %d %d %d\n", email, tanggal, waktu, description, durasi, sets, reps, kalori);
        }
        fclose(fp);
        printf("Your exercise has been saved.\n");
    } else {
        printf("Error. Could not save log.\n");
    }
}

// ----------------------------------------------------------------
//                     5.4 HITUNG KALORI OLAHRAGA
// ----------------------------------------------------------------
// Tujuan: Mencari jumlah kalori aktivitas olahraga berdasarkan berat & durasi dari dataset "exercise.txt"
int lookupExerciseCalories(const char *description, int durasiMenit, int beratBadan) {
    FILE *fp = fopen("exercise.txt", "r");
    if (fp == NULL) {
        perror("Oops! We couldn’t open the exercise data.");
        return -1;
    }

    char line[512];
    char aktivitas[150];
    int kal130, kal155, kal180, kal205;
    int perJamKalori;

    // Melewati baris header
    fgets(line, sizeof(line), fp);

    // Membaca file baris per baris
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';

        // Mengambil data dari tiap kolom
        if (sscanf(line, "%[^\t]\t%d\t%d\t%d\t%d", aktivitas, &kal130, &kal155, &kal180, &kal205) == 5) {
            // Jika deskripsi ditemukan dalam aktivitas (tanpa case sensitive)
            if (strcasestr_custom(aktivitas, description) != NULL) {
                fclose(fp);
                
                // Menentukan kalori per jam sesuai berat badan    
                if (beratBadan <= 140) perJamKalori = kal130;
                else if (beratBadan <= 170) perJamKalori = kal155;
                else if (beratBadan <= 190) perJamKalori = kal180;
                else perJamKalori = kal205;
                
                // Menghitung total kalori
                return (perJamKalori * durasiMenit) / 60;
            }
        }
    }
    fclose(fp);
    return -1;
}