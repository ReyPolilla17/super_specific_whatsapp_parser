/**
 * @file files.c
 * @author Luis Julián Zamora Treviño
 * @date 12/32/2024
 * 
 * @brief Contiene todas las funciones encargadas de interactuar con archivos
 */
#include "parser.h"

/**
 * Carga todos los mensajes de una transcripción de mensajes de WhatsApp guardada en un .txt, separandolos entre mensajes de sistema, mensajes con reportes y otros.
 * Los mensajes con reportes entonces son separados por aplicativo o servicio y después por incidente
 * 
 * @param *info La estructua en la que se almacena toda la información extraida del archivo
 * @param *filename El nombre del archivo a cargar
 * 
 * @returns void
 */
void loadFile(COLLECTOR *info, char *filename)
{
    FILE *fp; // El archivo con los mensajes

    MESSAGE *new_m; // Nuevo mensaje
    MESSAGE *cur_m; // Mensaje actual

    FILE_LINE *new_l; // Nueva línea
    FILE_LINE *temp_l; // Línea temporal

    int line_len = 0; // Longitud de la línea

    // Momento en el que se envió el mensaje
    int day = 0;
    int month = 0;
    int year = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;

    char cur = 0; // Carácter actual del mensaje archivo

    fp = fopen(filename, "rb");

    if(fp != NULL) // Si existe el archivo
    {
        while(fread(&cur, sizeof(char), 1, fp)) // Mientras pueda leer un carácter
        {
            line_len ++; // Suma a la longitud de la línea

            if(cur == '\n') // si el carácter es un salto de línea
            {
                // crea un nuevo elemento de línea y guarda la línea del archivo
                new_l = malloc(sizeof(FILE_LINE));
                new_l->l = line_len;
                new_l->next = NULL;
                new_l->line = malloc((line_len + 1) * sizeof(char));

                fseek(fp, -1 * (line_len * sizeof(char)), SEEK_CUR);
                fread(new_l->line, sizeof(char), line_len, fp);
                new_l->line[line_len] = 0;

                if(matchesFormat(new_l, &day, &month, &year, &hour, &minute, &second)) // si la línea contiene el formato de encabezado de mensaje de WhatsApp
                {
                    // crea un nuevo mensaje y coloca la información obtenida hasta el momento
                    new_m = malloc(sizeof(MESSAGE));
                    new_m->start = new_l;
                    new_m->next = NULL;
                    new_m->RSD = NULL;
                    new_m->SN = NULL;
                    new_m->emojis = NULL;
                    new_m->r_start = NULL;
                    new_m->r_report = NULL;
                    new_m->r_recover = NULL;
                    new_m->service = NULL;

                    new_m->timestamp = malloc(strlen("[dd/mm/yyyy, hh:mm:ss] ") * sizeof(char));
                    sprintf(new_m->timestamp, "[%02d/%02d/20%02d, %02d:%02d:%02d]", day, month, year, hour, minute, second);
                    
                    // inserta el mensaje al inicio de la lista de mensajes de reportes (temporalmente)
                    if(info->report_messages == NULL)
                    {
                        info->report_messages = new_m;
                        cur_m = new_m;
                    }
                    else
                    {
                        cur_m->next = new_m;
                        cur_m = cur_m->next;
                    }
                }
                else // de lo contrario
                {
                    // inserta la línea en la lista de líneas del mensaje actual
                    temp_l = cur_m->start;

                    while(temp_l->next != NULL)
                    {
                        temp_l = temp_l->next;
                    }

                    temp_l->next = new_l;
                }

                line_len = 0; // reinicia la longitud de la línea
            }            
        }

        fclose(fp); // cierra el archivo
    }
    else // De lo contrario, imprime error
    {
        printf("Error al leer el archivo...\n");
    }

    return;
}

/**
 * Guarda todos los reportes de cada aplicativo o servicio en un directorio diferente dentro de un directorio con un nombre establecido por aplicativo o servicio.
 * Al mimso tiempo, libera la memoria alocada para cada servicio.
 * 
 * @param *info Toda la información recolecata del archivo
 * @param *dirname La ruta relativa del directorio en el que se crearán los diversos directorios
 * 
 * @returns void
 */
