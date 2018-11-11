#ifndef SERVIDOR_H_
#define SERVIDOR_H_

// Tipos ==============================================================
struct datos_t
{
    int socket;
};

// Constantes =========================================================
extern const char* CERRAR;
extern const int   BUFFER;

// Funciones ==========================================================
/** Inicia un cliente de una conexión TCP. El cliente se ejecuta cíclicamente
 * hasta que se sólo se presione "Enter".
 *
 * @param host Nombre del host del servidor.
 * @param puerto Número del puerto del servidor.
 * @param enviar Función que determina los datos a enviar a través de la
 * conexión.
 * @param recibir Función que recibe los datos del servidor a través de la
 * conexión.
 */
void cliente(const char* host, int puerto, void enviar(char*), void recibir(char*));

/** Inicia un cliente de una conexión TCP. Se ejecuta un solo ciclo, después
 * del cual cierra la conexión con el servidor.
 *
 * @param host Nombre del host del servidor.
 * @param puerto Número del puerto del servidor.
 * @param enviar Función que determina los datos a enviar a través de la
 * conexión.
 * @param recibir Función que recibe los datos del servidor a través de la
 * conexión.
 */
void cliente_uniq(const char* host, int puerto, void enviar(char*), void recibir(char*));

/** Inicia un servidor concurrente.
 *
 * @param host Nombre del host del servidor.
 * @param puerto Número del puerto del servidor.
 * @param atender Función que determina lo que el servidor debe hacer cuando
 * recibe una conexión.
 */
void servidor(const char* host, int puerto, void* atender);

#endif /* SERVIDOR_H_ */
