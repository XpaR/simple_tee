#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define GETOPT_PARAMS "a"

#define MAX_STDIN_READ (1024)

char* ERRORS_TEXT[] = 
{
    "",
    "getopt failed",
    "open failed",
    "read failed",
    "write failed",
    "close failed"
};

enum ERRORS
{
    ERR_SUCCESS,
    ERR_GETOPT,
    ERR_OPEN,
    ERR_READ,
    ERR_WRITE,
    ERR_CLOSE
} state;


int main(int argc, char* argv[])
{
    state = ERR_SUCCESS;
    
    opterr = 0; // suppress getopt error messages

    int file_flags = (O_CREAT | O_WRONLY); // by default, create new file and write only mode
    
    int opt = 0;
    while( -1 != (opt = getopt(argc, argv, GETOPT_PARAMS)))
    {
        switch(opt)
        {
            case 'a':
            {
                file_flags |= O_APPEND; // add append flag because -a was found
                break;
            }
            default:
            {
                file_flags |= O_TRUNC;
                break;
            }
        }
    }

    if(optind == argc)
    {
        printf("Usage: %s [-a] filename", argv[0]);
        goto cleanup;
    }

    int fd = -1;
    if (optind != (argc - 1))
    {
        state = ERR_GETOPT;
        goto cleanup;
    }

    fd = open(argv[optind], file_flags, S_IRUSR|S_IWUSR|S_IWGRP|S_IRGRP|S_IROTH); // -rw-r--r--
    if (-1 == fd)
    {
        state = ERR_OPEN;
        goto cleanup;
    }
    ++optind;

    char buf[MAX_STDIN_READ + 1] = { '\0' };
    ssize_t bytes_written = -1;
    ssize_t bytes_read = -1;
    do
    {
        bytes_read = read(STDIN_FILENO, buf, sizeof(buf));
        if (0 == bytes_read)
        {
            goto cleanup;
        }
        else if (-1 == bytes_read)
        {
            state = ERR_READ;
            goto cleanup;
        }
        bytes_written = write(fd, buf, bytes_read);
        if (-1 == bytes_written)
        {
            state = ERR_WRITE;
            goto cleanup;
        }
    }while(0 != bytes_read);
    if (-1 == close(fd))
    {
        state = ERR_CLOSE;
        goto cleanup;
    }
    
cleanup:
    printf("%s\n", ERRORS_TEXT[state]);
    return state;
}
