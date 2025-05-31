#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define MAX_LINE 256

struct User {
    char email[50];
    char password[11];
};

void createAccount();
void login();
void forgotPassword();
void dashboard(const char *email, int targetKalori);
void exerciseTrackMenu(const char *email);
void showExerciseHistory(const char *email);
void addNewExercise(const char *email);
void showMealHistory(const char *email);
void dailyMealMenu(const char *email);
void mealSubMenu(const char *email, const char *mealType);
void printMealHistory(const char *email, const char *mealType);
void addFood(const char *email, const char *mealType);
void createDirectories();
void showWeeklySummary(const char *email, int targetKalori);

int main() {
    int choice;

    // Create directories if they don't exist
    createDirectories();

    printf("=== SELAMAT DATANG ===\n");
    printf("1. Create Account\n");
    printf("2. Login\n");
    printf("Pilihan Anda: ");
    scanf("%d", &choice);
    getchar(); // Clear newline

    switch(choice) {
        case 1:
            createAccount();
            break;
        case 2:
            login();
            break;
        default:
            printf("Pilihan tidak valid.\n");
    }

    return 0;
}

void createDirectories() {
    #ifdef _WIN32
        system("mkdir logs 2>nul");
        system("mkdir dataset 2>nul");
    #else
        system("mkdir -p logs");
        system("mkdir -p dataset");
    #endif
}

int hitungKaloriTarget(char *gender, int umur, int tinggi, int berat, int aktivitasLevel, int *goals) {
    double bmr;
    
    // Hitung BMR
    if (strcmp(gender, "Male") == 0) {
        bmr = 10 * berat + 6.25 * tinggi - 5 * umur + 5;
    } else {
        bmr = 10 * berat + 6.25 * tinggi - 5 * umur - 161;
    }

    // Faktor aktivitas
    double faktorAktivitas;
    switch (aktivitasLevel) {
        case 1: faktorAktivitas = 1.2; break;
        case 2: faktorAktivitas = 1.375; break;
        case 3: faktorAktivitas = 1.55; break;
        case 4: faktorAktivitas = 1.725; break;
        default: faktorAktivitas = 1.2; break;
    }

    double kebutuhanKalori = bmr * faktorAktivitas;

    // Penyesuaian berdasarkan goals
    if (goals[0]) { // nurunin bb
        kebutuhanKalori -= 500;
    } else if (goals[1]) { // naikin bb
        kebutuhanKalori += 500;
    } else if (goals[2]) { // naikin tinggi
        kebutuhanKalori += 250;
    } else if (goals[3]) { // memperkuat otot
        kebutuhanKalori += 300;
    }

    return (int)kebutuhanKalori;
}

