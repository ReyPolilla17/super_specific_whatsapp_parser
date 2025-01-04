#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ID_BEGGINNING "INC" // Identificador de ID de reporte
#define SYSTEM_DIFERENCIATOR "‎" // Identificador de mensajes de sistema
#define MESSAGE_EDITED "‎<Se editó este mensaje.>" // Indicación de edición de mensaje
#define MESSAGE_EDITED_2 "‎<Se editó este mensaje.>\n" // Variación de indicación de edición de mensaje
#define SN_CERO_COUNT 5 // cantidad promedio de ceros en un ID de tipo SN
#define RSD_CERO_COUNT 3 // cantidad promedio de ceros en un ID de tipo RSD

// Lista dinámica con lineas de un archivo
typedef struct def_file_line
{
    int l; // Longitud de la línea
    char *line; // Contenido de la línea (dinaámico)
    struct def_file_line *next; // Siguiente línea (dinámico)
} FILE_LINE;

// Lista dinámica con mensajes
typedef struct def_message
{
    char *SN; // ID de tipo SN contenido en el mensaje (dinámico)
    char *RSD; // ID de tipo RSD contenido en el mensaje (dinámico)
    char *service; // Aplicativo o servicio mencionado en el mensaje (dinámico)
    char *timestamp; // Fecha de envío del mensaje (dinámico)
    char *r_start; // Fecha de inicio del inicidente (dinámico)
    char *r_report; // Fecha de reporte del incidente (dinámico)
    char *r_recover; // Fecha de recuperación del incidente (dinámico)
    char *emojis; // Importancia y estatús del reporte indicado con emogis (dinámico)
    FILE_LINE *start; // Lineas del mensaje (dinámico)
    struct def_message *next; // Siguiente mensaje (dinámico)
} MESSAGE;

// Lista dinámica con cada incidente de un aplicativo o servicio
typedef struct def_report
{
    char *SN; // ID de tipo SN del incidente (dinámico)
    char *RSD; // ID de tipo RSD del incidente (dinámico)
    char *r_start; // Fecha de inicio del inicidente (dinámico)
    char *r_report; // Fecha de reporte del incidente (dinámico)
    char *r_recover; // Fecha de recuperación del incidente (dinámico)
    MESSAGE *timeline; // Lista de mensajes que hacen referencia al incidente (dinámico)
    struct def_report *next; // Siguiente reporte (dinámico)
} REPORT;

// Lista dinámica con cada aplicativo o servicio
typedef struct def_service
{
    char *name; // Nombre del aplicativo o servicio (dinámico)
    char *compressed_name; // Nombre del aplicativo o servicio tratado (dinámico)
    REPORT *reports; // Lista de reportes del aplicativo o servicio (dinámico)
    struct def_service *next; // Siguiente aplicativo o servicio (dinámico)
} SERVICE;

// Contenedor de información del archivo
typedef struct def_collector
{
    MESSAGE *report_messages; // Lista de mensajes considerados como reportes de incidentes
    MESSAGE *other_messages; // Lista de mensajes considerados como irrelevantes
    MESSAGE *system_messages; // Lista de mensajes considerados del sistema
    SERVICE *services; // dynamic
} COLLECTOR;

// files.c
void loadFile(COLLECTOR *info, char *filename);
void printAndFreeServices(COLLECTOR *info, char *dirname);
void printAndFreeReports(REPORT **start, char *dirname);
void printAndFree(MESSAGE **start, char *filename);

// strings.c
int startsWith(char *string, char *match);
int endsWith(char *string, char *match);
int containsLetters(char *string);
int containsString(char *string, char *match);
int startsWithEmojis(char *string);
void firstElementOfString(char *string, char **result);
void compressString(char *string, char **result);
void swapSpaces(char *string, char **result);
void customTrim(char *string, char **result);

// messages.c
int matchesFormat(FILE_LINE *line_info, int *day, int *month, int *year, int *hour, int *minute, int *second);
void treatMessages(COLLECTOR *info);
void joinLines(FILE_LINE **start);
void addToMessages(MESSAGE **start, MESSAGE *newMessage);

// info.c
int getIDs(char *string, char **SN, char **RSD);
int getServiceName(MESSAGE *message);
void getEmogis(MESSAGE *start);
void getTimestamps(MESSAGE *start);

// sort.c
void sortReports(COLLECTOR *info);
void insertIntoServices(SERVICE **start, MESSAGE **message);
void insertIntoReports(REPORT **start, MESSAGE **message);
int messageIsFromReport(MESSAGE *message, REPORT *report);

/**
 * Funciones que podrpian tener problemas con la memoria
 *      - printAndFreeServices
 */