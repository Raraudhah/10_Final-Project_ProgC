#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct User {
    char email[50];
    char password[11];
};

void createAccount();
void login();
void forgotPassword();

int main() {
    int choice;

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

    printf("\nJenis Kelamin (Male/Female): ");
    scanf("%s", gender);

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
        printf("Masukkan password baru: ");
        scanf("%s", newPassword);

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