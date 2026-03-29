from dataclasses import dataclass
from enum import Enum
from typing import Optional
from .vehicle import Vehicle, VehicleType

class SpotType(Enum):
    CAR = "CAR"
    MOTORCYCLE = "MOTORCYCLE"
    ANY = "ANY"

@dataclass
class ParkingSpot:
    _spot_id: str
    _allowed: SpotType
    _occupied: bool = False
    _current_vehicle: Optional[Vehicle] = None

    @property
    def spot_id(self) -> str:
        return self._spot_id

    @property
    def occupied(self) -> bool:
        return self._occupied

    def is_available_for(self, v: Vehicle) -> bool:
        if self._occupied:
            return False
        
        # Si es ANY, acepta cualquiera. Si no, debe coincidir el tipo.
        if self._allowed == SpotType.ANY:
            return True
            
        return self._allowed.name == v.get_type().name

    def park(self, v: Vehicle) -> None:
        # Encapsulación: La validación vive dentro del método
        if not self.is_available_for(v):
            raise ValueError(f"El lugar {self._spot_id} no está disponible para este vehículo.")
        
        self._occupied = True
        self._current_vehicle = v

    def release(self) -> None:
        self._occupied = False
        self._current_vehicle = None