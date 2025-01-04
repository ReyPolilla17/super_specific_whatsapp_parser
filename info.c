/**
 * @file info.c
 * 
 * @date 1/1/2025
 * @author Luis Julián Zamora Treviño
 * 
 * @brief Contiene todas las funciones dedicadas a extraer información específica de un mensaje.
 */

#include "parser.h"

/**
 * Obtiene todas las IDs mencionadas en un mensaje y las cataloga como SN o RSD según el contexto del mensaje.
 * Regresa la cantidad de IDs encontrados
 * 
 * @param *string El mensaje
 * @param **SN Donde se almacenará el SN en caso de que exista
 * @param **RSD Donde se almacenará el RSD en caso de que exista
 * 
 * @returns int
 */
int getIDs(char *string, char **SN, char **RSD)
{
    int i = 0; // Contador
    int j = 0; // Contador
    int k = 0; // Contador de IDs encontrados
    int n = 0; // Contador de saltos de línea

    int c = 0; // Coincidencias con el inicio de una ID (ID_BEGGINNING)

    int ceros[2]; // Cantidad de ceros en cada ID
    int known_type[2]; // Valor conocido de cada ID
    int cons = 1; // Si la cuenta es constante

    int l = 0; // Longitud del mensaje
    int l2 = 0; // Longitud del inicio de una ID (ID_BEGGINNING)

    char *placeholder = string; // Apuntador que recorrerá toda la cadena

    char *found_ids[2]; // Cadenas de IDs encontrados
    char *id_start; // Apuntador al inicio de un ID dentro del mensaje
    char a = 0; // Mini buffer para almacenar un caracter

    // inicializa variables
    l = strlen(string);
    l2 = strlen(ID_BEGGINNING);

    *SN = NULL;
    *RSD = NULL;

    for(i = 0; i < 2; i++)
    {
        found_ids[i] = NULL;
        ceros[i] = 0;
        known_type[i] = 0;
    }

    for(i = 0; i < l && k < 2 && n < 5; i++) // recorre todo el mensaje hasta encontrar 2 IDs o detectar 5 saltos de línea (los IDs importantes suelen estar antes del 5° renglón)
    {
        if(string[i] == '\n') // suma a la cuenta de saltos de línea en caso de haber uno
        {
            n++;
        }

        if(string[i] == ID_BEGGINNING[c]) // intenta encontrar un inicio de ID
        {
            c++;
        }
        else
        {
            c = 0;
        }

        if(c == l2) // al encontrar un inicio de ID
        {
            // inicializa variables
            cons = 1;
            c = 0;

            for(j = i + 1; j < l && (string[j] >= '0' && string[j] <= '9'); j++) // se recorre hasta dejar de encontrar números
            {
                if(string[j] == '0') // si encuentra un 0 al inicio, lo suma a la cuenta de ceros de ese ID
                {
                    if(cons)
                    {
                        ceros[k]++;
                    }
                }
                else // de lo contrario, deja de buscar 0 al inicio
                {
                    cons = 0;
                }
            }

            // aloca el espacio exacto del ID encontrado
            found_ids[k] = malloc((j - i + strlen(ID_BEGGINNING)) * sizeof(char)); // j - i contiene el espacio extra para el caracter nulo

            // guarda el valor del último caracter y lo reemplaza por un caracter nulo
            a = string[j];
            string[j] = 0;

            // imprime el valor del id
            id_start = &string[i + 1];
            sprintf(found_ids[k], "%s%s", ID_BEGGINNING, id_start);

            if(strlen(found_ids[k]) == l2) // si el ID no contiene números, libera la memoria
            {
                free(found_ids[k]);
                found_ids[k] = NULL;
            }
            else // si el ID contiene valores
            {
                if(containsString(placeholder, "SN")) // si el mensaje hasta el momento contiene la palabra clave SN
                {
                    known_type[k] = 1; // el posible tipo de ID será 1 (SN)
                }
                else if(containsString(placeholder, "RSD")) // si el mensaje hasta el momento contiene la palabra clave RSD
                {
                    known_type[k] = 2; // el posible tipo de ID será 2 (RSD)
                }

                k++; // incrementa la cuenta de IDs encontrados
            }

            // regresa la cadena a la normalidad y recorre el apuntador de la cadena para buscar otro tipo de ID
            string[j] = a;
            placeholder = &string[j];
        }
    }

    switch(k) // según la cantidad de IDs encontrados
    {
        case 1: // 1 encontrado
            switch(known_type[0]) // según el tipo de ID conocido
            {
                case 1: // SN, guarda el ID como SN
                    *SN = found_ids[0];

                    break;
                case 2: // RSD, guarda el ID como RSD
                    *RSD = found_ids[0];

                    break;
                default: // desconocido
                    if(ceros[0] > RSD_CERO_COUNT) // si la cuenta de ceros al inicio es mayor a la cuenta de ceros esperada para un RSD
                    {
                        *SN = found_ids[0]; // guarda el ID como SN
                    }
                    else // de lo contrario, lo guarda como un RSD
                    {
                        *RSD = found_ids[0];
                    }

                    break;
            }

            break;
        case 2: // 2 encontrados
            switch(known_type[0]) // según el tipo conocido del primer ID
            {
                case 1: // SN, lo guarda como SN y el otro como RSD
                    *SN = found_ids[0];
                    *RSD = found_ids[1];

                    break;
                case 2: // RSD, lo guarda como RSD y el otro como SN
                    *SN = found_ids[1];
                    *RSD = found_ids[0];

                    break;
                default: // desconocido
                    switch(known_type[1]) // según el tipo conocido del segundo ID
                    {
                        case 1: // SN, lo guarda como SN y el otro como RSD
                            *SN = found_ids[1];
                            *RSD = found_ids[0];

                            break;
                        case 2: // RSD, lo guarda como RSD y el otro como SN
                            *SN = found_ids[0];
                            *RSD = found_ids[1];

                            break;
                        default: // desconocido
                            if(ceros[0] >= ceros[1]) // si el primer ID tiene más ceros al inicio que el segundo
                            {
                                // guarda el primero como SN y el segundo como RSD
                                *SN = found_ids[0];
                                *RSD = found_ids[1];
                            }
                            else // de lo contrario, guarda el primero como RSD y el segundo como SN
                            {
                                *SN = found_ids[1];
                                *RSD = found_ids[0];
                            }

                            break;
                    }

                    break;
            }

            break;
    }

    return k;
}

