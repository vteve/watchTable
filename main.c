#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIGNED_TYPE(typename) ({volatile typename v = 0; volatile typename v1 = v - 1; v > v1;})

typedef unsigned int uint;

typedef struct Item {
    int *info;
} Item;

typedef struct KeySpace {
    int busy;
    int key;
    int release;
    Item *info;
} KeySpace;

typedef struct Table {
    KeySpace *ks;
    int csize;
    int msize;
} Table;

int get_int(int *number) {
    int n, f = -10;
    char symbol;
    do {
        f = 0;
        n = scanf("%u", number);
        if (n < 0) {
            return 0;
        } else if (n == 0) {
            printf("Error! Repeat input, please:\n");
            scanf("%*[^\n]");
        } else if (*number < 0) {
            printf("Error! Repeat input, please:\n");
            scanf("%*[^\n]");
        } else if (n == 1) {
            f = scanf("%c", &symbol);
            while (symbol == ' ' || symbol == '\t') {
                f = scanf("%c", &symbol);
            }
            if (symbol != '\n') {
                scanf("%*[^\n]");
                printf("Error! Repeat input, please:\n");
            }
        }
    } while (n == 0 || *number < 0 || symbol != '\n');
    return 1;
}

int count_release(Table table, int new_key) {
    int i = 0, count = 1;
    while (i < table.csize) {
        if (table.ks[i].key == new_key) {
            count++;
        }
        i++;
    }
    return count;
}

int add(Table *table, int info, int key) {
    (*table).ks[(*table).csize].busy = 1;
    (*table).ks[(*table).csize].key = key;
    (*table).ks[(*table).csize].release = count_release(*table, (*table).ks[(*table).csize].key);
    (*table).ks[(*table).csize].info = malloc(sizeof(Item));
    (*table).ks[(*table).csize].info->info = malloc(sizeof(int));
    (*table).csize++;
    (*table).ks[(*table).csize - 1].info->info[0] = info;
    return 0;
}

int d_add(Table *table) {
    int key, info;
    if ((*table).csize == (*table).msize) {
        printf("You can't add more elements in the table. Try do something else.\n");
        return 1;
    }
    printf("Enter the key:\n");
    if (get_int(&key) != 1) {
        printf("Error! Restart program, please.\n");
        return 2;
    }
    printf("Enter the information:\n");
    if (get_int(&info) != 1) {
        printf("Error! Restart program, please.\n");
        return 2;
    }
    add(table, info, key);
    return 0;
}


int remove_without_reorg(Table *table, int flag, int delete_key, int release) {
    if (flag == 1) {
        int i = 0;
        while (i < table->csize) {
            if (table->ks[i].key == delete_key) {
                free((*table).ks[i].info->info);
                table->ks[i].info->info = NULL;
                table->ks[i].busy = 0;
            }
            i++;
        }
    } else {
        int i = 0;
        while (i < table->csize) {
            if (table->ks[i].key == delete_key && table->ks[i].release == release) {
                free(table->ks[i].info->info);
                table->ks[i].info->info = NULL;
                table->ks[i].busy = 0;
            }
            i++;
        }
    }
    return 0;
}

int d_remove_without_reorg(Table *table) {
    if (table->csize == 0) {
        printf("You can't delete elements from the table, because table is empty. Try do something else.\n");
        return 1;
    }
    printf("Enter the key would you like to delete:\n");
    int delete_key, flag2 = 0;
    do {
        if (get_int(&delete_key) != 1) {
            printf("Error! Restart program, please.\n");
            return 2;
        }
        int k = 0;
        while (k < table->csize) {
            if (table->ks[k].key == delete_key) {
                flag2 = 1;
                break;
            }
            k++;
        }
        if (flag2 != 1) {
            printf("Table doesn't contain this key. Try again:\n");
        }
    } while (flag2 != 1);
    printf("You would like to delete all releases of this key or only one version?\n");
    printf("Enter 1, if all releases.\n");
    printf("Enter 2, if only one version.\n");
    int flag = 0;
    while (flag != 1 && flag != 2) {
        if (get_int(&flag) != 1) {
            printf("Error! Restart program, please.\n");
            return 2;
        }
        if (flag != 2 && flag != 1) {
            printf("You've entered incorrect data. Try again:\n");
        }
    }
    int release = 0;
    if (flag == 2) {
        printf("Enter the release:\n");
        int max_release = count_release(*table, delete_key) - 1;
        do {
            if (get_int(&release) != 1) {
                printf("Error! Restart program, please.\n");
                return 2;
            }
            if (release > max_release || release == 0) {
                printf("You've entered incorrect data. Try again:\n");
            }
        } while (release > max_release || release == 0);
    }
    remove_without_reorg(table, flag, delete_key, release);
    return 0;
}

