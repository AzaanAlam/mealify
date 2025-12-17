#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_LINE_LENGTH 1024
#define MAX_FIELDS 10
#define WATER_GOAL_ML 2500

// --- UTILITY FUNCTIONS ---

// Clears the input buffer to prevent skipping scanf
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Get current date string (YYYY-MM-DD)
void getCurrentDate(char *buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

// Get current timestamp (YYYY-MM-DD HH:MM)
void getCurrentTimestamp(char *buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min);
}

// Print a styled header
void printHeader(char *title) {
    printf("\n==========================================\n");
    printf("              Mealify                       \n");
    printf("          %s \n", title);
    printf("==========================================\n");
}

// --- SMART CSV PARSER (Fixes the "Not getting CSV" issue) ---
// Handles quoted fields like "Flour, Salt" correctly.
void parseCSVRow(char *line, char *fields[], int max_fields) {
    int field_idx = 0;
    int in_quote = 0;
    int i = 0, j = 0;
    static char buffer[MAX_LINE_LENGTH]; // Temp buffer to reconstruct fields
    
    // Initialize first field pointer
    fields[field_idx] = buffer;
    
    for (i = 0; line[i] != '\0' && line[i] != '\n'; i++) {
        if (line[i] == '"') {
            in_quote = !in_quote; // Toggle quote state, don't copy the quote char
        } else if (line[i] == ',' && !in_quote) {
            // End of field
            buffer[j++] = '\0'; // Terminate string
            field_idx++;
            if (field_idx >= max_fields) break;
            fields[field_idx] = &buffer[j]; // Point to start of next field
        } else {
            buffer[j++] = line[i]; // Copy character
        }
    }
    buffer[j] = '\0'; // Null terminate the last field
}

// =================================================================
// MODULE: RECIPE SUGGESTER
// =================================================================

int isCalorieAllowed(int calories, int bmiChoice) {
    if (bmiChoice == 1 && calories >= 500) return 1;          // Underweight
    if (bmiChoice == 2 && (calories >= 300 && calories <= 800)) return 1; // Normal
    if (bmiChoice == 3 && calories <= 500) return 1;          // Overweight
    if (bmiChoice == 4 && calories <= 400) return 1;          // Obese
    return 0;
}

void showRecipes(char filename[], int bmiChoice) {
    FILE *fp = fopen(filename, "r");
    if (!fp) { 
        printf("[Error] Could not open '%s'.\nMake sure the .csv file is in the same folder as this program.\n", filename); 
        return; 
    }

    char line[MAX_LINE_LENGTH];
    char *fields[MAX_FIELDS];
    int found = 0;
    
    // Skip header
    fgets(line, sizeof(line), fp);

    printHeader("Recommended Recipes");
    printf("Source: %s\n", filename);

    while (fgets(line, sizeof(line), fp)) {
        // Use Smart Parser instead of strtok
        parseCSVRow(line, fields, 5);
        
        // Fields: 0=Budget, 1=Dish, 2=Ingredients, 3=Calories, 4=Recipe
        char *dish = fields[1];
        char *ing = fields[2];
        int cal = atoi(fields[3]); // atoi handles strings like "350" correctly now
        char *rec = fields[4];
        char *cost = fields[0];

        if (isCalorieAllowed(cal, bmiChoice)) {
            found = 1;
            printf("\nDish: %s\n", dish);
            printf("Cost: Rs. %s | Cal: %d kcal\n", cost, cal);
            printf("Ingredients: %s\n", ing);
            printf("Recipe: %s\n", rec);
            printf("------------------------------------------");
        }
    }
    
    if (!found) printf("\nNo recipes found matching your criteria in this file.\n");
    fclose(fp);
    
    printf("\nPress Enter to return...");
    getchar();
}

