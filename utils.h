#ifndef UTILS_H
#define UTILS_H

void createDirectories();
void getToday(char *buffer, int size);
void getCurrentTime(char *buffer, int size);
int isWithin7Days(const char *dateStr);
char *strcasestr_custom(const char *haystack, const char *needle);

#endif