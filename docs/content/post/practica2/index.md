+++
date = '2026-02-13T17:58:46-08:00'
draft = false
title = 'Práctica 3: El paradigma funcional'
+++

# Práctica 02: Simulador de Estacionamiento con POO y MVC

**40032 - Paradigmas de la Programación**
Marzo 2026

---

## 1. Introducción

### 1.1 Problema

El presente reporte documenta el desarrollo de la Práctica 02, cuyo objetivo fue construir un simulador funcional de estacionamiento aplicando los principios fundamentales de la Programación Orientada a Objetos (POO) y el patrón de arquitectura Modelo-Vista-Controlador (MVC) con el framework Django.

El problema central consiste en modelar las operaciones básicas de un estacionamiento: la entrada y salida de vehículos (automóviles y motocicletas), la asignación automática de lugares disponibles y compatibles con el tipo de vehículo, y el cálculo del costo de estancia con base en una política de cobro configurable.

### 1.2 Objetivos

1. Modelar el dominio del estacionamiento aplicando los cuatro pilares de POO: encapsulación, abstracción, herencia y polimorfismo.
2. Implementar una arquitectura limpia separando la lógica de negocio (dominio puro) de la capa de presentación web (Django MVC).
3. Demostrar el uso de composición e inyección de dependencias en la clase principal `ParkingLot`.
4. Registrar entradas y salidas de vehículos manteniendo el estado en memoria durante la sesión del servidor.
5. Validar el sistema mediante pruebas manuales de flujos completos de entrada/salida y cobro.

---

## 2. Modelo del Dominio

### 2.1 Diagrama UML de Clases

```
                    ┌──────────────────────────────┐
                    │         <<enum>>             │
                    │       VehicleType            │
                    │  CAR / MOTORCYCLE            │
                    └──────────────────────────────┘
                              ▲
         ┌────────────────────┴────────────────────┐
         │           Vehicle (dataclass)           │
         │  - _plate: str                         │
         │  - _type: VehicleType                  │
         │  + get_plate(): str                    │
         │  + get_type(): VehicleType             │
         └────────────────────────────────────────┘
              ▲                       ▲
    ┌─────────┴──────┐       ┌────────┴────────┐
    │      Car       │       │   Motorcycle   │
    └────────────────┘       └────────────────┘

  ┌──────────────────────────────────────────────┐
  │             ParkingLot                      │
  │  - _spots: List[ParkingSpot]                │
  │  - _active_tickets: Dict[int, Ticket]       │
  │  - _policy: RatePolicy                      │
  │  + enter(v, now): Ticket                   │
  │  + exit(ticket_id, now): float             │
  └──────────────────────────────────────────────┘
       ◆ compone            ◆ compone
  ┌────────────────┐    ┌───────────────────────┐
  │  ParkingSpot   │    │       Ticket          │
  │ - _spot_id     │    │ - _ticket_id: int     │
  │ - _allowed     │    │ - _vehicle: Vehicle   │
  │ - _occupied    │    │ - _spot: ParkingSpot  │
  └────────────────┘    └───────────────────────┘

  ┌────────────────────────────────────────────┐
  │       <<Protocol>>  RatePolicy            │
  │  + calculate(hours, v): float             │
  └────────────────────────────────────────────┘
       ▲                       ▲
  ┌────┴──────────┐    ┌────────┴───────────┐
  │HourlyRatePolicy│   │  FlatRatePolicy    │
  └────────────────┘    └────────────────────┘
```

### 2.2 Clases y Responsabilidades

| Clase | Responsabilidad |
| - | - |
| `Vehicle` | Entidad base que encapsula placa y tipo. Proporciona getters para proteger el estado interno. |
| `Car` / `Motorcycle` | Subclases que especializan `Vehicle`; simplifican la instanciación y dan identidad de tipo. |
| `ParkingSpot` | Representa un lugar físico. Decide si acepta un vehículo según su tipo y gestiona su ocupación. |
| `Ticket` | Registra la relación entre un vehículo y un lugar durante un periodo de tiempo; calcula la duración. |
| `RatePolicy` (Protocol) | Interfaz de cobro que define el contrato `calculate(hours, vehicle)`. Permite cambiar la tarifa sin modificar `ParkingLot`. |
| `HourlyRatePolicy` | Implementa cobro por hora diferenciado: $20/h para autos, $10/h para motos. |
| `FlatRatePolicy` | Implementa cobro de tarifa plana fija independientemente del tiempo. |
| `ParkingLot` | Orquestador principal: gestiona la lista de spots, el diccionario de tickets activos y delega el cobro a la política inyectada. |

---

## 3. Evidencia de Conceptos POO

### 3.1 Encapsulación