void shift_left(Table *table, int index) {
    for (int i = index; i < table->csize - 1; ++i) {
        table->ks[i].info[0] = table->ks[i + 1].info[0];
        table->ks[i].key = table->ks[i + 1].key;
        table->ks[i].busy = table->ks[i + 1].busy;
        table->ks[i].release = table->ks[i + 1].release;
    }
    --table->csize;
    free(table->ks[table->csize].info);
}

int find(Table *table, int find_key) {
    for (int i = 0; i < table->csize; ++i) {
        if (table->ks[i].key == find_key) {
            printf("Key number %d: %d\n", i + 1, table->ks[i].key);
            printf("Release: %d\n", table->ks[i].release);
            printf("Busy: %d\n", table->ks[i].busy);
            if (table->ks[i].info->info) {
                printf("Information: %d\n", *table->ks[i].info->info);
            } else {
                printf("Information: null\n");
            }
            printf("======================\n");
        }
    }
    return 0;
}

int d_find(Table *table) {
    if (table->csize == 0) {
        printf("Table is empty. Try to do something else.\n");
        return 1;
    }
    printf("Enter the key would you like to find:\n");
    int find_key, flag2 = 0;
    do {
        if (get_int(&find_key) != 1) {
            printf("Error! Restart program, please.\n");
            return 2;
        }
        int k = 0;
        while (k < table->csize) {
            if (table->ks[k].key == find_key) {
                flag2 = 1;
                break;
            }
            k++;
        }
        if (flag2 != 1) {
            printf("Table doesn't contain this key. Try again:\n");
        }
    } while (flag2 != 1);
    printf("======================\n");
    find(table, find_key);
    return 0;
}

int remove_with_reorg(Table *table) {
    int i = 0;
    while (i < table->csize) {
        if (table->ks[i].busy == 0) {
            shift_left(table, i);
        } else {
            ++i;
        }
    }
    return 0;
}

int d_remove_with_reorg(Table *table) {
    int flag = 0;
    for (int i = 0; i < table->csize; ++i) {
        if (table->ks[i].busy == 0) {
            flag = 1;
            break;
        }
    }
    if (flag == 0) {
        printf("Table doesn't contain elements, which can be deleted. Try do something else.\n");
        return 1;
    }
    remove_with_reorg(table);
    return 0;
}

int output(Table *table) {
    if (table->csize == 0) {
        printf("Table is empty. Try do something else.\n");
        return 1;
    }
    int i = 0;
    printf("======================\n");
    while (i < table->csize) {
        printf("Key number %d: %d\n", i + 1, table->ks[i].key);
        printf("Release: %d\n", table->ks[i].release);
        printf("Busy: %d\n", table->ks[i].busy);
        if (table->ks[i].info->info) {
            printf("Information: %d\n", *table->ks[i].info->info);
        } else {
            printf("Information: null\n");
        }
        printf("======================\n");
        i++;
    }
    return 0;
}

void free_table(Table *table) {
    int i = 0;
    while (i < table->csize) {
        if (table->ks[i].info->info) {
            free(table->ks[i].info->info);
        }
        if (table->ks[i].info) {
            free(table->ks[i].info);
        }
        i++;
    }
    if (table->ks) {
        free(table->ks);
    }
}


int dialog(const char *msgs[], int len_msg) {
    int rc;
    for (int i = 0; i < len_msg; ++i) {
        puts(msgs[i]);
    }
    do {
        if (get_int(&rc) != 1) {
            printf("Error! Restart program, please.\n");
            return -1;
        }
    } while (rc < 0 || rc > len_msg - 1);
    printf("%d\n", rc);
    return rc;
}