void createAccount() {
    struct User user;
    char savedEmail[50], savedPassword[11];
    int emailExists = 0;

    printf("\n=== Create Account ===\n");

    // === Goals ===
    const char *goalsList[] = {
        "Nurunin BB",
        "Naikin BB",
        "Naikin Tinggi",
        "Memperkuat Otot",
        "Rencana Makanan",
        "Manajemen Stres"
    };
    int selectedGoals[6] = {0};
    int totalSelected = 0;

    printf("Pilih goals Anda (maksimal 3, masukkan nomor dipisahkan spasi, contoh: 1 4 6):\n");
   for (int i = 0; i < 6; i++) {
        printf("%d. %s\n", i + 1, goalsList[i]);
    }

    printf("Pilihan Anda: ");
    char goalsInput[100];
    fgets(goalsInput, sizeof(goalsInput), stdin);

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

    // Validasi pilihan goals
    int bodyGoals = selectedGoals[0] + selectedGoals[1] + selectedGoals[2];
        
    if (totalSelected == 0) {
        printf("Anda harus memilih minimal 1 goals!\n");
        return;
    }
    if (totalSelected > 3) {
        printf("Tidak valid. Maksimal pilih 3 goals saja.\n");
        return;
    }
    if (bodyGoals > 1) {
        printf("Tidak valid. Untuk tujuan kondisi badan (nurunin/naikin BB/tinggi), hanya boleh pilih satu!\n");
        return;
    }

    // === Activity Level ===
    printf("\nApa tingkat aktivitas Anda? Pilih satu:\n");
    printf("1. Tidak terlalu aktif\n2. Cukup aktif\n3. Aktif\n4. Sangat aktif\n");
    int activityChoice;
    printf("Pilihan Anda (1-4): ");
    scanf("%d", &activityChoice);
    getchar(); // clear newline

    if (activityChoice < 1 || activityChoice > 4) {
        printf("Tidak valid. Anda harus memilih satu tingkat aktivitas.\n");
        return;
    }

    // === Data Diri ===
    char gender[10];
    int age, height, weight;

    do {
        printf("\nJenis Kelamin:\n");
        printf("1. Male\n2. Female\n");
        printf("Pilihan Anda (1-2): ");
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
            printf("Pilihan tidak valid. Pilih 1 atau 2.\n");
        }
    } while(1);

    printf("\nBerapa usia Anda (tahun): ");
    scanf("%d", &age);

    printf("\nTinggi badan Anda (cm): ");
    scanf("%d", &height);

    printf("\nBerat badan Anda (kg): ");
    scanf("%d", &weight);
    getchar(); // clear newline
   
    // === Email dan Password ===
    FILE *file;

    do {
        emailExists = 0;

        printf("\nMasukkan email (@gmail.com): ");
        scanf("%s", user.email);

        // Validasi domain
        if (strstr(user.email, "@gmail.com") == NULL) {
            printf("Email harus menggunakan domain @gmail.com!\n");
            continue;
        }

        // Cek apakah email sudah terdaftar
        file = fopen("akun.txt", "r");
        if (file != NULL) {
            while (fscanf(file, "%s %s", savedEmail, savedPassword) != EOF) {
                if (strcmp(user.email, savedEmail) == 0) {
                    emailExists = 1;
                    printf("Email sudah terdaftar! Gunakan email lain.\n");
                    break;
                }
            }
            fclose(file);
        }
        
    } while (strstr(user.email, "@gmail.com") == NULL || emailExists);

    do {
        printf("Masukkan password (maks 10 karakter): ");
        scanf("%s", user.password);
        if (strlen(user.password) > 10) {
            printf("Password terlalu panjang! Maksimum 10 karakter.\n");
        }
    } while (strlen(user.password) > 10);

    // === Hitung target kalori ===
    int targetKalori = hitungKaloriTarget(gender, age, height, weight, activityChoice, selectedGoals);
    printf("\nTarget kalori harian Anda: %d kkal\n", targetKalori);

    // Simpan akun baru
    file = fopen("akun.txt", "a"); // Gunakan append agar tidak menimpa akun lama
    if (file != NULL) {
        fprintf(file, "%s %s %d\n", user.email, user.password, targetKalori);
        fclose(file);
        printf("Akun berhasil dibuat dan disimpan!\n");
    } else {
        printf("Gagal menyimpan akun.\n");
    }
}

void login() {
    char inputEmail[50], inputPassword[11];
    char savedEmail[50], savedPassword[11];
    int savedKalori;
    int found = 0;

    printf("\n=== Login ===\n");
    printf("Email: ");
    scanf("%s", inputEmail);
    printf("Password: ");
    scanf("%s", inputPassword);

    FILE *file = fopen("akun.txt", "r");
    if (file == NULL) {
        printf("Belum ada akun. Silakan create account terlebih dahulu.\n");
        return;
    }

    while (fscanf(file, "%s %s %d", savedEmail, savedPassword, &savedKalori) != EOF) {
        if (strcmp(inputEmail, savedEmail) == 0 && strcmp(inputPassword, savedPassword) == 0) {
            found = 1;
            break;
        }
    }
    fclose(file);

    if (found) {
        printf("Login berhasil! Selamat datang.\n");
        printf("Target kalori harian Anda: %d kkal\n", savedKalori);
        dashboard(inputEmail, savedKalori);
    } else {
        printf("Login gagal. Email atau password salah.\n");
        int opsi;
        printf("1. Lupa Password\n2. Kembali\nPilihan: ");
        scanf("%d", &opsi);

        if (opsi == 1) {
            forgotPassword();
        }
    }   
}

void forgotPassword() {
    char email[50], newPassword[11];
    char savedEmail[50], savedPassword[11];
    int found = 0;

    struct {
        char email[50];
        char password[11];
        int kalori;
    } users[100];
    int count = 0;

    printf("\n=== Lupa Password ===\n");
    printf("Masukkan email Anda: ");
    scanf("%s", email);

    FILE *file = fopen("akun.txt", "r");
    if (file == NULL) {
        printf("Belum ada akun tersimpan.\n");
        return;
    }

    while (fscanf(file, "%s %s %d", users[count].email, users[count].password, &users[count].kalori) != EOF) {
        if (strcmp(email, users[count].email) == 0) {
            found = 1;
        }
        count++;
    }
    fclose(file);

    if (found) {
        do {
            printf("Masukkan password baru (maksimum 10 karakter): ");
            scanf("%s", newPassword);
            if (strlen(newPassword) > 10) {
                printf("Password terlalu panjang! Maksimum 10 karakter.\n");
            }
        } while (strlen(newPassword) > 10);

        for (int i = 0; i < count; i++) {
            if (strcmp(email, users[i].email) == 0) {
                strcpy(users[i].password, newPassword);
            }
        }

        file = fopen("akun.txt", "w");
        for (int i = 0; i < count; i++) {
            fprintf(file, "%s %s %d\n", users[i].email, users[i].password, users[i].kalori);
        }
        fclose(file);

        printf("Password berhasil diubah!\n");
    } else {
        printf("Email tidak ditemukan. Silakan create account terlebih dahulu.\n");
    }
}

