#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ITEMS 20
#define TAX_RATE 0.025 
#define MAX_BILLS 100 

// Structure to store menu items
typedef struct {
    int id;
    char name[30];
    float price;
} MenuItem;

// Structure to store ordered items
typedef struct {
    char name[30];
    int quantity;
    float price;
    float total;
} Order;

// Structure to store a bill
typedef struct {
    int billNo;
    char customerName[50];
    char date[15];
    char paymentMode[10];
    float grandTotal;
    float sgst;
    float cgst;
    Order orders[ITEMS];
    int orderCount;
} Bill;

// Function to display the menu
void displayMenu(MenuItem menu[]) {
    printf("--------- Menu ---------\n");
    printf("ID   Item Name                 Price\n");
    for (int i = 0; i < ITEMS; i++) {
        printf("%-4d %-25s Rs. %.2f\n", menu[i].id, menu[i].name, menu[i].price);
    }
    printf("-------------------------\n");
}

// Function to display a bill
void displayBill(Bill bill) {
    printf("\n********** CENTRAL PERK **********\n");
    printf("Bill No.: %d\n", bill.billNo);
    printf("Customer Name: %s\n", bill.customerName);
    printf("Date: %s\n", bill.date);
    printf("-----------------------------------\n");
    printf("Sr.  Item Name                 Quantity  Price    Total\n");
    for (int i = 0; i < bill.orderCount; i++) {
        printf("%-4d %-25s %-9d Rs. %.2f   Rs. %.2f\n", i + 1, bill.orders[i].name, bill.orders[i].quantity, bill.orders[i].price, bill.orders[i].total);
    }
    printf("-----------------------------------\n");
    printf("Subtotal: Rs. %.2f\n", bill.grandTotal - bill.sgst - bill.cgst);
    printf("SGST (2.5%%): Rs. %.2f\n", bill.sgst);
    printf("CGST (2.5%%): Rs. %.2f\n", bill.cgst);
    printf("Grand Total: Rs. %.2f\n", bill.grandTotal);
    printf("Payment Mode: %s\n", bill.paymentMode);
    printf("-----------------------------------\n");
    printf("Address: CENTRAL PERK, MG Road, Pune, Maharashtra - 411001\n");
    printf("Contact Us: +91 9876543210\n");
    printf("***********************************\n");
}

// Function to save bills to a file
void saveBillsToFile(const char *filename, Bill bills[], int totalBills) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not save bills to file.\n");
        return;
    }

    fwrite(&totalBills, sizeof(int), 1, file);
    fwrite(bills, sizeof(Bill), totalBills, file);

    fclose(file);
    printf("Bills saved to file: %s\n", filename);
}

// Function to load bills from a file
int loadBillsFromFile(const char *filename, Bill bills[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not load bills from file.\n");
        return 0;
    }

    int totalBills;
    fread(&totalBills, sizeof(int), 1, file);
    fread(bills, sizeof(Bill), totalBills, file);

    fclose(file);
    return totalBills;
}

