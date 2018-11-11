// Librerías ==========================================================
#include <arpa/inet.h>
#include <json.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <signal.h>

#include "json-util.h"
#include "modulo-firebird.h"
#include "modulo-mysql.h"
#include "modulo-postgres.h"
#include "servidor-concurrente.h"

// Constantes =========================================================
#define ARCHIVO_CONFIG "switch.json"
#define ARCHIVO_INFO   "servidores.json"
#define KEY_HOST       "host"
#define KEY_PORT       "puerto"

// Tipos de SGBD:
#define TIPO_MYSQL    'm'
#define TIPO_POSTGRES 'p'
#define TIPO_FIREBIRD 'f'

// Tipos ==============================================================
struct basedatos_t
{
    char  tipo;
    char  nom_servidor[50];
    char  nom_bd[50];
    void* conexion;
};

// Globales ===========================================================
char*               HOST;
int                 PORT;
int                 cantidad_bds;
json_object*        obj_servidores;
struct basedatos_t* bases_de_datos;

// Funciones ==========================================================
void  sigint_handler();
void  terminar();
void  importar_config();
void  importar_servidores_json();
void* atender(void*);
void  conectar_basesdedatos();
void  cerrar_conexiones();
void  cantidad_basesdedatos();

void  print_servidores_json();
void  print_arreglo_bds();

json_object* atender_consulta(json_object* datos, struct basedatos_t* bd);
json_object* atender_lista_atributos(json_object* datos, struct basedatos_t* bd);
json_object* atender_lista_bds(json_object* datos, struct basedatos_t* bd);
json_object* atender_lista_servidores();
json_object* atender_lista_tablas(json_object* datos, struct basedatos_t* bd);

struct basedatos_t* buscar_conexion(const char*, const char*);


int main(int argc, char **argv)
{
    signal(SIGINT, sigint_handler);

    // Importo la ubicación del servidor switch desde un JSON:
    importar_config();
    // Importo la lista de servidores desde un JSON:
    importar_servidores_json();
    print_servidores_json();
    cantidad_basesdedatos();
    conectar_basesdedatos();
    print_arreglo_bds();

    // Iniciar servidor:
    servidor(HOST, PORT, atender);

    terminar(0);
}

void sigint_handler()
{
    terminar(0);
}

void terminar(int exit_code)
{
    cerrar_conexiones();
    json_object_put(obj_servidores);
    free(bases_de_datos);
    free(HOST);

    printf("Terminado...\n");

    exit(exit_code);
}

void importar_config()
{
    json_object* objeto = json_object_from_file(ARCHIVO_CONFIG);
    if (!objeto)
    {
        printf("No se pudo importar la configuración...\n");
        terminar(1);
    }

    HOST = malloc(15 * sizeof(char));
    strcpy(HOST, json_get_string(objeto, KEY_HOST));
    if (!HOST)
    {
        printf("No se pudo leer el host (Clave: %s)...\n", KEY_HOST);
        terminar(1);
    }

    PORT = json_get_int(objeto, KEY_PORT);
    if (!PORT)
    {
        printf("No se pudo leer el puerto (Clave: %s)...\n", KEY_PORT);
        terminar(1);
    }

    printf("Configuración importada:\n");
    printf("\tHOST: %s\n", HOST);
    printf("\tPORT: %d\n", PORT);
    printf("--------------------------------------------------------------------------------");

    json_object_put(objeto);

}

void importar_servidores_json()
{
    obj_servidores = json_object_from_file(ARCHIVO_INFO);
    if (!obj_servidores)
    {
        printf("No se pudo importar la lista de servidores...\n");
        terminar(1);
    }
    printf("Lista de servidores importada\n");
}

