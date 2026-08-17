/*
*   Remaking bfetch for linux systems due to the same issues I had with fastfetch on FreeBSD
*   Probably wont open source this one since it's the same shit just different OS type
*   Idk tho lmao
*/

#include <unistd.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <pwd.h>

int main(void) {
    char hostname[1024];

    gethostname(hostname, sizeof(hostname));

    struct passwd *pw = getpwuid(getuid());   

    printf("%s@%s\n",pw->pw_name, hostname);
    return 0;
}

