/**
 * @file sort.c
 * 
 * @date 3/1/2025
 * @author Luis Julián Zamora Treviño
 * 
 * @brief Contiene todas las funciones encargadas de organizar los reportes por Aplicativo o servicio y por incidente
 */
#include "parser.h"

/**
 * Ordena todos los mensajes de reportes en aplicativos o servicios y reportes
 * 
 * @param *info Estructura con la información de los mensajes y la lista de aplicativos o servicios.
 * 
 * @returns void
 */
void sortReports(COLLECTOR *info)
{
    MESSAGE *temp = info->report_messages; // apuntador a los mensajes de reportes
    MESSAGE *temp2; // apuntador auxiliar
    
    while(temp != NULL) // para cada mensaje
    {
        temp2 = temp->next; // guarda la posición del siguiente

        insertIntoServices(&info->services, &temp); // inserta el mensaje actual a algun servicio

        temp = temp2; // se recorre al siguiente mensaje
    }

    info->report_messages = NULL; // termina de desligar los mensajes a la lista de mensajes de reportes

    return;
}

/**
 * Inserta un mensaje en la lista del aplicativo o servicio que le corresponde y lo agrupa segun el reporte al que pertenezca
 * 
 * @param **start La lista de aplicativos o servicios.
 * @param **message El mensaje a insertar.
 * 
 * @returns void
 */
void insertIntoServices(SERVICE **start, MESSAGE **message)
{
    SERVICE *temp = *start; // apuntador a la lista de aplicativos o servicios
    SERVICE *new; // apuntador auxiliar

    int n = 1; // Indicador de creación de nuevo aplicativo o servicio

    char *compressed_name; // nombre del aplicativo o servicio del mensaje comprimido

    compressString((*message)->service, &compressed_name); // obtiene una verisón reducida de la cadena

    while(temp != NULL && n) // busca un aplicativo o servicio con el nombre del servicio del mensaje
    {
        if(!strcmp(compressed_name, temp->compressed_name))
        {
            free(compressed_name); // libera memoria
            n = 0; // indica que encontró el aplicativo al que pertenece
        }

        temp = temp->next;
    }

    if(n) // si no lo encuentra, crea un aplicativo o servicio con ese nombre y lo agrega a la lista
    {
        new = malloc(sizeof(SERVICE));

        new->reports = NULL;

        new->compressed_name = compressed_name;
        firstElementOfString((*message)->service, &new->name);

        new->next = *start;
        *start = new;

    }
    
    insertIntoReports(&new->reports, message); // inserta el mensaje al reporte correspondiente

    return;
}

/**
 * Inserta un mensaje al reporte correspondiente del aplicativo o servicio
 * 
 * @param **start La lista de reportes del aplicativo o servicio.
 * @param **mesage El mensaje a insertar
 */