void* atender(void *arg)
{
    // Para que el hilo termine cuando termine la función:
    pthread_detach(pthread_self());

    char buf_in[BUFFER];
    char buf_out[BUFFER];
    int  nb;

    // Recibo los datos pasados al hilo por el puntero:
    struct datos_t *datos = (struct datos_t*) arg;

    printf("[Hilo][%d] Atendiendo socket\n", datos->socket);

    char* error = malloc(256 * sizeof(char));
    while (1)
    {
        error[0] = 0; // Sin errores

        // Recibir datos:
        nb = read(datos->socket, buf_in, BUFFER);
        buf_in[nb] = 0;

        // Presionar solo enter para terminar la sesión
        if (!strcmp(buf_in, CERRAR))
        {
            break;
        }
        buf_in[nb] = 0; // Saco el salto de línea

        // Convierto los datos recibidos a JSON:
        json_object* obj_in = json_tokener_parse(buf_in);

        json_object* resultado;
        if (obj_in)
        {
            const char*         bd       = json_get_string(obj_in, "base_de_datos");
            const char*         comando  = json_get_string(obj_in, "comando");
            const char*         servidor = json_get_string(obj_in, "servidor");
            struct basedatos_t* conexion = buscar_conexion(servidor, bd);

            if (comando[0] == 's')
            {
                resultado = atender_lista_servidores();
            }
            else
            {
                if (!conexion)
                {
                    sprintf(error, "No se encontró la base de datos buscada (Servidor: '%s', Base de datos: '%s')",
                        servidor, bd);
                }

                if (comando)
                {
                    switch (comando[0])
                    {
                        case 'a': // Listar atributos de tabla
                            resultado = atender_lista_atributos(obj_in, conexion);
                            break;

                        case 'b': // Listar bases de datos
                            resultado = atender_lista_bds(obj_in, conexion);
                            break;

                        case 'q': // Consulta
                            resultado = atender_consulta(obj_in, conexion);
                            break;

                        case 't': // Listar tablas de base de datos
                            resultado = atender_lista_tablas(obj_in, conexion);
                            break;

                        default: // Comando inválido
                            sprintf(error, "Comando inválido: '%s'", comando);
                    }
                }
                else
                {
                    sprintf(error, "No hay comando");
                }
            }
        }
        else
        {
            sprintf(error, "Datos inválidos: '%s'", buf_in);
        }

        // Objeto JSON de salida:
        json_object* obj_out = json_object_new_object();
        if (*error)
        {
            json_object_object_add(obj_out,
                "error", json_object_new_string(error));
        }
        else
        {
            printf("[Hilo][%d] Filas enviadas: %d\n", datos->socket, json_get_int(resultado, "cantidad"));
            json_object_object_add(obj_out, "resultado", resultado);
        }

        // Enviar datos:
        //~ printf("\nSalida: %s\n\n", json_object_to_json_string_ext(obj_out, 0));
        strcpy(buf_out, json_object_to_json_string_ext(obj_out, 0));
        write(datos->socket, buf_out, BUFFER);

        // Liberar objetos JSON:
        json_object_put(obj_out);
        json_object_put(resultado);
    }

    // Cerrar socket:
    close(datos->socket);
    printf("[Hilo][%d] Socket cerrado\n", datos->socket);

    free(datos);
    free(error);
    return NULL;
}

void print_servidores_json()
{
    json_object* arr_servidores;
    json_object_object_get_ex(obj_servidores, "servidores", &arr_servidores);
    if (!arr_servidores)
    {
        fprintf(stderr, "No se encontró el objeto 'servidores'...\n");
        terminar(1);
    }

    int          i = 0;
    json_object* obj_servidor;
    while ((obj_servidor = json_object_array_get_idx(arr_servidores, i++)))
    {
        if (json_get_bool(obj_servidor, "activo"))
        {
            printf("--------------------------------------------------------------------------------");
            printf("Nombre\t%s\n", json_get_string(obj_servidor, "nombre"));
            printf("Tipo\t%s\n",   json_get_string(obj_servidor, "tipo"));
            printf("Host\t%s\n",   json_get_string(obj_servidor, "host"));
            printf("Puerto\t%d\n", json_get_int   (obj_servidor, "puerto"));

            int          j = 0;
            json_object* obj_bd;
            json_object* arr_bases;
            json_object_object_get_ex(obj_servidor, "bases_de_datos", &arr_bases);

            while ((obj_bd = json_object_array_get_idx(arr_bases, j++)))
            {
                printf("\t  BD: \"%s\"\tusuario: \"%s\"\n",
                    json_get_string(obj_bd, "base"),
                    json_get_string(obj_bd, "usuario"));
            }
        }
    }
    printf("--------------------------------------------------------------------------------");
}

void print_arreglo_bds()
{
    for (int i = 0; i < cantidad_bds; i++)
    {
        printf("%c\t%15s\t%15s\t%s\n",
            bases_de_datos[i].tipo,
            bases_de_datos[i].nom_servidor,
            bases_de_datos[i].nom_bd,
            (bases_de_datos[i].conexion) ? "Conectado" : "");
    }
    printf("--------------------------------------------------------------------------------");
}

