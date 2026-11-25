#!/bin/bash

# ==========================================
# Script de Pruebas Automáticas - Práctica 2
# ==========================================

# Definición de colores para la salida
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # Sin color

# Arrays con los comandos de cada nivel
declare -a tests_nivel2=(
    "./practica2SG -m ./mapas/2ez.map -n 2 -i 23 19 2 -t 14 25 6 -seed 0 -Tiempo 3000 -Ambiental 2219 -Energia 8445 -O 14 16"
    "./practica2SG -m ./mapas/mapaop.map -n 2 -i 69 62 5 -t 20 45 6 -seed 0 -Tiempo 3000 -Ambiental 674 -Energia 5941 -O 53 4"
    "./practica2SG -m ./mapas/vertigo.map -n 2 -i 12 28 6 -t 87 45 3 -seed 0 -Tiempo 3000 -Ambiental 2752 -Energia 9316 -O 82 11"
    "./practica2SG -m ./mapas/mapa75.map -n 2 -i 55 30 3 -t 12 53 2 -seed 0 -Tiempo 3000 -Ambiental 1919 -Energia 9247 -O 51 53"
    "./practica2SG -m ./mapas/scape26.map -n 2 -i 20 15 0 -t 22 12 1 -seed 0 -Tiempo 3000 -Ambiental 2303 -Energia 7662 -O 22 21"
    "./practica2SG -m ./mapas/mapa75_espirales.map -n 2 -i 24 68 1 -t 23 22 1 -seed 0 -Tiempo 3000 -Ambiental 2222 -Energia 9163 -O 24 66"
    "./practica2SG -m ./mapas/mapa75.map -n 2 -i 35 35 6 -t 49 19 5 -seed 0 -Tiempo 3000 -Ambiental 60 -Energia 4920 -O 51 35"
    "./practica2SG -m ./mapas/parchis.map -n 2 -i 81 53 7 -t 40 62 5 -seed 0 -Tiempo 3000 -Ambiental 674 -Energia 7897 -O 50 51"
    "./practica2SG -m ./mapas/mapaop.map -n 2 -i 51 42 6 -t 69 39 5 -seed 0 -Tiempo 3000 -Ambiental 1510 -Energia 7923 -O 65 15"
    "./practica2SG -m ./mapas/mapa50.map -n 2 -i 10 16 0 -t 15 38 5 -seed 0 -Tiempo 3000 -Ambiental 885 -Energia 5036 -O 41 37"
    "./practica2SG -m ./mapas/vertigo.map -n 2 -i 15 27 2 -t 26 58 2 -seed 0 -Tiempo 3000 -Ambiental 1701 -Energia 4866 -O 26 41"
    "./practica2SG -m ./mapas/F_islas.map -n 2 -i 51 10 2 -t 56 86 6 -seed 0 -Tiempo 3000 -Ambiental 1225 -Energia 9135 -O 68 82"
    "./practica2SG -m ./mapas/luminalia25.map -n 2 -i 49 32 1 -t 90 59 0 -seed 0 -Tiempo 3000 -Ambiental 751 -Energia 9954 -O 33 43"
    "./practica2SG -m ./mapas/mapa30.map -n 2 -i 16 12 1 -t 24 18 3 -seed 0 -Tiempo 3000 -Ambiental 2687 -Energia 6375 -O 5 21"
    "./practica2SG -m ./mapas/mapa30_26.map -n 2 -i 16 23 3 -t 16 20 7 -seed 0 -Tiempo 3000 -Ambiental 2624 -Energia 9779 -O 25 17"
    "./practica2SG -m ./mapas/islas_cambio_climatico.map -n 2 -i 25 96 3 -t 61 27 6 -seed 0 -Tiempo 3000 -Ambiental 145 -Energia 8847 -O 56 76"
    "./practica2SG -m ./mapas/mapa75_espirales.map -n 2 -i 25 47 2 -t 27 10 6 -seed 0 -Tiempo 3000 -Ambiental 1171 -Energia 8666 -O 16 41"
)

