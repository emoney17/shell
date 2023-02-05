// initialize
// interpret
// terminate

#include <stdio.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE -1
#define SH_RL_BUFSIZE 1024

// start with a block size. If they exceed it reallocate with more space
char* sh_read_line(void)
{
    int bufsize = SH_RL_BUFSIZE;
    int position = 0;
    char* buffer = malloc(sizeof(char) * bufsize); // 'string'
    int c; // store char as an int
           // EOF is an int so to check you need int

    if (!buffer)
    {
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // read a character
        c = getchar();

        // check if EOF
        // if we hit EOF replace it will a null character and return
        if (c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        }
        else
        {
            buffer[position] = c;
        }
        position++;

        // check if exceeded buffer
        // if we have exceeded the buffer, reallocate
        if (position >= bufsize)
        {
            bufsize += SH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer)
            {
                fprintf(stderr, "sh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void sh_loop(void)
{
    char* line;
    char** args;
    int status;

    do
    {
        printf("> ");
        line = sh_read_line();
        args = sh_split_line(line);
        status = sh_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char *argv[])
{
    // load config files if any

    // run command loop
    // read
    // parse
    // execute
    sh_loop();

    // perform shutdown/cleanup

    return EXIT_SUCCESS;
}