// Fungsi helper untuk dapatkan tanggal hari ini string "YYYY-MM-DD"
void getToday(char *buffer, int size) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d", tm);
}

// Fungsi helper untuk cek apakah tanggal tsb dalam 7 hari terakhir dari hari ini
int isWithin7Days(const char *dateStr) {
    struct tm tm_date = {0};
    struct tm tm_today = {0};
    time_t t_date, t_today;

    // Parse input date "YYYY-MM-DD"
    sscanf(dateStr, "%4d-%2d-%2d",
           &tm_date.tm_year, &tm_date.tm_mon, &tm_date.tm_mday);
    tm_date.tm_year -= 1900; // years since 1900
    tm_date.tm_mon -= 1;     // months 0-11

    // Get today
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    tm_today = *lt;

    t_date = mktime(&tm_date);
    t_today = mktime(&tm_today);

    double diff = difftime(t_today, t_date);
    if (diff >= 0 && diff <= 7 * 86400) return 1;
    else return 0;
}

// Fungsi helper untuk dapatkan waktu sekarang dalam format "HH:MM:SS"
void getCurrentTime(char *buffer, int size) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(buffer, size, "%H:%M:%S", tm);
}

int foodExistsInDataset(const char *namaMakananInput) {
    FILE *fp = fopen("dataset/calories.txt", "r");
    if (!fp) return 0;

    char line[256];
    char kategori[100], nama[100], berat[20], kaloriStr[20], kj[20];

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^\n]", kategori, nama, berat, kaloriStr, kj) == 5) {
            if (strcasecmp(nama, namaMakananInput) == 0) {
                fclose(fp);
                return 1; // ditemukan
            }
        }
    }

    fclose(fp);
    return 0; // tidak ditemukan
}


int lookupFoodCalories(const char *namaMakananInput) {
    FILE *fp = fopen("dataset/calories.txt", "r");
    if (fp == NULL) return -1;

    char line[256];
    char kategori[100], nama[100], berat[20], kaloriStr[20], kj[20];

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^\n]", kategori, nama, berat, kaloriStr, kj) == 5) {
            if (strcasecmp(nama, namaMakananInput) == 0) {
                int kalori = 0;
                sscanf(kaloriStr, "%d", &kalori);
                fclose(fp);
                return kalori;
            }
        }
    }

    fclose(fp);
    return -1;
}


char *strcasestr_custom(const char *haystack, const char *needle);

// Fungsi baca kalori exercise strength dari dataset
// Dataset ada di "dataset/exercises_strength.txt" format:
// description sets reps calories
// Contoh:
// pushup 3 10 50
// Fungsi pencarian kalori berdasarkan aktivitas, durasi, dan berat
int lookupExerciseCalories(const char *description, int durasiMenit, int beratBadan) {
    FILE *fp = fopen("exercise.txt", "r");
    if (fp == NULL) {
        perror("Gagal membuka exercise.txt");
        return -1;
    }

    char line[512];
    char aktivitas[150];
    int kal130, kal155, kal180, kal205;

    // Lewati header
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        // Hapus newline
        line[strcspn(line, "\n")] = '\0';

        if (sscanf(line, "%[^\t]\t%d\t%d\t%d\t%d", aktivitas, &kal130, &kal155, &kal180, &kal205) == 5) {
            if (strcasestr_custom(aktivitas, description) != NULL) {
                fclose(fp);

                int perJamKalori;
                if (beratBadan <= 140) perJamKalori = kal130;
                else if (beratBadan <= 170) perJamKalori = kal155;
                else if (beratBadan <= 190) perJamKalori = kal180;
                else perJamKalori = kal205;

                return (perJamKalori * durasiMenit) / 60;
            }
        }
    }

    fclose(fp);
    return -1;
}

// Fungsi untuk hitung kalori burned exercise cardio (input manual)
// langsung input kalori burned dari user

