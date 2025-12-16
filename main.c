#include <stdio.h>

// ABDULLAH //
// BMI CALCULATOR START //

/* Basic data available for use to everyone */
void userbasicdata(int *age, char *gender, float *weight, float *height, int system) {

    /* Gender check */
    while (1) {
        printf("Enter your gender (M/F): ");
        scanf(" %c", gender);

        if (*gender == 'M' || *gender == 'm' || *gender == 'F' || *gender == 'f') {
            break;

        } else {
            printf("Error: Invalid gender. Please enter M or F only.\n");
        }
    }

    /* Age check */
    while (1) {
        printf("Enter your age: ");
        scanf("%d", age);

        if (*age > 0) {
            break;
        } else {
            printf("Error: Age must be greater than 0. Please try again.\n");
        }
    }

    /* Weight, height input */
    if (system == 1) {
        printf("Enter your weight (pounds): ");
        scanf("%f", weight);
        printf("Enter your height (inches): ");
        scanf("%f", height);
    } else {
        printf("Enter your weight (kg): ");
        scanf("%f", weight);
        printf("Enter your height (meters): ");
        scanf("%f", height);
    }
}

/* BMI category */
void BMICategory(float bmi) {
    if (bmi < 16)
        printf("BMI Category: Severe Thinness\n");
    else if (bmi < 17)
        printf("BMI Category: Moderate Thinness\n");
    else if (bmi < 18.5)
        printf("BMI Category: Mild Thinness\n");
    else if (bmi < 25)
        printf("BMI Category: Normal\n");
    else if (bmi < 30)
        printf("BMI Category: Overweight\n");
    else if (bmi < 35)
        printf("BMI Category: Obese Class I\n");
    else if (bmi < 40)
        printf("BMI Category: Obese Class II\n");
    else
        printf("BMI Category: Obese Class III\n");
}

/* BMI actual function */
void BMI(float weight, float height, int system) {
    float bmi, minWeight, maxWeight;

    printf("\n---- BMI Calculator ----\n");

    if (system == 1) {
        bmi = 703 * (weight / (height * height));
        minWeight = (18.5 * height * height) / 703;
        maxWeight = (25.0 * height * height) / 703;

        printf("BMI: %.2f kg/m^2\n", bmi);
        printf("Healthy BMI Range: 18.5 - 25 kg/m^2\n");
        printf("Healthy Weight Range: %.2f lb - %.2f lb\n", minWeight, maxWeight);
    } 
    
    else {
        bmi = weight / (height * height);
        minWeight = 18.5 * height * height;
        maxWeight = 25.0 * height * height;

        printf("BMI: %.2f kg/m^2\n", bmi);
        printf("Healthy BMI Range: 18.5 - 25 kg/m^2\n");
        printf("Healthy Weight Range: %.2f kg - %.2f kg\n", minWeight, maxWeight);
    }

    BMICategory(bmi);
}

// BMI CALCULATOR END //
// ABDULLAH //

int main() {
    int age;
    float weight, height;
    char gender;
    int select;

    printf("------ MAIN MENU ------\n");
    printf("1) Budget based recipe suggestions\n");
    printf("2) Recipe details\n");
    printf("3) Water Intake Tracker\n");
    printf("4) Weight Tracker\n");
    printf("5) BMI Calculator\n");
    printf("Choose an option: ");
    scanf("%d", &select);

    switch (select) {

        case 1: {
            break;
        }

        case 2: {
            break;
        }

        case 3: {
            break;
        }

        case 4: {
            break;
        }

        // ABDULLAH //
        // BMI CALCULATOR INSIDE MAIN FUNCTION //
        case 5: {
            int system;

            printf("\nChoose measurement system:\n");
            printf("1) US Customary (pounds & inches)\n");
            printf("2) Metric (kg & meters)\n");
            printf("Your choice: ");
            scanf("%d", &system);

            userbasicdata(&age, &gender, &weight, &height, system);

            if (system == 1) {
                printf("\nUser Data: %c, %d years, %.2f lb, %.2f inches\n", gender, age, weight, height);
            } 
            
            else {
                printf("\nUser Data: %c, %d years, %.2f kg, %.2f meters\n", gender, age, weight, height);
            }

            BMI(weight, height, system);
            break;
        }

        default:
            printf("Option not implemented yet.\n");
    }

    return 0;
}
