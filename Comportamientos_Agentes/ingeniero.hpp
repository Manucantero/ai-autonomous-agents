#ifndef COMPORTAMIENTOINGENIERO_H
#define COMPORTAMIENTOINGENIERO_H

#include <chrono>
#include <list>
#include <map>
#include <set>
#include <thread>
#include <time.h>
#include <cmath>
#include <queue>

#include "comportamientos/comportamiento.hpp"

// =========================================================================
// ESTRUCTURAS DE APOYO PARA BÚSQUEDA (Nivel 1 y 2)
// =========================================================================

/**
 * @brief Representa el estado del Ingeniero en el mundo.
 * Define de forma única la situación del agente para algoritmos de búsqueda.
 */
struct EstadoI {
    ubicacion site;      // f (fila), c (columna), brujula (orientación)
    bool zapatillas;     // Indica si el agente posee el objeto 'Zapatillas'

    // Operador de igualdad para comparar si dos estados son idénticos
    bool operator==(const EstadoI &st) const {
        return site.f == st.site.f && site.c == st.site.c &&
               site.brujula == st.site.brujula && zapatillas == st.zapatillas;
    }
    
    // Operador de orden para permitir el uso de este estado en contenedores como std::map o std::set
    bool operator<(const EstadoI &st) const {
        if (site.f != st.site.f) return site.f < st.site.f;
        if (site.c != st.site.c) return site.c < st.site.c;
        if (site.brujula != st.site.brujula) return site.brujula < st.site.brujula;
        return zapatillas < st.zapatillas;
    }
};


/**
 * @brief Nodo del árbol de búsqueda.
 * Almacena el estado actual, el coste acumulado y la secuencia de acciones para llegar a él.
 */
struct NodoI {
    EstadoI estado;
    list<Action> secuencia; // Plan de acciones acumulado
    int coste;              // Coste acumulado 

    bool operator==(const NodoI &node) const {
        return estado == node.estado;
    }
    
    // Operador de ordenación detallado para la gestión de estados visitados
    bool operator<(const NodoI &node) const {
        if (estado.site.f < node.estado.site.f) return true;
        else if (estado.site.f == node.estado.site.f && estado.site.c < node.estado.site.c) return true;
        else if (estado.site.f == node.estado.site.f && estado.site.c == node.estado.site.c && 
                 estado.site.brujula < node.estado.site.brujula) return true;
        else if (estado.site.f == node.estado.site.f && estado.site.c == node.estado.site.c && 
                 estado.site.brujula == node.estado.site.brujula && estado.zapatillas < node.estado.zapatillas) return true;
        else return false;
    }
};


/**
 * @brief Functor de comparación para la cola de prioridad.
 * Los nodos con MENOR coste salen primero.
 */
struct ComparaCosteNodoI {
    bool operator()(const NodoI &a, const NodoI &b) const {
        return a.coste > b.coste; 
    }
};

/**
 * @brief Estructura específica para la planificación de la red de tuberías (A*).
 */
struct NodoTuberia {
    int f, c;
    int altura_efectiva;    // Altura modificada tras operación RAISE/DIG
    int g, h;               // g: coste real acumulado, h: heurística
    int impacto_acumulado;  // Impacto ecológico generado por la obra
    list<Paso> secuencia;   // Lista de pasos (f, c, operacion) de la tubería

    // Operador para priority_queue orientado a minimizar (g + h)
    bool operator<(const NodoTuberia& otro) const {
        if ((g + h) == (otro.g + otro.h)) {
            // Desempate por menor impacto ecológico
            return impacto_acumulado > otro.impacto_acumulado; 
        }
        return (g + h) > (otro.g + otro.h);
    }
};

// =========================================================================
// CLASE COMPORTAMIENTO INGENIERO
// =========================================================================


class ComportamientoIngeniero : public Comportamiento {
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================
  
  /**
   * @brief Constructor para niveles 0, 1 y 6 (sin mapa completo)
   * @param size Tamaño del mapa (si es 0, se inicializa más tarde)
   */
  ComportamientoIngeniero(unsigned int size = 0) : Comportamiento(size) {

    last_action = IDLE;
    tiene_zapatillas = false;
    giro45Izq = 0;
    
     prioridad_derecha = true; 
     
     for (int i=0; i<100; i++)
       for (int j=0; j<100; j++)
         matriz_visitados[i][j] = 0;
  }