/**
 * Intenta obtener el aplicativo o servicio al que se hace referencia en un mensaje.
 * Regresa 1 en caso de encontrarlo y 0 de lo contrario.
 * 
 * @param *message El mensaje del que se intenta obtener la información
 * 
 * @returns int
 */
int getServiceName(MESSAGE *message)
{
    int i = 0;

    int break_max = 1; // Máximo de saltos de linea a iterar
    int break_count = 0; // Cuenta de saltos de línea iterados
    int bc = 0; // Cuenta de saltos de línea en el mensaje
    int cs = 0; // Si una cadena contiene una cadena
    int a = 0; // Cuenta de *

    char b = 0; // Mini buffer de un caracter

    char *placeholder = NULL; // apuntador que recorrerá el mensaje

    message->service = NULL; // inicializa el espacio del servicio

    for(i = 0; message->start->line[i] != 0; i++) // cuenta la cantidad de saltos de línea del mensaje
    {
        if(message->start->line[i] == '\n')
        {
            bc++;
        }
    }
    
    // busca en todo el mensaje hasta superar la cuenta de saltos de línea o encontrar el aplicativo o servicio
    for(i = 0; message->start->line[i] != 0 && break_count < break_max && a != 2; i++)
    {
        cs = 0; // reinicia la variable

        if(message->start->line[i] == '\n') // aumenta la cuenta de saltos en caso de encontrar uno
        {
            break_count++;
        }
        else if(message->start->line[i] == '*') // si hay un *
        {
            if(!a) // si es el primero que encuentra
            {
                placeholder = &message->start->line[i]; // coloca el apuntador en ese lugar
                a = 1; // encontró un *
            }
            else // de lo contrario
            {
                b = message->start->line[i]; // guarda el caracter en el que está
                message->start->line[i] = 0; // lo cambia por un caracter nulo

                cs = containsString(placeholder, "INFORMATIV"); // determina si la sección seleccionada contiene la palabra INFORMATIV (hay quienes la terminaban con A y quienes la terminaban con O)

                if(containsLetters(placeholder) && !cs) // Si el texto seleccionado contiene letras y no contiene la cadena INFORMATIV
                {
                    a = 2; // ha encontrado el aplicativo o servicio
                }
                else // de lo contrario
                {
                    placeholder = &message->start->line[i]; // recorre el apuntador a ese lugar

                    if(cs) // si contiene la la cadena INFORMATIV, aumenta el maximo de saltos de línea
                    {
                        break_max ++;
                    }
                }

                message->start->line[i] = b; // regresa la cadena a la normalidad
            }
        }

        // si no se ha encontrado el aplicativo o servicio y ya se pasó la primera línea, aumenta la cantidad máxima de saltos de línea
        if(break_max == 1 && break_count == 1 && placeholder == NULL && containsString(message->start->line, "INFORMATIV"))
        {
            break_max++;
        }
    }

    // si no encontró nada que pueda ser el aplicativo o servicio, el mensaje contiene SN o RSD e inicia con emojis
    if(placeholder == NULL && (message->SN != NULL || message->RSD != NULL) && startsWithEmojis(message->start->line))
    {
        // coloca el apuntador al inicio del mensaje y recorre el contador hasta el primer salto de línea (asume que el aplicativo está en el primer renglón del mensaje)
        placeholder = &message->start->line[0];

        for(i = 0; placeholder[i] != '\n'; i++);
    }

    if(placeholder != NULL && bc >= 2) // si hay un posible nombre de aplicativo o servicio y el mensaje tiene más de 2 saltos de línea
    {
        // guarda el carácter de dónde termina el nombre del aplicativo y lo reemplaza por un caracter nulo
        b = message->start->line[i];
        message->start->line[i] = 0;

        customTrim(placeholder, &message->service); // trata esa porción del mensaje para extraer el primer aplicativo mencionado

        message->start->line[i] = b; // devuelve la cadena a su estado original

        if(containsString(message->service, ID_BEGGINNING)) // Si el posible servicio en realidad es una mención a un ID, libera memoria
        {
            free(message->service);
            message->service = NULL;
        }
        else // de lo contrario, encontró el servicio y regresa 1
        {
            return 1;
        }
    }

    return 0;
}

