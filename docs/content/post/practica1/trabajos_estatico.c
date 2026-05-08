// Alumno: González Borbas Fernando Alberto - Matricula: 379792 - Grupo:#941
// Alumno: Moreno Calderón Troy Leonardo    - Matricula: 379169 - Grupo:#941
// Alumno: Rojas Arroyo Kenan               - Matricula: 379748 - Grupo:#941

// Descripción: Práctica #1 Estatica.
// Fecha: 20 - 02 - 2026

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_USER 32
#define MAX_DOC 48
#define MAX_JOBS 10

//*****************************************************************************

typedef enum prioridad
{
    NORMAL = 0,
    URGENTE = 1
} Prioridad_t;

//*****************************************************************************

typedef enum estado
{
    EN_COLA = 0,
    IMPRIMIENDO = 1,
    COMPLETADO = 2,
    CANCELADO = 3
} Estado_t;

//*****************************************************************************

typedef struct PrintJob_t
{
    int id;                  // Numero autoincremental
    char usuario[MAX_USER];  // Nombre del usuario que envio la impresion
    char documento[MAX_DOC]; // Titulo del documento
    int totalPgs;            // Numero total de paginas
    int restantePgs;         // Paginas restantes por imprimir
    int copias;              // Copias por imprimir
    int msPagina;            // Simulacion de progreso
    Prioridad_t prioridad;   // Normal, urgente
    Estado_t estado;         // En cola, imprimiendo, completado, cancelado
} PrintJob_t;

//*****************************************************************************

typedef struct QueueStatic_t
{
    PrintJob_t datos[MAX_JOBS];
    int size;
} QueueStatic_t;

//*****************************************************************************

void qs_init(QueueStatic_t *q);
int qs_is_empty(const QueueStatic_t *q);
int qs_is_full(const QueueStatic_t *q);
int qs_enqueue(QueueStatic_t *q, PrintJob_t job);     // Insertar al final de la cola
int qs_peek(const QueueStatic_t *q, PrintJob_t *out); // Ver elemento de enfrente
int qs_dequeue(QueueStatic_t *q, PrintJob_t *out);    // Eliminar el elementeo de enfrente
void qs_print(const QueueStatic_t *q);

//*****************************************************************************

