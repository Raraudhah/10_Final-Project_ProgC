#ifndef MEAL_H
#define MEAL_H

void dailyMealMenu(const char *email);
void mealSubMenu(const char *email, const char *mealType);
void addFood(const char *email, const char *tipeMeal);
void printMealHistory(const char *email, const char *mealType);
void showMealHistory(const char *email);
int foodExistsInDataset(const char *namaMakananInput);
int lookupFoodCalories(const char *namaMakananInput);

#endif