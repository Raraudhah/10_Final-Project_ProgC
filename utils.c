#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "utils.h"

// ----------------------------------------------------------------
//             F O L D E R:  'L O G S'  &  'D A T A S E T'
// ----------------------------------------------------------------
// Tujuan: Menyiapkan folder yang dibutuhkan program sesuai sistem operasi (Windows atau Linux/Mac)
void createDirectories() {
#ifdef _WIN32
    system("mkdir logs 2>nul");
    system("mkdir dataset 2>nul");
#else
    system("mkdir -p logs");
    system("mkdir -p dataset");
#endif
}

// ----------------------------------------------------------------
//                             P A R T  3
//                       3.1 TANGGAL HARI INI
// ----------------------------------------------------------------
// Tujuan: Mendapatkan tanggal hari ini dalam format "YYYY-MM-DD"
void getToday(char *buffer, int size) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d", tm);
}

// ----------------------------------------------------------------
//                         3.2 JAM SAAT INI
// ----------------------------------------------------------------
// Tujuan: Mendapatkan jam saat ini dalam format "HH:MM:SS"
void getCurrentTime(char *buffer, int size) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(buffer, size, "%H:%M:%S", tm);
}

// ----------------------------------------------------------------
//                     3.3 CEK 7 HARI TERKAHIR
// ----------------------------------------------------------------
// Tujuan: Mengecek apakah sebuah tanggal termasuk dalam 7 hari terakhir
int isWithin7Days(const char *dateStr) {
    struct tm tm_date = {0}, tm_today = {0};
    time_t t_date, t_today;

    // Mengubah string tanggal input ke struct tm
    sscanf(dateStr, "%4d-%2d-%2d", &tm_date.tm_year, &tm_date.tm_mon, &tm_date.tm_mday);
    tm_date.tm_year -= 1900; // Format tahun disesuaikan (sejak 1900)
    tm_date.tm_mon -= 1;     // Format bulan disesuaikan (0-11)

    // Menyimpan tanggal hari ini
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);
    tm_today = *lt;

    // Menghitung selisih waktu
    t_date = mktime(&tm_date);
    t_today = mktime(&tm_today);

    // Mengecek apakah selisih kurang dari atau sama dengan 7 hari
    double diff = difftime(t_today, t_date);
    return (diff >= 0 && diff <= 7 * 86400);
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