/**
 * Obtiene los emogis de todos los mensajes de reportes que se usan para indicar el estatus de un reporte.
 * 
 * @param *start EL inicio de los mensajes de reporte
 * 
 * @returns void
 */
void getEmogis(MESSAGE *start)
{
    MESSAGE *temp = start; // Apuntador para recorrer la lista

    int i = 0; // contador
    
    char *placeholder; // apuntador que recorrerá la cadena del mensaje
    char b = 0; // mini buffer de un caracter

    while(temp != NULL) // para todos los mensajes
    {
        placeholder = &temp->start->line[0]; // coloca el apuntador al inicio de la cadena del mensaje

        for(i = 0; !startsWithEmojis(placeholder) && temp->start->line[i] != 0; i++) // recorre toda la cadena del mensaje hasta encontrar un emoji
        {
            placeholder = &temp->start->line[i];
        }

        if(startsWithEmojis(placeholder))// si se encontró un emogi
        {
            for(i = 0; placeholder[i] != 0 && placeholder[i] < 0; i++); // se recorre hasta donde dejen de haber emogis

            // guarda el último carácter y lo reemplaza por un carácter nulo
            b = placeholder[i];
            placeholder[i] = 0;

            // guarda los emojis del mensaje
            temp->emojis = malloc((strlen(placeholder) + 1) * sizeof(char));
            strcpy(temp->emojis, placeholder);

            placeholder[i] = b; // regresa la cadena a la normalidad
        }

        temp = temp->next; // se recorre al siguiente mensaje
    }

    return;
}

