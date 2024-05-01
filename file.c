#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BLOCK_DIM 1024

int main(int argc, char* argv[]) {
    FILE *file;
    unsigned char buffer[BLOCK_DIM];
    int n;
    int pid;
    int status;
    int p[2];

    if (argc != 3) {
        printf("Uso: %s file-origine file-destinazione\n", argv[0]);
        return 0;
    }

    if (pipe(p) < 0) {
        printf("Errore creazione pipe\n");
        return 0;
    }
// process ID processo figlio
    pid = fork();//comando per creare nuovo processo
    if (pid < 0) {
        printf("Errore invocazione fork\n");
        close(p[0]);
        close(p[1]);
        return 0;
    }
    
    if (pid > 0) {
        close(p[0]);
        file = fopen(argv[1], "rb");// apre in lettura file  produttore 
        if (file == NULL) {
            printf("Errore apertura file \"%s\"\n", argv[1]);
            close(p[1]);//chiude il file
            wait(&status);
            return 0;
        }
        while ((n = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            write(p[1], buffer, n); //scrive sul file cosumatore 
        }
        fclose(file);// chiude il file 
        close(p[1]);
        wait(&status);
        return 1;
    } else {
        close(p[1]);
        file = fopen(argv[2], "wb");
        if (file == NULL) {
            printf("Errore apertura file \"%s\"\n", argv[2]);
            close(p[1]);
            return 0;
        }
        while ((n = read(p[0], buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, n, file);
        }
        fclose(file);
        close(p[0]);
        return 1;
    }
}
