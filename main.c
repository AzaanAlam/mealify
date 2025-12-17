#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024
#define MAX_ENTRIES 100
#define WATER_GOAL_ML 2500

// --- STRUCTS ---
struct WeightEntry {
    float weight;
    char date[15]; // Format: DD-MM-YYYY
};

// --- UTILITY FUNCTIONS ---

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void printHeader(char *title) {
    printf("\n==========================================\n");
    printf("        MEALIFY   \n");
    printf("          %s \n", title);
    printf("==========================================\n");
}

// Check if date format is DD-MM-YYYY
int isValidDate(char *date) {
    int d, m, y;
    if (strlen(date) != 10) return 0;
    if (date[2] != '-' || date[5] != '-') return 0;
    if (sscanf(date, "%d-%d-%d", &d, &m, &y) != 3) return 0;
    
    if (y < 1900 || y > 2100) return 0;
    if (m < 1 || m > 12) return 0;
    
    int daysInMonth = 31;
    if (m == 4 || m == 6 || m == 9 || m == 11) daysInMonth = 30;
    if (m == 2) {
        if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) daysInMonth = 29;
        else daysInMonth = 28;
    }
    if (d < 1 || d > daysInMonth) return 0;
    
    return 1;
}

// Helper to parse date for sorting
void parseDate(char *date, int *d, int *m, int *y) {
    sscanf(date, "%d-%d-%d", d, m, y);
}

// Sorts entries chronologically
void sortEntriesByDate(struct WeightEntry entries[], int count) {
    struct WeightEntry temp;
    int d1, m1, y1, d2, m2, y2;
    
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            parseDate(entries[j].date, &d1, &m1, &y1);
            parseDate(entries[j+1].date, &d2, &m2, &y2);
            
            int swap = 0;
            if (y1 > y2) swap = 1;
            else if (y1 == y2 && m1 > m2) swap = 1;
            else if (y1 == y2 && m1 == m2 && d1 > d2) swap = 1;
            
            if (swap) {
                temp = entries[j];
                entries[j] = entries[j+1];
                entries[j+1] = temp;
            }
        }
    }
}

// --- SMART CSV PARSER ---
void parseCSVRow(char *line, char *fields[], int max_fields) {
    int field_idx = 0;
    int in_quote = 0;
    int i = 0, j = 0;
    static char buffer[MAX_LINE_LENGTH]; 
    
    fields[field_idx] = buffer;
    
    for (i = 0; line[i] != '\0' && line[i] != '\n'; i++) {
        if (line[i] == '"') {
            in_quote = !in_quote; 
        } else if (line[i] == ',' && !in_quote) {
            buffer[j++] = '\0'; 
            field_idx++;
            if (field_idx >= max_fields) break;
            fields[field_idx] = &buffer[j]; 
        } else {
            buffer[j++] = line[i]; 
        }
    }
    buffer[j] = '\0'; 
}

// =================================================================
// MODULE: RECIPES
// =================================================================
int isCalorieAllowed(int calories, int bmiChoice) {
    if (bmiChoice == 1 && calories >= 500) return 1;          
    if (bmiChoice == 2 && (calories >= 300 && calories <= 800)) return 1; 
    if (bmiChoice == 3 && calories <= 500) return 1;          
    if (bmiChoice == 4 && calories <= 400) return 1;          
    return 0;
}

void showRecipes(char filename[], int bmiChoice) {
    FILE *fp = fopen(filename, "r");
    if (!fp) { printf("[Error] Could not open %s\n", filename); return; }

    char line[MAX_LINE_LENGTH];
    char *fields[10];
    int found = 0;
    
    fgets(line, sizeof(line), fp); // Skip Header

    printHeader("Recommended Recipes");

    while (fgets(line, sizeof(line), fp)) {
        parseCSVRow(line, fields, 5);
        if (isCalorieAllowed(atoi(fields[3]), bmiChoice)) {
            found = 1;
            printf("\nDish: %s\nCost: Rs. %s | Cal: %s kcal\nIngredients: %s\nRecipe: %s\n", 
                   fields[1], fields[0], fields[3], fields[2], fields[4]);
            printf("------------------------------------------");
        }
    }
    if (!found) printf("\nNo matching recipes found.\n");
    fclose(fp);
    printf("\nPress Enter..."); getchar();
}

