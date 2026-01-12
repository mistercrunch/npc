// Declaramos las variables
	#include <sys/io.h>
	#include <stdio.h>

	// Direcciones base de los puertos paralelo
	#define LPT1 0x378
	#define LPT2 0x278

	// Direcciones relativas de los registros
	#define REG_DATOS   0
	#define REG_ESTADO  1
	#define REG_CONTROL 2

	int lpt_base=LPT2;    // Direccion base del puerto a usar
	int estado_actual;
	int read_status()     // Leer el valor del registro de estado
	{
 		 return inb(lpt_base+REG_ESTADO);
	}

	void write_data(int valor) // Escribir un valor en el registro de datos
	{
		outb(valor,lpt_base+REG_DATOS);
	}

	main()
	{
		lpt_base=LPT1;    // Definimos puerto a utilizar
		ioperm(lpt_base,3,1); // Activar los permisos de acceso al puerto paralelo
		write_data(0xFF);
		printf ("Registro de estado = %x\n",read_status());
		estado_actual=read_status();
		if("estado_actual==0xFF")
  		{
			write_data(0x00);
			printf ("Registro de estado = %x\n",read_status());
			estado_actual=read_status();
		}
		ioperm(lpt_base,3,0); // Quitar permisos acceso
	}

	//Fin del programa