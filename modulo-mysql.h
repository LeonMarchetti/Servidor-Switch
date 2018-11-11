#ifndef MYSQL_H_
#define MYSQL_H_

// Librerías ==========================================================
#include <json.h>

// Tipos ==============================================================

// Constantes =========================================================

// Funciones ==========================================================
/** Realiza una conexión a una base de datos MySQL.
 *
 * @param host Dirección del servidor.
 * @param puerto Número de puerto del servidor.
 * @param usuario Nombre de usuario para la conexión.
 * @param contrasenia Contraseña para la conexión.
 * @param nombre_bd Nombre de la base de datos a conectarse.
 * @returns Puntero a una estructura MYSQL.
 */
void* mysql_conectar(const char* host, int puerto, const char* usuario,
                     const char* contrasenia, const char* nombre_bd);

/** Cierra la conexión con la base de datos MySQL.
 *
 * @param conexion Puntero a una estructura MySQL
 */
void mysql_cerrar(void* conexion);

/** Realiza una consulta a una base de datos MySQL.
 *
 * @param conexion Puntero a una estructura MySQL
 * @param consulta Consulta de SQL.
 * @returns Objeto JSON, con un entero para la cantidad de filas en el
 * resultado y un arreglo con las filas del resultado.
 */
json_object* mysql_consulta(void* conexion, const char* consulta);

/** Regresa los nombres de todas las tablas de la base de datos MySQL actual.
 *
 * @param conexion Puntero a una estructura MySQL
 * @returns Objeto JSON, con un entero para la cantidad de filas en el
 * resultado y un arreglo con las filas.
 */
json_object* mysql_tablas(void* conexion);

/** Regresa los nombres de todas las columnas de la tabla en la base de datos
 *  MySQL actual.
 *
 * @param conexion Puntero a una estructura MySQL
 * @param tabla Nombre de la tabla
 * @returns Objeto JSON, con un entero para la cantidad de filas en el
 * resultado y un arreglo con las filas.
 */
json_object* mysql_columnas(void* conexion, const char* tabla);

#endif /* MYSQL_H_ */
