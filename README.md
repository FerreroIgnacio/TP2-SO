# TPE Grupo 18

### Integrantes:
- Agustín Brunero
- Ignacio Ferrero
- Nicolás Stefan

## Compilación y ejecución

Lo primero que hay que hacer es clonar el repositorio. Eso se puede lograr con el siguiente comando:
`git clone https://github.com/FerreroIgnacio/TP2-SO.git`

Una vez hecho esto, en caso de que el commit de la entrega no sea el último commit, pueden ir al mismo con:
`git checkout [hash_del_commit_de_la_entrega]`

Para compilar el proyecto, debo correr los siguientes comandos desde la carpeta principal (e imagen de la cátedra, opcionalmente). El proyecto se puede compilar con dos memory managers distintos: Buddy y First Fit. Para ello, correr alguno de los siguientes comandos:
`make buddy`
`make all`

Una vez compilado, para correrlo solo hace falta correr:
`./run.sh`

En caso de querer compilarlo de vuelta, es conveniente borrar los archivos de la anterior compilación con:
`make clean`

## Limitaciones
Cantidad de procesos en simultáneo: debido a la poca memoria disponible, dar la posibilidad de tener una gran cantidad de procesos induce errores en el sistema, la memoria se termina pisando y se obtienen constantemente excepciones de instrucción inválida, determinamos que 20 procesos son más que suficientes para mostrar las funcionalidades implementadas. Esto debe ser tenido en cuenta a la hora de correr test como test_processes, que detecta (correctamente) como error la negativa del kernel a crear un proceso con un pid mayor a 20.

## Instrucciones de replicación

#### Comandos implementados:

- `help`             : muestra los comandos disponibles
- `clear`            : limpia la pantalla
- `mem`              : imprime el estado de la memoria
- `ps`               : imprime la lista de todos los procesos
- `loop <segundos>`  : imprime su ID con un saludo cada una determinada cantidad de segundos
- `kill <pid>`       : mata un proceso dado su ID
- `nice <pid> <pri>` : cambia la prioridad de un proceso dado su ID y la nueva prioridad
- `block <pid>`      : switch entre ready y blocked de un proceso dado su ID
- `cat`              : imprime el stdin tal como lo recibe
- `wc`               : cuenta la cantidad de líneas del input
- `filter`           : filtra las vocales del input
- `mvar`             : implementa el problema de múltiples lectores

#### Tests disponibles:
- `test_mm <max-bytes>`                  : ejecuta stress test del manejador de memoria 
- `test_processes <max-processes>`       : crea, bloquea, desbloquea y mata procesos aleatoriamente 
- `test_priority <end-val-for-process>`  : 3 procesos se ejecutan con misma prioridad y luego con distinta (recomen)
- `test_synchro <max-val>`               : varios procesos modifican 1 variable usando semáforos
- `test_no_synchro <max-val>`            : varios procesos modifican una variable sin semáforos

#### Controles especiales:
- `comando_1 <params> | comando_2 <params>` : concatenar comandos con pipe
- `& comando`                               : ejecutar proceso en background
- `Ctrl+D`                                  : enviar EOT por STDIN
- `Ctrl+C`                                  : matar proceso en foreground

### Aclaraciones: 
- En nuestro sistema operativo decidimos implementar las prioridades dejando que cada proceso corra más o menos tiempo DENTRO DE SU CICLO, es decir, la cantidad de switches a ese proceso permanece constante en un sistema round-robin. En este caso, se pierde la noción de prioridad cuando los procesos no son CPU-Bound como en `mvar`. Es por esto que decidimos no agregar el cambio de prioridades durante la ejecución en este comando, ya que no modifica el resultado. Si se implementó el kill a escritores y lectores mediante las teclas 'W' y 'R'.
- Para ver mejor el uso de prioridades en `test_priority`, se recomienda ejecutarlo con el valor 500000000. un valor menor premite que los procesos corran en un ciclo, perdiendose el efecto, un valor mayor hace muy lenta la ejecución.
- Tener en cuenta las limitaciones mencionadas en la sección anterior para `test_processes`, `<max-processes>` no debe superar los 20 procesos (incluyendo los creados previamente) en caso de hacerlo terminará (correctamente) con errores al no poder crear todos los procesos.


## Uso de IA
La inteligencia artificial fue una herramienta fundamental para el desarrollo de nuestro proyecto. Esta nos permitió resolver dudas específicas teniendo nuestro proyecto como contexto, algo que sería imposible con Google/StackOverflow. Era capaz de analizar errores y revisar cientos de líneas de código en minutos para proporcionar posibles soluciones a nuestro problema. Esto nos acotó el tiempo de debugging significativamente. Además, el poder crear funciones enteras solo especificando el funcionamiento de las mismas nos permitió dedicarle el tiempo a pensar las funciones de una forma más abstracta. Sin embargo, la IA era incapaz de implementar por sí misma funcionalidades completas. Al no ser específico, esta interpretaba mal las funcionalidades e implementaba algo diferente a lo que uno quería o directamente con suficientes errores como para que convenga implementarlo a mano. Por otro lado, varias veces era incapaz de diagnosticar correctamente la causa de los errores, por lo cual uno debía hacer seguimiento de las funciones que fallaban a ojo. En conclusión, si bien la IA fue una herramienta increíblemente útil para acotar el tiempo dedicado a cosas frívolas, su incapacidad de razonar hizo que tuviéramos que relegar en la nuestra para obtener un proyecto funcional.