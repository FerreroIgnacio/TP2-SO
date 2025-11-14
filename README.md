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
El trabajo presenta grandes limitaciones debido a las funcionalidades que no llegamos a implementar. Nuestro objetivo de esta entrega es recibir correcciones sobre aquello que sí pudimos implementar y consideramos que funciona correctamente. Las funcionalidades que presentan errores son (por ahora):
- El Buddy memory manager no pasa el test de memoria (freelist sí los pasa).
- El pasaje de parámetros en la creación de procesos no funciona: un proceso sin parámetros se crea correctamente desde shell, pero si se quiere que ese proceso reciba parámetros, no se puede en la versión actual. Este es el principal limitante a la hora de implementar funcionalidades de shell. Es por esto que prácticamente todos los comandos (si es que están implementados) tienen valores hardcodeado o no corren como procesos (corren como función dentro de shell).


## Instrucciones de replicación

Comandos implementados:
- `help`             : muestra los comandos disponibles
- `clear`            : limpia la pantalla
- `mem`              : muestra el estado de la memoria
- `ps`               : lista todos los procesos (también se lista a sí mismo)
- `loop <segs>`      : crea un proceso que envía un mensaje cada segs segundos (hardcodeado a 5 segs)
- `kill <pid>`       : mata el proceso pid (built-in, no crea un proceso)
- `block <pid>`      : bloquea el proceso pid (built-in, no crea un proceso)
- `test_mm <max_mem>`: test del memory manager (hardcodeado a 10000000 bytes)

- Falta implementar los test de procesos, prioridad y sincronización
- Faltan funcionalidades de shell: Pipear comandos, atajos (ctrl+d y ctrl+c), Elección de background o foreground (&)  
- Faltan los comandos de shell: `nice`, `cat`, `wc`, `filter`, `mvar` y comandos de test.

## Uso de IA
La inteligencia artificial fue una herramienta fundamental para el desarrollo de nuestro proyecto. Esta nos permitió resolver dudas específicas teniendo nuestro proyecto como contexto, algo que sería imposible con Google/StackOverflow. Era capaz de analizar errores y revisar cientos de líneas de código en minutos para proporcionar posibles soluciones a nuestro problema. Esto nos acotó el tiempo de debugging significativamente. Además, el poder crear funciones enteras solo especificando el funcionamiento de las mismas nos permitió dedicarle el tiempo a pensar las funciones de una forma más abstracta. Sin embargo, la IA era incapaz de implementar por sí misma funcionalidades completas. Al no ser específico, esta interpretaba mal las funcionalidades e implementaba algo diferente a lo que uno quería o directamente con suficientes errores como para que convenga implementarlo a mano. Por otro lado, varias veces era incapaz de diagnosticar correctamente la causa de los errores, por lo cual uno debía hacer seguimiento de las funciones que fallaban a ojo. En conclusión, si bien la IA fue una herramienta increíblemente útil para acotar el tiempo dedicado a cosas frívolas, su incapacidad de razonar hizo que tuviéramos que relegar en la nuestra para obtener un proyecto funcional.