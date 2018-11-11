// Librerías ==========================================================
#include "json-util.h"

// Funciones ==========================================================
const char* json_get_string(json_object* objeto, char* clave)
{
    json_object* objeto_hijo;
    json_object_object_get_ex(objeto, clave, &objeto_hijo);
    return json_object_get_string(objeto_hijo);
}

const int json_get_int(json_object* objeto, char* clave)
{
    json_object* objeto_hijo;
    json_object_object_get_ex(objeto, clave, &objeto_hijo);
    return json_object_get_int(objeto_hijo);
}

const int json_get_bool(json_object* objeto, char* clave)
{
    json_object* objeto_hijo;
    json_object_object_get_ex(objeto, clave, &objeto_hijo);
    return json_object_get_boolean(objeto_hijo);
}
