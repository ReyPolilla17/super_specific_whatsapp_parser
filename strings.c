/**
 * @file strings.c
 * 
 * @author Luis Julián Zamora Treviño
 * @date 3/1/2025
 * 
 * @brief Contiene varias funciones dedicadas a operar cadenas de caracteres.
 */
#include "parser.h"

/**
 * Determina si una cadena inicia de una forma específica,
 * regresa 1 en caso de que lo haga y 0 de lo contrario
 * 
 * @param *string La cadena a evaluar.
 * @param *match La cadena con la que debe iniciar.
 * 
 * @returns int
 */
int startsWith(char *string, char *match)
{
    int l = 0; // Longitud de la cadena con la que deba iniciar
    int i = 0; // Contador
    int m = 0; // Conincidencias

    l = strlen(match); // inicializa la variable

    for(i = 0; string[i] != 0; i++) // recorre la cadena a evaluar
    {
        if(string[i] != match[i] || i > l) // si dejan de haber coincidencias y no se ha llegado a la lonfitud de la cadena
        {
            return 0;
        }
        else // si sigue habiendo soincidencias
        {
            m++;
        }

        if(m == l) // si la coincidencia es completa
        {
            return 1;
        }
    }

    return 0;
}

/**
 * Determina si una cadena termina con la cadena señalada, en caso de que coincida, elimina esa parte de la cadena
 * regresando 1 en caso de que lo haga y 0 de lo contrario.
 * 
 * @param *string La cadena a evaluar.
 * @param *match La forma en la que debe terminar.
 * 
 * @returns int
 */
int endsWith(char *string, char *match)
{
    int i = 0; // Contador

    // obtiene la longitud de las cadenas
    int l1 = strlen(match);
    int l2 = strlen(string);

    // si la cadena a comprobar es menor a la forma de iniciar
    if(l2 < l1)
    {
        return 0;
    }

    // compara cada caracter en la forma de terminar
    for(i = 0; i < l1; i++)
    {
        // si algun caracter no coincide
        if((match[l1 - i] != string[l2 - i]))
        {
            return 0;
        }
    }

    string[l2 - i - 1] = 0; // coloca un caracter nulo antes del inicio de la cadena de coincidencia

    return 1;
}

/**
 * Determina si una cadena contiene carácteres alfabeticos,
 * Regresando 1 en caso de que los contenga y 0 en caso contrario
 * 
 * @param *string La cadena a evaluar.
 */
int containsLetters(char *string)
{
    int i = 0; // Contador

    for(i = 0; string[i] != 0; i++) // para toda la cadena
    {
        if((string[i] >= 'A' && string[i] <= 'Z') || (string[i] >= 'a' && string[i] <= 'z')) // si algun carácter es una letra
        {
            return 1;
        }
    }

    return 0;
}

/**
 * Evalua si en alguna parte de la cadena a evaluar aparece una cadena,
 * regresando 1 en caso de que lo haga y 0 de lo contrario.
 * 
 * @param *string La cadena a ecaluar.
 * @param *match La coincidencia a buscar.
 * 
 * @returns int
 */
int containsString(char *string, char *match)
{
    int l = 0; // Longitud de la cadena a buscar
    int i = 0; // Contador
    int m = 0; // Cantidad de caracteres encontrados
    
    l = strlen(match); // inicializa la variable

    for(i = 0; string[i] != 0; i++) // revisa tosa la cadena a evaluar
    {
        if(string[i] == match[m]) // si el caracter coincide, suma 1 a las coincidencias
        {
            m++;
        }
        else // de lo contrario, lo reinicia
        {
            m = 0;
        }

        if(m == l) // se confirma la existencia de la cadena
        {
            return 1;
        }
    }

    return 0;
}

/**
 * Determina si una cadena inicia con carácteres que no existan en la tabla ascii,
 * regresando 1 de ser el caso y 0 de lo contrario.
 * 
 * @param *string La cadena a evaluar.
 * 
 * @returns int
 */
int startsWithEmojis(char *string)
{
    if(string[0] < 0) // si el caracter inicial es menor a 0, es un caracter especial.
    {
        return 1;
    }

    return 0;
}

