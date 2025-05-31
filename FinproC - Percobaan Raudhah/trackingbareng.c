#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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
void printExerciseHistory(const char *email);
void addNewExercise(const char *email);
void dailyMealMenu(const char *email);
void mealSubMenu(const char *email, const char *mealType);
void printMealHistory(const char *email, const char *mealType);
void addFood(const char *email, const char *mealType);
void createDirectories();

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

    do {
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
    } while(1);

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
        printf("Jenis Kelamin:\n");
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

    printf("Berapa usia Anda (tahun): ");
    scanf("%d", &age);

    printf("Tinggi badan Anda (cm): ");
    scanf("%d", &height);

    printf("Berat badan Anda (kg): ");
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

// Fungsi baca kalori makanan dari dataset makanan
// Dataset makanan ada di folder "dataset/foods.txt" format:
// nama_makanan kalori
int lookupFoodCalories(const char *foodName) {
    FILE *file = fopen("dataset/foods.txt", "r");
    if (!file) {
        printf("Error: dataset makanan tidak ditemukan!\n");
        return -1;
    }
    char name[100];
    int calories;
    while (fscanf(file, "%s %d", name, &calories) != EOF) {
        if (strcmp(foodName, name) == 0) {
            fclose(file);
            return calories;
        }
    }
    fclose(file);
    return -1; // Tidak ditemukan
}

// Fungsi baca kalori exercise strength dari dataset
// Dataset ada di "dataset/exercises_strength.txt" format:
// description sets reps calories
// Contoh:
// pushup 3 10 50
int lookupStrengthExerciseCalories(const char *desc, int sets, int reps) {
    FILE *file = fopen("exercises.txt", "r");
    if (!file) {
        printf("Error: dataset strength exercise tidak ditemukan!\n");
        return -1;
    }
    char d[100];
    int s, r, cal;
    while (fscanf(file, "%s %d %d %d", d, &s, &r, &cal) != EOF) {
        if (strcmp(desc, d) == 0 && s == sets && r == reps) {
            fclose(file);
            return cal;
        }
    }
    fclose(file);
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
        FILE *fmeal = fopen("logs/meal_logs.txt", "r");
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

        // Tampilkan dashboard
        printf("\n=== DASHBOARD ===\n");
        printf("Target kalori harian: %d kkal\n", targetKalori);
        printf("Jumlah kalori hari ini dari makanan: %d kkal\n", kaloriMakanHariIni);
        printf("Jumlah makanan hari ini: %d\n", jumlahMakananHariIni);
        printf("Jumlah kalori terbakar dari exercise hari ini: %d kkal\n", kaloriExerciseHariIni);
        printf("Jumlah exercise hari ini: %d\n", jumlahExerciseHariIni);

        printf("\nMenu:\n");
        printf("1. EXERCISE TRACK\n");
        printf("2. DAILY MEAL\n");
        printf("3. Logout\n");
        printf("Pilihan Anda: ");
        scanf("%d", &pilihan);
        getchar();

        if (pilihan == 1) {
            exerciseTrackMenu(email);
        } else if (pilihan == 2) {
            dailyMealMenu(email);
        } else if (pilihan == 3) {
            printf("Logout berhasil.\n");
            break;
        } else {
            printf("Pilihan tidak valid.\n");
        }
    }
}

// --------------------------------------------------
// Fungsi menu Exercise Track
void exerciseTrackMenu(const char *email) {
    int pilih;
    while (1) {
        printf("\n=== EXERCISE TRACK ===\n");
        printf("1. History\n");
        printf("2. Add New Exercise\n");
        printf("3. Kembali\n");
        printf("Pilihan Anda: ");
        scanf("%d", &pilih);
        getchar();

        if (pilih == 1) {
            // Tampilkan history exercise user 7 hari terakhir
            printExerciseHistory(email);
        } else if (pilih == 2) {
            addNewExercise(email);
        } else if (pilih == 3) {
            break;
        } else {
            printf("Pilihan tidak valid.\n");
        }
    }
}

void printExerciseHistory(const char *email) {
    FILE *f = fopen("logs/exercise_logs.txt", "r");
    if (!f) {
        printf("Belum ada history exercise.\n");
        return;
    }
    char line[MAX_LINE];
    int found = 0;
    printf("\n--- Exercise History 7 Hari Terakhir ---\n");
    printf("Tanggal     Waktu     Jenis       Deskripsi       Durasi/Set-Reps    Kalori Burned\n");

    while (fgets(line, sizeof(line), f)) {
        char user[50], tanggal[20], waktu[20], type[20], desc[100];
        int duration, sets, reps, calories;

        if (sscanf(line, "%s %s %s %s %s %d %d %d %d",
                   user, tanggal, waktu, type, desc, &duration, &sets, &reps, &calories) >= 6) {
            if (strcmp(user, email) == 0 && isWithin7Days(tanggal)) {
                found = 1;
                if (strcmp(type, "cardio") == 0) {
                    printf("%-11s %-9s %-11s %-15s %-14d menit %8d kkal\n",
                           tanggal, waktu, "Cardio", desc, duration, calories);
                } else {
                    printf("%-11s %-9s %-11s %-15s %d sets x %d reps %5d kkal\n",
                           tanggal, waktu, "Strength", desc, sets, reps, calories);
                }
            }
        }
    }
    fclose(f);
    if (!found) {
        printf("Tidak ada data exercise dalam 7 hari terakhir.\n");
    }
}