int main()
{
    QueueStatic_t cola;
    qs_init(&cola);
    char buffer[100];
    int op, contId = 1;

    do
    {
        //system("CLS");
        printf("SISTEMA DE GESTION DE TRABAJOS\n\n");
        printf("1.- Agregar trabajo a la cola\n");
        printf("2.- Ver siguiente trabajo\n");
        printf("3.- Procesar trabajo\n");
        printf("4.- Listar cola\n");
        printf("0.- Salir\n");
        printf("Selecciona una opcion: ");

        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            op = (int)strtol(buffer, NULL, 10);
        }
        else
        {
            op = 0;
        }

        switch (op)
        {
        case 1:
        {

            //system("CLS");
            PrintJob_t nuevoTrabajo;
            nuevoTrabajo.id = contId;

            printf("Ingrese nombre de usuario: ");
            if (fgets(nuevoTrabajo.usuario, MAX_USER, stdin))
            {
                nuevoTrabajo.usuario[strcspn(nuevoTrabajo.usuario, "\n")] = '\0';
            }

            printf("Ingrese titulo del documento: ");
            if (fgets(nuevoTrabajo.documento, MAX_DOC, stdin))
            {
                nuevoTrabajo.documento[strcspn(nuevoTrabajo.documento, "\n")] = '\0';
            }

            printf("Ingrese total de paginas: ");
            if (fgets(buffer, sizeof(buffer), stdin))
            {
                nuevoTrabajo.totalPgs = (int)strtol(buffer, NULL, 10);
            }

            nuevoTrabajo.restantePgs = nuevoTrabajo.totalPgs;

            printf("Ingrese numero de copias: ");
            if (fgets(buffer, sizeof(buffer), stdin))
            {
                nuevoTrabajo.copias = (int)strtol(buffer, NULL, 10);
            }

            printf("Prioridad (0 = Normal, 1 = Urgente): ");
            if (fgets(buffer, sizeof(buffer), stdin))
            {
                int prio = (int)strtol(buffer, NULL, 10);
                if (prio == 1)
                {
                    nuevoTrabajo.prioridad = URGENTE;
                }
                else
                {
                    nuevoTrabajo.prioridad = NORMAL;
                }
            }

            nuevoTrabajo.estado = EN_COLA;
            nuevoTrabajo.msPagina = 500;

            if (qs_enqueue(&cola, nuevoTrabajo))
            {
                printf(">> Trabajo agregado exitosamente con el ID: %d\n", nuevoTrabajo.id);
                contId++;
            }
            system("PAUSE");
            break;
        }
        case 2:
        {
            //system("CLS");
            PrintJob_t frente;
            if (qs_peek(&cola, &frente))
            {
                printf("\n>> SIGUIENTE EN LA FILA:\n");
                printf("ID: %d \nUsuario: %s \nDoc: %s \nPags: %d \nPrioridad: ", frente.id, frente.usuario, frente.documento, frente.totalPgs);

                if (frente.prioridad == URGENTE)
                {
                    printf("Urgente\n");
                }
                else
                {
                    printf("Normal\n");
                }
            }
            system("PAUSE");
            break;
        }
        case 3:
        {
            //system("CLS");
            PrintJob_t procesado;
            if (qs_dequeue(&cola, &procesado))
            {
                printf("\n>> PROCESANDO TRABAJO ID: %d...\n", procesado.id);
                printf("Imprimiendo '%s' de %s (%d paginas).\n", procesado.documento, procesado.usuario, procesado.totalPgs);
                procesado.estado = COMPLETADO;
                printf("Trabajo completado.\n");
            }
            system("PAUSE");
            break;
        }
        case 4:
            //system("CLS");
            printf("\n");
            qs_print(&cola);
            system("PAUSE");
            break;
        case 0:
            //system("CLS");
            printf("\nSaliendo del sistema de impresion.\n");
            system("PAUSE");
            break;
        default:
            printf("Error: opcion no valida.\n");
            break;
        }
    } while (op != 0);

    return 0;
}

//*****************************************************************************

void qs_init(QueueStatic_t *q)
{
    q->size = 0;
}

//*****************************************************************************
int qs_is_empty(const QueueStatic_t *q)
{
    if (q->size == 0)
    {
        return 1;
    }
    return 0;
}

//*****************************************************************************
int qs_is_full(const QueueStatic_t *q)
{
    if (q->size == MAX_JOBS)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//*****************************************************************************

int qs_enqueue(QueueStatic_t *q, PrintJob_t job)
{
    if (qs_is_full(q) == 1)
    {
        printf("Error: La cola esta llena \n");
        return 0;
    }

    q->datos[q->size] = job;
    q->size++;
    return 1;
}

//*****************************************************************************
int qs_dequeue(QueueStatic_t *q, PrintJob_t *out)
{
    if (qs_is_empty(q) == 1)
    {
        printf("Error: La cola esta vacia \n");
        return 0;
    }

    *out = q->datos[0];

    for (int i = 1; i < q->size; i++)
    {
        q->datos[i - 1] = q->datos[i];
    }

    q->size--;
    return 1;
}

//*****************************************************************************

int qs_peek(const QueueStatic_t *q, PrintJob_t *out)
{
    if (qs_is_empty(q) == 1)
    {
        printf("Error: La cola esta vacia \n");
        return 0;
    }

    *out = q->datos[0];
    return 1;
}

//*****************************************************************************

void qs_print(const QueueStatic_t *q)
{
    if (qs_is_empty(q) == 1)
    {
        printf("Aviso: La cola esta vacia.\n");
        return;
    }

    printf("--- LISTADO DE TRABAJOS (Total: %d) ---\n", q->size);

    for (int i = 0; i < q->size; i++)
    {
        printf("ID: %d | Usuario: %-10s | Pags: %d | Prioridad: ",
               q->datos[i].id,
               q->datos[i].usuario,
               q->datos[i].totalPgs);
        if (q->datos[i].prioridad == URGENTE)
        {
            printf("Urgente\n");
        }
        else
        {
            printf("Normal\n");
        }
    }
    printf("---------------------------------------\n");
}