La encapsulación protege el estado interno de un objeto obligando a interactuar con él a través de métodos definidos. En `ParkingSpot`, el método `park()` valida que el lugar esté disponible antes de modificar el estado, impidiendo inconsistencias desde el exterior:

```python
# spot.py
def park(self, v: Vehicle) -> None:
    # Encapsulación: la validación vive dentro del método,
    # el exterior no puede modificar _occupied directamente.
    if not self.is_available_for(v):
        raise ValueError(
            f"El lugar {self._spot_id} no está disponible."
        )
    self._occupied = True
    self._current_vehicle = v
```

Los atributos `_occupied` y `_current_vehicle` sólo se modifican a través de `park()` y `release()`, preservando la integridad del objeto.

### 3.2 Abstracción

La abstracción define un contrato sin exponer la implementación concreta. El protocolo `RatePolicy` expone únicamente lo que `ParkingLot` necesita saber para calcular el cobro: una firma `calculate(hours, vehicle)`. Cómo se calcule internamente es irrelevante para el orquestador.

```python
# parking_lot.py
class RatePolicy(Protocol):
    """Contrato abstracto para cualquier política de cobro."""
    def calculate(self, hours: float, v: Vehicle) -> float:
        ...

class HourlyRatePolicy:
    def __init__(self, car_rate: float, moto_rate: float):
        self._car_rate = car_rate
        self._moto_rate = moto_rate

    def calculate(self, hours: float, v: Vehicle) -> float:
        rate = self._car_rate if v.get_type().name == 'CAR' else self._moto_rate
        return hours * rate
```

La clase `FlatRatePolicy` proporciona una implementación alternativa que devuelve siempre un monto fijo, sin alterar ningún otro componente del sistema.

### 3.3 Composición

`ParkingLot` no hereda de ninguna clase; en cambio, administra una lista de `ParkingSpot`, un diccionario de `Ticket` y una política de cobro inyectada en el constructor. Esto es composición: el todo se construye a partir de partes independientes y reemplazables.

```python
# parking_lot.py
class ParkingLot:
    def __init__(self, spots: List[ParkingSpot], policy: RatePolicy):
        self._spots = spots              # Composición: lista de ParkingSpot
        self._active_tickets: Dict[int, Ticket] = {}  # Composición: mapa de Ticket
        self._policy = policy            # Inyección de dependencias
        self._next_ticket_id = 1
        self._total_revenue = 0.0
```

La inyección de la política en el constructor permite intercambiar `HourlyRatePolicy` por `FlatRatePolicy` (o cualquier implementación futura) sin modificar `ParkingLot`.

### 3.4 Herencia y Subtipos

`Car` y `Motorcycle` son subclases concretas de `Vehicle`. La herencia permite que cualquier método que reciba un `Vehicle` pueda trabajar con `Car` o `Motorcycle` sin modificaciones, simplificando la instanciación en la vista:

```python
# vehicle.py
class Vehicle:  # Clase base
    def __init__(self, _plate: str, _type: VehicleType):
        self._plate = _plate
        self._type = _type

class Car(Vehicle):  # Subtipo especializado
    def __init__(self, plate: str):
        super().__init__(_plate=plate, _type=VehicleType.CAR)

class Motorcycle(Vehicle):  # Subtipo especializado
    def __init__(self, plate: str):
        super().__init__(_plate=plate, _type=VehicleType.MOTORCYCLE)
```

```python
# views.py — uso de subtipos en el controlador
vehicle = Car(plate) if v_type == 'CAR' else Motorcycle(plate)
```

### 3.5 Polimorfismo

El polimorfismo se aprecia en el método `exit()` de `ParkingLot`: se llama a `self._policy.calculate()` sin importar qué implementación concreta de `RatePolicy` esté inyectada. El comportamiento cambia según el objeto real en tiempo de ejecución:

```python
# parking_lot.py
def exit(self, ticket_id: int, now: datetime) -> float:
    ticket = self._active_tickets[ticket_id]
    ticket.close(now)
    ticket.spot.release()

    hours = ticket.get_duration_hours()
    # Polimorfismo: misma llamada, comportamiento distinto
    # según si _policy es HourlyRatePolicy o FlatRatePolicy.
    cost = self._policy.calculate(hours, ticket.vehicle)

    self._total_revenue += cost
    del self._active_tickets[ticket_id]
    return cost
```

La ventaja es la extensibilidad: si se necesita una nueva política (por ejemplo, tarifa nocturna), basta con crear una clase que implemente el protocolo `RatePolicy`; `ParkingLot` no necesita cambiar.

---

## 4. MVC con Django

### 4.1 Distribución de Responsabilidades

