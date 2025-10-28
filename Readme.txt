El repositorio deberá poseer un README en formato markdown y desarrollar de forma breve los ítems listados a continuación:

● Instrucciones de compilación y ejecución.
● Instrucciones de replicación:
  ○ Nombre preciso y breve descripción de cada comando / test y parámetros que admiten.
  ○ Caracteres especiales para pipes y comandos en background.
  ○ Atajos de teclado para interrumpir la ejecución y enviar EOF.
  ○ Ejemplos, por fuera de los tests, para demostrar el funcionamiento de cada requerimiento.
  ○ Requerimientos faltantes o parcialmente implementados.
● Limitaciones.
● Citas de fragmentos de código / uso de IA
---
x64BareBones is a basic setup to develop operating systems for the Intel 64 bits architecture.

The final goal of the project is to provide an entryPoint point for a kernel and the possibility to load extra binary modules separated from the main kernel.

Environment setup:
1- Install the following packages before building the Toolchain and Kernel:

nasm qemu gcc make

2- Build the Toolchain

Execute the following commands on the x64BareBones project directory:

  user@linux:$ cd Toolchain
  user@linux:$ make all

3- Build the Kernel

From the x64BareBones project directory run:

  user@linux:$ make all

4- Run the kernel

From the x64BareBones project directory run:

  user@linux:$ ./run.sh


Author: Rodrigo Rearden (RowDaBoat)
Collaborator: Augusto Nizzo McIntosh