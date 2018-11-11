# Servidor Switch

Programa que ejecuta un servidor que acepta peticiones de procesos clientes y 
responde comunicándose con varios servidores de bases de datos, permitiendo al 
cliente elegir una base de datos y hacerle consultas.

El switch se comunica con los distintos servidores usando las API respectivas,
tienendo soporte para:
* MySQL/MariaDB
* PostgreSQL

Este switch puede conectarse a varios servidores de cada tipo 

### Comunicación
Los clientes se comunican con el switch usando el formato JSON para indicar la
información pedida y los parámetros de la petición del cliente, y el resultado de las 
consultas de la respuesta del switch.

### Configuración

#### switch.json
La ubicación del servidor switch se configura a través de **switch.json**, en 
donde se indican nombre de host y número de puerto. A este archivo lo acceden 
tanto el cliente como el servidor switch.

#### servidores.json
Este archivo reúne la información de todos los servidores a los que el switch
puede conectarse. Para cada servidor también se indican las bases de datos a
los que se puede conectar.

* **servidores**:
    * *nombre*: Nombre para designar al servidor desde el sistema.
    * *activo*: (true/false) Determina si el servidor switch lo ignora.
    * *tipo*: Caracter que indica el SGBD del servidor, siendo:
        * m: MySQL/MariaDB
        * p: PostgreSQL
    * *host*: Nombre de host del servidor.
    * *puerto*: Número de puerto del servidor.
    * *bases_de_datos*: Arreglo con la información de las bases de datos a las 
    que se puede conectar el switch.
* **Bases de datos**:
    * *base*: Nombre de la base de datos.
    * *usuario*: Nombre de usuario con el que se quiere conectar a la base de 
    datos.
    * *contraseña*: Contraseña del usuario.

## Servidor:
El servidor se inicia primero mostrando el host y el número de puerto que le
fue asignado en el archivo switch.json.

Luego muestra todos los servidores
activos que aparecen en servidores.json junto con las bases de datos 
disponibles.

Después, todas las conexiones a las bases de datos, mostrando "Conectado" en
las bases de datos a los que se pudo conectar.

Y al final, va mostrando mensajes de los sockets que se van abriendo para 
atender a los clientes.

## Cliente:
Al ejecutar el cliente se muestran los nombres de los servidores junto al tipo 
de SGBD que utilizan. Luego muestra una serie de comandos disponibles con su
descripción y los datos solicitados.

El cliente luego ingresa el comando (una letra) y pulsa Enter, y el cliente
envía la petición al switch para que la resuelva. Luego el cliente recibe el
resultado y lo muestra en forma de tabla.

Se cierra el cliente usando el comando **x**.

## Compilación de ejecutables
* Clonar repositorio desde https://github.com/LeonMarchetti/Servidor-Switch.git 
o descargar como ZIP con https://github.com/LeonMarchetti/Servidor-Switch/archive/master.zip
* Ejecutar **make**. Esto crea las carpetas **run** con los archivos 
ejecutables y **object** con los archivos objeto. 
    * Es posible tener que modificar el archivo **Makefile** dependiendo de 
    donde se tengan las librerías de los conectores a las bases de datos
    para compilar los ejecutables.

## Ejecución

### Servidor:

Ejecutar **Servidor-Switch.exe** desde el mismo directorio que los archivos
**switch.json** y **servidores.json**.

### Cliente:
Ejecutar **Cliente-Switch.exe** desde el mismo directorio que el archivo de 
configuración **switch.json**.
