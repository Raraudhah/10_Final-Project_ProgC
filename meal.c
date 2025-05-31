#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meal.h"
#include "utils.h"

#define MAX_LINE 256

// ----------------------------------------------------------------
//                             P A R T  6
//                         6.1 Menu Daily Meal
// ----------------------------------------------------------------
// Tujuan: Menampilkan menu kepada user sehigga fitur daily meal dapat diakses
void dailyMealMenu(const char *email) {
    int pilih;

    // Menampilkan menu fitur yang ada di daily meal
    while (1) {
        printf("\n===== DAILY MEAL =====\n");
        printf("1. Meal History\n");
        printf("2. Add Food\n");
        printf("3. Back\n");
        printf("Your choice: ");
        scanf("%d", &pilih);
        getchar();

        // Mengeksekusi fitur daily meal
        if (pilih == 1) {
            showMealHistory(email);
        } else if (pilih == 2) {
            printf("\nPick your meal type:\n");
            printf("1. Breakfast\n2. Lunch\n3. Dinner\nYour choice: ");
            int mealTypeChoice;
            scanf("%d", &mealTypeChoice);
            getchar();

            char mealType[20];
            if (mealTypeChoice == 1) strcpy(mealType, "breakfast");
            else if (mealTypeChoice == 2) strcpy(mealType, "lunch");
            else strcpy(mealType, "dinner");

            addFood(email, mealType);
        } else if (pilih == 3) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
}

// ----------------------------------------------------------------
//                   6.2 Riwayat Semua MealType
// ----------------------------------------------------------------
// Menampilkan semua meal history untuk semua mealType
void showMealHistory(const char *email) {
    FILE *f = fopen("logs/calories_logs.txt", "r");
    if (!f) {
        printf("You haven't log any meals yet.\n");
        return;
    }

    char line[MAX_LINE];
    int found = 0; // Variabel untuk validasi

    printf("\n--- 7-Day Meal History ---\n");
    printf("Date        Time      Type       Meal                       Calories\n");

    // loop membaca tiap baris dalam file log
    while (fgets(line, sizeof(line), f)) {
        char user[50], tanggal[20], waktu[20], mealType[20];
        char makanan[150];
        int kalori;

        // Mengambil bagian awal (4 elemen)
        int matched = sscanf(line, "%s %s %s %s", user, tanggal, waktu, mealType);
        if (matched != 4) continue;

        // Mengambil string setelah mealType
        char *afterMealType = strstr(line, mealType);
        if (!afterMealType) continue;
        afterMealType += strlen(mealType); // skip mealType

        // Membuang newline
        afterMealType[strcspn(afterMealType, "\n")] = '\0';

        // Mencari spasi terakhir dalam bagian makanan dan kalori yang diasumsikan diikuti kalori
        char *lastSpace = strrchr(afterMealType, ' ');
        if (!lastSpace) continue;
        kalori = atoi(lastSpace + 1); 
        *lastSpace = '\0'; 
        strcpy(makanan, afterMealType);

        // Filtering data user saat ini dan menampilkannya
        if (strcmp(user, email) == 0 && isWithin7Days(tanggal)) {
            found = 1;
            printf("%-11s %-9s %-9s %-27s %d kkal\n",
                   tanggal, waktu, mealType, makanan, kalori);
        }
    }

    fclose(f);

    if (!found) {
        printf("You haven't logged any meals in the last 7 days.\n");
    }
}

// ----------------------------------------------------------------
//                         6.3 Tambah Makanan
// ----------------------------------------------------------------
// Tujuan: Mencatat makanan yang dikonsumsi user, menghitung kalorinya, dan menyimpannya ke log harian.
void addFood(const char *email, const char *tipeMeal) {
    char namaMakanan[100];
    int kaloriPer100g;
    int kaloriTotal;
    int gram;

    // Meminta input nama makanan dari user
    printf("Please enter the name of the food: ");
    fgets(namaMakanan, sizeof(namaMakanan), stdin);
    namaMakanan[strcspn(namaMakanan, "\n")] = '\0'; // Menghapus newline

    // Jika makanan tidak ditemukan di dataset
    if (!foodExistsInDataset(namaMakanan)) {
        printf("This food is not found in our dataset.\n");
        printf("Please enter the estimated calories per 100 grams for %s: ", namaMakanan);
        scanf("%d", &kaloriPer100g);
        getchar();

        // Meminta jumlah gram yang dikonsumsi
        printf("How many grams of %s did you eat? ", namaMakanan);
        scanf("%d", &gram);
        getchar();

        kaloriTotal = (kaloriPer100g * gram) / 100;
        printf("Estimated calories: %d kcal (from %d grams).\n", kaloriTotal, gram);

        // Menyimpan makanan baru ke dalam dataset (sebagai UserAdded)
        FILE *fappend = fopen("dataset/calories.txt", "a");
        if (fappend != NULL) {
            float kj = kaloriPer100g * 4.184;
            fprintf(fappend, "UserAdded,%s,100g,%d cal,%.0f kJ\n", namaMakanan, kaloriPer100g, kj);
            fclose(fappend);
            printf("Thanks! %s has been added to the dataset.\n", namaMakanan);
        } else {
            printf("Oops! Failed to save the food to the dataset.\n");
        }
    } else {
        // Jika makanan sudah ada di dataset, ambil kalori per 100g
        kaloriPer100g = lookupFoodCalories(namaMakanan);
        printf("How many grams of %s did you eat? ", namaMakanan);
        scanf("%d", &gram);
        getchar();

        kaloriTotal = (kaloriPer100g * gram) / 100;
        printf("Estimated calories: %d kcal (from %d grams).\n", kaloriTotal, gram);
    }

    // Ambil tanggal dan waktu saat ini
    char tanggal[20], waktu[20];
    getToday(tanggal, sizeof(tanggal));
    getCurrentTime(waktu, sizeof(waktu));

    // Menyimpan data makanan ke dalam log harian
    FILE *fp = fopen("logs/calories_logs.txt", "a");
    if (fp != NULL) {
        fprintf(fp, "%s %s %s %s %s %d\n", email, tanggal, waktu, tipeMeal, namaMakanan, kaloriTotal);
        fclose(fp);
        printf("Your food has been logged successfully!\n");
    } else {
        printf("Failed to save the food log. Please try again.\n");
    }
}

// ----------------------------------------------------------------
//                     6.4 CEK KEBERADAAN MAKANAN
// ----------------------------------------------------------------
// Tujuan: Mengecek apakah nama makanan ada dalam dataset "calories.txt"
int foodExistsInDataset(const char *namaMakananInput) {
    FILE *fp = fopen("dataset/calories.txt", "r");
    if (!fp) return 0; // Gagal membuka file

    char line[256];
    char kategori[100], nama[100], berat[20], kaloriStr[20], kj[20];

    // Membaca file baris per baris
    while (fgets(line, sizeof(line), fp)) {
        // Parsing kolom dari format CSV
        if (sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^\n]", kategori, nama, berat, kaloriStr, kj) == 5) {
            // Bandingkan nama makanan (tidak case sensitive)
            if (strcasecmp(nama, namaMakananInput) == 0) {
                fclose(fp);
                return 1; // Ditemukan
            }
        }
    }
    fclose(fp);
    return 0; // Tidak ditemukan
}

// ----------------------------------------------------------------
//                     6.5 AMBIL KALORI MAKANAN
// ----------------------------------------------------------------
// Tujuan: Mengambil nilai kalori dari makanan jika ada dalam dataset
int lookupFoodCalories(const char *namaMakananInput) {
    FILE *fp = fopen("dataset/calories.txt", "r");
    if (fp == NULL) return -1; // Gagal membuka file

    char line[256];
    char kategori[100], nama[100], berat[20], kaloriStr[20], kj[20];

    // Membaca file baris per baris
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^\n]", kategori, nama, berat, kaloriStr, kj) == 5) {
            if (strcasecmp(nama, namaMakananInput) == 0) {
                int kalori = 0;
                sscanf(kaloriStr, "%d", &kalori); // Mengubah string jadi int
                fclose(fp);
                return kalori;
            }
        }
    }
    fclose(fp);
    return -1; // Tidak ditemukan
}