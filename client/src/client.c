#include "client.h"
#include <commons/log.h>

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente
	log_info(logger, "Hola! Soy un log");


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	ip = config_get_string_value(config, "IP");
    puerto = config_get_string_value(config, "PUERTO");

	// Loggeamos el valor de config

    valor = config_get_string_value(config, "CLAVE");


	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje
    enviar_mensaje(valor, conexion);

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger = log_create("tp0.log", "CLIENTE", 1 , LOG_LEVEL_INFO);

	// Buenas prácticas en C: Siempre validar que los punteros no sean nulos.
    // Si log_create falló (ej: no tenés permisos de escritura en la carpeta), devuelve NULL.
    if (nuevo_logger == NULL) {
        printf("¡No se pudo crear el logger!\n");
        abort(); // abort() termina el programa abruptamente de forma segura.
    }
    
    return nuevo_logger;
}

t_config* iniciar_config(void)
{
    t_config* nuevo_config = config_create("cliente.config");
    if (nuevo_config == NULL) {
        printf("¡No se pudo crear el config!\n");
        abort();
    }
    return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa
	leido = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while (strcmp(leido, "") != 0) {
		// 1. Logueamos lo que el usuario envio
		log_info(logger, "ingresaste: %s" , leido);
		// 2. Liberamos la memoria
		free(leido);
		// 3. Volvemos a leer para la siguiente vuelta del bucle
		leido = readline("> ");
	}

	// Si el while terminó, significa que el usuario apretó Enter sin escribir nada.
    // Esa línea vacía que rompió el bucle también ocupa memoria, hay que liberarla.
    free(leido);

}

void paquete(int conexion)
{
    char* leido;
    t_paquete* paquete = crear_paquete(); // 1. Creamos la "caja" vacía

    // 2. Leemos la primera línea
    leido = readline("> ");

    // 3. Mientras no sea un string vacío
    while (strcmp(leido, "") != 0) {
        // Agregamos la línea al paquete. 
        // IMPORTANTE: el tamaño es strlen(leido) + 1 para incluir el '\0'
        agregar_a_paquete(paquete, leido, strlen(leido) + 1);
        
        free(leido); // Liberamos la memoria de la línea actual
        leido = readline("> "); // Leemos la siguiente
    }

    // 4. Liberamos la línea vacía que rompió el while
    free(leido);

    // 5. Enviamos la "caja" entera por la red
    enviar_paquete(paquete, conexion);

    // 6. Destruimos el paquete para liberar esa memoria
    eliminar_paquete(paquete);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
    /* Y por ultimo, hay que liberar lo que utilizamos */
    if (logger != NULL) log_destroy(logger);
    if (config != NULL) config_destroy(config);
    
    // Cerramos la conexión de red
    liberar_conexion(conexion);
}