int reading_file(Table *table) {
    free_table(table);
    table->csize = 0;
    table->ks = (KeySpace *) malloc(sizeof(KeySpace) * table->msize);
    FILE *start;
    start = fopen("data.txt", "r");
    if (start == NULL) {
        printf("Вы не указали входной файл или передали несуществующий.\n");
        return 1;
    }
    while (!feof(start)) {
        char *b;
        int len_str = 0;
        int flag = 0;
        b = (char *) malloc(1);
        int flag2 = 0;
        while (flag != 1) {
            int lenBuf = 0;
            char buf[1];
            int gg = fscanf(start, "%1[^\n]%n", buf, &lenBuf);
            if (gg != 1) {
                flag = 1;
            }
            if (lenBuf == 1) {
                b = (char *) realloc(b, sizeof(char) * (len_str + 1));
                b[len_str] = buf[0];
                len_str++;
            }
        }
        if (flag2 == 0) {
            table->ks[table->csize].info = malloc(sizeof(Item));
            table->ks[table->csize].info->info = malloc(sizeof(int));
            table->ks[table->csize].key = atoi(b);
            table->ks[table->csize].busy = 1;
            table->ks[table->csize].release = count_release(*table, table->ks[table->csize].key);
            flag2 = 1;
        } else {
            table->ks[table->csize].info->info[0] = atoi(b);
            flag2 = 0;
        }
        fscanf(start, "%*1[\n]");
    }
    fclose(start);
    return 0;
}

char *create_str() {
    int len_str = 0;
    char *stdin_str = malloc(len_str + 2);
    char symbol = ' ';
    while (symbol != '\n') {
        stdin_str = realloc(stdin_str, len_str + 2);
        symbol = getchar();
        if (symbol == EOF) {
            printf("End of working.\n");
            free(stdin_str);
            return NULL;
        } else {
            stdin_str[len_str] = symbol;
            len_str++;
        }
    }
    stdin_str[len_str - 1] = '\0';
    return stdin_str;
}

int read_file(Table *table, FILE *start, char *st) {
    char buffer[256];
    if (start) {
        int flag2 = 0;
        while ((fgets(buffer, 256, start)) != NULL) {
            if (table->csize == table->msize){
                return 1;
            }
            if (strpbrk(buffer, "\"!@#$%^&*()_+-=QWERTYUIOP{}ASDFGHJKL:ZXCVBNM<>?qwertyuiop[]asdfghjkl;'/.,mnbvcxz',ЮБЬТИМСЧЯФЫВАПРОЛДЖЭЪХЗЩШГНЕКУЦЙйцукенгшщзхъэждлорпавыфячмитьбю.") != NULL){
                continue;
            }
            if (flag2 == 0) {
                table->ks[table->csize].info = malloc(sizeof(Item));
                table->ks[table->csize].info->info = malloc(sizeof(int));
                table->ks[table->csize].key = atoi(buffer);
                table->ks[table->csize].busy = 1;
                table->ks[table->csize].release = count_release(*table, table->ks[table->csize].key);
                flag2 = 1;
            } else {
                table->ks[table->csize].info->info[0] = atoi(buffer);
                table->csize++;
                flag2 = 0;
            }
        }
        if (flag2 == 1){
            table->ks[table->csize].busy = 0;
            free(table->ks[table->csize].info->info);
            free(table->ks[table->csize].info);
        }
        fclose(start);
    }
    free(st);
    return 0;
}

int d_read_file(Table *table){
    FILE *start;
    printf("Enter file name:\n");
    char *st = create_str(), st1[100], st2[100];
    start = fopen(st, "r");
    if (start == NULL) {
        printf("No such file exists.\n");
        free(st);
        return 1;
    }
    if (read_file(table, start, st) == 1){
        printf("Table is full. Try to do something else.\n");
    }
    return 0;
}

int main() {
    const char *msg[] = {"0. Quit",
                         "1. Add element in the table",
                         "2. Remove element in the table with reorganization",
                         "3. Remove element in the table without reorganization",
                         "4. Show element by a key in a the table",
                         "5. Show whole table",
                         "6. Read from the file"};
    const int len_msg = sizeof(msg) / sizeof(msg[0]);

    Table table;
    table.csize = 0;
    printf("Enter the maximum count of elements in keyspace:\n");
    if (get_int(&table.msize) != 1) {
        printf("Error! Restart program, please.\n");
        return 0;
    }
    table.ks = (KeySpace *) malloc(sizeof(KeySpace) * table.msize);
    int (*fptr[])(Table *) = {NULL, d_add, d_remove_with_reorg, d_remove_without_reorg, d_find, output, d_read_file};
    int rc = 142412;
    while (rc != 0) {
        if ((rc = dialog(msg, len_msg)) == -1) {
            break;
        }
        if (rc == 0) {
            break;
        }
        if (fptr[rc](&table) == 2) {
            break;
        }
    }
    printf("That's all. Bye!\n");
    free_table(&table);
    return 0;
}