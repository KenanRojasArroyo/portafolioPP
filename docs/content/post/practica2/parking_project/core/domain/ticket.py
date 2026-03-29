from dataclasses import dataclass
from datetime import datetime
from enum import Enum
from typing import Optional
from .vehicle import Vehicle
from .spot import ParkingSpot

# Definimos los estados del ticket según tu diagrama UML
class TicketStatus(Enum):
    ACTIVE = "ACTIVE"
    CLOSED = "CLOSED"

@dataclass
class Ticket:
    _ticket_id: int
    _vehicle: Vehicle
    _spot: ParkingSpot
    _entry_time: datetime
    _exit_time: Optional[datetime] = None
    _status: TicketStatus = TicketStatus.ACTIVE

    @property
    def ticket_id(self) -> int:
        return self._ticket_id

    @property
    def spot(self) -> ParkingSpot:
        return self._spot
        
    @property
    def vehicle(self) -> Vehicle:
        return self._vehicle

    @property
    def entry_time(self) -> datetime:
        return self._entry_time
    
    def close(self, exit_time: datetime) -> None:
        self._exit_time = exit_time
        self._status = TicketStatus.CLOSED

    def get_duration_hours(self) -> float:
        if not self._exit_time:
            return 0.0
        # Calculamos la diferencia en horas
        diff = self._exit_time - self._entry_time
        hours = diff.total_seconds() / 3600.0
        # Cobramos mínimo 1 hora, redondeando hacia arriba para simplificar
        return max(1.0, round(hours, 2))