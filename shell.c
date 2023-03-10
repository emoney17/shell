#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE -1
#define SH_RL_BUFSIZE 1024
#define SH_TOK_BUFSIZE 64
#define SH_TOK_DELIM " \t\r\n\a"
#define HOST_NAME_MAX 255
#define LOGIN_NAME_MAX 255

// getline version of readline
char* sh_read_line(void)
{
    char* line = NULL;
    ssize_t bufsize = 0; // have getline allocate a buffer

    if (getline(&line, &bufsize, stdin) == -1)
    {
        if (feof(stdin))
        {
            exit(EXIT_SUCCESS); // we received an EOF
        }
        else
        {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

// parsing the line from readline
// same as readline expand buffer dynamically
char** sh_split_line(char* line)
{
    int bufsize = SH_TOK_BUFSIZE, position = 0;
    char** tokens = malloc(bufsize * sizeof(char*));
    char* token;

    if (!tokens)
    {
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    // strtok returns pointers from the string given
    // and place \0 bytes at the end of each token
    token = strtok(line, SH_TOK_DELIM);
    while (token != NULL)
    {
        // store each token in an array of character pointers
        tokens[position] = token;
        position++;

        // reallocate if necessary
        if (position >= bufsize)
        {
            bufsize += SH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens)
            {
                fprintf(stderr, "sh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, SH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

// launch the shell
int sh_launch(char** args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        // child process
        if (execvp(args[0], args) == -1) // run command from user
        {
            perror("sh");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) // check if fork had an error
    {
        // error trying to fork
        perror("sh");
    }
    else // fork successfull
    {
        // parent process
        do
        {
            // wait for child to finish executing a command
            wpid = waitpid(pid, &status, WUNTRACED);
            // while process is not exited or killed by signal
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

// forward declerations for functions
int sh_cd(char** args);
int sh_help(char** args);
int sh_exit(char** args);

// list of builtin commands, alternative to switch statement
char* builtin_str[] =
{
    "cd",
    "help",
    "exit",
};

// an array of function pointers
// (that take an array of strings and returns an int)
int (*builtin_func[]) (char**) =
{
    &sh_cd,
    &sh_help,
    &sh_exit
};

int sh_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char*);
}

// built in function implementations
int sh_cd(char** args)
{
    if (args[1] == NULL) // check that second argument exists
    {
        fprintf(stderr, "sh: expected argument to \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0) // cd to the dir
        {
            perror("sh");
        }
    }
    return 1;
}

int sh_help(char** args)
{
    int i;
    printf("SH shell version: 0.0.0.1\n");
    printf("Type program names and arguments then hit enter\n");
    printf("The following are builtin\n");

    for (i = 0; i < sh_num_builtins(); i++)
    {
        printf("    %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs\n");
    return 1;
}

int sh_exit(char** args)
{
    return 0;
}

// check if the command eachs each builting function if it does, run it
// if not call sh_launch to lunch the process
int sh_execute(char** args)
{
    int i;

    if (args[0] == NULL) // empty command was entered
    {
        return 1;
    }

    for (i = 0; i < sh_num_builtins(); i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }

    return sh_launch(args);
}

void sh_loop(void)
{
    char* line;
    char** args;
    int status;
    char hostname[HOST_NAME_MAX];
    char username[LOGIN_NAME_MAX];

    // get the host and username from computer
    gethostname(hostname, HOST_NAME_MAX);
    getlogin_r(username, LOGIN_NAME_MAX);

    do
    {

        printf("%s@%s> ", username, hostname);
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

    return EXIT_SUCCESS;
}
