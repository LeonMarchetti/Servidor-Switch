#ifndef FIREBIRD_H_
#define FIREBIRD_H_

// Librerías ==========================================================
#include <json.h>

// Tipos ==============================================================

// Constantes =========================================================

// Funciones ==========================================================
/** Realiza una conexión a una base de datos Firebird.
 *
 * @param host Dirección del servidor.
 * @param puerto Número de puerto del servidor.
 * @param usuario Nombre de usuario para la conexión.
 * @param contrasenia Contraseña para la conexión.
 * @param nombre_bd Nombre de la base de datos a conectarse.
 * @returns Puntero a una estructura Firebird.
 */
void* firebird_conectar(const char* host, int puerto, const char* usuario,
                        const char* contrasenia, const char* nombre_bd);

/** Cierra la conexión con la base de datos Firebird.
 *
 * @param conexion Puntero a una estructura Firebird
 */
void firebird_cerrar(void* conexion);

/** Realiza una consulta a una base de datos Firebird.
 *
 * @param conexion Puntero a una estructura Firebird
 * @param consulta Consulta de SQL.
 * @returns Objeto JSON, con un entero para la cantidad de filas en el
 * resultado y un arreglo con las filas del resultado.
 */
json_object* firebird_consulta(void* conexion, const char* consulta);

/** Regresa los nombres de todas las tablas de la base de datos Firebird
 *  actual.
 *
 * @param conexion Puntero a una estructura Firebird
 * @returns Objeto JSON, con un entero para la cantidad de tablas en el
 * resultado y un arreglo con los nombres de las tablas.
 */
json_object* firebird_tablas(void* conexion);

/** Regresa los nombres de todas las columnas de la tabla en la base de datos
 *  Firebird actual.
 *
 * @param conexion Puntero a una estructura Firebird
 * @param tabla Nombre de la tabla
 * @returns Objeto JSON, con un entero para la cantidad de tablas en el
 * resultado y un arreglo con los nombres de las columnas.
 */
json_object* firebird_columnas(void* conexion, const char* tabla);

#endif /* FIREBIRD_H_ */