  /**
   * @brief Constructor para niveles 2, 3, 4 y 5 (con mapa completo conocido)
   * @param mapaR Mapa de terreno conocido
   * @param mapaC Mapa de cotas conocido
   */
  ComportamientoIngeniero(std::vector<std::vector<unsigned char>> mapaR, 
                         std::vector<std::vector<unsigned char>> mapaC): 
                         Comportamiento(mapaR, mapaC) {

        last_action = IDLE;
        tiene_zapatillas = false;
        giro45Izq = 0;
        
        hayPlan = false;
        
        fase_actual = GENERAR_RED_TUBERIAS;
        lista_tuberias.clear();
        plan_viaje_ing.clear();
        tecnico_llamado = false;
  }

  ComportamientoIngeniero(const ComportamientoIngeniero &comport)
      : Comportamiento(comport) {}
  ~ComportamientoIngeniero() {}

  /**
   * @brief Bucle principal de decisión del agente.
   * Estudia los sensores y decide la siguiente acción.
   * 
   * EJEMPLO DE USO:
   * Action accion = think(sensores);
   * return accion; // El motor ejecutará esta acción
   */
  Action think(Sensores sensores);

  ComportamientoIngeniero *clone() {
    return new ComportamientoIngeniero(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================

  // Funciones específicas para cada nivel (para ser implementadas por el alumno)
  
  /**
   * @brief Implementación del Nivel 0.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_0(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 1.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_1(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 2.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */ 
  Action ComportamientoIngenieroNivel_2(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 3.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_3(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 4.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_4(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 5.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_5(Sensores sensores);
  
  /**
   * @brief Implementación del Nivel 6.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_6(Sensores sensores);
  
  
  

protected:
  // =========================================================================
  // FUNCIONES PROPORCIONADAS
  // =========================================================================

  /**
   * @brief Actualiza la información del mapa interno basándose en los sensores.
   * IMPORTANTE: Esta función ya está implementada. Actualiza mapaResultado y mapaCotas
   * con la información de los 16 sensores (casilla actual + 15 casillas alrededor).
   */
  void ActualizarMapa(Sensores sensores);

  /**
   * @brief Comprueba si una casilla es transitable.
   * @param f Fila de la casilla.
   * @param c Columna de la casilla.
   * @param tieneZapatillas Indica si el agente posee zapatillas.
   * @return true si la casilla es transitable (no es muro ni precipicio).
   */
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * REGLAS: Desnivel máximo 1 sin zapatillas, 2 con zapatillas.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const ubicacion &actual, bool zap);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  ubicacion Delante(const ubicacion &actual) const;

  bool es_camino(unsigned char c) const;

  /**
 * @brief Imprime por consola la secuencia de acciones de un plan para un agente.
 * @param plan  Lista de acciones del plan.
 */
  void PintaPlan(const list<Action> &plan);


/**
 * @brief Imprime las coordenadas y operaciones de un plan de tubería.
 * @param plan  Lista de pasos (fila, columna, operación).
 */
  void PintaPlan(const list<Paso> &plan);


  /**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa gráfico.
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
  void VisualizaPlan(const ubicacion &st, const list<Action> &plan);

  /**
 * @brief Convierte un plan de tubería en la lista de casillas usada
 *        por el sistema de visualización.
 * @param st    Estado de partida (no utilizado directamente).
 * @param plan  Lista de pasos del plan de tubería.
 */
  void VisualizaRedTuberias(const list<Paso> &plan);



private:
  // =========================================================================
  // VARIABLES DE ESTADO (PUEDEN SER EXTENDIDAS POR EL ALUMNO)
  // =========================================================================
  
// --- Variables de Estado Internas ---
  static constexpr int COST_INFINITE = 999999;
  Action last_action;
  bool tiene_zapatillas;
  int giro45Izq;
 
  
// Estado Nivel 0 y 1 (Exploración)
  bool prioridad_derecha;
  int matriz_visitados[100][100];
  int fil, col;
  Orientacion brujula;
  
  
// Estado Nivel 2+ (Planificación)
  bool hayPlan;
  list<Action> plan;
    
// --- Nivel 5: Máquina de Estados para la Obra ---
  enum FaseIngeniero { 
    GENERAR_RED_TUBERIAS,     // Llama a A_Estrella_Tuberias
    DESPLAZAMIENTO_BELKANITA, // Camino hacia el origen de la tubería
    PREPARAR_BELKANITA,       // Primer ajuste de terreno (si es necesario)
    SOLICITAR_TECNICO,        // Grita COME
    DESPLAZAMIENTO_TRAMO,     // Va a la baldosa donde toca instalar
    ESPERAR_ALINEACION,       // Se gira hasta ver al técnico de frente
    NIVELAR_TERRENO,          // Hace RAISE, DIG o IDLE según el plan
    COLOCAR_TUBERIA           // Ejecuta el INSTALL
  };

  FaseIngeniero fase_actual;
  list<Paso> lista_tuberias;   // Guardará el resultado de A_Estrella_Tuberias

  Paso tramo_objetivo;         // El paso individual que estamos procesando

  EstadoI estadoDestino;
  list<Action> plan_viaje_ing;

  bool tecnico_llamado;
  
  // =========================================================================
  // Nivel 0
  // =========================================================================
  
  /**
  * @brief Determina la mejor opción entre las 3 casillas que tiene delante
  * @param i terreno que hay en la posicion 1 de la superficie (45 izq)
  * @param c terreno que hay en la posicion 2 de la superficie (justo delante)  
  * @param d terreno que hay en la posicion 3 de la superficie (45 dch)
  * @return 2 si es mejor que WALK, 1 para TURN_SL y 3 para TURN_SR. 0 no hay nada interesante
  */
  int VeoCasillaInteresanteI(char i, char c, char d, bool zap, bool pref_dcha, const Sensores &sensores);

  /**
  * @brief Determina si casilla viable por altura
  * @param casilla tipo de terreno
  * @param dif diferencia de altura entre casillas
  * @param zap indica si estoy en posesión de las zapatillas
  * @ 'P' si no es accesible por altura y casilla en otro caso
  */
 
  char ViablePorAlturaI (char casilla, int dif, bool zap) ;
  
 /**
  * @brief Filtra el tipo de casilla en el Nivel 0.
  * @details En el nivel 0, el agente solo puede pisar 'C', 'D' o 'U'. 
  * Esta función determina si una casilla es válida según las reglas reactivas.
  * @param sensores Objeto con la información actual del entorno.
  * @param pos Índice del vector de visión (0-15) que se quiere analizar.
  * @param zap Booleano que indica si el agente tiene zapatillas (en N0 es informativo).
  * @return El carácter del terreno si es transitable, o un valor de bloqueo si no.
  */  
  char FiltrarCasillaNivel0(Sensores sensores, int pos, bool zap);
  
 
  // =========================================================================
  // Nivel 1
  // =========================================================================
  
  
  /**
   * @brief Asigna un valor de "atractivo" a una casilla según su tipo de superficie.
   * @param casilla El carácter del terreno (sensores.terreno[i]).
   * @return int Puntuación de utilidad (Caminos y Senderos tienen prioridad).
   */
  int PuntuacionCasilla(unsigned char casilla) ;
  
 /**
  * @brief Filtra el tipo de casilla en el Nivel 1 (Exploración).
  * @details A diferencia del Nivel 0, aquí se busca descubrir 'C' y 'S'. 
  * Se usa paso por referencia constante para mayor eficiencia.
  * @param sensores Referencia a los sensores del agente.
  * @param pos Posición en el cono de visión.
  * @param tiene_zapatillas Estado de posesión del objeto zapatillas.
  * @return El carácter del terreno procesado.
  */
  char FiltrarCasillaNivel1(const Sensores &sensores, int pos, bool tiene_zapatillas);
  
 /**
  * @brief Determina si una casilla es óptima para avanzar en el Nivel 1.
  * @param i Carácter de la casilla izquierda.
  * @param c Carácter de la casilla central (enfrente).
  * @param d Carácter de la casilla derecha.
  * @param zap Indica si tiene zapatillas .
  * @param pref_dcha Prioridad de giro (útil para algoritmos de seguimiento de muros).
  * @param sensores Referencia a los sensores para consultar cotas o agentes.
  * @return Un entero que representa la valoración de la casilla o la dirección preferida.
  */
  int VeoCasillaNivel1(char i, char c, char d, bool zap, bool pref_dcha, const Sensores &sensores) ;
  
 /**
  * @brief Consulta el valor de "calor" o frecuencia de visita de una ubicación.
  * @details Útil para agentes con memoria que quieren evitar bucles en zonas ya exploradas.
  * @param ub Estructura con la fila y columna (posF, posC).
  * @return Cantidad de veces que se ha pasado por esa casilla.
  */
  int leerCalor(const ubicacion &ub);
  
 /**
  * @brief Calcula el coste energético de entrar en una casilla.
  * @details Implementa las tablas de costes del enunciado.
  * @param visitas Número de veces visitada (puede penalizar el coste para fomentar exploración).
  * @param terreno Tipo de casilla ('A', 'H', 'S', 'C', etc.).
  * @return El coste total en unidades de energía (long int).
  */
  long int CalcularCosteExploracion(int visitas, char terreno);
  
  // =========================================================================
  // Nivel 2
  // =========================================================================
  
  /**
  * @brief Calcula la ubicación teórica resultante de avanzar desde el estado actual.
  * @note No comprueba si la casilla es transitable, solo calcula la coordenada matemática.
  * @param st Estado actual (posición y orientación).
  * @return Un nuevo objeto EstadoI con la fila y columna resultantes.
  */
  EstadoI NextCasillaIngeniero(const EstadoI &st);
 
 
  /**
  * @brief Filtro de colisiones y accesibilidad.
  * @details Comprueba tres cosas: 
  * 1. Que la casilla no sea un obstáculo insalvable ('M', 'P').
  * 2. Que la diferencia de altura sea válida (máx 1, o máx 2 con zapatillas).
  * 3. Que no nos salgamos de los límites del mapa.
  * @param st Estado al que se pretende llegar.
  * @param terreno Referencia al mapa de terreno.
  * @param altura Referencia al mapa de alturas.
  * @return true si el Ingeniero puede ocupar esa casilla legalmente.
  */
  bool CasillaAccesibleIngeniero(const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  
  /**
  * @brief Generador de sucesores: Aplica una acción a un estado y devuelve el estado resultante.
  * @details Si la acción es girar, cambia la orientación. Si es avanzar, usa NextCasillaIngeniero y valida si es posible mediante CasillaAccesibleIngeniero.
  * @param accion La acción a simular (WALK, TURN_SR, etc).
  * @param st Estado desde el que se parte.
  * @param terreno Mapa de tipos de suelo para validar el movimiento.
  * @param altura Mapa de cotas para validar desniveles.
  * @return El estado resultante tras ejecutar la acción.
  */
  
  EstadoI applyI(Action accion, const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  
  /**
  * @brief Algoritmo de búsqueda de camino con coste acumulado 
  * @details Explora el espacio de estados buscando la secuencia de acciones que minimiza el coste de tiempo/energía para ir de A hasta B. Utiliza applyI para expandir nodos y CasillaAccesibleIngeniero para descartar rutas muertas.
  * @param inicio Estado inicial del agente.
  * @param fin Estado objetivo (coordenadas destino).
  * @param terreno Mapa de terreno para calcular costes (agua, bosque, etc).
  * @param altura Mapa de cotas para validar la viabilidad del camino.
  * @return Una lista de acciones (Action) que el agente debe seguir secuencialmente.
  */
  list<Action> BusquedaCaminoMasCorto(const EstadoI &inicio, const EstadoI &fin, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);


// =========================================================================
// Nivel 3
// =========================================================================

 /**
  * @brief Realiza una búsqueda en anchura (BFS) avanzada para encontrar el camino más corto.
  * @details A diferencia de la versión básica, esta versión considera el estado del agente (objetos, orientación) para determinar la transición entre nodos en el grafo de estados.
  * @param inicio Estado inicial del agente (fila, columna, orientación, equipamiento).
  * @param fin Estado objetivo al que se desea llegar.
  * @param terreno Matriz representativa del mapa con los tipos de superficie.
  * @param altura Matriz con las cotas de elevación de cada casilla del mapa.
  * @return list<Action> Lista secuencial de acciones (FORWARD, TURN_L, etc.) para llegar al destino.
  */
  list <Action> B_Anchura_V2(const EstadoI &inicio, const EstadoI &fin, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);


// =========================================================================
// Nivel 4
// =========================================================================

 /**
  * @brief Implementa el algoritmo A* especializado en el trazado óptimo de tuberías/infraestructura.
  * @details Utiliza una función heurística para minimizar el coste total de instalación, considerando tanto la distancia al objetivo como los costes de alteración del terreno.
  * @param terreno Matriz con los tipos de terreno que afectan al coste de excavación.
  * @param altura Matriz de alturas para calcular pendientes o costes de nivelación.
  * @param f_ini Fila de inicio de la instalación.
  * @param c_ini Columna de inicio de la instalación.
  * @param max_eco Presupuesto máximo o límite económico para la construcción.
  * @return list<Paso> Lista de coordenadas y operaciones que conforman la ruta de la tubería.
  */
  list<Paso> A_Estrella_Tuberias(const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura, int f_ini, int c_ini, int max_eco);

 /**
  * @brief Calcula el coste de modificar una casilla según la operación realizada.
  * @param tipo_terreno Carácter que representa el terreno.
  * @param op Identificador de la operación (ej. 0: nivelar, 1: excavar).
  * @return int Valor entero con el coste económico de la alteración.
  */
  int CosteAlteracionTerreno(char tipo_terreno, int op) ;

 /**
  * @brief Determina el coste fijo de instalación de infraestructura sobre un tipo de terreno.
  * @param tipo_terreno Tipo de suelo sobre el que se va a posicionar el elemento.
  * @return int Coste de materiales y mano de obra según la superficie.
  */
  int CosteInstall(char tipo_terreno) ;
  
 /**
  * @brief Calcula la distancia mínima desde una posición actual a la planta más cercana.
 * @details Se suele utilizar como componente de la función heurística h(n) en el Nivel 4 para guiar la construcción hacia puntos de conexión existentes.
 * @param f Fila de la casilla actual.
 * @param c Columna de la casilla actual.
 * @param plantas Vector de pares (f, c) con las ubicaciones de todas las plantas disponibles.
 * @return int Distancia (usualmente Manhattan o Euclídea) a la planta más próxima.
 */
  int DistanciaU(int f, int c, const vector<pair<int,int>> &plantas);
  
// =========================================================================
// Nivel 5
// =========================================================================

 /**
  * @brief Fase que decide la configuración global de la red de tuberías.
  * @details Analiza el mapa completo para determinar qué plantas conectar y calcula 
  * los tramos principales, invocando a A_Estrella_Tuberias.
  * @param sensores Referencia a los datos actuales del entorno.
  * @return Action Acción de movimiento o espera mientras se procesa el plan.
  */
  Action FaseGenerarRed(Sensores sensores);
  
 /**
  * @brief Gestiona el movimiento del agente hacia el punto de interés.
  * @details Se encarga de la navegación del agente principal hacia una ubicación 
  * estratégica o de suministros antes de iniciar una tarea técnica.
  * @param sensores Estado actual de los sensores del agente.
  * @return Action Siguiente paso en la ruta.
  */
  Action FaseDesplazamientoBelkanita(Sensores sensores);
  
 /**
  * @brief Prepara el terreno en la ubicación actual mediante nivelación física.
  * @details Consulta la operación pendiente (op) del primer tramo en la lista de tuberías. Si el terreno requiere ser elevado (RAISE) o excavado (DIG), ejecuta la acción y marca la operación como completada (op = 0). Una vez nivelado, transiciona a la fase de solicitud del técnico.
  * @param sensores Datos sensoriales (no utilizados directamente en esta lógica, pero mantenidos por firma).
  * @return Action RAISE si op == 1, DIG si op == -1, o IDLE al terminar y cambiar de fase.
  */
  Action FasePrepararBelkanita(Sensores sensores);
  
 /**
  * @brief Gestiona la llamada y coordinación con el técnico.
  * @details Se activa cuando la tarea requiere la intervención del técnico,
   gestionando la espera o señalización.
  * @param sensores Datos actuales del entorno.
  * @return Action Acción para solicitar o esperar al técnico.
  */
  Action FaseSolicitarTecnico(Sensores sensores);
  
 /**
  * @brief Navegación específica hacia un tramo concreto de la tubería.
  * @details A diferencia del desplazamiento general, este se enfoca en el 
  * posicionamiento preciso sobre el segmento de red que se está construyendo.
  * @param sensores Estado actual de la percepción.
  * @return Action Movimiento de aproximación al tramo.
  */
  Action FaseDesplazamientoTramo(Sensores sensores);
  
 /**
  * @brief Fase de espera para asegurar que los agentes están alineados.
  * @details Bloquea acciones de construcción hasta que el agente y el tramo 
  * de red cumplen los requisitos geométricos para la instalación.
  * @param sensores Sensores del agente.
  * @return Action Acción de espera (IDLE) o ajuste de orientación.
  */
  Action FaseEsperarAlineacion(Sensores sensores);
  
 /**
  * @brief Realiza modificaciones en la cota de elevación de la casilla actual.
  * @details Modifica la matriz de alturas para que el terreno sea apto para 
  * la tubería, consumiendo presupuesto según CosteAlteracionTerreno.
  * @param sensores Referencia a los sensores (para verificar altura actual).
  * @return Action Acción de nivelación (frecuentemente una acción especial de interacción).
  */
  Action FaseNivelarTerreno(Sensores sensores);
  
 /**
  * @brief Finaliza el proceso colocando físicamente el segmento de tubería.
  * @details Representa la culminación de la secuencia de construcción en una 
  * casilla tras haber sido desplazado, alineado y nivelado.
  * @param sensores Información de los sensores para confirmar la ubicación.
  * @return Action Acción de colocar infraestructura.
  */
  Action FaseColocarTuberia(Sensores sensores);
  


// =========================================================================
// Nivel 6
// =========================================================================


 };


#endif