void runRecipeSuggester() {
    int budget, bmiChoice;
    printHeader("Budget Recipes");
    printf("Select BMI Goal:\n1. Underweight (Gain)\n2. Normal (Maintain)\n3. Overweight (Lose)\n4. Obese (Strict Lose)\n");
    printf("Choice: ");
    if (scanf("%d", &bmiChoice) != 1 || bmiChoice < 1 || bmiChoice > 4) {
        printf("Invalid choice.\n"); clearInputBuffer(); return;
    }
    clearInputBuffer();

    printf("Enter Budget (Rs): ");
    if (scanf("%d", &budget) != 1) {
        printf("Invalid budget.\n"); clearInputBuffer(); return;
    }
    clearInputBuffer();

    if (budget <= 300) showRecipes("budget_0_300.csv", bmiChoice);
    else if (budget <= 1000) showRecipes("budget_300_1000.csv", bmiChoice);
    else if (budget <= 3000) showRecipes("budget_1000_3000.csv", bmiChoice);
    else if (budget <= 5000) showRecipes("budget_3000_5000.csv", bmiChoice);
    else printf("Budget too high for current database (Max 5000).\n");
}

// =================================================================
// MODULE: WATER TRACKER (File Based)
// =================================================================

int getTodaysWaterTotal() {
    FILE *fp = fopen("water_log.txt", "r");
    if (!fp) return 0;

    char line[256];
    char today[20];
    getCurrentDate(today); // e.g., "2023-10-25"
    
    int total = 0;
    while (fgets(line, sizeof(line), fp)) {
        // Line format: YYYY-MM-DD HH:MM,Amount
        // Check if line starts with today's date
        if (strncmp(line, today, 10) == 0) {
            // Parse amount (skip past comma)
            char *comma = strchr(line, ',');
            if (comma) {
                total += atoi(comma + 1);
            }
        }
    }
    fclose(fp);
    return total;
}

void logWater(int amount) {
    FILE *fp = fopen("water_log.txt", "a");
    if (!fp) { printf("[Error] Could not save water log.\n"); return; }
    
    char timestamp[30];
    getCurrentTimestamp(timestamp);
    fprintf(fp, "%s,%d\n", timestamp, amount);
    fclose(fp);
}

