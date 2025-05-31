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

// Fungsi baca kalori makanan dari dataset makanan
// Dataset makanan ada di folder "dataset/foods.txt" format:
// nama_makanan kalori
// 

int lookupFoodCalories(const char *namaMakanan) {
    FILE *fp = fopen("dataset/foods.txt", "r");
    if (fp == NULL) {
        perror("Gagal membuka file foods.txt");
        return -1;
    }

    char line[256];
    char nama[100];
    int kalori;

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "%[^\t]\t%d", nama, &kalori) == 2) {
            if (strcasecmp(nama, namaMakanan) == 0) {  // tidak case-sensitive
                fclose(fp);
                return kalori;
            }
        }
    }

    fclose(fp);
    return -1;  // Tidak ditemukan
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
            showExerciseHistory(email);
        } else if (pilih == 2) {
            addNewExercise(email);
        } else if (pilih == 3) {
            break;
        } else {
            printf("Pilihan tidak valid.\n");
        }
    }
}

void showExerciseHistory(const char *email) {
    char filename[100];
    sprintf(filename, "logs/%s_exercise_logs.txt", email);
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Belum ada history exercise.\n");
        return;
    }

    char line[MAX_LINE];
    int found = 0;
    printf("\n--- Exercise History 7 Hari Terakhir ---\n");
    printf("Tanggal     Waktu     Jenis       Deskripsi       Durasi/Set-Reps    Kalori Burned\n");

    while (fgets(line, sizeof(line), fp)) {
        char user[50], tanggal[20], waktu[20], type[20], desc[100];
        int duration, sets, reps, calories;

        if (sscanf(line, "%s %s %s %s %s %d %d %d %d",
                   user, tanggal, waktu, type, desc, &duration, &sets, &reps, &calories) >= 6) {
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
        printf("Tidak ada data exercise dalam 7 hari terakhir.\n");
    }
}

// Implementasi fungsi custom di bawah, buat ngeganti strcaster
char *strcasestr_custom(const char *haystack, const char *needle) {
    if (!*needle) return (char *) haystack;

    for (; *haystack; haystack++) {
        const char *h = haystack;
        const char *n = needle;

        while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n)) {
            h++;
            n++;
        }

        if (!*n) return (char *) haystack;
    }

    return NULL;
}

// Menambahkan exercise baru
void addNewExercise(const char *email) {
    int choice;
    printf("\n=== Add New Exercise ===\n");
    printf("1. Cardiovascular\n");
    printf("2. Strength\n");
    printf("Pilihan Anda: ");
    scanf("%d", &choice);

    char description[100];
    int kalori = 0, durasi = 0, beratkg = 0;
    int sets = 0, reps = 0;
    getchar(); // flush newline

    printf("Deskripsi aktivitas (harus sesuai dataset): ");
    fgets(description, sizeof(description), stdin);
    description[strcspn(description, "\n")] = '\0';

    printf("Masukkan berat badan Anda saat ini (kg): ");
    scanf("%d", &beratkg);
    int beratlb = beratkg * 2.20462;
    printf("Berat badan Anda dalam pound: %d lb\n", beratlb);

    if (choice == 1) {
        // Cardiovascular
        printf("Durasi (menit): ");
        scanf("%d", &durasi);
    } else if (choice == 2) {
        // Strength
        printf("Jumlah set: ");
        scanf("%d", &sets);
        printf("Reps per set: ");
        scanf("%d", &reps);
        durasi = sets; // Asumsi 1 set = ±1 menit
    } else {
        printf("Pilihan tidak valid.\n");
        return;
    }

    // Hitung kalori otomatis dari dataset
    kalori = lookupExerciseCalories(description, durasi, beratlb);
    if (kalori == -1) {
        printf("Aktivitas tidak ditemukan di dataset. Input manual kalori: ");
        scanf("%d", &kalori);
    } else {
        printf("Kalori dihitung otomatis: %d kkal\n", kalori);
    }

    // Simpan ke log
    char tanggal[20], waktu[20];
    getToday(tanggal, sizeof(tanggal));
    getCurrentTime(waktu, sizeof(waktu));

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
        printf("Data exercise berhasil disimpan.\n");
    } else {
        printf("Gagal menyimpan log.\n");
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

void addFood(const char *email, const char *tipeMeal) {
    char namaMakanan[100];
    printf("Masukkan nama makanan: ");
    getchar();  // flush newline sebelumnya
    fgets(namaMakanan, sizeof(namaMakanan), stdin);
    namaMakanan[strcspn(namaMakanan, "\n")] = '\0'; // hapus newline

    int kalori = lookupFoodCalories(namaMakanan);
    if (kalori == -1) {
        printf("Makanan tidak ditemukan di dataset. Masukkan jumlah kalorinya: ");
        scanf("%d", &kalori);
    } else {
        printf("Makanan berhasil ditambahkan. Kalori: %d kkal\n", kalori);
    }

    // Simpan log ke file
    char filename[100];
    sprintf(filename, "logs/%s_meal_logs.txt", email);
    FILE *fp = fopen(filename, "a");
    if (fp != NULL) {
        fprintf(fp, "%s\t%s\t%d\n", tipeMeal, namaMakanan, kalori);
        fclose(fp);
    } else {
        printf("Gagal menyimpan log.\n");
    }
}