Django implementa el patrón MVC bajo la nomenclatura MVT (Model-View-Template), donde la lógica de negocio se separa claramente de la presentación:

| Capa MVC | Equivalente Django | Archivos del Proyecto |
| - | - | - |
| Model | Lógica de dominio pura (sin ORM) | `vehicle.py`, `spot.py`, `ticket.py`, `parking_lot.py` |
| View | Templates HTML con Jinja2 | `dashboard.html`, `entry.html`, `exit.html` |
| Controller | Views de Django | `views.py` (funciones `dashboard`, `entry`, `exit_vehicle`) |

### 4.2 Modelo (Model)

Las clases del paquete `domain/` constituyen el modelo. No dependen de Django en absoluto: no heredan de `models.Model` ni acceden a base de datos. Esto garantiza que la lógica de negocio sea portable y testeable de forma aislada.

El estado se mantiene en memoria mediante una instancia global de `ParkingLot` en `views.py`, inicializada con 5 lugares para autos (C1–C5) y 5 para motocicletas (M1–M5), y una `HourlyRatePolicy` de $20/h y $10/h respectivamente.

### 4.3 Vista (View / Template)

Los tres templates HTML renderizan el estado pasado desde el controlador:

* `dashboard.html`: muestra la tabla de tickets activos, disponibilidad y total de ingresos acumulados.
* `entry.html`: formulario con campos para placa y tipo de vehículo.
* `exit.html`: formulario con campo para el ID del ticket a cerrar.

Todos los templates usan el messages framework de Django para comunicar al usuario el resultado de la operación (éxito o error).

### 4.4 Controlador (Controller)

Las tres funciones en `views.py` actúan como controladores: reciben el HTTP request, transforman los datos del formulario, delegan la operación al modelo y redirigen con un mensaje de retroalimentación:

```python
# views.py
def entry(request):
    if request.method == 'POST':
        plate = request.POST.get('plate')
        v_type = request.POST.get('type')
        vehicle = Car(plate) if v_type == 'CAR' else Motorcycle(plate)
        ticket = parking.enter(vehicle, datetime.now())   # Delega al Modelo
        messages.success(request, f'Ticket ID: {ticket.ticket_id}')
        return redirect('dashboard')                       # Redirige a la Vista
    return render(request, 'entry.html')                  # GET: muestra el template
```

### 4.5 Rutas

```python
# urls.py
urlpatterns = [
    path('', views.dashboard, name='dashboard'),
    path('entry/', views.entry, name='entry'),
    path('exit/', views.exit_vehicle, name='exit'),
]
```

---

## 5. Pruebas Manuales

### 5.1 Flujo 1 — Entrada y Salida de Automóvil

| Paso | Descripción / Resultado Esperado |
| - | - |
| 1. Navegar a `/entry/` | Se muestra el formulario con campos Placa y Tipo. |
| 2. Ingresar placa `ABC-123`, tipo `CAR` | Se envía POST. `parking.enter()` asigna lugar C1. |
| 3. Redirección a `/` | Dashboard muestra ticket ID 1, lugar C1, placa ABC-123. Disponibilidad: 4 libres para autos. |
| 4. Navegar a `/exit/` | Se muestra formulario con campo ID Ticket. |
| 5. Ingresar `ticket_id = 1` | `parking.exit()` cierra el ticket, libera C1 y calcula costo (mínimo 1h = $20.00). |
| 6. Redirección a `/` | Mensaje: "Total cobrado: $20.00". Tabla de tickets activos vacía. Ingresos: $20.00. |

### 5.2 Flujo 2 — Entrada de Motocicleta y Validación de Error

| Paso | Descripción / Resultado Esperado |
| - | - |
| 1. Ingresar placa `MOTO-01`, tipo `MOTORCYCLE` | `parking.enter()` busca spot de tipo MOTORCYCLE. Asigna M1. |
| 2. Dashboard | Ticket ID 2, lugar M1. Disponibilidad: 5 autos libres, 4 motos libres. |
| 3. Intentar salida con ticket inválido (ID 99) | `parking.exit()` lanza `ValueError`. Mensaje en rojo: "Ticket no encontrado o ya cerrado." |
| 4. Salida correcta con ID 2 | Costo: $10.00 (1h × $10). Lugar M1 liberado. Ingresos acumulados: $10.00. |

### 5.3 Flujo 3 — Estacionamiento Lleno

| Paso | Descripción / Resultado Esperado |
| - | - |
| 1. Ingresar 5 autos consecutivos (C1–C5 ocupados) | Cada entrada asigna el siguiente spot disponible. |
| 2. Intentar ingresar 6° auto | `parking.enter()` no encuentra spot disponible. Mensaje: "Sin lugares compatibles." |
| 3. Ingresar una moto | Los spots M1–M5 siguen disponibles; la moto se asigna normalmente. |

