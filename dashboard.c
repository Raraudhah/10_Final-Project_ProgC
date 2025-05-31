#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "dashboard.h"
#include "exercise.h"
#include "meal.h"
#include "utils.h"

#define MAX_LINE 256

// ----------------------------------------------------------------
//                             P A R T  4
//                           4.1 DASHBOARD
// ----------------------------------------------------------------
// Tujuan: Menampilkan ringkasan kalori harian pengguna serta opsi untuk log aktivitas atau melihat riwayat berdasarkan email
void dashboard(const char *email, int targetKalori) {
    int pilihan;
    char today[20];
    getToday(today, sizeof(today));

    // Reset nilai untuk dihitung ulang setiap loop
    int kaloriMakanHariIni = 0;
    int jumlahMakananHariIni = 0;
    int kaloriExerciseHariIni = 0;
    int jumlahExerciseHariIni = 0;
    char user[50], tanggal[20], waktu[20], mealType[20], food[50];
    int kalori;

    // ===== Hitung Total Kalori Makan Hari Ini Dari File Log Makanan =====
    char line[MAX_LINE];
    char type[20], desc[100];
    int duration, caloriesBurned, sets, reps;

    while (1) {
        FILE *fmeal = fopen("logs/calories_logs.txt", "r");
        if (fmeal) {
            while (fgets(line, sizeof(line), fmeal)) {

                // Format baris: user tanggal waktu mealType food kalori
                if (sscanf(line, "%s %s %s %s %s %d", user, tanggal, waktu, mealType, food, &kalori) == 6) {
                    // Jika log milik user yang sedang login dan untuk hari ini
                    if (strcmp(user, email) == 0 && strcmp(tanggal, today) == 0) {
                        kaloriMakanHariIni += kalori;
                        jumlahMakananHariIni++;
                    }
                }
            }
            fclose(fmeal);
        }

        // ===== Hitung Total Kalori Latihan Hari Ini Dari File Log Exercise =====
        FILE *fex = fopen("logs/exercise_logs.txt", "r");
        if (fex) {
            while (fgets(line, sizeof(line), fex)) {
                // Format baris: user tanggal waktu type desc duration sets reps caloriesBurned
                if (sscanf(line, "%s %s %s %s %s %d %d %d %d",
                           user, tanggal, waktu, type, desc, &duration, &sets, &reps, &caloriesBurned) >= 6) {
                    // Jika log milik user dan untuk hari ini 
                    if (strcmp(user, email) == 0 && strcmp(tanggal, today) == 0) {
                        kaloriExerciseHariIni += caloriesBurned;
                        jumlahExerciseHariIni++;
                    }
                }
            }
            fclose(fex);
        }

        // DASHBOARD
        printf("\n===== DASHBOARD =====\n");
        printf("Daily calorie target: %d kcal\n", targetKalori);
        printf("Today's meal calories: %d kcal\n", kaloriMakanHariIni);
        printf("Total meals today: %d\n", jumlahMakananHariIni);
        printf("Today's burned calories from exercise: %d kcal\n", kaloriExerciseHariIni);
        printf("Total exercises today: %d\n", jumlahExerciseHariIni);

        // MENU
        printf("\nMenu:\n");
        printf("1. DAILY EXERCISE\n");
        printf("2. DAILY MEAL\n");
        printf("3. Weekly Summary\n");
        printf("4. Logout\n");
        printf("Your choice: ");
        scanf("%d", &pilihan);
        getchar();

        // Eksekusi Menu
        if (pilihan == 1) {
            exerciseTrackMenu(email);
        } else if (pilihan == 2) {
            dailyMealMenu(email);
        } else if (pilihan == 3) {
            showWeeklySummary(email, targetKalori);
        } else if (pilihan == 4) {
            printf("Successfully logged out.\n");
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
}

// ----------------------------------------------------------------
//                      4.2 Ringkasan Seminggu
// ----------------------------------------------------------------
// Tujuan: Menampilkan summary kalori yang masuk dari konsumsi makanan
// dan pembakaran kalori selama 7 hari terakhir, based on 2 files log
// yaitu 'calories_logs.txt' dan '[email]_exercise_logs.txt'
void showWeeklySummary(const char *email, int targetKalori) {
    printf("======================= 7-DAY CALORIE SUMMARY =======================\n");
    printf("+------------+-------------------+--------------------+-------------------+\n");
    printf("| Date       | Meal Calories     | Exercise Calories  | Net Calories      |\n");
    printf("+------------+-------------------+--------------------+-------------------+\n");

    time_t now = time(NULL); 
    int totalMealAll = 0, totalExAll = 0, daysWithData = 0;

    // loop mundur dari hari ke-6 (7 hari lalu) sampai hari ini
    for (int i = 6; i >= 0; i--) {
        time_t t = now - i * 86400;
        struct tm *date = localtime(&t);
        char tanggal[20];
        strftime(tanggal, sizeof(tanggal), "%Y-%m-%d", date);

        int totalMeal = 0, totalExercise = 0;

        // Mengecek file calories (untuk meal) lalu menghitung total kalori meal
        FILE *fmeal = fopen("logs/calories_logs.txt", "r");
        if (fmeal) {
            char line[MAX_LINE];
            while (fgets(line, sizeof(line), fmeal)) {
                char u[50], d[20], w[20], m[20];
                int kalori;
                char *afterMealType;

                if (sscanf(line, "%s %s %s %s", u, d, w, m) != 4) continue;
                if (strcmp(u, email) != 0 || strcmp(d, tanggal) != 0) continue;

                afterMealType = strstr(line, m);
                if (!afterMealType) continue;
                afterMealType += strlen(m);
                afterMealType[strcspn(afterMealType, "\n")] = '\0';

                char *lastSpace = strrchr(afterMealType, ' ');
                if (!lastSpace) continue;
                kalori = atoi(lastSpace + 1);

                totalMeal += kalori;
            }
            fclose(fmeal);
        }

        // Mengecek file exercise lalu menghitung total kalori exercise
        char exFile[100];
        sprintf(exFile, "logs/%s_exercise_logs.txt", email);
        FILE *fex = fopen(exFile, "r");
        if (fex) {
            char line[MAX_LINE];
            while (fgets(line, sizeof(line), fex)) {
                char u[50], d[20], w[20], t[20], desc[100];
                int dur, sets, reps, cal;
                if (sscanf(line, "%s %s %s %s %s %d %d %d %d", u, d, w, t, desc, &dur, &sets, &reps, &cal) >= 6) {
                    if (strcmp(u, email) == 0 && strcmp(d, tanggal) == 0) {
                        totalExercise += cal;
                    }
                }
            }
            fclose(fex);
        }

        int netto = totalMeal - totalExercise;

        printf("| %-10s | %-17d | %-18d | %-17d |\n", tanggal, totalMeal, totalExercise, netto);

        // Total akumulasi
        if (totalMeal > 0 || totalExercise > 0) {
            totalMealAll += totalMeal;
            totalExAll += totalExercise;
            daysWithData++;
        }
    }

    printf("+------------+-------------------+--------------------+-------------------+\n");


    // Summary dan analisis
    if (daysWithData > 0) {
        double avgMeal = (double)totalMealAll / daysWithData;
        double avgEx = (double)totalExAll / daysWithData;
        double netto = avgMeal - avgEx;

        printf("\n----- Weekly Summary -----\n");
        printf("Avg. calories in      : %.0f kkal/hari\n", avgMeal);
        printf("Avg. calories out     : %.0f kkal/hari\n", avgEx);
        printf("Avg. calories net     : %.0f kkal/hari\n", netto);
        printf("Daily calorie target  : %d kkal\n", targetKalori);

        printf("\n------- A little note for you -------\n");

        // Pemberian narasi based on analisis
        if (netto > targetKalori + 200) {
            printf("Looks like you've had a huge calorie surplus this week.\n");
            printf("Maybe try slightly reducing your portions, or add a bit more physical activity to your routine.\n");
        } else if (netto < targetKalori - 200) {
            printf("You've been in a noticeable calorie deficit.\n");
            printf("If you're aiming to lose weight, that's okay! But if not, make sure you're getting enough energy to stay fueled.\n");
        } else {
            printf("Nice job! Your net calorie intake is quite close to your target. Keep up the healthy balance!\n");
        }

        if (avgEx < 150) {
            printf("Your activity level has been a bit low this week.\n");
            printf("Try going for short walks, doing light stretches, or sneaking in a few active breaks during the day.\n");
        } else if (avgEx > 400) {
            printf("You've been super active this week, amazing!\n");
            printf("Keep that energy going! Your body definitely appreciates the movement.\n");
        } else {
            printf("Your activity level is solid.\n");
            printf("Just remember to stay consistent. You're on the right track!\n");
        }
    } else {
        printf("\nData collection is incomplete for this week's analysis\n");
    }
}