int main() {
    MenuItem menu[ITEMS] = {
        {1, "Pizza", 199.99}, {2, "Burger", 99.99}, {3, "Pasta", 149.99},
        {4, "French Fries", 99.99}, {5, "Salad", 89.99}, {6, "Soup", 79.99},
        {7, "Sandwich", 109.99}, {8, "Tacos", 119.99}, {9, "Ice Cream", 49.99},
        {10, "Brownie", 79.99}, {11, "Coffee", 39.99}, {12, "Tea", 24.99},
        {13, "Milkshake", 89.99}, {14, "Pizza Slice", 49.99}, {15, "Noodles", 129.99},
        {16, "Spring Roll", 99.99}, {17, "Paneer Tikka", 199.99}, {18, "Chicken Wings", 229.99},
        {19, "Mojito", 69.99}, {20, "Garlic Bread", 89.99}
    };

    Bill bills[MAX_BILLS];
    int totalBills = 0;
    int billNo = 1;
    int totalItemsSold = 0;
    float totalByUPI = 0, totalByCash = 0, totalByCard = 0;
    int option;

    printf("Enter the date for this session (DD_MM_YYYY): ");
    char sessionDate[15];
    scanf("%s", sessionDate);

    char filename[50];
    sprintf(filename, "%s.txt", sessionDate);

    // Load bills from file if exists
    totalBills = loadBillsFromFile(filename, bills);
    if (totalBills > 0) {
        printf("Loaded %d bills from %s\n", totalBills, filename);
        
        // Calculate summary after loading bills
        for (int i = 0; i < totalBills; i++) {
            for (int j = 0; j < bills[i].orderCount; j++) {
                totalItemsSold += bills[i].orders[j].quantity;
            }
            if (strcmp(bills[i].paymentMode, "UPI") == 0) {
                totalByUPI += bills[i].grandTotal;
            } else if (strcmp(bills[i].paymentMode, "Cash") == 0) {
                totalByCash += bills[i].grandTotal;
            } else if (strcmp(bills[i].paymentMode, "Card") == 0) {
                totalByCard += bills[i].grandTotal;
            }
        }

        // Display summary
        printf("\n***** SUMMARY *****\n");
        printf("Total Bills Loaded: %d\n", totalBills);
        printf("Total Items Sold: %d\n", totalItemsSold);
        printf("Total Amount by Payment Mode:\n");
        printf("  UPI: Rs. %.2f\n", totalByUPI);
        printf("  Cash: Rs. %.2f\n", totalByCash);
        printf("  Card: Rs. %.2f\n", totalByCard);
        printf("********************\n");
    }

    do {
        printf("\n***** CENTRAL PERK *****\n");
        printf("1. Add a Bill\n");
        printf("2. Search for a Bill\n");
        printf("3. View Summary\n");
        printf("4. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &option);
        getchar(); 

        switch (option) {
            case 1: {
                Bill currentBill;
                currentBill.billNo = billNo++;
                int itemId, quantity;
                float total = 0, sgst, cgst, grandTotal;

                printf("Enter your name: ");
                fgets(currentBill.customerName, sizeof(currentBill.customerName), stdin);
                currentBill.customerName[strcspn(currentBill.customerName, "\n")] = 0; 
                strcpy(currentBill.date, sessionDate);

                displayMenu(menu);

                printf("Start placing your order. Enter -1 as ID to stop ordering.\n");
                currentBill.orderCount = 0;

                // Taking orders
                while (1) {
                    printf("Enter Item ID: ");
                    scanf("%d", &itemId);
                    if (itemId == -1) {
                        break; 
                    }

                    printf("Enter Quantity: ");
                    scanf("%d", &quantity);

                    // Validate item ID
                    if (itemId >= 1 && itemId <= ITEMS) {
                        currentBill.orders[currentBill.orderCount].quantity = quantity;
                        strcpy(currentBill.orders[currentBill.orderCount].name, menu[itemId - 1].name);
                        currentBill.orders[currentBill.orderCount].price = menu[itemId - 1].price;
                        currentBill.orders[currentBill.orderCount].total = menu[itemId - 1].price * quantity;
                        total += currentBill.orders[currentBill.orderCount].total;
                        totalItemsSold += quantity; 
                        currentBill.orderCount++;
                    } else {
                        printf("Invalid item ID. Please try again.\n");
                    }
                }

                printf("Enter mode of payment (UPI/Cash/Card): ");
                scanf("%s", currentBill.paymentMode);

                sgst = total * TAX_RATE;
                cgst = total * TAX_RATE;
                grandTotal = total + sgst + cgst;
                currentBill.grandTotal = grandTotal;
                currentBill.sgst = sgst;
                currentBill.cgst = cgst;

                // Update payment mode totals
                if (strcmp(currentBill.paymentMode, "UPI") == 0) {
                    totalByUPI += grandTotal;
                } else if (strcmp(currentBill.paymentMode, "Cash") == 0) {
                    totalByCash += grandTotal;
                } else if (strcmp(currentBill.paymentMode, "Card") == 0) {
                    totalByCard += grandTotal;
                }

                // Store the current bill
                bills[totalBills++] = currentBill;

                displayBill(currentBill);
                break;
            }

            case 2: {
                char searchName[50];
                printf("Enter the customer name to search: ");
                fgets(searchName, sizeof(searchName), stdin);
                searchName[strcspn(searchName, "\n")] = 0; 

                int found = 0;
                for (int i = 0; i < totalBills; i++) {
                    if (strcmp(bills[i].customerName, searchName) == 0) {
                        displayBill(bills[i]);
                        found = 1;
                    }
                }

                if (!found) {
                    printf("No bill found for customer name: %s\n", searchName);
                }
                break;
            }

            case 3: {
                printf("\n***** SUMMARY *****\n");
                printf("Total Bills Generated: %d\n", totalBills);
                printf("Total Items Sold: %d\n", totalItemsSold);
                printf("Total Amount by Payment Mode:\n");
                printf("  UPI: Rs. %.2f\n", totalByUPI);
                printf("  Cash: Rs. %.2f\n", totalByCash);
                printf("  Card: Rs. %.2f\n", totalByCard);
                printf("********************\n");
                break;
            }

            case 4:
                saveBillsToFile(filename, bills, totalBills);
                printf("Exiting the program. Thank you!\n");
                break;

            default:
                printf("Invalid option. Please try again.\n");
                break;
        } 
    } while (option != 4);

    return 0;
}