---

## 6. Respuestas a las Preguntas Guía

**¿Qué clase concentra la responsabilidad de asignar spots y por qué?**

La clase `ParkingLot` concentra la responsabilidad de asignar spots mediante su método `enter()`. Esto sigue el principio de responsabilidad única (SRP): `ParkingLot` es el único objeto que conoce tanto la colección de spots disponibles como el registro de tickets activos. Delegar esa lógica a otro componente violaría la cohesión del modelo.

**¿Qué invariantes protege el modelo?**

* Un spot no puede ser ocupado por dos vehículos simultáneamente. `ParkingSpot.park()` lanza `ValueError` si `is_available_for()` devuelve `False`, garantizando esta restricción sin depender de código externo.
* Un ticket cerrado no puede procesarse dos veces. `ParkingLot.exit()` elimina el ticket del diccionario `_active_tickets` al procesarlo; un segundo intento con el mismo ID lanza `ValueError("Ticket no encontrado")`.

**¿Dónde se aplica polimorfismo y qué ventaja aporta?**

El polimorfismo se aplica en la llamada `self._policy.calculate(hours, ticket.vehicle)` dentro de `ParkingLot.exit()`. La ventaja es la extensibilidad: se pueden agregar nuevas políticas de cobro implementando el protocolo `RatePolicy` sin tocar `ParkingLot`. El sistema es abierto a extensión y cerrado a modificación (principio Open/Closed).

**¿Qué parte del sistema pertenece a Model, View y Controller en Django?**

* **Model**: el paquete `domain/` (`vehicle.py`, `spot.py`, `ticket.py`, `parking_lot.py`). Contiene toda la lógica de negocio independiente de Django.
* **View (Template)**: `dashboard.html`, `entry.html`, `exit.html`. Renderizan el estado recibido del controlador.
* **Controller**: las funciones `dashboard()`, `entry()` y `exit_vehicle()` en `views.py`. Orquestan la interacción entre la petición HTTP, el modelo y la vista.

**Si mañana cambian las tarifas, ¿qué clases tocarías y por qué?**

Sólo sería necesario modificar los parámetros de `HourlyRatePolicy` en la inicialización de `views.py`. Si se requiere una lógica completamente distinta, se crearía una nueva clase que implemente `calculate(hours, vehicle)` y se inyectaría en `ParkingLot`. Ninguna otra clase del dominio requeriría modificación.

```python
# views.py — único cambio necesario
policy = HourlyRatePolicy(car_rate=25.0, moto_rate=12.0)
```

---

## 7. Conclusiones

Esta práctica demostró que los cuatro pilares de la POO no son conceptos aislados, sino herramientas complementarias que, aplicadas en conjunto, producen un diseño robusto, mantenible y extensible.

La encapsulación protegió el estado de `ParkingSpot` y `Ticket` ante modificaciones externas no controladas. La abstracción a través del protocolo `RatePolicy` desacoplió la política de cobro del orquestador, permitiendo intercambiarla sin efectos colaterales. La herencia simplificó la creación de vehículos especializados. El polimorfismo permitió que `ParkingLot` calculara costos sin conocer la implementación concreta de la política.

La separación en capas MVC fue natural: el dominio puro en Python resultó completamente independiente de Django, lo que facilitaría migrar a otro framework o añadir una API REST sin reescribir la lógica de negocio.

Como área de mejora, se identificó que el uso de un estado global en memoria es frágil: reiniciar el servidor resetea todos los datos. Una versión futura podría persistir el estado usando los modelos ORM de Django, conservando la misma lógica de dominio.

---

## 8. Referencias

1. Gamma, E., Helm, R., Johnson, R., & Vlissides, J. (1994). *Design patterns: Elements of reusable object-oriented software*. Addison-Wesley.
2. Martin, R. C. (2003). *Agile software development: Principles, patterns, and practices*. Prentice Hall.
3. Python Software Foundation. (2024). *dataclasses — Data Classes*. Python 3.12 documentation. https://docs.python.org/3/library/dataclasses.html
4. Python Software Foundation. (2024). *typing — Support for type hints*. Python 3.12 documentation. https://docs.python.org/3/library/typing.html#typing.Protocol
5. Django Software Foundation. (2024). *Django documentation: The view layer*. https://docs.djangoproject.com/en/5.0/topics/http/views/
6. Django Software Foundation. (2024). *Django documentation: The template layer*. https://docs.djangoproject.com/en/5.0/topics/templates/
7. Lutz, M. (2013). *Learning Python* (5th ed.). O'Reilly Media.
