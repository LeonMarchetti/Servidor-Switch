// Librerías ==========================================================
#include "modulo-firebird.h"

#include <json.h>
#include <stdio.h>
#include <string.h>

// Constantes =========================================================

// Funciones ==========================================================
//~ json_object* firebird_res_a_json(PGresult* resultado);


void* firebird_conectar(const char* host, int puerto, const char* usuario,
                        const char* contrasenia, const char* nombre_bd)
{
    printf("Conexión a Firebird todavía no implementada.\n");
    return NULL;
}

void firebird_cerrar(void* conexion)
{
    printf("Conexión a Firebird todavía no implementada.\n");
}

json_object* firebird_consulta(void* conexion, const char* consulta)
{
    printf("Conexión a Firebird todavía no implementada.\n");
    return NULL;
    //~ return firebird_res_a_json(resultado);
}

json_object* firebird_tablas(void* conexion)
{
    printf("Conexión a Firebird todavía no implementada.\n");
    return NULL;
}

json_object* firebird_columnas(void* conexion, const char* tabla)
{
    printf("Conexión a Firebird todavía no implementada.\n");
    return NULL;
}

//~ json_object* firebird_res_a_json(PGresult* resultado)
//~ {
    //~ return NULL;
//~ }

