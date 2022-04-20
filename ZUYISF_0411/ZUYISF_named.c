#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
    int child;

    mkfifo("Keseru Otto", S_IRUSR | S_IWUSR);
    child = fork();

    if (child > 0)
    {
        char s[1024];
        int fd;

        fd = open("Keseru Otto", O_RDONLY);
        read(fd, s, sizeof(s));
        printf("%s", s);
        close(fd);
        unlink("Keseru Otto");
    }
    else if (child == 0)
    {
        int fd = open("Keseru Otto", O_RDONLY);
        write(fd, "JB ZUYISF\n", 17);
        close(fd);
    }

    return 0;
}