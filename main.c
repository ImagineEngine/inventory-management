#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char filename[40] = "products.txt";
FILE *fp;

//try to add feature to export to xlsx

struct ProductNode {
    int prod_id;
    char name[50];
    int price;
    struct ProductNode * next;
};

typedef struct ProductNode ProductNode;

// ProductNode functions
ProductNode * new_product(int prod_id, char name[50], int price) {
    ProductNode * product = malloc(sizeof(ProductNode));
    product -> prod_id = prod_id;
    strcpy(product -> name, name);
    product -> price = price;
    return product;
}
//

struct ProductLL {
    ProductNode * head;
    int length;
    ProductNode * tail;
};

typedef struct ProductLL ProductLL;

// LinkedList functions
void append(ProductLL * productList, ProductNode * product) {
    if (productList -> length == 0) {
        productList -> head = product;
        productList -> tail = product;
    }
    else{
        productList -> tail -> next = product;
        productList -> tail = product;
    }
    productList -> length++;
}

void delete(ProductLL * productList, int index){
    if (index == 0) {
        ProductNode * free_node = productList -> head;
        productList -> head = productList -> head -> next;
        free(free_node);
        productList -> length--;
        return;
    }
    if (index >= productList->length) {
        return;
    }
    ProductNode * node = productList -> head;
    for (int i = 0; i < index - 1; i++) {
        node = node -> next;
    }
    ProductNode * free_node = node -> next;
    if (node -> next -> next == NULL) {
        productList->tail = node;
    }
    node -> next = node -> next -> next;
    free(free_node);
    productList -> length--;
}

int search(ProductLL * productList, int prod_id) {
    int length = productList -> length;
    if (length == 0) {
        return 0;
    }
    ProductNode * p = productList -> head;
    for (int i = 0; i < length; i++) {
        if (p -> prod_id == prod_id) {
            return i;
        }
        p = p -> next;
    }
    return -1;
}

// add insert function

struct Inventory {
    ProductLL * productList;
};

typedef struct Inventory Inventory;

// Inventory functions

Inventory * init_inventory() {
    Inventory * inventory = malloc(sizeof(Inventory));
    ProductLL * productList = malloc(sizeof(ProductLL));
    productList -> length = 0;
    inventory -> productList = productList;
    return inventory;
}

int add_product(Inventory * inventory, ProductNode * product) {
    if (search(inventory -> productList, product -> prod_id) == -1) {
        append(inventory -> productList, product);
        return 1;
    }
    return 0;
}

int delete_product(Inventory * inventory, int prod_id) {
    int inx = search(inventory -> productList, prod_id);
    if (inx >= 0) {
        delete(inventory -> productList, inx);
        return 1;
    }
    return 0;
}

Inventory * load_inventory() {
    FILE * fp;
    fp = fopen("products.txt", "r");
    Inventory * inventory = init_inventory();
    char c, id[50], name[100], price[50];
    int nid, nname, nprice = 0;
    int e = fscanf(fp, "%c", &c);
    while (e != EOF) {
        nid = nname = nprice = 0;
        while (c != '|') {
            id[nid++] = c;
            fscanf(fp, "%c", &c);
        }
        id[nid] = '\0';
        fscanf(fp, "%c", &c);
        while (c != '|') {
            name[nname++] = c;
            fscanf(fp, "%c", &c);
        }
        name[nname] = '\0';
        fscanf(fp, "%c", &c);
        while (c != '\n') {
            price[nprice++] = c;
            fscanf(fp, "%c", &c);
        }
        price[nprice] = '\0';
        //printf("%d %s %d\n", atoi(id), name, atoi(price));
        append(inventory -> productList, new_product(atoi(id), name, atoi(price)));
        e = fscanf(fp, "%c", &c);
    }

    fclose(fp);

    return inventory;
}

void save_inventory(Inventory * inventory) {
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("File could not be opened\n");
        return;
    }
    int length = inventory -> productList -> length;
    if (length == 0) {
        return;
    }
    ProductNode * p = inventory -> productList -> head;
    for (int i = 0; i < length; i++) {
        //printf("%d|%s|%d\n", p -> prod_id, p -> name, p -> price);
        fprintf(fp, "%d|%s|%d\n", p -> prod_id, p -> name, p -> price);
        p = p -> next;
    }
    fclose(fp);
}

void list_inventory(Inventory * inventory) {
    int length = inventory -> productList -> length;
    if (length == 0) {
        return;
    }
    ProductNode * p = inventory -> productList -> head;
    printf("ID\tname\tprice\n", p -> prod_id, p -> name, p -> price);
    for (int i = 0; i < length; i++) {
        printf("%d\t%s\t%d\n", p -> prod_id, p -> name, p -> price);
        p = p -> next;
    }
}


int main(void) {
    Inventory * inventory = load_inventory();

    printf("=====================================================\n");
    printf("         Welcome to Retail Management System!        \n");
    printf("=====================================================\n");
    printf("Use commands ['add', 'delete', 'save', 'exit', 'list']\n");
    printf("Type 'save' to modify inventory\n");

    int edited = 0;

    while (1) {
        char str[50];
        printf("cmd %c >> ", (edited == 0) ? ' ' : '*');
        scanf("%s", str);
        if (strcmp(str, "add") == 0) {
            int prod_id;
            char name[50];
            int price;
            printf("Enter Product ID: ");
            scanf(" %d", &prod_id);
            printf("Enter Product Name: ");
            scanf(" %[^\n]%*c", name);
            printf("Enter Product Price: ");
            scanf(" %d", &price);
            int status = add_product(inventory, new_product(prod_id, name, price));
            if (status == 1){
                printf("Added product [id: %d, name: %s, price: %d] to inventory\n", prod_id, name, price);
                edited = 1;
            }
            else {
                printf("Products must have a unique Product ID\n");
            }
        }
        else if (strcmp(str, "delete") == 0) {
            int prod_id;
            printf("Enter Product ID to delete: ");
            scanf("%d", &prod_id);
            int status = delete_product(inventory, prod_id);
            if (status == 1) {
                printf("Product deleted!\n");

                edited = 1;
            }
            else {
                printf("No product with Product ID: %d\n", prod_id);
            }
        }
        else if (strcmp(str, "save") == 0) {
            edited = 0;
            save_inventory(inventory);
        }
        else if (strcmp(str, "list") == 0) {
            list_inventory(inventory);
        }
        else if (strcmp(str, "exit") == 0) {
            if (edited) {
                printf("Exit without saving? (y/n) ");
                char b;
                scanf(" %c", &b);
                if (b == 'n') {
                    printf("Run command 'save' to save changes to the inventory\n");
                    continue;
                }
                if (b == 'y') {
                    break;
                }
            }
            printf("\nBye bye!");
            break;
        }
        else {
            printf("Enter a valid command!\n");
        }
    }
}