void runWaterTracker() {
    int choice;
    do {
        int current = getTodaysWaterTotal();
        
        printHeader("Water Tracker");
        printf("Daily Goal:   %d ml\n", WATER_GOAL_ML);
        printf("Today's Total: %d ml\n", current);
        
        // Logic to show status without a bar
        if (current >= WATER_GOAL_ML) {
            printf("\nSTATUS: GOAL REACHED! Great job staying hydrated.\n");
        } else {
            printf("STATUS: %d ml remaining to reach goal.\n", WATER_GOAL_ML - current);
        }

        printf("------------------------------------------\n");
        printf("1. Add Water\n");
        printf("2. View Full History (From File)\n");
        printf("3. Back to Main Menu\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) { clearInputBuffer(); continue; }
        clearInputBuffer();

        if (choice == 1) {
            int amount;
            printf("Enter amount (ml): ");
            if (scanf("%d", &amount) == 1 && amount > 0) {
                logWater(amount);
                printf("[Success] Added %d ml.\n", amount);
            } else {
                printf("[Error] Invalid amount.\n");
            }
            clearInputBuffer();
        } else if (choice == 2) {
            printHeader("Water History");
            FILE *fp = fopen("water_log.txt", "r");
            if (fp) {
                char line[256];
                printf("Timestamp        | Amount\n");
                printf("-----------------|-------\n");
                while(fgets(line, sizeof(line), fp)) {
                    printf("%s", line); // Line already has newline
                }
                fclose(fp);
            } else {
                printf("No history found.\n");
            }
            printf("\nPress Enter...");
            getchar();
        }

    } while (choice != 3);
}

// =================================================================
// MODULE: WEIGHT TRACKER (File Based)
// =================================================================

void logWeight(float weight) {
    FILE *fp = fopen("weight_log.txt", "a");
    if (!fp) { printf("[Error] Could not save weight log.\n"); return; }
    
    char date[20];
    getCurrentDate(date);
    // Format: Date,Weight
    fprintf(fp, "%s,%.2f\n", date, weight);
    fclose(fp);
}

void runWeightTracker() {
    int choice;
    do {
        printHeader("Weight Tracker");
        printf("1. Add Current Weight\n");
        printf("2. View History (From File)\n");
        printf("3. Back to Main Menu\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) { clearInputBuffer(); continue; }
        clearInputBuffer();

        if (choice == 1) {
            float w;
            printf("Enter Weight (kg): ");
            if (scanf("%f", &w) == 1 && w > 0) {
                logWeight(w);
                printf("[Success] Weight logged to file.\n");
            } else {
                printf("[Error] Invalid weight.\n");
            }
            clearInputBuffer();
        } else if (choice == 2) {
            printHeader("Weight History");
            FILE *fp = fopen("weight_log.txt", "r");
            if (fp) {
                char line[256];
                printf("Date       | Weight (kg)\n");
                printf("-----------|------------\n");
                while(fgets(line, sizeof(line), fp)) {
                    // Simple parsing to replace comma with pipe for display
                    char *comma = strchr(line, ',');
                    if(comma) *comma = ' '; 
                    printf("%s", line);
                }
                fclose(fp);
            } else {
                printf("No history found.\n");
            }
            printf("\nPress Enter...");
            getchar();
        }
    } while (choice != 3);
}

// =================================================================
// MODULE: BMI CALCULATOR
// =================================================================

void BMICategory(float bmi) {
    printf("BMI Category: ");
    if (bmi < 16) printf("Severe Thinness\n");
    else if (bmi < 17) printf("Moderate Thinness\n");
    else if (bmi < 18.5) printf("Mild Thinness\n");
    else if (bmi < 25) printf("Normal\n");
    else if (bmi < 30) printf("Overweight\n");
    else if (bmi < 35) printf("Obese Class I\n");
    else if (bmi < 40) printf("Obese Class II\n");
    else printf("Obese Class III\n");
}

void runBMICalculator() {
    printHeader("BMI Calculator");
    int system, age;
    float weight, height, bmi;
    char gender;

    printf("1. US Customary (lbs, inches)\n");
    printf("2. Metric (kg, meters)\n");
    printf("Choose System: ");
    if (scanf("%d", &system) != 1) { clearInputBuffer(); return; }
    clearInputBuffer();

    printf("Gender (M/F): ");
    scanf("%c", &gender);
    clearInputBuffer();

    printf("Age: ");
    scanf("%d", &age);
    clearInputBuffer();

    if (system == 1) {
        printf("Weight (lbs): "); scanf("%f", &weight);
        printf("Height (in): "); scanf("%f", &height);
        bmi = 703 * (weight / (height * height));
        printf("\nBMI: %.2f kg/m^2\n", bmi);
    } else {
        printf("Weight (kg): "); scanf("%f", &weight);
        printf("Height (m): "); scanf("%f", &height);
        bmi = weight / (height * height);
        printf("\nBMI: %.2f kg/m^2\n", bmi);
    }
    clearInputBuffer(); 
    BMICategory(bmi);
    
    printf("\nPress Enter to return...");
    getchar();
}

// =================================================================
// MAIN MENU
// =================================================================

int main() {
    int choice;
    while (1) {
        printHeader("MAIN MENU");
        printf("1. Budget Recipe Suggestions\n");
        printf("2. Water Intake Tracker\n");
        printf("3. Weight Tracker\n");
        printf("4. BMI Calculator\n");
        printf("5. Exit\n");
        printf("------------------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("[Error] Invalid input. Enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: runRecipeSuggester(); break;
            case 2: runWaterTracker(); break;
            case 3: runWeightTracker(); break;
            case 4: runBMICalculator(); break;
            case 5: printf("\nExiting. Stay Healthy!\n"); return 0;
            default: printf("\n[Error] Invalid Option.\n");
        }
    }
    return 0;
}

