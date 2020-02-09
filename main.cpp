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
    int duty;//1 ?? 2 ?? 0 ?
    char phoneNumber[12];
    char email[31];
    Property other;
}Contact;

//string tools
int strToInt(char *str);
void strcut(char *dest, char *source, int start, int over);

//system & ui
void showMainMenu();
void error(int n);
int cmdInterpreter(char *cmd);
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
    int contactNumber=0, number;
    char cmd[100], str[100];
    Contact *list, *more, buf;
    FILE *db=fopen("Contact.db","a+");
    fclose(db);

    //load from file
    printf("Loading from file...\n");
    fopen("Contact.db","r");
    list = loadFromFile(db,&contactNumber);
    fclose(db);
    if (list == 0) {
        error(1);
        exit(1);
    }
    printf("Loading finished. Total: %d contacts.\n",contactNumber);

    printf("Input command. Use \"help\" for help.\n");//1

    command:
    printf("cmd>");
    label1:
    gets(cmd);
    if (cmd[0] == '\n') {
        number=0;
        while (cmd[number] != 0) {
            cmd[number] = cmd[number+1];
            number++;
        }
    }
    if (strcmp(cmd,"") == 0)
        goto label1;

    switch (cmdInterpreter(cmd)) {
        case 0:
            printf("Saving to file...");
            db=fopen("Contact.db","w");
            saveToFile(db,list,contactNumber);
            fclose(db);
            free(list);
            printf("Finished.\n");
            break;
        case 1:
            showMainMenu();
            goto command;
            break;
        case 2:
            more = (Contact*)realloc(list, sizeof(Contact)*(contactNumber+1));
            if (more == 0) {
                error(1);
            } else {
                list=more;
                buf=getContactFromKeyboard();
                contactNumber++;
                insertContactInNumberOrder(list, buf, contactNumber);
            }
            goto command;
            break;
        case 3:
            for (number = 5; cmd[number] != ' ' && cmd[number] != '\n' && cmd[number] != 0; ++number) ;
            strcut(str, cmd, 5, number);
            number=strToInt(str);
            for (int i = 0; i < contactNumber; ++i) {
                if (list[i].number == number) {
                    contactEditor(&list[i]);
                    goto command;
                }
            }
            error(3);
            goto command;
            break;
        case 4:
            //del
            switch (cmd[5]) {
               case 'i':
                   for (number = 7; cmd[number] != ' ' && cmd[number] != 0 && cmd[number] != '\n'; ++number) ;
                   strcut(str, cmd, 7, number);
                   number=strToInt(str);
                   printf("Finding this contact...");
                  for (int i = 0; i < contactNumber; ++i) {
                      if (list[i].number == number) {
                          printf("Found, deleting...");
                            list=delContact(list,contactNumber,i);
                            contactNumber--;
                            printf("Finished.\n");
                            goto command;
                       }
                    }
                    error(3);
                    break;
               case 'n':
                    for (number = 7; cmd[number] != ' ' && cmd[number] != '\n' && cmd[number] != 0; ++number) ;
                    strcut(str, cmd, 7, number);
                    printf("Finding this contact...");
                    for (int i = 0; i < contactNumber; ++i) {
                        if (strcmp(str, list[i].name) == 0) {
                            printf("Found, deleting...");
                            list=delContact(list,contactNumber,i);
                            contactNumber--;
                            printf("Finished.\n");
                            goto command;
                        }
                   }
                    error(3);
                    goto command;
                    break;
             default:
                    error(2);
                    goto command;
                    break;
           }
           break;
        case 5:
            //info
            switch (cmd[6]) {
                case 'n'://name
                    for (number = 8; cmd[number] != ' ' && cmd[number] != '\n' && cmd[number] != 0; ++number) ;
                    strcut(str, cmd, 8, number);
                    printf("Finding this contact...");
                    for (int i = 0; i < contactNumber; ++i) {
                        if (strcmp(str, list[i].name) == 0) {
                            printf("Found\n");
                            printContactInfo(list[i]);
                            printf("\n");
                            goto command;
                        }
                    }
                    error(3);
                    goto command;
                    break;
                case 'p'://phone
                    for (number = 8; cmd[number] != ' ' && cmd[number] != '\n' && cmd[number] != 0; ++number) ;
                    strcut(str, cmd, 8, number);
                    printf("Finding this contact...");
                    for (int i = 0; i < contactNumber; ++i) {
                        if (strcmp(str, list[i].phoneNumber) == 0) {
                            printf("Found\n");
                            printContactInfo(list[i]);
                            printf("\n");
                            goto command;
                        }
                    }
                    error(3);
                    goto command;
                    break;
                case 'c'://class
                    for (number = 8; cmd[number] != ' ' && cmd[number] != '\n' && cmd[number] != 0; ++number) ;
                    strcut(str, cmd, 8, number);
                    for (int i = 0; i < contactNumber; ++i) {
                        if (strcmp(str, list[i].classInfo) == 0)
                            printContactInfo(list[i]);
                    }
                    printf("\n");
                    goto command;
                    break;
                case 'l':
                    //leader
                    for (int i = 0; i < contactNumber; ++i) {
                        if (list[i].duty)
                            printContactInfo(list[i]);
                    }
                    printf("\n");
                    goto command;
                    break;
                case 'a':
                    //all
                    for (int i = 0; i < contactNumber; ++i)
                        printContactInfo(list[i]);
                    printf("\n");
                    goto command;
                    break;
                default:
                    error(2);
                    goto command;
                    break;
            }
            break;
        case -1:
        default:
            error(2);
            goto command;
            break;
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

int cmdInterpreter(char *cmd) {
    //finished
    char buf[50];
    int digit;
    for (digit = 0; cmd[digit] != ' ' && cmd[digit] != 0; ++digit)
        buf[digit]=cmd[digit];
    buf[digit]=0;

    if (strcmp(buf,"exit") == 0)
        return 0;
    else if (strcmp(buf,"help") == 0)
        return 1;
    else if (strcmp(buf,"add") == 0)
        return 2;
    else if (strcmp(buf,"edit") == 0)
        return 3;
    else if (strcmp(buf,"del") == 0)
        return 4;
    else if (strcmp(buf,"info") == 0)
        return 5;
    else
        return -1;
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
    char cmd;
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
    printf("Email>");
    scanf("%s",newContact.email);
    printf("Add other properties? [Y/N]>");
    getchar();
    cmd=getchar();
    getchar();
    while (cmd == 'Y' || cmd == 'y') {
        printf("Property name>");
        scanf("%s",newContact.other.name[counter]);
        printf("Property contain>");
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
void strcut(char *dest, char *source, int start, int over) {
    //finished
    for (int i = 0; i+start < over; ++i) {
        dest[i] = source[i+start];
    }
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
    char cmd[100];
    command:
    printf("What do you want to edit? Use \"help\" for help>");
    scanf("%s",cmd);
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
    char cmd[100];
    command:
    printf("Input what to edit. Use \"list\" to see the list of other properties, use\"exit\" to exit>");
    scanf("%s",cmd);
    if (strcmp(cmd,"list") == 0) {
        for (int i = 0; i < other->num; ++i)
            printf("[%s] %s\n",other->name[i],other->consist[i]);
    } else if (strcmp(cmd,"exit") == 0) {
        return;
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