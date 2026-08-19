/*
*   Remaking bfetch for linux systems due to the same issues I had with fastfetch on FreeBSD
*   Probably wont open source this one since it's the same shit just different OS type
*   Idk tho lmao
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/utsname.h>
#include <unistd.h>

#define INFO_LINES 7

// gets distro from file
const char *distributions(void) {
    static char distro[256];
    FILE *file = fopen("/etc/os-release", "r");

    if (!file)
        return "unknown";

    while (fgets(distro, sizeof(distro), file)) {
        if (strncmp(distro, "PRETTY_NAME=", 12) == 0) {
            char *name = distro + 12;

            if (*name == '"') {
                name++;
                char *quote = strchr(name, '"');
                if (quote)
                    *quote = '\0';
            } else {
                name[strcspn(name, "\n")] = '\0';
            }

            fclose(file);
            return name;
        }
    }

    fclose(file);
    return "Unknown";
}

// gets cpu from file
const char *cpuinfo(void) {
    static char cpu[256];
    FILE *file = fopen("/proc/cpuinfo", "r");

    if (!file)
        return "unknown";

    while (fgets(cpu, sizeof(cpu), file)) {
        if (strncmp(cpu, "model name", 10) == 0) {
            char *name = strchr(cpu, ':');

            if (name) {
                name++;

                while (*name == ' ' || *name == '\t')
                    name++;

                name[strcspn(name, "\n")] = '\0';

                fclose(file);
                return name;
            }
        }
    }

    fclose(file);
    return "Unknown";
}

int main(void) {
    // stores the path to the build and ascii files
    char exe_path[PATH_MAX];
    char ascii_path[PATH_MAX];

    // gets the path to the build 
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        perror("readlink");
        return 1;
    }
    
    char *slash = strrchr(exe_path, '/');
    if (slash)
        *slash = '\0';

    snprintf(ascii_path, sizeof(ascii_path), "%s/ascii.txt", exe_path); // makes sure that the ascii art file must be in the same dir as the build file

    // opens the ascii art file
    FILE *ascii = fopen(ascii_path, "r");
    if (!ascii) {
        perror(ascii_path);
        return 1;
    }
    
    // holds the amount of lines in the ascii file
    char *art_lines[INFO_LINES] = {0};
    char buf[256];
    int total_art = 0;

    // reads the amount of lines that was held
    while (total_art < INFO_LINES &&
           fgets(buf, sizeof(buf), ascii) != NULL) {

        buf[strcspn(buf, "\n")] = '\0'; // makes sure the ascii art and the info arent colliding 

        art_lines[total_art] = strdup(buf); // stores the amount of lines in the array

        // checks memory for allocation failure
        if (!art_lines[total_art]) {
            perror("strdup");
            fclose(ascii);

            for (int i = 0; i < total_art; i++)
                free(art_lines[i]);

            return 1;
        }

        total_art++;
    }

    fclose(ascii);

    // holds system info as strings
    char hostname[1024];
    char *term = getenv("TERM");
    char *user = getenv("USER");
    char *shell = getenv("SHELL");
    if (!user)
        user = "unknown";
    if (!term)
        term = "unknown";
    if (!shell)
        shell = "unknown";

    gethostname(hostname, sizeof(hostname));

    struct utsname uts;
    if (uname(&uts) == -1) {
        perror("uname");
        return 1;
    }

    // makes sure full shell path isnt printed
    char *shell_name = strrchr(shell, '/');
    shell_name = shell_name ? shell_name + 1 : shell;

    // prints the ascii art
    for (int line = 0; line < INFO_LINES; line++) {
        printf("\033[31m%-20s\033[0m",
               line < total_art ? art_lines[line] : "");

        // prints info
        switch (line) {
            case 0:
                printf("%s@%s\n", user, hostname);
                break;
            case 1:
                printf("OS: %s\n", distributions());
                break;
            case 2:
                printf("CPU: %s\n", cpuinfo());
                break;
            case 3:
                printf("terminal: %s\n", term);
                break;
            case 4:
                printf("shell: %s\n", shell_name);
                break;
            default:
                printf("\n");
                break;
        }
    }
    return 0;
}
