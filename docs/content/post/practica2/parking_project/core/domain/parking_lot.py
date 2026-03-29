from typing import List, Dict, Protocol
from datetime import datetime

from .vehicle import Vehicle
from .spot import ParkingSpot
from .ticket import Ticket

# --- ABSTRACCIÓN Y POLIMORFISMO (Sesión 2) ---
class RatePolicy(Protocol):
    def calculate(self, hours: float, v: Vehicle) -> float:
        ...

class HourlyRatePolicy:
    def __init__(self, car_rate: float, moto_rate: float):
        self._car_rate = car_rate
        self._moto_rate = moto_rate

    def calculate(self, hours: float, v: Vehicle) -> float:
        # Cobro distinto dependiendo del subtipo de vehículo
        rate = self._car_rate if v.get_type().name == "CAR" else self._moto_rate
        return hours * rate

class FlatRatePolicy:
    def __init__(self, flat_amount: float):
        self._flat_amount = flat_amount

    def calculate(self, hours: float, v: Vehicle) -> float:
        return self._flat_amount

# --- CLASE PRINCIPAL ---
class ParkingLot:
    def __init__(self, spots: List[ParkingSpot], policy: RatePolicy):
        self._spots = spots
        self._active_tickets: Dict[int, Ticket] = {}
        self._policy = policy  # Inyección de dependencias (Composición)
        self._next_ticket_id = 1
        self._total_revenue = 0.0

    def enter(self, v: Vehicle, now: datetime) -> Ticket:
        # 1. Buscar lugar disponible y compatible (Invariante protegida)
        available_spot = next((s for s in self._spots if s.is_available_for(v)), None)
        if not available_spot:
            raise ValueError("Estacionamiento lleno o sin lugares compatibles.")

        # 2. Ocupar el lugar
        available_spot.park(v)

        # 3. Generar el ticket
        ticket = Ticket(self._next_ticket_id, v, available_spot, now)
        self._active_tickets[self._next_ticket_id] = ticket
        self._next_ticket_id += 1
        
        return ticket

    def exit(self, ticket_id: int, now: datetime) -> float:
        # 1. Validar ticket
        if ticket_id not in self._active_tickets:
            raise ValueError("Ticket no encontrado o ya cerrado.")

        ticket = self._active_tickets[ticket_id]

        # 2. Cerrar ticket y liberar lugar
        ticket.close(now)
        ticket.spot.release()

        # 3. Calcular cobro usando polimorfismo
        hours = ticket.get_duration_hours()
        cost = self._policy.calculate(hours, ticket.vehicle)

        # 4. Actualizar ingresos y remover de activos
        self._total_revenue += cost
        del self._active_tickets[ticket_id]

        return cost

    def get_occupancy(self) -> str:
        libres = sum(1 for s in self._spots if not s.occupied)
        ocupados = len(self._spots) - libres
        return f"Libres: {libres}, Ocupados: {ocupados}"

    def get_active_tickets(self) -> List[Ticket]:
        return list(self._active_tickets.values())