void printAndFreeServices(COLLECTOR *info, char *dirname)
{
    SERVICE *temp; // Aplicativo o servicio actual

    char *terminal_command; // Comando de terminal a ejecutar
    char *dir_name; // Nombre del directorio tratado
    char *dir_son; // Nombre del subdirectorio tratado
    char *directory; // nombre del archivo en el que se guardará el reporte

    swapSpaces(dirname, &dir_name); // trata el nombre del directorio principal para eliminar espacios y carácteres problemáticos

    // ejecuta un comando para eliminar el directorio principal en caso de que exista
    terminal_command = malloc((strlen("rm -rf ./ ") + strlen(dir_name)) * sizeof(char));
    sprintf(terminal_command, "rm -rf ./%s", dir_name);
    system(terminal_command);
    free(terminal_command);

    // ejecuta un comando para crear el directorio principal
    terminal_command = malloc((strlen("mkdir ") + (strlen(dir_name))) * sizeof(char));
    sprintf(terminal_command, "mkdir %s", dir_name);
    system(terminal_command);
    free(terminal_command);

    while(info->services != NULL) // para cadas aplicativo o servicio en la lista
    {
        temp = info->services; // apunta al aplicativo o servicio

        swapSpaces(temp->name, &dir_son); // trata el nombre del servicio

        // crea un sub directorio en el directorio principal con el nombre tratado del servicio
        terminal_command = malloc((strlen("mkdir .// ") + strlen(dir_son) + strlen(dir_name)) * sizeof(char));
        sprintf(terminal_command, "mkdir ./%s/%s", dir_name, dir_son);
        system(terminal_command);
        free(terminal_command);

        // crea la ruta relativa al directorio en el que se van a guardar los archivos de los reportes
        directory = malloc(strlen("./// ") + (strlen(dir_name) + strlen(dir_son)) * sizeof(char));
        sprintf(directory, "./%s/%s/", dir_name, dir_son);

        printAndFreeReports(&temp->reports, directory); // imprime y libera la memoria de cada reporte 

        // se recporre al siguiente aplicativo o servicio y libera memoria
        info->services = info->services->next;
        free(temp->compressed_name);
        free(temp->name);
        free(temp);

        free(directory);
        free(dir_son);
    }
    
    free(dir_name); // libera memoria
    
    return;
}

/**
 * Guarda cada reporte de un servicio en un directorio específico.
 * Al mimso tiempo, libera la memoria alocada del reporte.
 * 
 * @param **start El inicio de los reportes del servicio
 * @param *dirname La ruta relativa al directorio en el que se crearán los archivos
 * 
 * @returns void
 */
void printAndFreeReports(REPORT **start, char *dirname)
{
    FILE *fp; // Archivo en el que se guardará el reporte
    REPORT *temp; // Reporte actual

    char *filename; // nombre del archivo en el que se guardará el reporte

    int i = 1; // contador de reportes sin ID

    while(*start != NULL)
    {
        temp = *start; // se recorre al reporte actual

        if(temp->SN != NULL) // si el reporte tiene ID de tipo SN, el archivo tendrá ese ID por nombre
        {
            filename = malloc((strlen(dirname) + strlen("SN_.txt ") + strlen(temp->SN)) * sizeof(char));
            sprintf(filename, "%sSN_%s.txt", dirname, temp->SN);
        }
        else if(temp->RSD != NULL) // de lo contrario, si el reporte tiene un ID de tipo RST, el archivo tendrá ese ID por nombre
        {
            filename = malloc((strlen(dirname) + strlen("RSD_.txt ") + strlen(temp->RSD)) * sizeof(char));
            sprintf(filename, "%sRSD_%s.txt", dirname, temp->RSD);
        }
        else // de lo contrario, el nombre del archivo indicará que no contiene ID y el número de reporte sin ID
        {
            filename = malloc((strlen(dirname) + strlen("NO_ID(    ).txt ")) * sizeof(char));
            sprintf(filename, "%sNO_ID(%d).txt", dirname, i);
            i++;
        }

        fp = fopen(filename, "wt"); // abre el archivo

        // coloca la información general del reporte obtenida
        fprintf(fp, "-------------------------------------------INFORMACIÓN DEL INCIDENTE------------------------------------------\n");

        // si existe el dato, lo imprime y libera memoria, de lo contrario, imprime no encontrado
        // información que imprime: SN, RSD, Inicio del incidente, Reporte del incidente, Recuperación del incidente
        if(temp->SN != NULL)
        {
            fprintf(fp, "SN: '%s'\n", temp->SN);
            free(temp->SN);
        }
        else
        {
            fprintf(fp, "SN: 'NOT FOUND'\n");
        }

        if(temp->RSD != NULL)
        {
            fprintf(fp, "RSD: '%s'\n", temp->RSD);
            free(temp->RSD);
        }
        else
        {
            fprintf(fp, "RSD: 'NOT FOUND'\n");
        }

        if(temp->r_start != NULL)
        {
            fprintf(fp, "INICIO: '%s'\n", temp->r_start);
            free(temp->r_start);
        }
        else
        {
            fprintf(fp, "'INICIO: NOT FOUND'\n");
        }

        if(temp->r_report != NULL)
        {
            fprintf(fp, "REPORTE: '%s'\n", temp->r_report);
            free(temp->r_report);
        }
        else
        {
            fprintf(fp, "REPORTE: 'NOT FOUND'\n");
        }

        if(temp->r_recover != NULL)
        {
            fprintf(fp, "RECUPERACIÓN: '%s'\n", temp->r_recover);
            free(temp->r_recover);
        }
        else
        {
            fprintf(fp, "RECUPERACIÓN: 'NOT FOUND'\n");
        }

        fprintf(fp, "----------------------------------------------------REPORTES--------------------------------------------------\n");

        fclose(fp); // cierra el archivo

        printAndFree(&temp->timeline, filename); // imprime todos los mensajes del reporte en el mismo archivo

        // se recorre al siguiente reporte y libera memoria
        *start = temp->next;
        free(temp);
        free(filename);
    }

    return;
}

