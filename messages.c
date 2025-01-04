/**
 * @file messages.c
 * 
 * @author Luis Julián Zamora Treviño
 * @date 1/1/2025
 * 
 * @brief Contiene todas las funciones que operan de forma básica las estructuras de mensajes
 */
#include "parser.h"

/**
 * Revisa si una línea del archivo contiene el formato de WhatsApp que indica que inicia un nuevo mensaje, regresando 1 en caso de que lo haga y 0 de lo contrario
 * 
 * @param *line_info La línea a evaluar
 * @param *day El día en el que se mandó el mensaje
 * @param *month El mes en el que se mandó el mensaje
 * @param *year El año en el que se mandó el mensaje
 * @param *hour La hora en el que se mandó el mensaje
 * @param *minute El minuto en el que se mandó el mensaje
 * @param *second El segundo en el que se mandó el mensaje
 */
int matchesFormat(FILE_LINE *line_info, int *day, int *month, int *year, int *hour, int *minute, int *second)
{
    char a[1000], b[1000], c[1000]; // Buffers de lectura
    char *placeholder; // Apuntador al inicio del mensaje
    char *new; // Nueva línea del mensaje (contendrá solo los contenidos del mensaje)

    int i = 0; // contadpr
    int cnt = 0; // cuenta de "":""

    // si es capaz de leer en la cadena alguno de los 2 formatos indicados (...[nn/nn/nn, nn:nn:nn]...)
    if(sscanf(line_info->line, "[%d/%d/%d, %d:%d:%d] %[^:]: %[^\n]", day, month, year, hour, minute, second, b, c) == 8 || sscanf(line_info->line, "%[^[][%d/%d/%d, %d:%d:%d] %[^:]: %[^\n]", a, day, month, year, hour, minute, second, b, c) == 9)
    {
        for(i = 0; line_info->line[i] != 0 && cnt < 3; i++) // se recorre hasta el contenido del mensaje
        {
            if(line_info->line[i] == ':')
            {
                cnt ++;
            }
        }

        placeholder = &line_info->line[i + 1]; // úbica el apuntador al inicio del mensaje
        
        // indica el nuevo contenido de la línea y su nueva longitud
        new = malloc((strlen(placeholder) + 1) * sizeof(char));
        strcpy(new, placeholder);
        free(line_info->line);

        line_info->line = new;
        line_info->l = strlen(line_info->line);

        return 1;
    }

    return 0;
}

/**
 * Opera todos los mensajes encontrados en el archivo, separandolos entre sistema, reportes y otros,
 * los mensajes de reportes entonces son tratados para extraer información específica.
 * 
 * @param *info La estructura que contiene toda la información
 * 
 * @returns void
 */
void treatMessages(COLLECTOR *info)
{
    MESSAGE *temp = info->report_messages; // Apuntador a los mensajes
    MESSAGE *temp2; // Apuntador auxiliar

    int r = 0; // Si se debe recorrer al siguiente mensaje

    int name_found = 0; // Si se encontró el nombre del aplicativo o servicio

    while(temp != NULL) // para todos los mensajes
    {
        r = 0; // reinicia la variable

        getTimestamps(temp); // obtiene las marcas de tiempo del mensaje
        joinLines(&temp->start); // une todas las lineas del mensaje
        getIDs(temp->start->line, &temp->SN, &temp->RSD); // obtiene los IDs que hay en el mensaje
        getEmogis(temp); // obtiene los primeros emojis del mensaje

        name_found = getServiceName(temp); // intenta encontrar el nombre del aplicativo o servicio

        if(!name_found) // si no encuentra el nombre del aplicativo o servico, no cuenta como mensaje de reporte
        {
            // extrae el mensaje de la lista
            if(temp != info->report_messages) // si no es el primer mensaje de la lista
            {
                // se recorre hasta el mensaje anterior
                temp2 = info->report_messages;

                while(temp2->next != temp)
                {
                    temp2 = temp2->next;
                }

                // desconecta el mensaje de la lista
                temp2->next = temp->next;
            }
            else // de lo contrario
            {
                // desconecta el mense de la lista e indica que no se debe recorrer al siguiente mansaje
                temp2 = temp->next;
                info->report_messages = temp2;
                
                r = 1;
            }

            temp->next = NULL; // aisla al mensaje

            if(startsWith(temp->start->line, SYSTEM_DIFERENCIATOR)) // si el mensaje es de sistema, lo guarda en la lista de sistema
            {
                addToMessages(&info->system_messages, temp);
            }
            else // de lo contrario, lo guarda en la lista de otros
            {
                addToMessages(&info->other_messages, temp);
            }

            temp = temp2; // regresa al puntero a la lista en la que está trabajando
        }

        if(!r) // si se debe recorrer al siguiente mensaje, lo hace
        {
            temp = temp->next;
        }
    }

    return;
}

/**
 * Une todas las lineas de un mensaje en una sola
 * 
 * @param **start La primera línea del mensaje
 */
void joinLines(FILE_LINE **start)
{
    FILE_LINE *temp = *start; // Apuntador a las líneas
    FILE_LINE *temp2 = NULL; // Apuntador auxiliar

    char *buffer; // Buffer del mensaje
    char *buffer2; // Buffer auxiliar

    int l = 0; // longitud del mensaje

    // inicializa el buffer auxiliar como un mensaje sin contenido
    buffer2 = malloc(sizeof(char));
    buffer2[0] = 0;
    
    while(temp != NULL) // para todas las líneas del mensaje
    {
        l += temp->l; // suma a la longitud del mensaje

        // elimina la indicación del sistema de que el mensaje fue editado en caso de que la linea la contenga
        endsWith(temp->line, MESSAGE_EDITED);
        endsWith(temp->line, MESSAGE_EDITED_2);

        // aloca el epacio para juntar lo que se lleve del mensaje con la siguiente línea del mensaje
        buffer = malloc((l + strlen(buffer2) + 1) * sizeof(char));

        // junta lo que se lleva del mensaje con la sigueitne linea en el buffer y libera el buffer auxiliar
        sprintf(buffer, "%s%s", buffer2, temp->line);
        free(buffer2);

        buffer2 = buffer; // hace que el buffer auxiliar contenga lo que se lleva del mensaje

        if(temp != *start) // si no es el primer elemento de la lista
        {
            temp2 = temp; // guarda el puntero
        }

        temp = temp->next; // se recorre a la siguiente línea

        if(temp2 != NULL) // libera la memoria de la linea anterior si la hay
        {
            free(temp2->line);
            free(temp2);
        }
    }

    // regresa a la primera linea, línea, libera memoria y sobreescribe la nueva información
    temp = *start;

    free(temp->line);

    temp->l = l;
    temp->line = buffer2;
    temp->next = NULL;

    return;
}

/**
 * Agrega un mensaje ya alocado a una lista dinámica de mensajes
 * 
 * @param **start El inicio de la lista dinámica de mensajes
 * @param *newMessage El mensaje a agregar
 * 
 * @returns void
 */
void addToMessages(MESSAGE **start, MESSAGE *newMessage)
{
    MESSAGE *temp; // Apuntador auxiliar

    if(*start == NULL) // si no hay mensajes en la lista
    {
        *start = newMessage; // asigna el mensaje como el primer mensaje de la lista
    }
    else // de lo contrario
    {
        // se recorre hasta el último mensaje
        temp = *start;

        while(temp->next != NULL)
        {
            temp = temp->next;
        }

        // el siguiente mensaje del ultimo mensaje será el nuevo mensaje
        temp->next = newMessage;
    }

    return;
}