from django.shortcuts import render, redirect
from django.contrib import messages
from datetime import datetime

# Importamos nuestra lógica de dominio pura (POO)
from .domain.spot import ParkingSpot, SpotType
from .domain.vehicle import Car, Motorcycle
from .domain.parking_lot import ParkingLot, HourlyRatePolicy

# --- INICIALIZACIÓN DEL ESTADO EN MEMORIA ---
# Como pide la práctica, no usamos base de datos.
# Creamos una configuración inicial: 5 lugares para autos y 5 para motos
spots = [ParkingSpot(f"C{i}", SpotType.CAR) for i in range(1, 6)] + \
        [ParkingSpot(f"M{i}", SpotType.MOTORCYCLE) for i in range(1, 6)]

# Definimos una política de cobro ($20 la hora para auto, $10 para moto)
policy = HourlyRatePolicy(car_rate=20.0, moto_rate=10.0)

# Instancia GLOBAL del estacionamiento que vivirá mientras el servidor esté corriendo
parking = ParkingLot(spots, policy)
# -------------------------------------------

def dashboard(request):
    """Vista principal: Muestra el estado del estacionamiento."""
    context = {
        'occupancy': parking.get_occupancy(),
        'active_tickets': parking.get_active_tickets(),
        'total_revenue': parking._total_revenue 
    }
    return render(request, 'dashboard.html', context)

def entry(request):
    """Controlador para la entrada de vehículos."""
    if request.method == 'POST':
        plate = request.POST.get('plate')
        v_type = request.POST.get('type')
        
        try:
            # Instanciamos el subtipo correcto
            vehicle = Car(plate) if v_type == 'CAR' else Motorcycle(plate)
            
            # Le pasamos la responsabilidad a nuestra clase ParkingLot
            ticket = parking.enter(vehicle, datetime.now())
            messages.success(request, f"¡Vehículo ingresado! Ticket ID: {ticket.ticket_id} | Lugar: {ticket.spot.spot_id}")
            return redirect('dashboard')
            
        except ValueError as e:
            # Si el estacionamiento está lleno, capturamos el error
            messages.error(request, str(e))
            
    return render(request, 'entry.html')

def exit_vehicle(request):
    """Controlador para la salida y cobro de vehículos."""
    if request.method == 'POST':
        try:
            ticket_id = int(request.POST.get('ticket_id'))
            
            # ParkingLot se encarga de calcular el costo, liberar el lugar y cerrar el ticket
            cost = parking.exit(ticket_id, datetime.now())
            messages.success(request, f"Salida exitosa. Total cobrado: ${cost:.2f}")
            return redirect('dashboard')
            
        except ValueError as e:
            messages.error(request, str(e))
        except TypeError:
             messages.error(request, "Por favor, ingresa un ID de ticket válido.")
            
    return render(request, 'exit.html')