declare -a tests_nivel3=(
    "./practica2SG -m ./mapas/mapa30.map -n 3 -i 5 55 2 -t 10 10 4 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 12 5"
    "./practica2SG -m ./mapas/mapa75.map -n 3 -i 5 5 2 -t 8 5 2 -seed 0 -Tiempo 3000 -Ambiental 2362 -Energia 8505 -O 31 54"
    "./practica2SG -m ./mapas/mapa75.map -n 3 -i 5 5 2 -t 8 5 2 -seed 0 -Tiempo 3000 -Ambiental 2362 -Energia 8505 -O 31 50"
    "./practica2SG -m ./mapas/minivertiguito.map -n 3 -i 11 19 1 -t 25 9 1 -seed 0 -Tiempo 3000 -Ambiental 2109 -Energia 5874 -O 11 13"
    "./practica2SG -m ./mapas/islas25.map -n 3 -i 11 7 3 -t 49 58 2 -seed 0 -Tiempo 3000 -Ambiental 2400 -Energia 5947 -O 55 68"
    "./practica2SG -m ./mapas/paldea25.map -n 3 -i 88 18 3 -t 70 49 4 -seed 0 -Tiempo 3000 -Ambiental 861 -Energia 4664 -O 69 71"
    "./practica2SG -m ./mapas/mapa30.map -n 3 -i 17 17 3 -t 19 19 5 -seed 0 -Tiempo 3000 -Ambiental 2461 -Energia 3813 -O 22 6"
    "./practica2SG -m ./mapas/vertigo.map -n 3 -i 52 44 2 -t 50 13 3 -seed 0 -Tiempo 3000 -Ambiental 2812 -Energia 5966 -O 42 8"
    "./practica2SG -m ./mapas/parchis.map -n 3 -i 29 34 0 -t 34 11 6 -seed 0 -Tiempo 3000 -Ambiental 476 -Energia 4331 -O 22 29"
    "./practica2SG -m ./mapas/mapa50_cuadricula.map -n 3 -i 13 6 3 -t 27 38 4 -seed 0 -Tiempo 3000 -Ambiental 2533 -Energia 8505 -O 34 28"
    "./practica2SG -m ./mapas/mapa30_26.map -n 3 -i 13 26 5 -t 16 9 6 -seed 0 -Tiempo 3000 -Ambiental 2364 -Energia 7342 -O 26 3"
    "./practica2SG -m ./mapas/luminalia25.map -n 3 -i 22 50 1 -t 61 41 7 -seed 0 -Tiempo 3000 -Ambiental 2818 -Energia 4312 -O 12 96"
    "./practica2SG -m ./mapas/islas_cambio_climatico.map -n 3 -i 87 23 4 -t 65 17 0 -seed 0 -Tiempo 3000 -Ambiental 2922 -Energia 6305 -O 4 28"
    "./practica2SG -m ./mapas/minivertiguito.map -n 3 -i 11 19 1 -t 25 9 1 -seed 0 -Tiempo 3000 -Ambiental 2109 -Energia 5874 -O 11 13"
    "./practica2SG -m ./mapas/mapaop.map -n 3 -i 56 45 0 -t 51 38 3 -seed 0 -Tiempo 3000 -Ambiental 2796 -Energia 9842 -O 11 28"
    "./practica2SG -m ./mapas/mapa30.map -n 3 -i 21 9 6 -t 21 10 2 -seed 0 -Tiempo 3000 -Ambiental 200 -Energia 4054 -O 9 24"
    "./practica2SG -m ./mapas/luminalia25.map -n 3 -i 51 30 0 -t 32 71 6 -seed 0 -Tiempo 3000 -Ambiental 1834 -Energia 5531 -O 58 38"
)