/**
 * Guarda en un archivo todos los mensajes de una lista dinámica de mensajes, al archivo se le agrega la información de los mensajes en lugar de sobreescribirlo por completo.
 * Al mimso tiempo, libera la memoria alocada para cada mensaje.
 * 
 * @param **start El inicio de la lista dinámica de mensajes.
 * @param *filename El nombre del archivo en el que agregar la información.
 * 
 * @returns void
 */
void printAndFree(MESSAGE **start, char *filename)
{
    FILE *fp; // El archivo a escribir

    MESSAGE *temp_m; // Mensaje temporal
    FILE_LINE *temp_l; // Línea del mensaje temporal

    int i = 0; // Contador de mensajes

    fp = fopen(filename, "at"); // Abre el archivo

    while(*start != NULL) // para todos los elementos de la lista de mensajes
    {
        i ++; // Suma al contador de mensajes

        temp_m = *start; // se recorre al mensaje actual

        // imprime la fecha en la que se mandó el mensaje
        fprintf(fp, "\n--------------------------------------------%s--------------------------------------------\n", temp_m->timestamp);
        
        // imprime toda la información específica que se encontró en el archivo
        fprintf(fp, "{");

        // si existe el elemento, lo imprime y libera memoria, de lo contrario imprime que no fue encontrado
        // información que imprime: SN, RSD, Nombre del aplicativo o servicio, Emogis del mensaje, Inicio del incidente, Reporte del incidente, Recuperación del incidente
        if(temp_m->SN != NULL)
        {
            fprintf(fp, "SN: '%s', ", temp_m->SN);
            free(temp_m->SN);
        }
        else
        {
            fprintf(fp, "SN: 'NOT FOUND', ");
        }

        if(temp_m->RSD != NULL)
        {
            fprintf(fp, "RSD: '%s', ", temp_m->RSD);
            free(temp_m->RSD);
        }
        else
        {
            fprintf(fp, "RSD: 'NOT FOUND', ");
        }

        if(temp_m->service != NULL)
        {
            fprintf(fp, "SERVICE: '%s', ", temp_m->service);
            free(temp_m->service);
        }
        else
        {
            fprintf(fp, "SERVICE: 'NOT FOUND', ");
        }

        if(temp_m->emojis != NULL)
        {
            fprintf(fp, "EMOGIS: '%s', ", temp_m->emojis);
            free(temp_m->emojis);
        }
        else
        {
            fprintf(fp, "EMOGIS: 'NOT FOUND', ");
        }

        fprintf(fp, "TIMESTAMPS: [");

        if(temp_m->r_start != NULL)
        {
            fprintf(fp, "'%s', ", temp_m->r_start);
            free(temp_m->r_start);
        }
        else
        {
            fprintf(fp, "'NOT FOUND', ");
        }

        if(temp_m->r_report != NULL)
        {
            fprintf(fp, "'%s', ", temp_m->r_report);
            free(temp_m->r_report);
        }
        else
        {
            fprintf(fp, "'NOT FOUND', ");
        }

        if(temp_m->r_recover != NULL)
        {
            fprintf(fp, "'%s'], ", temp_m->r_recover);
            free(temp_m->r_recover);
        }
        else
        {
            fprintf(fp, "'NOT FOUND'], ");
        }

        fprintf(fp, "}\n");

        while(temp_m->start != NULL) // para todas las líneas del mensaje
        {
            // imprime la línea actual, libera la memoria y se recorre a la iguiente línea
            temp_l = temp_m->start;
            temp_m->start = temp_l->next;

            fprintf(fp, "%s\n", temp_l->line);

            free(temp_l->line);
            free(temp_l);
        }

        fprintf(fp, "--------------------------------------------------------------------------------------------------------------\n");
        
        *start = temp_m->next; // se recorre al siguiente mensaje
        
        // libera memoria del mensaje
        free(temp_m->timestamp);
        free(temp_m);
    }
    
    fclose(fp); // cierra el mensaje
    
    return;
}

