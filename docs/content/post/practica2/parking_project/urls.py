from django.urls import path
from . import views

urlpatterns = [
    path('', views.dashboard, name='dashboard'),
    path('entry/', views.entry, name='entry'),
    path('exit/', views.exit_vehicle, name='exit'),
]