/**
 * Obtiene la primera frase de una cadedna, la detecta al encontrar un /, ( o un salto de renglón.
 * Aloca memoria para regresar el elemento.
 * 
 * @param *string La cadena a evaluar.
 * @param **result La cadena que contenga la primera frase o elemento.
 * 
 * @returns void
 */
void firstElementOfString(char *string, char **result)
{
    int i = 0; // Contador

    char *copy = malloc((strlen(string) + 1) * sizeof(char)); // copia de la cadena
    strcpy(copy, string);

    for(i = 0; copy[i] != 0 && copy[i] != '/' && copy[i] != '('; i++); // hasta encontrar un cadacter de "final de frase"
    copy[i] = 0; // coloca un caracter nulo en esa parte
    
    while(copy[strlen(copy) - 1] == ' ') // elimina todos los espacios que haya al final
    {
        copy[strlen(copy) - 1] = 0;
    }

    *result = malloc((strlen(copy) + 1) * sizeof(char)); // copia el resultado en la cadena correspondiente, alocando memoria
    strcpy(*result, copy);
    
    free(copy); // libera la memoria de la copia
    
    return;
}

/**
 * Obtiene una version de la cadena dada que no contenga espacios, mayusculas o caracteres especiales.
 * 
 * @param *string La cadena a convertir.
 * @param **result La cadena resultante, alocada en memoria.
 * 
 * @returns void
 */
void compressString(char *string, char **result)
{
    char accent_letters[13][3] = {"Á", "É", "Í", "Ó", "Ú", "á", "é", "í", "ó", "ú", "Ñ", "ñ", "´"}; // caracteres especiales
    char no_accent_letters[13][3] = {"A", "E", "I", "O", "U", "a", "e", "i", "o", "u", "N", "n", "'"}; // versión ascii
    int accent_count = 13; // cuenta de caracteres especiales

    // contadores
    int i = 0;
    int j = 0;

    // buffers
    char *buffer = NULL;
    char *buffer2 = NULL;

    // apuntadores para recorrer cadenas
    char *placeholder = NULL;
    char *placeholder2 = NULL;

    firstElementOfString(string, &buffer); // obtiene el primer elemento y lo guarda en el primer buffer

    for(i = 0; buffer[i] != 0; i++) // revisa en todo el buffer
    {
        placeholder = &buffer[i]; // se coloca en el caracter de la cadena

        for(j = 0; j < accent_count; j++) // revisa todos los posibles caracteres especiales
        {
            if(startsWith(placeholder, accent_letters[j])) // si el apuntador inicia con un caracter especial
            {
                placeholder2 = &buffer[i + strlen(accent_letters[j])]; // coloca el segundo apuntador después del caracter especial

                // guarda los caracteres despues del caracter especial
                buffer2 = malloc((strlen(placeholder2) + 1) * sizeof(char));
                strcpy(buffer2, placeholder2);

                // reemplaza el caracter especial, libera memoria y termina el loop
                buffer[i] = 0;

                strcat(buffer, no_accent_letters[j]);
                strcat(buffer, buffer2);

                free(buffer2);

                j = accent_count;
            }
        }

        if(buffer[i] >= 'A' && buffer[i] <= 'Z') // pasa a minuscula la letra si es que está en mayuscula
        {
            buffer[i] += 32;
        }
        else if(buffer[i] == ' ') // eliminia los espacios
        {
            // se coloca después del espacio y lo reemplaza por un caracter nulo
            placeholder2 = &buffer[i + 1];
            buffer[i] = 0;

            // copia el contenido después del espacio, lo coloca antes del espacio, libera memoria y retrocede la cuenta
            buffer2 = malloc((strlen(placeholder2) + 1) * sizeof(char));
            strcpy(buffer2, placeholder2);

            strcat(buffer, buffer2);
            
            free(buffer2);

            i--;
        }
    }

    // si la cadena resultante contiene la palabra conect (caso muy específico)
    if(containsString(buffer, "conect"))
    {
        // elimina esa palabra de la cadena y la reemplaza por la palabra connect
        for(i = 0; buffer[i] != 'c'; i++);
        buffer[i] = 0;

        *result = malloc((strlen(buffer) + strlen(" connect ")) * sizeof(char));
        sprintf(*result, "%s connect", buffer); // guarda el resultado final en una alocación del tamaño correcto
    }
    else // de lo contrario, guarda el resultado final en una alocación del tamaño correcto
    {
        *result = malloc((strlen(buffer) + 1) * sizeof(char));
        strcpy(*result, buffer);
    }

    free(buffer); // libera memoria

    return;
}

