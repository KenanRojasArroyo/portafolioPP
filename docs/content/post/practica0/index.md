+++
date = '2026-02-13T17:55:06-08:00'
draft = false
title = 'Práctica 0: Uso de Repositorios'
+++

# Markdown

Markdown es un lenguaje de marcado que permite añadir formato a un texto simple mediante símbolos fáciles de recordar. Fue creado para que el contenido sea legible tanto en su forma cruda como cuando se renderiza en HTML.

### Sintaxis

<!-- Esto es un comentario -->

# Esto es un encabezado H1
## Esto es un encabezado H2
### Esto es un encabezado H3
#### Esto es un encabezado H4
##### Esto es un encabezado H5

Esto es un texto en _italicas_
_Esto es un texto en italicas_
Esto es un texto en *italicas*
*Esto es un texto en italicas*
Esto es un texto en **negritas**
**Esto es un texto en negritas**
Esto es un texto en __negritas__
__Esto es un texto en negritas__

Este `es un texto que puede ser codigo`

Este es un texto ~~tachado~~

* Elemento 1
* Elemento 2
* Elemento 3
  * Elemento 3.1
  * Elemento 3.2
  * Elemento 3.3

- Elemento 1
- Elemento 2
- Elemento 3
  - Elemento 3.1
  - Elemento 3.2
  - Elemento 3.3

1. Elemento 1
2. Elemento 2
3. Elemento 3
  3. Elemento 3.1
  3. Elemento 3.2
  3. Elemento 3.3

[Texto de enlace](http://www.google.com "Texto del tooltip")

[UABC](http://www.uabc.mx)

[UABC](http://www.uabc.mx "Sitio Universitario")

```python
print("Hola mundo")
```

<!-- Tablas -->
| Productos| Precios | Cantidad |
| - | - | - |
|Laptop | 3.3 | 2 |
| Mouse | 13.3 | 1 |

> Esto es una nota

<!-- Tareas -->
* [x] Primera tarea
* [ ] segunda tarea
* [x] Tercera tarea
* [ ] Cuarta tarea

<!-- Divisores horizontales -->
***
---
___

<!-- Menciones -->
@darthrookie :+1: :smile:

# Git y GitHub

**Git** es un sistema de control de versiones de código fuente. **GitHub** es la plataforma en la nube que aloja esos repositorios y facilita la colaboración.

### Comandos escenciales de Git

1. **Inicializar:** `git init` (crea el repositorio local)
2. **Estado:** `git status` (revisa qué archivos han cambiado)
3. **Añadir:** `git add` . (prepara los archivos para la "foto")
4. **Confirmar:** `git commit -m "Mensaje"` (sirve para comentar los cambios)
5.  **Conectar:** `git remote add origin [URL del repositorio]` (conecta Git con GitHub)
6.  **Subir:** `git push -u origin master` (sube del repositorio local al repositorio online)

# HUGO

**Hugo** es uno de los generadores de sitios estáticos (SSG) más rápidos del mundo. Escrito en Go, toma archivos en Markdown y los transforma en un sitio web completo en milisegundos.

### Crear un sitio de hugo

1. **Instalar Hugo y ejecutar:** `hugo new site mi-sitio`
2. **Añadir un tema:** Descargar un tema en la carpeta `/themes`
3. **Crear contenido:** `hugo new posts/mi-primer-post.md`
4.  **Probar localmente:** `hugo server -D` (esto levanta un servidor `localhost:1313`)

# GitHub Actions 

**GitHub Actions** es una herramienta de integración y despliegue continuo que permite automatizar tareas. En este caso, lo usamos para que, cada vez que subamos un cambio. **GitHub** construya el el sitio de HUGO y lo publique en **GitHub Pages**