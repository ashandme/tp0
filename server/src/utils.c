#include"utils.h"

t_log* logger;
/*
  Consigna

  El entregable de esta etapa es enviar al servidor el valor de CLAVE en el archivo de configuración, y luego enviarle todas las líneas que se ingresaron por consola juntas en un paquete.

  Simplificando un poco, una conexión por socket hacia otro programa va a requerir de realizar lo siguiente:

  - Iniciar el servidor en la función iniciar_servidor() del utils del server.
  - Esperar a que el cliente se conecte mediante la función esperar_cliente()
  - Crear una conexión contra el servidor en la función crear_conexión() del utils del client.
  - Enviar como mensaje el valor de CLAVE.
  - Ir juntando las líneas que se leen por consola para luego enviarlas como paquete.
  - Cerrar la conexión.
 */

int iniciar_servidor(void)
{
  // Quitar esta línea cuando hayamos terminado de implementar la funcion
  // assert(!"no implementado!");

  int socket_servidor;

  struct addrinfo hints, *servinfo, *p;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  char port[4];
  sprintf(port, "%d", PUERTO);
  getaddrinfo(NULL, port, &hints, &servinfo);
  // Creamos el socket de escucha del servidor
  socket_servidor = socket(servinfo->ai_family,
                           servinfo->ai_socktype,
                           servinfo->ai_protocol);
  // Asociamos el socket a un puerto
  setsockopt(socket_servidor, SOL_SOCKET , SO_REUSEPORT, &(int){1}, sizeof(int));

  bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
  // Escuchamos las conexiones entrantes
  if (listen(socket_servidor, SOMAXCONN) < 0){
    log_error(logger, "chau");
    return -1;
  };
  freeaddrinfo(servinfo);
  log_trace(logger, "Listo para escuchar a mi cliente");

  return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
  // Quitar esta línea cuando hayamos terminado de implementar la funcion
  //assert(!"no implementado!");
  // Aceptamos un nuevo cliente
  int socket_cliente;
  while(socket_servidor){
    socket_servidor = accept(socket_cliente, NULL, NULL);
  }
  log_info(logger, "Se conecto un cliente!");

  return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
