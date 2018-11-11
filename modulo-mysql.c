// Librerías ==================================================================
#include "modulo-mysql.h"

#include <json.h>
#include <mysql.h>
#include <stdio.h>
#include <string.h>

// Constantes =================================================================

// Funciones ==================================================================
json_object* mysql_res_a_json(MYSQL_RES* resultado);


void* mysql_conectar(const char* host, int puerto, const char* usuario,
                     const char* contrasenia, const char* nombre_bd)
{
    MYSQL* conexion = mysql_init(NULL);
    if (!mysql_real_connect(conexion, host, usuario, contrasenia, nombre_bd, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conexion));
        return NULL;
    }
    return conexion;
}

void mysql_cerrar(void* conexion)
{
    mysql_close((MYSQL*) conexion);
}

json_object* mysql_consulta(void* conexion, const char* consulta)
{
    // Ejecutar consulta:
    if (mysql_query((MYSQL*) conexion, consulta))
    {
        fprintf(stderr, "%s\n", mysql_error((MYSQL*) conexion));
        return NULL;
    }

    // Obtener resultado de la consulta:
    MYSQL_RES* resultado = mysql_use_result((MYSQL*) conexion);
    if (!resultado)
    {
        fprintf(stderr, "%s\n", mysql_error((MYSQL*) conexion));
        return NULL;
    }
    return mysql_res_a_json(resultado);
}

json_object* mysql_tablas(void* conexion)
{
    MYSQL_RES* resultado = mysql_list_tables((MYSQL*) conexion, NULL);
    if (!resultado)
    {
        fprintf(stderr, "%s\n", mysql_error((MYSQL*) conexion));
        return NULL;
    }
    return mysql_res_a_json(resultado);
}

json_object* mysql_columnas(void* conexion, const char* tabla)
{
    const char* plantilla_sql = "Show Columns From %s";
    char*       consulta      = malloc(strlen(tabla) + strlen(plantilla_sql));

    sprintf(consulta, plantilla_sql, tabla);
    return mysql_consulta(conexion, consulta);
}

json_object* mysql_res_a_json(MYSQL_RES* resultado)
{
    // Longitud de string para cada fila:
    int          cant_columnas    = mysql_num_fields(resultado);
    json_object* arreglo_columnas = json_object_new_array();
    MYSQL_FIELD* columna;
    while ((columna = mysql_fetch_field(resultado)))
    {
        json_object_array_add(
            arreglo_columnas,
            json_object_new_string(columna->name));
    }

    // Arreglo con cada fila del resultado:
    json_object* arreglo = json_object_new_array(); // Arreglo con las tablas.
    int          i       = 0;
    MYSQL_ROW    fila;
    while ((fila = mysql_fetch_row(resultado)))
    {
        // Arreglo JSON con cada valor de la fila:
        json_object* arreglo_fila = json_object_new_array();
        for (int c = 0; c < cant_columnas; c++)
        {
            json_object_array_add(
                arreglo_fila,
                json_object_new_string(
                    (fila[c]) ? fila[c] : "NULL"));
        }
        json_object_array_add(arreglo, arreglo_fila);
        i++;
    }

    mysql_free_result(resultado); // Liberar resultado.

    // Regresar resultado como objeto JSON:
    json_object* objeto            = json_object_new_object();
    json_object* objeto_cant_filas = json_object_new_int(i);

    json_object_object_add(objeto, "cantidad", objeto_cant_filas);
    json_object_object_add(objeto, "columnas", arreglo_columnas);
    json_object_object_add(objeto, "filas", arreglo);
    return objeto;
}

