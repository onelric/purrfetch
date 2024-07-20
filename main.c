#include <ctype.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

#define BUFFER_SIZE 256

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

char* getDistro() {
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

const char* getEnvValue(const char* v) {
    char* s = getenv(v);
    char* env = s == NULL ? "Unknown" : s;
    return env;
}

int main(int argc, char* argv[]) {
    struct sysinfo sys_info;
    if (sysinfo(&sys_info) != 0) exit(EXIT_FAILURE);

    struct utsname unam;
    if (uname(&unam) != 0) exit(EXIT_FAILURE);

    char* distro = getDistro();
    for (int i = 0; i < strlen(distro); i++) distro[i] = tolower(distro[i]);

    const char* wm = getEnvValue("XDG_CURRENT_DESKTOP");
    const char* user = getEnvValue("USER");

    const char* shell = getEnvValue("SHELL");
    char* p_sh = malloc(strlen(shell) + 1);
    strcpy(p_sh, shell);

    long total_ram = sys_info.totalram * sys_info.mem_unit;
    long used_ram = total_ram - sys_info.freeram * sys_info.mem_unit;
    long bytes_gb = (1024 * 1024 * 1024);

    const char* modules[6][BUFFER_SIZE] = {
        {"host", unam.nodename, user}, {"wm", wm}, {"os", distro}, {"kernel", unam.sysname, unam.release},
        {"shell", basename(p_sh)},
    };

    printf("\n\n");
    printf("      ૮ ˶ᵔ ᵕ ᵔ˶ ა\n");
    printf("\x1b[3m");
    printf("%s%s   ->  %s@%s\n", ANSI_COLOR_CYAN, modules[0][0], modules[0][1], modules[0][2]);
    printf("%s%s ->  %s %s\n", ANSI_COLOR_YELLOW, modules[3][0], modules[3][1], modules[3][2]);
    printf("%s%s     ->  %s\n", ANSI_COLOR_GREEN, modules[2][0], modules[2][1]);
    printf("%s%s     ->  %s\n", ANSI_COLOR_RED, modules[1][0], modules[1][1]);
    printf("%s%s  ->  %s\n", ANSI_COLOR_MAGENTA, modules[4][0], modules[4][1]);
    printf("%s%s    ->  %ld GB / %ld GB\n", ANSI_COLOR_BLUE, "ram", used_ram / bytes_gb, total_ram / bytes_gb);
    printf("\n");

    free(distro);
    free(p_sh);

    return EXIT_SUCCESS;
}
