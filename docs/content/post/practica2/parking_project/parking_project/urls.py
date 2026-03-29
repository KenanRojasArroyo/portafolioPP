from django.contrib import admin
from django.urls import path, include

urlpatterns = [
    path('admin/', admin.site.urls),
    # Incluimos las rutas de nuestra aplicación "core" en la raíz del sitio
    path('', include('core.urls')), 
]