/**
 * Reemplaza carácteres de una cadena que pueden ser problemáticos en la creación de archivos o directorios (comillas, espacios, etc.)
 * 
 * @param *string La cadena a modificar.
 * @param **result La cadena resultante.
 * 
 * @returns void
 */
void swapSpaces(char *string, char **result)
{
    int i = 0; // contador
    char *buffer; // copia de la cadena

    buffer = malloc((strlen(string) + 1) * sizeof(char)); // copia la cadena
    strcpy(buffer, string);

    for(i = 0; buffer[i] != 0; i++) // para todos los caracteres de la cadena
    {
        if(buffer[i] == ' ') // reemplaza los espacios por _
        {
            buffer[i] = '_';
        }
        else if(buffer[i] == '\'') // reemplaza las comillas por ^
        {
            buffer[i] = '^';
        }
    }

    while(buffer[strlen(buffer) - 1] == '_') // elimina los espacios al final de la cadena
    {
        buffer[strlen(buffer) - 1] = 0;
    }

    *result = malloc((strlen(buffer) + 1) * sizeof(char)); // copia la cadena final en la cadena de salida
    strcpy(*result, buffer);

    free(buffer);

    return;
}

/**
 * Opera una cadena para obtener el primer elemento con caracteres alfanúmericos enunciados en la cadena.
 * 
 * @param *string La cadena a operar.
 * @param **result La cadena resultante.
 * 
 * @returns void
 */
void customTrim(char *string, char **result)
{
    int i = 0; // contador

    int open = 0; // parentesis

    int start = -1; // inicio del primer elemento
    int end = -1; // fin del primer elemento

    char *copy; // copia de la cadena
    char *placeholder; // apuntador a partes de la cadena

    // copia la cadena original y posiciona el apuntador
    copy = malloc((strlen(string) + 1) * sizeof(char));
    strcpy(copy, string);

    placeholder = copy;

    for(i = 0; copy[i] != 0 && end == -1; i++) // para toda la cadena, hasta encontrar el final del primer elemento
    {
        // conteo de parentesis abiertos y cerrados
        if(copy[i] == '(')
        {
            open++;
        }
        else if(copy[i] == ')')
        {
            open--;
        }

        placeholder = &copy[i]; // recorre el apuntador

        // al encontrar un caracter del alfabeto por primera vez, marca el inicio
        if(((placeholder[0] >= 'A' && placeholder[0] <= 'Z') || (placeholder[0] >= 'a' && placeholder[0] <= 'z')) && start == -1)
        {
            start = i;
        }
        // al encontrar alguno de los siguientes carácteres, determina el fin del primer elemento
        else if((startsWith(placeholder, "*") || startsWith(placeholder, "-") || startsWith(placeholder, "–") || startsWith(placeholder, "INC")) && start != -1 && !open)
        {
            copy[i] = 0;
            end = i;
        }   
    }

    placeholder = &copy[start]; // posisiona el apuntador en el inicio del elemento

    // elimina caracteres indeseados al final de la cadena
    while(placeholder[strlen(placeholder) - 1] == ' ' || placeholder[strlen(placeholder) - 1] == '.'|| placeholder[strlen(placeholder) - 1] == '\n' || placeholder[strlen(placeholder) - 1] == '*')
    {
        placeholder[strlen(placeholder) - 1] = 0;
    }

    // copia la cadena final en la cadena resultante y libera memoria
    *result = malloc((strlen(placeholder) + 1) * sizeof(char));
    strcpy(*result, placeholder);

    free(copy);

    return;
}