// Fungsi add new exercise
void addExercise(const char *email) {
    int jenis;
    printf("\nPilih jenis exercise:\n");
    printf("1. Cardiovascular\n");
    printf("2. Strength\n");
    printf("Pilihan Anda: ");
    scanf("%d", &jenis);
    getchar();

    char desc[100], waktu[20];
    int duration, caloriesBurned;
    int sets, reps;

    // Waktu sekarang
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(waktu, sizeof(waktu), "%H:%M:%S", tm);
    char tanggal[20];
    strftime(tanggal, sizeof(tanggal), "%Y-%m-%d", tm);

    if (jenis == 1) {
        printf("Deskripsi exercise cardio: ");
        fgets(desc, sizeof(desc), stdin);
        desc[strcspn(desc, "\n")] = 0; // remove newline

        printf("Berapa lama (menit): ");
        scanf("%d", &duration);
        getchar();

        printf("Kalori terbakar: ");
        scanf("%d", &caloriesBurned);
        getchar();

        // Simpan ke file log
        FILE *f = fopen("logs/exercise_logs.txt", "a");
        if (f) {
            fprintf(f, "%s %s %s cardio %s %d 0 0 %d\n",
                    email, tanggal, waktu, desc, duration, caloriesBurned);
            fclose(f);
            printf("Exercise cardio berhasil ditambahkan.\n");
        } else {
            printf("Gagal menyimpan exercise.\n");
        }
    } else if (jenis == 2) {
        printf("Deskripsi exercise strength (misal: pushup): ");
        scanf("%s", desc);
        printf("Jumlah sets: ");
        scanf("%d", &sets);
        printf("Reps per set: ");
        scanf("%d", &reps);

        // Cari kalori dari dataset
        caloriesBurned = lookupStrengthExerciseCalories(desc, sets, reps);
        if (caloriesBurned < 0) {
            printf("Data kalori exercise strength tidak ditemukan di dataset.\n");
            printf("Kalori terbakar akan diset 0.\n");
            caloriesBurned = 0;
        }

        // Simpan ke file log
        FILE *f = fopen("logs/exercise_logs.txt", "a");
        if (f) {
            fprintf(f, "%s %s %s strength %s 0 %d %d %d\n",
                    email, tanggal, waktu, desc, sets, reps, caloriesBurned);
            fclose(f);
            printf("Exercise strength berhasil ditambahkan.\n");
        } else {
            printf("Gagal menyimpan exercise.\n");
        }
    } else {
        printf("Pilihan jenis exercise tidak valid.\n");
    }
}

// --------------------------------------------------
// Fungsi menu Daily Meal
void dailyMealMenu(const char *email) {
    int pilih;
    while (1) {
        printf("\n=== DAILY MEAL ===\n");
        printf("1. Breakfast\n");
        printf("2. Lunch\n");
        printf("3. Dinner\n");
        printf("4. Kembali\n");
        printf("Pilihan Anda: ");
        scanf("%d", &pilih);
        getchar();

        if (pilih >= 1 && pilih <= 3) {
            char mealType[20];
            if (pilih == 1) strcpy(mealType, "breakfast");
            else if (pilih == 2) strcpy(mealType, "lunch");
            else strcpy(mealType, "dinner");

            mealSubMenu(email, mealType);
        } else if (pilih == 4) {
            break;
        } else {
            printf("Pilihan tidak valid.\n");
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
        printf("3. Kembali\n");
        printf("Pilihan Anda: ");
        scanf("%d", &pilih);
        getchar();

        if (pilih == 1) {
            printMealHistory(email, mealType);
        } else if (pilih == 2) {
            addFood(email, mealType);
        } else if (pilih == 3) {
            break;
        } else {
            printf("Pilihan tidak valid.\n");
        }
    }
}

void printMealHistory(const char *email, const char *mealType) {
    FILE *f = fopen("logs/meal_logs.txt", "r");
    if (!f) {
        printf("Belum ada history makanan.\n");
        return;
    }
    char line[MAX_LINE];
    int found = 0;
    char today[20];
    getToday(today, sizeof(today));

    printf("\n--- History %s 7 Hari Terakhir ---\n", mealType);
    printf("Tanggal     Waktu     Makanan       Kalori\n");

    while (fgets(line, sizeof(line), f)) {
        char user[50], tanggal[20], waktu[20], mt[20], food[50];
        int kalori;

        if (sscanf(line, "%s %s %s %s %s %d",
                   user, tanggal, waktu, mt, food, &kalori) == 6) {
            if (strcmp(user, email) == 0 && strcmp(mt, mealType) == 0 && isWithin7Days(tanggal)) {
                found = 1;
                printf("%-11s %-9s %-12s %d kkal\n", tanggal, waktu, food, kalori);
            }
        }
    }
    fclose(f);
    if (!found) {
        printf("Tidak ada data makanan %s dalam 7 hari terakhir.\n", mealType);
    }
}

void addFood(const char *email, const char *mealType) {
    char foodName[50];
    printf("Masukkan nama makanan (tanpa spasi, contoh: nasi, ayam): ");
    scanf("%s", foodName);

    int kalori = lookupFoodCalories(foodName);
    if (kalori < 0) {
        printf("Makanan tidak ditemukan di dataset.\n");
        return;
    }

    // Waktu sekarang
    char waktu[20], tanggal[20];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(waktu, sizeof(waktu), "%H:%M:%S", tm);
    strftime(tanggal, sizeof(tanggal), "%Y-%m-%d", tm);

    FILE *f = fopen("logs/meal_logs.txt", "a");
    if (f) {
        fprintf(f, "%s %s %s %s %s %d\n", email, tanggal, waktu, mealType, foodName, kalori);
        fclose(f);
        printf("Makanan berhasil ditambahkan.\n");
    } else {
        printf("Gagal menyimpan data makanan.\n");
    }
}