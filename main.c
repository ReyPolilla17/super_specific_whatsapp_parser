/**
 * @file main.c
 * @author Luis Julián Zamora Treviño
 * @date 3/1/2024
 * 
 * @brief Este programa lee una transcripción de mensajes de WhatsApp de un chat de reportes de inicdencias de diversos aplicativos y 
 * servicios de una organización para ordenarlos por aplicativo o servicio y número de reporte, los mensajes de reportes suelen tener:
 *      - Uno o varios Aplicativos o servicios involucrados en el incidente, solo el primer aplicativo debe considerarse para la 
 *        organización del reporte. En caso de no contar con éste campo, no se podrá agrupar el mensaje.
 *      - 2 tipos de ID, SN y/o RSD, puede haber solo uno de las dos, ambos o ninguno.
 *      - 3 fechas: fecha de inicio del incidente, fecha de reporte del incidente y fecha de fin o recuperación del incidente, 
 *        algunas fechas pueden no esta específicadas,
 *      -Emogis que indiquen el nivel de severidad del incidente y el estatus del mismo.
 *
 * Toda ésta información se almacenará en un directorio con sub carpetas por cada aplicativo o servicio, en las que se colocarán todos los 
 * reportes del aplicativo, agrupando toda la información pertinente al inicio y colocando todos los mensajes que hagan referencia al 
 * incidente de forma cronológica.
 * 
 * Todos los mensajes que no sean reportes de un incidente o que no se hayan podido organizar se colocarán en 3 archivos de texto 
 * adicionales:
 *      - basura.txt --> Mensajes que no son reportes de un incidente o con muy poca información, lo que dificulte su agrupamiento con 
 *        otros reportes.
 *      - restantes.txt --> Mensajes que se detectaron como reportes de un incidente, pero no pasaron correctamente al proceso de 
 *        acomodamiento.
 *      - sistema.txt --> Mensaes que WhatsApp envía, como notificaciones de mensajes eliminados, adición de miembros, creación del grupo,
 *        etc.
 * 
 * Por motivos legales, no puedo colocar la transcripción del chat en éste repositorio, pero gran parte del código es funcional para 
 * cualquier transcripción de un chat de WhatsApp a la fecha de subida de éste código.
 */
#include "parser.h"

/**
 * Función principal del programa, carga un mensaje de una transcripción de WhatsApp, acomoda la información y la guarda en archivos
 * 
 * @param argc La cuenta de argumentos de inicio
 * @param *argv[] Los argumentos de inicio
 * 
 * @returns int
 */
int main(int argc, char *argv[])
{
    COLLECTOR info; // información encontrada en el archivo

    // inicializa la variable
    info.report_messages = NULL;
    info.other_messages = NULL;
    info.system_messages = NULL;
    info.services = NULL;

    printf("Programa iniciado.\n");

    loadFile(&info, "./a.txt"); // obtiene la información del archivo
    printf("Archivo cargado.\n");

    treatMessages(&info); // extrae toda la información requerida de cada mensaje
    printf("Mensajes acondicionados.\n");
    
    sortReports(&info); // ordena cada mensaje de reporte en donde debe ir
    printf("Reportes organizados.\n");

    printAndFreeServices(&info, "REPORTES"); // guarda los reportes en archivos dentro de directorios con el nombre del servicio dentro de un directorio llamado REPORTES
    printf("Archivos de reportes creados.\n");

    printAndFree(&info.report_messages, "./REPORTES/restantes.txt"); // guarda los mensajes de reporte que no se hayan acomodado
    printAndFree(&info.other_messages, "./REPORTES/basura.txt"); // guarda los mensajes varíos
    printAndFree(&info.system_messages, "./REPORTES/sistema.txt"); // guarda los mensajes del sistema
    printf("Archivos adicionales creados.\n");

    return 0;
}