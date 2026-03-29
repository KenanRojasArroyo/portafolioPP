from dataclasses import dataclass
from enum import Enum

# Definimos los tipos según el diagrama UML
class VehicleType(Enum):
    CAR = "CAR"
    MOTORCYCLE = "MOTORCYCLE"

@dataclass
class Vehicle:
    _plate: str  # Usamos el guion bajo para indicar encapsulación (privado)
    _type: VehicleType

    def get_plate(self) -> str:
        return self._plate

    def get_type(self) -> VehicleType:
        return self._type

# Herencia y subtipos (Requisito para la Sesión 2)
class Car(Vehicle):
    def __init__(self, plate: str):
        super().__init__(_plate=plate, _type=VehicleType.CAR)

class Motorcycle(Vehicle):
    def __init__(self, plate: str):
        super().__init__(_plate=plate, _type=VehicleType.MOTORCYCLE)