void runRecipeSuggester() {
    int budget, bmiChoice;
    printHeader("Budget Recipes");
    printf("1. Underweight (Gain)\n2. Normal (Maintain)\n3. Overweight (Lose)\n4. Obese (Strict Lose)\n");
    printf("Choice: ");
    scanf("%d", &bmiChoice); clearInputBuffer();
    printf("Budget (Rs): ");
    scanf("%d", &budget); clearInputBuffer();

    if (budget <= 300) showRecipes("budget_0_300.csv", bmiChoice);
    else if (budget <= 1000) showRecipes("budget_300_1000.csv", bmiChoice);
    else if (budget <= 3000) showRecipes("budget_1000_3000.csv", bmiChoice);
    else if (budget <= 5000) showRecipes("budget_3000_5000.csv", bmiChoice);
    else printf("Budget too high (Max 5000).\n");
}

// =================================================================
// MODULE: WATER TRACKER
// =================================================================

void logWater(char *date, int amount) {
    FILE *fp = fopen("water_log.txt", "a");
    if (!fp) return;
    fprintf(fp, "%s,%d\n", date, amount);
    fclose(fp);
}

int getWaterTotalForDate(char *targetDate) {
    FILE *fp = fopen("water_log.txt", "r");
    if (!fp) return 0;
    
    char line[100], fileDate[15];
    int amount, total = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        // Line format: DD-MM-YYYY,Amount
        sscanf(line, "%[^,],%d", fileDate, &amount);
        if (strcmp(fileDate, targetDate) == 0) {
            total += amount;
        }
    }
    fclose(fp);
    return total;
}

void runWaterTracker() {
    int choice;
    do {
        printHeader("Water Tracker");
        printf("1. Add Water Entry\n");
        printf("2. Check Total for a Date\n");
        printf("3. Back\n");
        printf("Choice: ");
        scanf("%d", &choice); clearInputBuffer();

        if (choice == 1) {
            char date[15];
            int amount;
            
            // Manual Date Input
            while(1) {
                printf("Enter Date (DD-MM-YYYY): ");
                scanf("%s", date); clearInputBuffer();
                if (isValidDate(date)) break;
                printf("[Error] Invalid Date. Try again.\n");
            }

            printf("Enter Amount (ml): ");
            scanf("%d", &amount); clearInputBuffer();
            
            if (amount > 0) {
                logWater(date, amount);
                printf("[Success] Logged %d ml for %s.\n", amount, date);
            }
        } else if (choice == 2) {
            char date[15];
            printf("Enter Date to check (DD-MM-YYYY): ");
            scanf("%s", date); clearInputBuffer();
            
            int total = getWaterTotalForDate(date);
            printf("\nTotal for %s: %d ml / %d ml\n", date, total, WATER_GOAL_ML);
            if (total >= WATER_GOAL_ML) printf("Goal Reached!\n");
            else printf("Remaining: %d ml\n", WATER_GOAL_ML - total);
            
            printf("\nPress Enter..."); getchar();
        }
    } while (choice != 3);
}

// =================================================================
// MODULE: WEIGHT TRACKER
// =================================================================

// Load data from file into struct array
int loadWeightEntries(struct WeightEntry entries[]) {
    FILE *fp = fopen("weight_log.txt", "r");
    if (!fp) return 0;
    
    int count = 0;
    char line[100];
    
    while (fgets(line, sizeof(line), fp) && count < MAX_ENTRIES) {
        // Format: DD-MM-YYYY,Weight
        char *comma = strchr(line, ',');
        if (comma) {
            *comma = '\0'; // Split string
            strcpy(entries[count].date, line);
            entries[count].weight = atof(comma + 1);
            count++;
        }
    }
    fclose(fp);
    return count;
}

