#ifndef JSON_UTIL_H_
#define JSON_UTIL_H_

// Tipos ==============================================================

// Librerías ==========================================================
#include <json.h>

// Constantes =========================================================

// Funciones ==========================================================
const char* json_get_string(json_object*, char*);
const int   json_get_int(json_object*, char*);
const int   json_get_bool(json_object* objeto, char* clave);

#endif /* JSON_UTIL_H_ */
