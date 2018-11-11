// Librerías ==========================================================
#include "servidor-concurrente.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Constantes =========================================================
const int BUFFER = 512;
const char* CERRAR = "\n";

// Funciones ==========================================================
void cliente(const char* host, int puerto, void enviar(char*), void recibir(char*))
{
    struct sockaddr_in c_sock;
    int idsocks;
    int idsockc;
    int lensock;

    idsockc = socket(AF_INET, SOCK_STREAM, 0);

    c_sock.sin_family = AF_INET;
    c_sock.sin_port = htons(puerto);
    c_sock.sin_addr.s_addr = inet_addr(host);

    lensock = sizeof(c_sock);

    // Conectarse con el servidor:
    idsocks = connect(idsockc, (struct sockaddr*) &c_sock, lensock);
    if (idsocks == -1)
    {
        //~ printf("Falló el connect...\n");
        fprintf(stderr, "Falló el connect...\n");
        fprintf(stderr, "\thost: '%s'\n\tpuerto: %d\n", host, puerto);
        return;
    }

    int nb;
    char buf_in[BUFFER];
    char* buf_out;

    while (1)
    {
        // Preparar datos de salida:
        buf_out = malloc(BUFFER * sizeof(char));
        enviar(buf_out);

        // Mandar al servidor:
        write(idsockc, buf_out, strlen(buf_out));
        //~ sleep(1);

        // Recibir del servidor:
        nb = read(idsockc, buf_in, BUFFER);
        buf_in[nb-1] = 0;

        // Presionar solo enter para terminar la sesión
        if (!strcmp(buf_out, CERRAR))
        {
            break;
        }

        recibir(buf_in);

        free(buf_out);
    }
    close(idsockc);
}

void cliente_uniq(const char* host, int puerto, void enviar(char*), void recibir(char*))
{
    struct sockaddr_in c_sock;
    int                idsocks;
    int                idsockc;
    socklen_t          lensock;

    idsockc = socket(AF_INET, SOCK_STREAM, 0);

    c_sock.sin_family = AF_INET;
    c_sock.sin_port = htons(puerto);
    c_sock.sin_addr.s_addr = inet_addr(host);

    lensock = sizeof(c_sock);

    // Conectarse con el servidor:
    idsocks = connect(idsockc, (struct sockaddr*) &c_sock, lensock);
    if (idsocks == -1)
    {
        //~ printf("Falló el connect...\n");
        fprintf(stderr, "Falló el connect...\n");
        fprintf(stderr, "\thost: '%s'\n\tpuerto: %d\n", host, puerto);
        return;
    }

    int   nb;
    char  buf_in[BUFFER];
    char* buf_out;

    // Preparar datos de salida:
    buf_out = malloc(BUFFER * sizeof(char));
    enviar(buf_out);

    // Enviar al servidor:
    write(idsockc, buf_out, strlen(buf_out));

    // Recibir del servidor:
    nb = read(idsockc, buf_in, BUFFER);
    buf_in[nb-1] = 0;

    recibir(buf_in);

    // Enviar señal de terminado para cerrar conexión en el servidor:
    write(idsockc, CERRAR, strlen(buf_out));

    free(buf_out);

    close(idsockc);
}

void servidor(const char* host, int puerto, void* atender)
{
    struct sockaddr_in s_sock;
    struct sockaddr_in c_sock;
    int                idsocks;
    int                idsockc;
    socklen_t          lensock;
    struct datos_t*    datos; // Estructura con los datos a pasar al hilo

    idsocks = socket(AF_INET, SOCK_STREAM, 0);

    s_sock.sin_family = AF_INET;
    s_sock.sin_port = htons(puerto);
    s_sock.sin_addr.s_addr = inet_addr(host);

    lensock = sizeof(struct sockaddr_in);
    bind(idsocks, (struct sockaddr*) &s_sock, lensock);

    listen(idsocks, 5);

    lensock = sizeof(c_sock);

    while (1)
    {
        printf("[Main]    Esperando conexión en %s:%d...\n", host, puerto);

        // Acepto una conexión entrante:
        idsockc = accept(idsocks, (struct sockaddr*) &c_sock, &lensock);
        if (idsockc == -1)
        {
            printf("[Main]    Falló el accept\n");
            continue;
        }

        // Derivar conexión a un hilo:
        printf("[Main][%d] Derivando conexión de socket\n", idsockc);

        // Inicializo la estructura. Luego la libero en el hilo:
        datos = malloc(sizeof(struct datos_t));
        datos->socket = idsockc;

        // Iniciar hilo:
        pthread_t id_hilo;
        pthread_create(&id_hilo, NULL, atender, datos);
    }
}
