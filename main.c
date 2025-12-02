#include <stdio.h>

int BMI();
int BMI() {
    float weight, height, metric;

    printf("----BMI Calculator----\n");
    printf("Enter your weight: \n");
    scanf("%f", &weight);
    printf("Enter your height: \n");
    scanf("%f", &height);
    metric = weight / height * height;
    
}
//changes made by abdullah

int main() {
    int select;
    printf("------MAIN MENU--------\n");
    printf("Choose an option: \n");
    printf("1)Budget based recipe suggestions \n");
    printf("2)Recipe details \n");
    printf("3)Water Intake Tracker \n");
    printf("4)Weight Tracker \n");
    printf("5)BMI Calculator \n");
    scanf("%d", &select);
    switch(select) { 
        case 1:
        break;


        case 2:
        break;

        case 3:
        break;

        case 4:
        break;

        case 5:

        break;



    }

}