void cantidad_basesdedatos()
{
    cantidad_bds = 0;

    json_object* arr_servidores;
    json_object_object_get_ex(obj_servidores, "servidores", &arr_servidores);
    if (!arr_servidores)
    {
        fprintf(stderr, "No se encontró el objeto 'servidores'...\n");
        terminar(1);
    }

    int          i = 0;
    json_object* obj_servidor;
    while ((obj_servidor = json_object_array_get_idx(arr_servidores, i++)))
    {
        if (json_get_bool(obj_servidor, "activo"))
        {
            json_object* arr_bases;
            json_object_object_get_ex(obj_servidor, "bases_de_datos", &arr_bases);

            if (arr_bases)
            {
                cantidad_bds += json_object_array_length(arr_bases);
            }
        }
    }

    // Cierro el servidor si no hay bases de datos:
    if (cantidad_bds == 0)
    {
        printf("No hay bases de datos disponibles");
        terminar(1);
    }
}

void conectar_basesdedatos()
{
    json_object* arr_servidores;
    json_object_object_get_ex(obj_servidores, "servidores", &arr_servidores);

    bases_de_datos = malloc(sizeof(struct basedatos_t) * cantidad_bds);
    int b = 0;

    // Iterar arreglo de servidores en JSON:
    int          i = 0;
    json_object* obj_servidor;
    while ((obj_servidor = json_object_array_get_idx(arr_servidores, i++)))
    {
        const char* host   = json_get_string(obj_servidor, "host");
        int         port   = json_get_int(obj_servidor,    "puerto");
        int         activo = json_get_bool(obj_servidor,   "activo");

        if (activo)
        {
            // Iterar arreglo de bases de datos del servidor en JSON:
            int          j = 0;
            json_object* obj_bd;
            json_object* arr_bases;
            json_object_object_get_ex(obj_servidor, "bases_de_datos", &arr_bases);

            while ((obj_bd = json_object_array_get_idx(arr_bases, j++)))
            {
                bases_de_datos[b].tipo = json_get_string(obj_servidor, "tipo")[0];
                strcpy(bases_de_datos[b].nom_servidor, json_get_string(obj_servidor, "nombre"));
                strcpy(bases_de_datos[b].nom_bd, json_get_string(obj_bd, "base"));
                const char* usuario     = json_get_string(obj_bd, "usuario");
                const char* contrasenia = json_get_string(obj_bd, "contraseña");

                // Realizar conexión a base de datos según tipo:
                switch (bases_de_datos[b].tipo)
                {
                    case TIPO_MYSQL:
                        bases_de_datos[b].conexion = mysql_conectar(host, port, usuario, contrasenia, bases_de_datos[b].nom_bd);
                        break;
                    case TIPO_POSTGRES:
                        bases_de_datos[b].conexion = postgres_conectar(host, port, usuario, contrasenia, bases_de_datos[b].nom_bd);
                        break;
                    case TIPO_FIREBIRD:
                        bases_de_datos[b].conexion = firebird_conectar(host, port, usuario, contrasenia, bases_de_datos[b].nom_bd);
                        break;
                    default:
                        bases_de_datos[b].conexion = NULL;
                        printf("Tipo desconocido: '%c'\n", bases_de_datos[b].tipo);
                }
                b++; // Índice del arreglo global de bases de datos.
            }
        }
    }
}

void  cerrar_conexiones()
{
    for (int i = 0; i < cantidad_bds; i++)
    {
        switch (bases_de_datos[i].tipo)
        {
            case TIPO_MYSQL:
                mysql_cerrar(bases_de_datos[i].conexion);
                break;

            case TIPO_POSTGRES:
                postgres_cerrar(bases_de_datos[i].conexion);
                break;

            case TIPO_FIREBIRD:
                firebird_cerrar(bases_de_datos[i].conexion);
                break;
        }
    }
}

struct basedatos_t* buscar_conexion(const char* servidor, const char* bd)
{
    if (!servidor)
    {
        return NULL;
    }

    for (int i = 0; i < cantidad_bds; i++)
    {
        struct basedatos_t bd_t = bases_de_datos[i];
        if (strcmp(bd_t.nom_servidor, servidor) == 0)
        {
            if (!bd || strcmp(bd_t.nom_bd, bd) == 0)
            {
                return &bases_de_datos[i];
            }
        }
    }
    return NULL;
}

