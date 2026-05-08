/* Alumno: González Borbas Fernando Alberto - Matricula: 379792 - Grupo:#941 */
/* Alumno: Moreno Calderón Troy Leonardo    - Matricula: 379169 - Grupo:#941 */
/* Alumno: Rojas Arroyo Kenan               - Matricula: 379748 - Grupo:#941 */

/* Descripción: Práctica #1 Mejoras. */
/* Fecha: 20 - 02 - 2026 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_USER 32
#define MAX_DOC 48

/* ************************************************************************** */

typedef enum prioridad
{
    NORMAL = 0,
    URGENTE = 1
} Prioridad_t;

/* ************************************************************************** */

typedef enum estado
{
    EN_COLA = 0,
    IMPRIMIENDO = 1,
    COMPLETADO = 2,
    CANCELADO = 3
} Estado_t;

/* ************************************************************************** */

typedef struct PrintJob_t
{
    int id;
    char usuario[MAX_USER];
    char documento[MAX_DOC];
    int totalPgs;
    int restantePgs;
    int copias;
    int msPagina;
    Prioridad_t prioridad;
    Estado_t estado;
} PrintJob_t;

/* ************************************************************************** */

typedef struct Node_t
{
    PrintJob_t job;
    struct Node_t *next;
} Node_t;

/* ************************************************************************** */

typedef struct QueueDynamic_t
{
    Node_t *head;
    Node_t *tail;
    int size;
} QueueDynamic_t;

/* ************************************************************************** */

void qd_init(QueueDynamic_t *q);
int qd_is_empty(const QueueDynamic_t *q);
int qd_enqueue(QueueDynamic_t *q, PrintJob_t job);
int qd_peek(const QueueDynamic_t *q, PrintJob_t *out);
int qd_dequeue(QueueDynamic_t *q, PrintJob_t *out);
void qd_print(const QueueDynamic_t *q);
void qd_destroy(QueueDynamic_t *q);
void simular_impresion(QueueDynamic_t *q);

/* ************************************************************************** */

int main()
{
    QueueDynamic_t cola;
    qd_init(&cola);
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
        printf("5.- Simular impresion de toda la cola\n");
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
            // system("CLS");
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

            if (qd_enqueue(&cola, nuevoTrabajo))
            {
                printf(">> Trabajo agregado exitosamente con el ID: %d\n", nuevoTrabajo.id);
                contId++;
            }
            else
            {
                printf(">> Error: No se pudo asignar memoria para el trabajo.\n");
            }
            system("PAUSE");
            break;
        }
        case 2:
        {
            //system("CLS");
            PrintJob_t frente;
            if (qd_peek(&cola, &frente))
            {
                printf("\n>> SIGUIENTE EN LA FILA:\n");
                printf("ID: %d \nUsuario: %s \nDoc: %s \nPags: %d \nPrioridad: ",
                       frente.id, frente.usuario, frente.documento, frente.totalPgs);

                if (frente.prioridad == URGENTE)
                {
                    printf("Urgente\n");
                }
                else
                {
                    printf("Normal\n");
                }
            }
            else
            {
                printf("La cola esta vacia.\n");
            }
            system("PAUSE");
            break;
        }
        case 3:
        {
            //system("CLS");
            PrintJob_t procesado;
            if (qd_dequeue(&cola, &procesado))
            {
                printf("\n>> PROCESANDO TRABAJO ID: %d...\n", procesado.id);
                printf("Imprimiendo '%s' de %s (%d paginas).\n", procesado.documento, procesado.usuario, procesado.totalPgs);
                procesado.estado = COMPLETADO;
                printf("Trabajo completado.\n");
            }
            else
            {
                printf("La cola esta vacia. No hay trabajos por procesar.\n");
            }
            system("PAUSE");
            break;
        }
        case 4:
        {
            //system("CLS");
            printf("\n");
            qd_print(&cola);
            system("PAUSE");
            break;
        }
        case 5:
        {
            //system("CLS");
            simular_impresion(&cola);
            system("PAUSE");
            break;
        }
        case 0:
        {
            //system("CLS");
            printf("\nSaliendo del sistema de impresion.\n");
            qd_destroy(&cola);
            system("PAUSE");
            break;
        }
        default:
        {
            printf("Error: opcion no valida.\n");
            system("PAUSE");
            break;
        }
        }
    } while (op != 0);

    return 0;
}