void insertIntoReports(REPORT **start, MESSAGE **message)
{
    MESSAGE *msg = *message; // apuntador al mensaje
    MESSAGE *temp_m; // apuntado auxiliar
    
    REPORT *temp_r = *start; // apuntador a los reportes
    REPORT *new; // apuntador auxiliar

    int f = 0; // reporte encontrado

    msg->next = NULL; // saca al mensaje de la lista de mensajes

    while(temp_r != NULL && !f) // para todos los reportes
    {
        if(messageIsFromReport(msg, temp_r)) // si el mensaje tiene alguna relación con el reporte
        {
            temp_m = temp_r->timeline;

            if(temp_m != NULL) // se dirige al final de la lista de mensajes y lo inserta
            {
                while(temp_m->next != NULL)
                {
                    temp_m = temp_m->next;
                }

                temp_m->next = msg;
            }
            else
            {
                temp_r->timeline = msg;
            }

            f = 1; // indica que encontró donde colocar el mensaje
        }

        temp_r = temp_r->next;
    }

    if(!f) // si no se ha encontrado un reporte similar
    {
        new = malloc(sizeof(REPORT)); // crea un elemento de reprte y le coloca la información del mensaje
        new->next = NULL;

        if(msg->r_start != NULL)
        {
            new->r_start = malloc((strlen(msg->r_start) + 1) * sizeof(char));
            strcpy(new->r_start, msg->r_start);
        }
        else
        {
            new->r_start = NULL;
        }

        if(msg->r_report != NULL)
        {
            new->r_report = malloc((strlen(msg->r_report) + 1) * sizeof(char));
            strcpy(new->r_report, msg->r_report);
        }
        else
        {
            new->r_report = NULL;
        }

        if(msg->r_recover != NULL)
        {
            new->r_recover = malloc((strlen(msg->r_recover) + 1) * sizeof(char));
            strcpy(new->r_recover, msg->r_recover);
        }
        else
        {
            new->r_recover = NULL;
        }

        if(msg->RSD != NULL)
        {
            new->RSD = malloc((strlen(msg->RSD) + 1) * sizeof(char));
            strcpy(new->RSD, msg->RSD);
        }
        else
        {
            new->RSD = NULL;
        }

        if(msg->SN != NULL)
        {
            new->SN = malloc((strlen(msg->SN) + 1) * sizeof(char));
            strcpy(new->SN, msg->SN);
        }
        else
        {
            new->SN = NULL;
        }

        new->timeline = msg;

        if(*start != NULL) // inserta el elemento en la lista de reportes
        {
            temp_r = *start;

            while(temp_r->next != NULL)
            {
                temp_r = temp_r->next;
            }

            temp_r->next = new;
        }
        else
        {
            *start = new;
        }
    }

    return;
}

/**
 * Determina si un mensaje está relacionado con un reporte específico.
 * Regresando 1 si el mensaje está relacionado y 0 de l contrario.
 * 
 * @param *message El mensaje a evaluar
 * @param *report El reporte a evaluar
 * 
 * @returns int
 */
int messageIsFromReport(MESSAGE *message, REPORT *report)
{
    int v = 0; // El mensaje es del reporte mencionado

    // si el RSD del mensaje y el reporte coinciden, es parte del reporte
    if((message->RSD != NULL && report->RSD != NULL) && !strcmp(message->RSD, report->RSD))
    {
        v = 1;
    }
    // si el SN del mensaje y del reporte coinciden, es parte del reporte
    else if((message->SN != NULL && report->SN != NULL) && !strcmp(message->SN, report->SN))
    {
        v = 1;
    }
    // si la fecha de reporte y de inicio coinciden, es parte del reporte
    else if(((message->r_start != NULL && report->r_start != NULL) && !strcmp(message->r_start, report->r_start)) && ((message->r_report != NULL && report->r_report != NULL) && !strcmp(message->r_report, report->r_report)))
    {
        v = 1;
    }

    // actualiza la información del reporte
    if(v)
    {
        if(message->RSD != NULL && report->RSD == NULL)
        {
            report->RSD = malloc((strlen(message->RSD) + 1) * sizeof(char));
            strcpy(report->RSD, message->RSD);
        }

        if(message->SN != NULL && report->SN == NULL)
        {
            report->SN = malloc((strlen(message->SN) + 1) * sizeof(char));
            strcpy(report->SN, message->SN);
        }

        if(message->r_start != NULL && report->r_start == NULL)
        {
            report->r_start = malloc((strlen(message->r_start) + 1) * sizeof(char));
            strcpy(report->r_start, message->r_start);
        }

        if(message->r_report != NULL && report->r_report == NULL)
        {
            report->r_report = malloc((strlen(message->r_report) + 1) * sizeof(char));
            strcpy(report->r_report, message->r_report);
        }

        if(message->r_recover != NULL && report->r_recover == NULL)
        {
            report->r_recover = malloc((strlen(message->r_recover) + 1) * sizeof(char));
            strcpy(report->r_recover, message->r_recover);
        }
    }

    return v;
}