declare -a tests_nivel4=(
    "./practica2SG -m ./mapas/minivertiguito.map -n 4 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 648 -Energia 3000 -O 19 11"
    "./practica2SG -m ./mapas/minivertiguito.map -n 4 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 17 22"
    "./practica2SG -m ./mapas/minivertiguito.map -n 4 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 3 3"
    "./practica2SG -m ./mapas/mapa30_26.map -n 4 -i 13 26 5 -t 16 9 6 -seed 0 -Tiempo 3000 -Ambiental 2364 -Energia 3000 -O 26 3"
    "./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 3000 -O 26 3"
    "./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 3000 -O 22 13"
    "./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 3000 -Energia 3000 -O 22 13"
    "./practica2SG -m ./mapas/mapaop.map -n 4 -i 48 43 1 -t 52 38 1 -seed 0 -Tiempo 3000 -Ambiental 2280 -Energia 3403 -O 26 22"
    "./practica2SG -m ./mapas/mapa30.map -n 4 -i 26 12 5 -t 23 4 4 -seed 0 -Tiempo 3000 -Ambiental 1550 -Energia 7422 -O 5 23"
    "./practica2SG -m ./mapas/mapa30.map -n 4 -i 26 12 5 -t 23 4 4 -seed 0 -Tiempo 3000 -Ambiental 650 -Energia 7422 -O 19 15"
    "./practica2SG -m ./mapas/mapa30.map -n 4 -i 26 12 5 -t 23 4 4 -seed 0 -Tiempo 3000 -Ambiental 650 -Energia 7422 -O 18 15"
    "./practica2SG -m ./mapas/mapa75_espirales.map -n 4 -i 56 10 4 -t 52 30 4 -seed 0 -Tiempo 3000 -Ambiental 1700 -Energia 3648 -O 29 24"
    "./practica2SG -m ./mapas/luminalia25.map -n 4 -i 83 62 0 -t 39 75 0 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 7552 -O 63 61"
    "./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 900 -Energia 3000 -O 8 24"
    "./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 3000 -O 8 24"
    "./practica2SG -m ./mapas/mapaop.map -n 4 -i 48 43 1 -t 52 38 1 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3403 -O 26 22"
    "./practica2SG -m ./mapas/mapa30.map -n 4 -i 26 12 5 -t 23 4 4 -seed 0 -Tiempo 3000 -Ambiental 650 -Energia 7422 -O 16 8"
    

)

# Función para ejecutar un conjunto de tests
ejecutar_tests() {
    local nivel=$1
    local -n tests_array=$2 # Paso por referencia del array
    local total=${#tests_array[@]}
    local correctos=0
    local fallos=0

    echo -e "${CYAN}==========================================${NC}"
    echo -e "${CYAN}      INICIANDO TESTS DEL NIVEL $nivel    ${NC}"
    echo -e "${CYAN}==========================================${NC}"

    for i in "${!tests_array[@]}"; do
        num=$((i + 1))
        echo -e "\n${YELLOW}▶ Ejecutando Test T${nivel}.${num} / ${total}...${NC}"
        
        # Ejecuta el test. Se asume que el simulador termina (o puedes añadir timeout)
        ${tests_array[$i]}
        
        # Capturamos el código de salida
        status=$?
        
        if [ $status -eq 0 ]; then
            echo -e "${GREEN}✔ Test T${nivel}.${num} finalizado (Revisar salida para éxito real).${NC}"
            ((correctos++))
        else
            echo -e "${RED}✘ Test T${nivel}.${num} terminó con error (Código: $status)${NC}"
            ((fallos++))
        fi
    done

    echo -e "\n${CYAN}--- RESUMEN NIVEL $nivel ---${NC}"
    echo -e "Total: $total | ${GREEN}Éxitos aparentes: $correctos${NC} | ${RED}Fallos/Crashes: $fallos${NC}"
    echo "----------------------------"
}

# Comprobación de que existe el ejecutable
if [ ! -f "./practica2SG" ]; then
    echo -e "${RED}Error: No se encuentra el ejecutable './practica2SG' en el directorio actual.${NC}"
    echo "Compila el código antes de ejecutar el script."
    exit 1
fi

# Menú interactivo
echo -e "${YELLOW}¿Qué tests deseas ejecutar?${NC}"
echo "2) Nivel 2"
echo "3) Nivel 3"
echo "4) Nivel 4"
echo "A) Todos los niveles (All)"
read -p "Opción [2/3/4/A]: " opcion

case $opcion in
    2)
        ejecutar_tests 2 tests_nivel2
        ;;
    3)
        ejecutar_tests 3 tests_nivel3
        ;;
    4)
        ejecutar_tests 4 tests_nivel4
        ;;
    a|A)
        ejecutar_tests 2 tests_nivel2
        ejecutar_tests 3 tests_nivel3
        ejecutar_tests 4 tests_nivel4
        echo -e "\n${GREEN}★★★ TODOS LOS TESTS HAN FINALIZADO ★★★${NC}"
        ;;
    *)
        echo -e "${RED}Opción no válida.${NC}"
        exit 1
        ;;
esac

exit 0