// --------------------------------------------------
// Fungsi utama dashboard setelah login
// Parameter: email dan target kalori dari akun yang login
void dashboard(const char *email, int targetKalori) {
    int pilihan;
    char today[20];
    getToday(today, sizeof(today));

    while (1) {
        // Hitung kalori hari ini dari meal logs dan exercise logs
        int kaloriMakanHariIni = 0;
        int jumlahMakananHariIni = 0;
        int kaloriExerciseHariIni = 0;
        int jumlahExerciseHariIni = 0;

        // Baca logs makanan untuk user dan hari ini
        FILE *fmeal = fopen("logs/calories_logs.txt", "r");
        if (fmeal) {
            char line[MAX_LINE];
            while (fgets(line, sizeof(line), fmeal)) {
                char user[50], tanggal[20], waktu[20], mealType[20], food[50];
                int kalori;
                if (sscanf(line, "%s %s %s %s %s %d", user, tanggal, waktu, mealType, food, &kalori) == 6) {
                    if (strcmp(user, email) == 0 && strcmp(tanggal, today) == 0) {
                        kaloriMakanHariIni += kalori;
                        jumlahMakananHariIni++;
                    }
                }
            }
            fclose(fmeal);
        }

        // Baca logs exercise untuk user dan hari ini
        FILE *fex = fopen("logs/exercise_logs.txt", "r");
        if (fex) {
            char line[MAX_LINE];
            while (fgets(line, sizeof(line), fex)) {
                char user[50], tanggal[20], waktu[20], type[20], desc[100];
                int duration, caloriesBurned, sets, reps;

                // Format: user tanggal waktu type desc duration sets reps caloriesBurned
                // untuk cardio sets,reps=0
                if (sscanf(line, "%s %s %s %s %s %d %d %d %d",
                           user, tanggal, waktu, type, desc, &duration, &sets, &reps, &caloriesBurned) >= 6) {
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
        printf("Daily calorie target: %d kkal\n", targetKalori);
        printf("Today's meal calories: %d kkal\n", kaloriMakanHariIni);
        printf("Total meals today: %d\n", jumlahMakananHariIni);
        printf("Today's burned calories from exercise: %d kkal\n", kaloriExerciseHariIni);
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
//             R I N G K A S A N   S E M I N G G U
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

// ----------------------------------------------------------------
//         F U N G S I   M E N U  D A I L Y  E X E R C I S E
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
//               R I W A Y A T  E X E R C I S E
// ----------------------------------------------------------------
// Tujuan: Menampilkan riwayat exercise yang dilakukan user selama 7 hari terakhir
void showExerciseHistory(const char *email) {
    // membuat nama file log berdasarkan email user
    char filename[100];
    sprintf(filename, "logs/%s_exercise_logs.txt", email);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("You haven't logged any exercises yet.\n");
        return;
    }

    char line[MAX_LINE];
    int found = 0; // variabel untuk mengecek apakah data valid

    // Header tampilan history
    printf("\n--- 7-Day Exercise History ---\n");
    printf("Date     Time     Category       Description       Duration/Set-Reps    Calories Burned\n");

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
                    printf("%-11s %-9s %-11s %-15s %2d menit %13d kkal\n",
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

// Mencari substring (needle) dalam string tanpa memperdulikan kapital dan huruf kecil
char *strcasestr_custom(const char *haystack, const char *needle) {
    if (!*needle) return (char *) haystack;
    for (; *haystack; haystack++) {
        const char *h = haystack;
        const char *n = needle;

        // Selama karakter masih sama dan belum habis, membandingkan tetap dilanjut
        while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n)) {
            h++;
            n++;
        }

        // Jika seluruh substring sudah selesai dicocokkan, return posisi
        if (!*n) return (char *) haystack;
    }

    return NULL;
}

// ----------------------------------------------------------------
//            F I T U R  A D D  N E W  E X E R C I S E
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
//         F U N G S I   M E N U  D A I L Y  M E A L
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


// Submenu untuk tiap jenis meal: history dan add food
void mealSubMenu(const char *email, const char *mealType) {
    int pilih;
    while (1) {
        printf("\n=== %s ===\n", mealType);
        printf("1. History\n");
        printf("2. Add Food\n");
        printf("3. Back\n");
        printf("Your choice: ");
        scanf("%d", &pilih);
        getchar();

        if (pilih == 1) {
            printMealHistory(email, mealType);
        } else if (pilih == 2) {
            addFood(email, mealType);
        } else if (pilih == 3) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
}

// ----------------------------------------------------------------
//                    R I W A Y A T  M E A L
// ----------------------------------------------------------------
// Tujuan: Menampilkan riwayat meal yang dilakukan user selama 7 hari terakhir

// Menampilkan meal history untuk satu mealType 
void printMealHistory(const char *email, const char *mealType) {
    FILE *f = fopen("logs/calories_logs.txt", "r");
    if (!f) {
        printf("You haven't log any meals yet.\n");
        return;
    }

    char line[MAX_LINE];
    int found = 0;

    // Header tampilan meal history
    printf("\n--- 7-Day %s History ---\n", mealType);
    printf("Date     Time     Meal                      Calories\n");

    // Membaca file baris per baris
    while (fgets(line, sizeof(line), f)) {
        char user[50], tanggal[20], waktu[20], mt[20];
        char makanan[150];
        int kalori;

        // Mengambil 4 elemen pertama
        int matched = sscanf(line, "%s %s %s %s", user, tanggal, waktu, mt);
        if (matched != 4) continue;

        // Mendapatkan pointer untuk parsing makanan + kalori
        char *afterMealType = strstr(line, mt);
        if (!afterMealType) continue;
        afterMealType += strlen(mt); // skip mealType

        // Membuang newline
        afterMealType[strcspn(afterMealType, "\n")] = '\0';

        // Mengambil kalori sebagai angka terakhir
        char *lastSpace = strrchr(afterMealType, ' ');
        if (!lastSpace) continue;

        kalori = atoi(lastSpace + 1); // Mengonversi kalori
        *lastSpace = '\0';
        strcpy(makanan, afterMealType);

        // Mengecek kesesuaian user, mealType, dan tanggal selama 7 hari
        if (strcmp(user, email) == 0 &&
            strcmp(mt, mealType) == 0 &&
            isWithin7Days(tanggal)) {
            found = 1;

            // Mencetak jika data valid
            printf("%-11s %-9s %-27s %d kkal\n",
                   tanggal, waktu, makanan, kalori);
        }
    }

    fclose(f);

    if (!found) {
        printf("You haven't logged any %s in the last 7 days.\n", mealType);
    }
}

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
    printf("Date     Time     Type     Meal                      Calories\n");

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



void addFood(const char *email, const char *tipeMeal) {
    char namaMakanan[100];
    printf("Masukkan nama makanan: ");
    fgets(namaMakanan, sizeof(namaMakanan), stdin);
    namaMakanan[strcspn(namaMakanan, "\n")] = '\0';

    int kaloriPer100g;
    int kaloriTotal;
    int gram;

    if (!foodExistsInDataset(namaMakanan)) {
        printf("Makanan tidak ditemukan di dataset.\n");
        printf("Masukkan jumlah kalori untuk %s (per 100 gram): ", namaMakanan);
        scanf("%d", &kaloriPer100g);
        getchar();

        printf("Berapa gram yang Anda makan? ");
        scanf("%d", &gram);
        getchar();

        kaloriTotal = (kaloriPer100g * gram) / 100;
        printf("Kalori dihitung: %d kkal (dari %d gram)\n", kaloriTotal, gram);

        // Tambahkan ke dataset
        FILE *fappend = fopen("dataset/calories.txt", "a");
        if (fappend != NULL) {
            float kj = kaloriPer100g * 4.184;
            fprintf(fappend, "UserAdded,%s,100g,%d cal,%.0f kJ\n", namaMakanan, kaloriPer100g, kj);
            fclose(fappend);
            printf("Informasi %s berhasil ditambahkan ke dataset!\n", namaMakanan);
        } else {
            printf("Gagal menyimpan makanan ke dataset.\n");
        }

    } else {
        kaloriPer100g = lookupFoodCalories(namaMakanan);
        printf("Berapa gram yang Anda makan? ");
        scanf("%d", &gram);
        getchar();

        kaloriTotal = (kaloriPer100g * gram) / 100;
        printf("Kalori dihitung: %d kkal (dari %d gram)\n", kaloriTotal, gram);
    }

    // Simpan ke log
    char tanggal[20], waktu[20];
    getToday(tanggal, sizeof(tanggal));
    getCurrentTime(waktu, sizeof(waktu));

    FILE *fp = fopen("logs/calories_logs.txt", "a");
    if (fp != NULL) {
        fprintf(fp, "%s %s %s %s %s %d\n", email, tanggal, waktu, tipeMeal, namaMakanan, kaloriTotal);
        fclose(fp);
        printf("Data makanan berhasil disimpan!\n");
    } else {
        printf("Gagal menyimpan log.\n");
    }
}