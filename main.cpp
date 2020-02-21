/*
 * Contact manager designed by Kininaru as a course design.
 * You can use any part of these codes freely, but not for your own course design, tks.
 * time: 2020/02/21 22:04
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct property {
    int num;
    char name[100][10];
    char consist[100][100];
}Property;

typedef struct contact {
    int number;
    char name[15];
    int sex;
    char classInfo[50];
    int duty;
    char phoneNumber[12];
    char email[31];
    Property other;
}Contact;

//string tools
int strToInt(char *str);
int emailCheck(char *email);

//system & ui
void showMainMenu();
void error(int n);
int cmdInterpreter(char cmd[][100], char source[]);
int cmdInterpreter0(char *cmd);
void printContactInfo(Contact buf);
void showEditHelpMenu();

//io
Contact *loadFromFile(FILE *fp, int *num);
void saveToFile(FILE *fp, Contact *list, int contactNumber);
Property readPropertyFromFile(FILE *fp);
void putPropertyToFile(FILE *fp, Property buf);

//manage
Contact getContactFromKeyboard();
void insertContactInNumberOrder(Contact *list, Contact buf, int num);
Contact *delContact(Contact *list, int contactNumber, int digit);

//contact editor
void contactEditor(Contact *buf);
void propertyEditor(Property *other);

int main() {
    char cmd[10][100], strbuf[1000];
    int argc, contactNum, intBuf;
    FILE *db;
    Contact *list, *newList;

    db = fopen("Contact.db", "a+");
    fclose(db);

    printf("Loading contacts from file...");
    db = fopen("Contact.db", "r");
    list = loadFromFile(db, &contactNum);
    if (list == 0) {
        error(1);
        fclose(db);
        return 1;
    }
    fclose(db);
    printf("Loading finished.\nTotal %d contact(s).\nNow input commands, use \"help\" for help.", contactNum);

    Command:
    printf("\ncmd>");
    gets(strbuf);
    argc = cmdInterpreter(cmd, strbuf);
    if (argc == 0)
        goto Command;

    switch (cmdInterpreter0(cmd[0])) {
        case 0:
            //exit
            printf("Saving to file...");
            db = fopen("Contact.db", "w");
            saveToFile(db, list, contactNum);
            fclose(db);
            free(list);
            printf("Finished.\nExiting...\n");
            break;
        case 1:
            //help
            showMainMenu();
            goto Command;
        case 2:
            //add
            newList = (Contact*)realloc(list, sizeof(Contact) * (contactNum + 1));
            if (newList == 0) {
                error(1);
                printf("Failed.\n");
                goto Command;
            }
            list = newList;
            contactNum++;
            insertContactInNumberOrder(list, getContactFromKeyboard(), contactNum);
            printf("Added.\n");
            goto Command;
        case 3:
            //edit
            if (argc != 2) {
                printf("Syntax: edit [contactNumber]\n");
                goto Command;
            }
            intBuf = strToInt(cmd[1]);
            printf("Finding this contact...");
            for (int i = 0; i < contactNum; ++i) {
                if (list[i].number == intBuf) {
                    printf("Found.\nEditing...");
                    contactEditor(&list[i]);
                    printf("Finished.\n");
                    goto Command;
                }
            }
            printf("Contact not found.\n");
            goto Command;
        case 4:
            //del
            if (argc != 3) {
                printf("Syntax: del [-i|-n] [moreInfo]\n");
                goto Command;
            }
            switch (cmd[1][1]) {
                case 'i':
                    intBuf = strToInt(cmd[2]);
                    printf("Finding this contact...");
                    for (int i = 0; i < contactNum; ++i) {
                        if (list[i].number == intBuf) {
                            printf("Found.\nDeleting...");
                            list = delContact(list, contactNum, i);
                            contactNum--;
                            printf("Deleted.\n");
                            goto Command;
                        }
                    }
                    printf("Contact not found.\n");
                    goto Command;
                case 'n':
                    printf("Finding this contact...");
                    for (int i = 0; i < contactNum; ++i) {
                        if (strcmp(list[i].name, cmd[2]) == 0) {
                            printf("Found.\nDeleting...");
                            list = delContact(list, contactNum, i);
                            contactNum--;
                            printf("Deleted.\n");
                            goto Command;
                        }
                    }
                    printf("Contact not found.\n");
                    goto Command;
                default:
                    printf("Unknown argument.\nSyntax: del [-i|-n] [moreInfo]\n");
                    goto Command;
            }
        case 5:
            //info
            if (argc < 2) {
                printf("Syntax: info [-n|-p|-c|-l|-a] [moreInfo]\n");
                goto Command;
            }
            switch (cmd[1][1]) {
                case 'n':
                    //name
                    if (argc != 3) {
                        printf("Syntax: info -n [name]\n");
                        goto Command;
                    }
                    printf("Finding this contact...");
                    for (int i = 0; i < contactNum; ++i) {
                        if (strcmp(list[i].name, cmd[2]) == 0) {
                            printf("Found.\n");
                            printContactInfo(list[i]);
                            goto Command;
                        }
                    }
                    printf("Contact not found.\n");
                    goto Command;
                case 'p':
                    //phone
                    if (argc != 3) {
                        printf("Syntax: info [-p] [phoneNumber]\n");
                        goto Command;
                    }
                    printf("Finding this contact...");
                    for (int i = 0; i < contactNum; ++i) {
                        if (strcmp(list[i].phoneNumber, cmd[2]) == 0) {
                            printf("Found.\n");
                            printContactInfo(list[i]);
                            goto Command;
                        }
                    }
                    printf("Contact not found.\n");
                    goto Command;
                case 'c':
                    //class
                    if (argc != 3) {
                        printf("Syntax: info [-c] [classInformation]\n");
                        goto Command;
                    }
                    for (int i = 0; i < contactNum; ++i) {
                        if (strcmp(list[i].classInfo, cmd[2]) == 0)
                            printContactInfo(list[i]);
                    }
                    goto Command;
                case 'l':
                    //is leader
                    for (int i = 0; i < contactNum; ++i) {
                        if (list[i].duty != 0)
                            printContactInfo(list[i]);
                    }
                    goto Command;
                case 'a':
                    //all
                    for (int i = 0; i < contactNum; ++i) {
                        printContactInfo(list[i]);
                    }
                    goto Command;
                default:
                    printf("Unknown argument.\nSyntax: info [-n|-p|-c|-l|-a] [moreInfo]\n");
                    goto Command;
            }
        case -1:
        default:
            error(2);
            goto Command;
            //unknown command
    }

    return 0;
}

void showMainMenu() {
    //finished
    printf("-----------------------------------command list-----------------------------------\n");//help 1, unknown cmd -1
    printf("add : add a contact\n");//2
    printf("edit [int] : edit the information of a contact\n");//3
    printf("del -i [int:number] : delete the info of contact whose number is [int]\n");//4
    printf("del -n [string:name] : delete the information of contact whose name is [string]\n");
    printf("info -n [string:name] : print the information of the contact whose name is [string]\n");//5
    printf("info -p [string:phone number] : print the information of the contact whose phone number is [string]\n");
    printf("info -c [string:class name] : print the information of the whole class in number order\n");
    printf("info -l : print all the leaders in number order\n");
    printf("info -a : print all the contact information\n");
    printf("exit : save all the changes and exit\n");//0
    printf("-----------------------------------command list-----------------------------------\n");
}

void showEditHelpMenu() {
    printf("-----------------------------------command list-----------------------------------\n");
    printf("name\nnumber\nsex\nclassInfo\nduty\nphoneNumber\nemail\nother\n");
    printf("-----------------------------------command list-----------------------------------\n");
}

void error(int n) {
    //finished
    switch (n) {
        case 1:
            printf("Sort of RAM. Failed.\n");
            break;
        case 2:
            printf("Unknown command.\n");
            break;
        case 3:
            printf("No such contact.\n");
            break;
        default:
            break;
    }
}

int cmdInterpreter(char cmd[][100], char source[]) {
    //finished
    int argc = 0, digit, cmdDigit;
    while (source[0] == '\n' || source[0] == ' ') {
        digit = 0;
        while (digit < strlen(source)) {
            source[digit] = source[digit + 1];
            digit++;
        }
    }
    if (strlen(source) < 1)
        return 0;
    while (source[strlen(source) - 1] == ' ') {
        source[strlen(source) - 1] = 0;
    }
    digit = 0;
    cmdDigit = 0;
    while (digit < strlen(source)) {
        if (source[digit] != ' ') {
            cmd[argc][cmdDigit] = source[digit];
            cmdDigit++;
        } else {
            if (source[digit - 1] == ' ') {
                digit++;
                continue;
            }
            cmd[argc][cmdDigit] = 0;
            cmdDigit = 0;
            argc++;
        }
        digit++;
    }
    cmd[argc][cmdDigit] = 0;
    argc++;
    return argc;
}

int cmdInterpreter0(char *cmd) {
    if (strcmp(cmd, "help") == 0)
        return 1;
    else if (strcmp(cmd, "add") == 0)
        return 2;
    else if (strcmp(cmd, "edit") == 0)
        return 3;
    else if (strcmp(cmd, "del") == 0)
        return 4;
    else if (strcmp(cmd, "info") == 0)
        return 5;
    else if (strcmp(cmd, "exit") == 0)
        return 0;
    else
        return -1;
}

Property readPropertyFromFile(FILE *fp) {
    //finished
    Property buf;
    fscanf(fp,"%d",&buf.num);
    for (int i = 0; i < buf.num; ++i) {
        fscanf(fp,"%s",buf.name[i]);
        fscanf(fp,"%s",buf.consist[i]);
    }
    return buf;
}

void putPropertyToFile(FILE *fp, Property buf) {
    //finished
    fprintf(fp,"%d\n",buf.num);
    for (int i = 0; i < buf.num; ++i) {
        fprintf(fp,"%s\n",buf.name[i]);
        fprintf(fp,"%s\n",buf.consist[i]);
    }
}

Contact *loadFromFile(FILE *fp, int *num) {
    //finished
    Contact *list;
    fscanf(fp,"%d",num);
    list = (Contact*)malloc(sizeof(Contact)*(*num));
    if (list == 0)
        return 0;
    for (int i = 0; i < *num; ++i) {
        fscanf(fp,"%d",&list[i].number);
        fscanf(fp,"%s",list[i].name);
        fscanf(fp,"%d",&list[i].sex);
        fscanf(fp,"%s",list[i].classInfo);
        fscanf(fp,"%d",&list[i].duty);
        fscanf(fp,"%s",list[i].phoneNumber);
        fscanf(fp,"%s",list[i].email);
        list[i].other=readPropertyFromFile(fp);
    }
    return list;
}


void saveToFile(FILE *fp, Contact *list, int contactNumber) {
    //finished
    fprintf(fp,"%d\n",contactNumber);
    for (int i = 0; i < contactNumber; ++i) {
        fprintf(fp,"%d\n",list[i].number);
        fprintf(fp,"%s\n",list[i].name);
        fprintf(fp,"%d\n",list[i].sex);
        fprintf(fp,"%s\n",list[i].classInfo);
        fprintf(fp,"%d\n",list[i].duty);
        fprintf(fp,"%s\n",list[i].phoneNumber);
        fprintf(fp,"%s\n",list[i].email);
        putPropertyToFile(fp,list[i].other);
    }
}

Contact getContactFromKeyboard() {
    //finished
    Contact newContact;
    int counter=0;
    char cmd, buffer[101];
    printf("Number>");
    scanf("%d",&newContact.number);
    printf("Name>");
    scanf("%s",newContact.name);
    printf("Sex [1.Man|2.Woman]>");
    scanf("%d",&newContact.sex);
    printf("Class information>");
    scanf("%s",newContact.classInfo);
    printf("Duty [0.no|1.leader|2.study]>");
    scanf("%d",&newContact.duty);
    printf("Phone>");
    scanf("%s",newContact.phoneNumber);
    Email:
    printf("Email>");
    scanf("%s", buffer);
    switch (emailCheck(buffer)) {
        case 1:
            printf("Email too long, shorter than 30 chars. Enter again.\n");
            goto Email;
        case 2:
            printf("Too many \'@\', please check your input.\n");
            goto Email;
        case 0:
            strcpy(newContact.email, buffer);
    }
    printf("Add other properties? [Y/N]>");
    getchar();
    cmd=getchar();
    getchar();
    while (cmd == 'Y' || cmd == 'y') {
        printf("Property name, [SPACE] is not allowed>");
        scanf("%s",newContact.other.name[counter]);
        printf("Property contain, [SPACE] is not allowed>");
        scanf("%s",newContact.other.consist[counter]);
        counter++;
        printf("Add another? [Y/N]>");
        getchar();
        scanf("%c",&cmd);
    }
    newContact.other.num=counter;
    return newContact;
}

void insertContactInNumberOrder(Contact *list, Contact buf, int num) {
    //finished
    int i;
    for (i = 0; i < num-2; ++i) {
        if (list[i].number <= buf.number && buf.number < list[i+1].number)
            break;
    }
    if (i <= num-3) {
        //normal
        i++;
        for ( ; i < num; ++i) {
            Contact m = list[i];
            list[i] = buf;
            buf = m;
        }
    } else if (i == num-2) {
        list[num-1]=buf;
    } else if (i == num-1) {
        list[num-1]=buf;
    }
}

int strToInt(char *str) {
    //finished
    int res=0, digit=0;
    while (str[digit]) {
        res*=10;
        res+=str[digit]-'0';
        digit++;
    }
    return res;
}

void printContactInfo(Contact buf) {
    //finished
    printf("\n");
    printf("[number] %d\n",buf.number);
    printf("[name] %s\n",buf.name);
    if (buf.sex == 1)
        printf("[sex] man\n");
    else if (buf.sex == 2)
        printf("[sex] woman\n");
    else
        printf("[sex] unknown\n");
    printf("[class] %s\n",buf.classInfo);
    if (buf.duty == 1)
        printf("[duty] leader\n");
    else if (buf.duty == 2)
        printf("[duty] study\n");
    else
        printf("[duty] no\n");
    printf("[phone] %s\n",buf.phoneNumber);
    printf("[email] %s\n",buf.email);
    for (int i = 0; i < buf.other.num; ++i) {
        printf("[%s] %s\n",buf.other.name[i],buf.other.consist[i]);
    }
}

Contact *delContact(Contact *list, int contactNumber, int digit) {
    //finished
    for ( ; digit < contactNumber-1; ++digit) {
        list[digit]=list[digit+1];
    }
    list=(Contact*)realloc(list, sizeof(Contact)*(contactNumber-1));
    return list;
}

void contactEditor(Contact *buf) {
    //finished
    char cmd[100], buffer[101], strBuf[10][100];
    command:
    printf("What do you want to edit? Use \"help\" for help>");
    gets(cmd);
    cmdInterpreter(strBuf, cmd);
    strcpy(cmd, strBuf[0]);
    if (strcmp(cmd,"number") == 0) {
        printf("Input new number>");
        scanf("%d",&buf->number);
    }else if (strcmp(cmd,"name") == 0) {
        printf("Input new name>");
        scanf("%s",buf->name);
    } else if (strcmp(cmd,"sex") == 0) {
        printf("Input sex [1.man|2.woman]>");
        scanf("%d",&buf->sex);
    } else if (strcmp(cmd,"classInfo") == 0) {
        printf("Input new class information>");
        scanf("%s",buf->classInfo);
    } else if (strcmp(cmd,"duty") == 0) {
        printf("Input new duty [0.no|1.leader|2.study]>");
        scanf("%d",&buf->duty);
    } else if (strcmp(cmd,"phone") == 0) {
        printf("Input new phone number>");
        scanf("%s",buf->phoneNumber);
    } else if (strcmp(cmd,"email") == 0) {
        printf("Input new email>");
        Email:
        printf("Email>");
        scanf("%s", buffer);
        switch (emailCheck(buffer)) {
            case 1:
                printf("Email too long, shorter than 30 chars. Enter again.\n");
                goto Email;
            case 2:
                printf("Too many \'@\', please check your input.\n");
                goto Email;
            case 0:
                strcpy(buf->email, buffer);
        }
        scanf("%s",buf->email);
    } else if (strcmp(cmd,"other") == 0) {
        propertyEditor(&(buf->other));
    } else if (strcmp(cmd,"exit") == 0) {
        return;
    } else if (strcmp(cmd,"help") == 0) {
        showEditHelpMenu();
    } else {
        printf("Unknown command.\n");
    }
    goto command;
}

void propertyEditor(Property *other) {
    //finished
    char cmd[100], strBuf[10][100];
    command:
    printf(R"(Input what to edit. Or use "list" to see the list of other properties,use "del" to delete a property, use"exit" to exit>)");
    gets(cmd);
    cmdInterpreter(strBuf, cmd);
    strcpy(cmd, strBuf[0]);
    if (strcmp(cmd,"list") == 0) {
        for (int i = 0; i < other->num; ++i)
            printf("[%s] %s\n",other->name[i],other->consist[i]);
    } else if (strcmp(cmd,"exit") == 0) {
        return;
    } else if (strcmp(cmd, "del") == 0) {
        printf("Input property name to delete it>");
        gets(cmd);
        cmdInterpreter(strBuf, cmd);
        strcpy(cmd, strBuf[0]);
        printf("Finding...");
        for (int i = 0; i < other->num; ++i) {
            if (strcmp(other->name[i], cmd) == 0) {
                printf("Found.\nDeleting...");
                for (int j = i; j < other->num - 1; ++j) {
                    strcpy(other->name[j], other->name[j + 1]);
                    strcpy(other->consist[j], other->consist[j + 1]);
                }
                other->num--;
                printf("Deleted.\n");
                goto command;
            }
        }
        printf("Property not found.\n");
    } else {
        for (int i = 0; i < other->num; ++i) {
            if (strcmp(cmd,other->name[i]) == 0) {
                printf("Input contains>");
                scanf("%s",other->consist[i]);
                goto command;
            }
        }
        printf("Unknown command.\n");
    }
    goto command;
}

int emailCheck(char *email) {
    //finished
    int at = 0, len= strlen(email);
    if (strlen(email) > 30)
        return 1;
    for (int i = 0; i < len; ++i) {
        if (email[i] == '@')
            at++;
        //to many '@'
        if (at >= 2)
            return 2;
    }
    //Email is legal
    return 0;
}
