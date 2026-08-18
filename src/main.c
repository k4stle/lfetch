/*
*   Remaking bfetch for linux systems due to the same issues I had with fastfetch on FreeBSD
*   Probably wont open source this one since it's the same shit just different OS type
*   Idk tho lmao
*/

#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

// gets distro name from file
const char *distributions(void) {
    static char distro[256];
    FILE *file = fopen("/etc/os-release", "r");

    if (!file)
        return "Unknown";

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

int main(void) {
    int uname(struct utsname *buf);

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

    // print info
    printf("%s@%s\n",user, hostname);
    printf("Distro: %s\n", distributions());
    printf("terminal: %s\n",term);
    printf("shell: %s\n", shell_name);
    return 0;
}