void viewWeightProgress(struct WeightEntry entries[], int count) {
    if (count < 2) {
        printf("\n[Info] Need at least 2 entries for comparison.\n");
        return;
    }

    // Array is sorted, so count-1 is Latest, count-2 is Previous
    float current = entries[count - 1].weight;
    float previous = entries[count - 2].weight;
    float diff = current - previous;
    
    char *currDate = entries[count - 1].date;
    char *prevDate = entries[count - 2].date;

    printf("\n      PROGRESS REPORT       \n");
    printf("----------------------------\n");
    printf("Comparison: %s vs %s\n", prevDate, currDate);
    printf("Previous: %.1f kg\n", previous);
    printf("Current:  %.1f kg\n", current);
    printf("----------------------------\n");
    
    if (diff < 0) printf("You have LOST %.1f kg.\n", diff * -1);
    else if (diff > 0) printf("You have GAINED %.1f kg.\n", diff);
    else printf("Weight is STABLE.\n");
    printf("----------------------------\n");
}

void runWeightTracker() {
    struct WeightEntry entries[MAX_ENTRIES];
    int count = 0;
    int choice;

    do {
        // Reload and Sort every time we enter menu to ensure fresh data
        count = loadWeightEntries(entries);
        sortEntriesByDate(entries, count);

        printHeader("Weight Tracker");
        printf("1. Add New Weight Entry\n");
        printf("2. View History\n");
        printf("3. View Progress (Gain/Loss)\n");
        printf("4. Back\n");
        printf("Choice: ");
        scanf("%d", &choice); clearInputBuffer();

        if (choice == 1) {
            char date[15];
            float weight;
            
            while(1) {
                printf("Enter Date (DD-MM-YYYY): ");
                scanf("%s", date); clearInputBuffer();
                if (isValidDate(date)) break;
                printf("[Error] Invalid Date.\n");
            }
            
            printf("Enter Weight (kg): ");
            scanf("%f", &weight); clearInputBuffer();
            
            // Append to file
            FILE *fp = fopen("weight_log.txt", "a");
            if (fp) {
                fprintf(fp, "%s,%.2f\n", date, weight);
                fclose(fp);
                printf("[Success] Weight saved.\n");
            }
        } else if (choice == 2) {
            printf("\nDate        | Weight (kg)\n");
            printf("------------|------------\n");
            for (int i = 0; i < count; i++) {
                printf("%-11s | %.1f\n", entries[i].date, entries[i].weight);
            }
            printf("\nPress Enter..."); getchar();
        } else if (choice == 3) {
            viewWeightProgress(entries, count);
            printf("\nPress Enter..."); getchar();
        }

    } while (choice != 4);
}

// =================================================================
// MODULE: BMI CALCULATOR
// =================================================================
void runBMICalculator() {
    printHeader("BMI Calculator");
    int system;
    float weight, height, bmi;

    printf("1. US (lbs, in)\n2. Metric (kg, m)\nChoice: ");
    scanf("%d", &system); clearInputBuffer();

    if (system == 1) {
        printf("Weight (lbs): "); scanf("%f", &weight);
        printf("Height (in): "); scanf("%f", &height);
        bmi = 703 * (weight / (height * height));
    } else {
        printf("Weight (kg): "); scanf("%f", &weight);
        printf("Height (m): "); scanf("%f", &height);
        bmi = weight / (height * height);
    }
    clearInputBuffer();

    printf("\nBMI: %.2f -> ", bmi);
    if (bmi < 18.5) printf("Underweight\n");
    else if (bmi < 25) printf("Normal\n");
    else if (bmi < 30) printf("Overweight\n");
    else printf("Obese\n");
    
    printf("\nPress Enter..."); getchar();
}

// =================================================================
// MAIN
// =================================================================
int main() {
    int choice;
    while (1) {
        printHeader("MAIN MENU");
        printf("1. Recipe Suggestions\n");
        printf("2. Water Tracker\n");
        printf("3. Weight Tracker\n");
        printf("4. BMI Calculator\n");
        printf("5. Exit\n");
        printf("Choice: ");
        if (scanf("%d", &choice) != 1) { clearInputBuffer(); continue; }
        clearInputBuffer();

        switch (choice) {
            case 1: runRecipeSuggester(); break;
            case 2: runWaterTracker(); break;
            case 3: runWeightTracker(); break;
            case 4: runBMICalculator(); break;
            case 5: return 0;
            default: printf("Invalid.\n");
        }
    }
    return 0;
}