/* ************************************************************************** */

void qd_init(QueueDynamic_t *q)
{
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

/* ************************************************************************** */

int qd_is_empty(const QueueDynamic_t *q)
{
    return (q->head == NULL);
}

/* ************************************************************************** */

int qd_enqueue(QueueDynamic_t *q, PrintJob_t job)
{
    Node_t *newNode = (Node_t *)malloc(sizeof(Node_t));
    if (newNode == NULL)
    {
        return 0;
    }

    newNode->job = job;
    newNode->next = NULL;

    if (qd_is_empty(q))
    {
        q->head = newNode;
        q->tail = newNode;
    }
    else if (job.prioridad == URGENTE)
    {
        if (q->head->job.prioridad == NORMAL)
        {
            newNode->next = q->head;
            q->head = newNode;
        }
        else
        {
            Node_t *current = q->head;
            while (current->next != NULL && current->next->job.prioridad == URGENTE)
            {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
            if (newNode->next == NULL)
            {
                q->tail = newNode;
            }
        }
    }
    else
    {
        q->tail->next = newNode;
        q->tail = newNode;
    }

    q->size++;
    return 1;
}

/* ************************************************************************** */

int qd_dequeue(QueueDynamic_t *q, PrintJob_t *out)
{
    if (qd_is_empty(q))
    {
        return 0;
    }

    Node_t *temp = q->head;

    *out = temp->job;
    q->head = q->head->next;

    if (q->head == NULL)
    {
        q->tail = NULL;
    }

    free(temp);
    q->size--;
    return 1;
}

/* ************************************************************************** */

int qd_peek(const QueueDynamic_t *q, PrintJob_t *out)
{
    if (qd_is_empty(q))
    {
        return 0;
    }

    *out = q->head->job;
    return 1;
}

/* ************************************************************************** */

void qd_print(const QueueDynamic_t *q)
{
    if (qd_is_empty(q))
    {
        printf("La cola de impresion esta vacia.\n");
        return;
    }

    printf("--- LISTADO DE TRABAJOS (Total: %d) ---\n", q->size);

    Node_t *current = q->head;
    while (current != NULL)
    {
        printf("ID: %d | Usuario: %-10s | Pags: %d | Prioridad: ",
               current->job.id,
               current->job.usuario,
               current->job.totalPgs);

        if (current->job.prioridad == URGENTE)
        {
            printf("Urgente\n");
        }
        else
        {
            printf("Normal\n");
        }

        current = current->next;
    }
    printf("---------------------------------------\n");
}

/* ************************************************************************** */

void qd_destroy(QueueDynamic_t *q)
{
    Node_t *current = q->head;
    Node_t *next_node;

    while (current != NULL)
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }

    q->head = NULL;
    q->tail = NULL;
    q->size = 0;

    printf("Ejecutando qd_destroy: toda la memoria de la cola ha sido liberada.\n");
}

/* ************************************************************************** */

void simular_impresion(QueueDynamic_t *q)
{
    if (qd_is_empty(q))
    {
        printf("La cola esta vacia. No hay trabajos para simular.\n");
        return;
    }

    int completados = 0;
    int paginasTotales = 0;

    printf("--- INICIANDO SIMULACION ---\n");

    while (!qd_is_empty(q))
    {
        PrintJob_t job;
        qd_dequeue(q, &job);

        job.estado = IMPRIMIENDO;
        printf("\n[ESTADO: IMPRIMIENDO] ID: %d | Doc: %s | Usuario: %s\n", job.id, job.documento, job.usuario);

        while (job.restantePgs > 0)
        {
            printf("\rImprimiendo pagina %d de %d...", (job.totalPgs - job.restantePgs + 1), job.totalPgs);
            fflush(stdout);
            Sleep(job.msPagina);
            job.restantePgs--;
        }

        job.estado = COMPLETADO;
        printf("\n[ESTADO: COMPLETADO] Trabajo ID %d finalizado.\n", job.id);

        completados++;
        paginasTotales += job.totalPgs;
    }

    printf("\n--- ESTADISTICAS DE LA SESION ---\n");
    printf("Trabajos completados: %d\n", completados);
    printf("Total paginas impresas: %d\n", paginasTotales);
    printf("---------------------------------\n");
}