json_object* atender_consulta(json_object* datos, struct basedatos_t* bd)
{
    const char*  consulta  = json_get_string(datos, "consulta");
    json_object* resultado;

    switch (bd->tipo)
    {
        case TIPO_MYSQL:
            resultado = mysql_consulta(bd->conexion, consulta);
            break;

        case TIPO_POSTGRES:
            resultado = postgres_consulta(bd->conexion, consulta);
            break;

        case TIPO_FIREBIRD:
            resultado = firebird_consulta(bd->conexion, consulta);
            break;

        default:
            resultado = NULL;
            printf("Tipo desconocido: '%c'\n", bd->tipo);
    }

    return resultado;
}

json_object* atender_lista_atributos(json_object* datos, struct basedatos_t* bd)
{
    const char*  tabla = json_get_string(datos, "tabla");
    json_object* resultado;

    switch (bd->tipo)
    {
        case TIPO_MYSQL:
            resultado = mysql_columnas(bd->conexion, tabla);
            break;

        case TIPO_POSTGRES:
            resultado = postgres_columnas(bd->conexion, tabla);
            break;

        case TIPO_FIREBIRD:
            resultado = firebird_columnas(bd->conexion, tabla);
            break;

        default:
            resultado = NULL;
            printf("Tipo desconocido: '%c'\n", bd->tipo);
    }

    return resultado;
}

json_object* atender_lista_bds(json_object* datos, struct basedatos_t* bd)
{
    const char*  servidor = json_get_string(datos, "servidor");
    int          cantidad = 0;
    json_object* arr_bds  = json_object_new_array();

    for (int i = 0; i < cantidad_bds; i++)
    {

        if (!strcmp(bases_de_datos[i].nom_servidor, servidor))
        {
            json_object* arr_fila = json_object_new_array();
            json_object_array_add(
                arr_fila,
                json_object_new_string(bases_de_datos[i].nom_bd));
            json_object_array_add(arr_bds, arr_fila);
            cantidad++;
        }
    }

    json_object* arr_columnas = json_object_new_array();
    json_object_array_add(
        arr_columnas,
        json_object_new_string("nombre"));

    json_object* resultado = json_object_new_object();
    json_object_object_add(resultado, "cantidad", json_object_new_int(cantidad));
    json_object_object_add(resultado, "columnas", arr_columnas);
    json_object_object_add(resultado, "filas",    arr_bds);
    return resultado;
}

json_object* atender_lista_servidores()
{
    int          i            = 0;
    int          cantidad     = 0; // Cantidad de bases de datos activas
    json_object* arr_serv_in;
    json_object* arr_serv_out = json_object_new_array();
    json_object* obj_servidor;

    json_object_object_get_ex(obj_servidores, "servidores", &arr_serv_in);

    while ((obj_servidor = json_object_array_get_idx(arr_serv_in, i++)))
    {
        if (json_get_bool(obj_servidor, "activo"))
        {
            // Arreglo JSON con cada valor de la fila:
            json_object* arreglo_fila = json_object_new_array();

            char* tipo;
            switch (json_get_string(obj_servidor, "tipo")[0])
            {
                case TIPO_MYSQL:    tipo = "MySQL"; break;
                case TIPO_POSTGRES: tipo = "PostgreSQL"; break;
                case TIPO_FIREBIRD: tipo = "Firebird"; break;
                default:            tipo = "desconocido";
            }

            json_object_array_add(arreglo_fila, json_object_new_string(json_get_string(obj_servidor, "nombre")));
            json_object_array_add(arreglo_fila, json_object_new_string(tipo));

            json_object_array_add(arr_serv_out, arreglo_fila);
            cantidad++;
        }
    }

    json_object* arr_columnas = json_object_new_array();
    json_object_array_add(arr_columnas, json_object_new_string("servidor"));
    json_object_array_add(arr_columnas, json_object_new_string("tipo"));

    json_object* resultado = json_object_new_object();
    json_object_object_add(resultado, "cantidad", json_object_new_int(cantidad));
    json_object_object_add(resultado, "columnas", arr_columnas);
    json_object_object_add(resultado, "filas",    arr_serv_out);
    return resultado;
}

json_object* atender_lista_tablas(json_object* datos, struct basedatos_t* bd)
{
    json_object* resultado;

    switch (bd->tipo)
    {
        case TIPO_MYSQL:
            resultado = mysql_tablas(bd->conexion);
            break;

        case TIPO_POSTGRES:
            resultado = postgres_tablas(bd->conexion);
            break;

        case TIPO_FIREBIRD:
            resultado = firebird_tablas(bd->conexion);
            break;

        default:
            resultado = NULL;
            printf("Tipo desconocido: '%c'\n", bd->tipo);
    }

    return resultado;
}

