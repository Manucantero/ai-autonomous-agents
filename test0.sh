#!/bin/bash

# Colores para mejorar la visibilidad
VERDE='\033[0;32m'
AZUL='\033[0;34m'
NC='\033[0m' # No Color

# Función para imprimir un separador visual
imprimir_separador() {
    echo -e "${AZUL}======================================================================"
    echo -e "  PRUEBA NÚMERO: $1"
    echo -e "======================================================================${NC}"
}

# Comprobar si el ejecutable existe
if [ ! -f "./practica2SG" ]; then
    echo "Error: El ejecutable ./practica2SG no se encuentra en este directorio."
    exit 1
fi

# Lista de ejecuciones
imprimir_separador "1"
./practica2SG -m ./mapas/mapa30.map -n 0 -i 17 5 0 -t 17 17 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "2"
./practica2SG -m ./mapas/mapa30.map -n 0 -i 24 7 2 -t 17 17 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "3"
./practica2SG -m ./mapas/mapa30.map -n 0 -i 24 10 2 -t 17 17 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "4"
./practica2SG -m ./mapas/2ez.map -n 0 -i 19 26 3 -t 11 27 1 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "5"
./practica2SG -m ./mapas/mapa30.map -n 0 -i 16 9 2 -t 16 14 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "6"
./practica2SG -m ./mapas/mapa50.map -n 0 -i 38 10 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "7"
./practica2SG -m ./mapas/mapa75.map -n 0 -i 69 3 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "8"
./practica2SG -m ./mapas/mapa50_cuadricula.map -n 0 -i 34 39 7 -t 13 34 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "9"
./practica2SG -m ./mapas/bosque_venenoso.map -n 0 -i 12 21 1 -t 12 3 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "10"
./practica2SG -m ./mapas/gemini2.map -n 0 -i 19 26 3 -t 22 23 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "11"
./practica2SG -m ./mapas/2ez.map -n 0 -i 10 16 3 -t 25 18 1 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "12"
./practica2SG -m ./mapas/mapaop.map -n 0 -i 15 55 0 -t 15 33 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "13"
./practica2SG -m ./mapas/luminalia25.map -n 0 -i 55 55 4 -t 56 46 7 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "14"
./practica2SG -m ./mapas/minivertiguito.map -n 0 -i 20 23 1 -t 6 8 7 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "15"
./practica2SG -m ./mapas/minivertiguito.map -n 0 -i 8 10 5 -t 6 8 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "16"
./practica2SG -m ./mapas/F_islas.map -n 0 -i 59 56 2 -t 59 57 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "17"
./practica2SG -m ./mapas/F_islas.map -n 0 -i 12 3 2 -t 92 88 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"
imprimir_separador "18"
./practica2SG -m ./mapas/minivertiguito.map -n 1 -i 7 4 0 -t 24 24 7 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "19"
./practica2SG -m ./mapas/minivertiguito.map -n 1 -i 7 4 0 -t 24 24 7 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 1000
echo -e "\n"

imprimir_separador "20"
./practica2SG -m ./mapas/gemini2.map -n 1 -i 20 8 0 -t 13 13 3 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "21"
./practica2SG -m ./mapas/mapa30.map -n 1 -i 16 7 7 -t 17 16 4 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "22"
./practica2SG -m ./mapas/mapa50.map -n 1 -i 38 10 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "23"
./practica2SG -m ./mapas/mapa50_cuadricula.map -n 1 -i 45 25 0 -t 45 26 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "24"
./practica2SG -m ./mapas/bosque_prohibido.map -n 1 -i 4 11 0 -t 9 10 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "25"
./practica2SG -m ./mapas/2ez.map -n 1 -i 19 26 3 -t 11 27 1 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "26"
./practica2SG -m ./mapas/bosque_prohibido.map -n 1 -i 4 11 0 -t 9 10 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 1000
echo -e "\n"

imprimir_separador "27"
./practica2SG -m ./mapas/2ez.map -n 1 -i 19 26 3 -t 11 27 1 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 1000
echo -e "\n"

imprimir_separador "28"
./practica2SG -m ./mapas/chess.map -n 1 -i 38 41 2 -t 31 44 3 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

#imprimir_separador "29"
#./practica2SG -m ./mapas/nuevo_mapa40.map -n 1 -i 33 33 3 -t 33 35 4 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
#echo -e "\n"

imprimir_separador "30"
./practica2SG -m ./mapas/mapaop.map -n 1 -i 69 63 6 -t 24 67 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "31"
./practica2SG -m ./mapas/mapa75.map -n 1 -i 69 3 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "32"
./practica2SG -m ./mapas/mapa75_espirales.map -n 1 -i 6 14 2 -t 66 71 4 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "33"
./practica2SG -m ./mapas/islas_cambio_climatico.map -n 1 -i 47 54 0 -t 47 55 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

imprimir_separador "34"
./practica2SG -m ./mapas/mapa100.map -n 1 -i 47 54 0 -t 48 54 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
echo -e "\n"

echo -e "${VERDE}Todas las pruebas han finalizado.${NC}"