/**
 * Obtiene las marcas de tiempo indicadas en el mensaje, esto se hace antes de jutnar las lineas de cada mensaje para mayor simplicidad
 * 
 * @param *message El mensaje del que se quieren obtener las marcas de tiempo
 * 
 * @returns void
 */
void getTimestamps(MESSAGE *message)
{
    FILE_LINE *temp = message->start; // El inicio de las líneas del mensaje

    char *placeholder; // Apuntador que recorrerá cada cadena de caracteres

    // información de la marca de tiempo
    int day = -1;
    int month = -1;
    int year = -1;
    int hour = -1;
    int minute = -1;
    
    int i = 0; // contador
    int f = 0; // marca de tiempo encontrada

    while(temp != NULL) // para todas las lineas del mensaje
    {
        // reinicia variables
        day = -1;
        month = -1;
        year = -1;
        hour = -1;
        minute = -1;
        f = 0;

        placeholder = &temp->line[0]; // coloca el apuntador al inicio de la cadena

        // recorre el apuntador por toda la cadena hasta poder leer 3 numeros en el formato nn/nn/nn
        for(i = 0; temp->line[i] != 0 && sscanf(placeholder, "%d/%d/%d", &day, &month, &year) != 3; i++)
        {
            placeholder = &temp->line[i];
        }

        if(temp->line[i] == 0) // si no encontó el formato, guarda la fecha en la que se mandó el mensaje
        {
            placeholder = &temp->line[0]; // coloca el apuntador al inicio de la cadena
            i = 0;

            sscanf(message->timestamp, "[%d/%d/%d", &day, &month, &year);
        }

        for(i = i; temp->line[i] != 0 && !f; i++) // recorre el apuntador hasta leer 2 números en el formato hh:mm o hh.mm
        {
            placeholder = &temp->line[i];

            if(sscanf(placeholder, "%d:%d", &hour, &minute) == 2 || sscanf(placeholder, "%d.%d", &hour, &minute) == 2)
            {
                f = 1;
            }
        }

        // si se econtraron todos los datos de la marca de tiempo en la línea
        if((day <= 31 && day > 0) && (month <= 12 && month > 0) && (year <= 3000 && year > 0) && (hour <= 24 && hour >= 0) && (minute <= 60 && minute >= 0))
        {
            // si la línea indica que es de inicio y no hay un inicio indicado todavía, guarda esa marca de tiempo como inicio
            if(containsString(temp->line, "nicio") && message->r_start == NULL)
            {
                message->r_start = malloc(strlen("dd/mm/aaaa hh:mm ") * sizeof(char));
                sprintf(message->r_start, "%02d/%02d/%02d %02d:%02d", day, month, year, hour, minute);                
            }
            // si la línea indica que es de reporte y no hay un reporte indicado todavía, guarda esa marca de tiempo como reporte
            else if(containsString(temp->line, "eporte") && message->r_report == NULL)
            {
                message->r_report = malloc(strlen("dd/mm/aaaa hh:mm ") * sizeof(char));
                sprintf(message->r_report, "%02d/%02d/%02d %02d:%02d", day, month, year, hour, minute);
            }
            // si la línea indica que es de fin o recuperación y no hay un fin o recuperación indicado todavía, guarda esa marca de tiempo como fin o recupreación
            else if((containsString(temp->line, "ecupera") || containsString(temp->line, "Fin")) && message->r_recover == NULL)
            {
                message->r_recover = malloc(strlen("dd/mm/aaaa hh:mm ") * sizeof(char));
                sprintf(message->r_recover, "%02d/%02d/%02d %02d:%02d", day, month, year, hour, minute);
            }
        }

        temp = temp->next; // se recorre a la siguiente línea
    }

    if(message->r_report == NULL) // si no hay fecha de reporte, guarda la fecha en la que se mandó el mensaje
    {
        sscanf(message->timestamp, "[%d/%d/%d, %d:%d", &day, &month, &year, &hour, &minute);
        
        message->r_report = malloc(strlen("dd/mm/aaaa hh:mm ") * sizeof(char));
        sprintf(message->r_report, "%02d/%02d/%02d %02d:%02d", day, month, year, hour, minute);
    }

    return;
}