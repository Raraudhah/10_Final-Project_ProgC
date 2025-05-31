#ifndef EXERCISE_H
#define EXERCISE_H

void exerciseTrackMenu(const char *email);
void showExerciseHistory(const char *email);
void addNewExercise(const char *email);
int lookupExerciseCalories(const char *description, int durasiMenit, int beratBadan);

#endif