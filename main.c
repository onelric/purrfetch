#include <ctype.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

#define BUFFER_SIZE 256

#define ANSI_COLOR_RED "\x1b[3;31m"
#define ANSI_COLOR_GREEN "\x1b[3;32m"
#define ANSI_COLOR_YELLOW "\x1b[3;33m"
#define ANSI_COLOR_BLUE "\x1b[3;34m"
#define ANSI_COLOR_MAGENTA "\x1b[3;35m"
#define ANSI_COLOR_CYAN "\x1b[3;36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define ASCII_PADDING 3

typedef struct {
    char name[BUFFER_SIZE / 2];
    char value[BUFFER_SIZE];
} Stat;

const Stat createStat(const char* name, const char* value) {
    Stat stat;

    strcpy(stat.name, name);
    strcpy(stat.value, value);

    return stat;
}

void printStat(const char* c, Stat* s, const char* seperator) {
    printf("%s%s %s %s\n", c, s->name, seperator, s->value);
}

char* getDistro(void) {
    FILE* p_file = fopen("/etc/os-release", "r");
    if (!p_file) exit(EXIT_FAILURE);

    char buffer[BUFFER_SIZE];
    char* pretty_name = malloc(BUFFER_SIZE);
    if (pretty_name == NULL) exit(EXIT_FAILURE);

    while (fgets(buffer, sizeof(buffer) / sizeof(char), p_file) != NULL) {
        if (strncmp(buffer, "PRETTY_NAME=", 12) == 0) {
            fclose(p_file);

            char* first = strchr(buffer, '"');
            char* last = strchr(first + 1, '"');
            if (last == NULL || first == NULL) {
                free(pretty_name);
                exit(EXIT_FAILURE);
            }
            strlcpy(pretty_name, first + 1, last - first);

            return pretty_name;
        }
    }

    fclose(p_file);
    free(pretty_name);
    return NULL;
}

char* getEnvValue(const char* v) {
    char* s = getenv(v);
    char* env = s == NULL ? "Unknown" : s;
    return env;
}

void strlower(char* s) {
    for (int i = 0; i < strlen(s); i++) s[i] = tolower(s[i]);
}

int main(int argc, char* argv[]) {
    // Parsing arguments

    // Fetching system information
    struct sysinfo sys_info;
    if (sysinfo(&sys_info) != 0) exit(EXIT_FAILURE);

    // Fetching uname
    struct utsname unam;
    if (uname(&unam) != 0) exit(EXIT_FAILURE);

    // Getting username and hostname
    char* user = getEnvValue("USER");
    strncat(user, "@", strlen(user));
    Stat user_stat = createStat("", strcat(user, unam.nodename));

    // Get linux distribution
    char* distro = getDistro();
    strlower(distro);
    Stat distro_stat = createStat("󰌽", distro);

    // Fetch window manager being used
    const char* wm = getEnvValue("XDG_CURRENT_DESKTOP");
    Stat wm_stat = createStat("󰖲", wm);

    // Fetching users shell
    char* shell = getEnvValue("SHELL");
    Stat shell_stat = createStat("shell", basename(shell));

    // TODO)) Used ram works wonky. TODO))
    // Some wonky calculations to convert bits to gigabytes
    long total_ram = sys_info.totalram * sys_info.mem_unit;
    long used_ram = total_ram - sys_info.freeram * sys_info.mem_unit;
    long bytes_gb = (1024 * 1024 * 1024);
    char buff[BUFFER_SIZE];
    sprintf(buff, "%ld GB / %ld GB", used_ram / bytes_gb, total_ram / bytes_gb);
    Stat ram_stat = createStat("ram", buff);

    char* kernel_name = strncat(unam.sysname, " ", strlen(unam.sysname));
    strlower(kernel_name);
    Stat kernel_stat = createStat("", strcat(kernel_name, unam.release));

    /*
       cute cat ascii:
        　／l、
        （ﾟ､ ｡ ７ 　
        　l、~ ヽ
        　ししと ）ノ
    */

    char padding[BUFFER_SIZE] = "";
    for (int i = 0; i < ASCII_PADDING; i++) strcat(padding, " ");

    char seperator[5] = " ~>";

    printf("\n");
    printf("%s   ／l、      %s", ANSI_COLOR_RESET, padding);
    printStat(ANSI_COLOR_CYAN, &user_stat, seperator);
    printf("%s （ﾟ､ ｡ ７    %s", ANSI_COLOR_RESET, padding);
    printStat(ANSI_COLOR_YELLOW, &kernel_stat, seperator);
    printf("%s   l、~ ヽ    %s", ANSI_COLOR_RESET, padding);
    printStat(ANSI_COLOR_GREEN, &distro_stat, seperator);
    printf("%s   ししと ）ノ%s", ANSI_COLOR_RESET, padding);
    printStat(ANSI_COLOR_RED, &wm_stat, seperator);
    /* printStat(ANSI_COLOR_MAGENTA, &shell_stat, " ~> "); */
    /* printStat(ANSI_COLOR_BLUE, &ram_stat, "   ~> "); */
    printf("\n");

    free(distro);

    return EXIT_SUCCESS;
}
