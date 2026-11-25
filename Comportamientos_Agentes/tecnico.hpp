#ifndef COMPORTAMIENTOTECNICO_H
#define COMPORTAMIENTOTECNICO_H

#include <chrono>
#include <time.h>
#include <thread>
#include <list>
#include <cmath>

#include "comportamientos/comportamiento.hpp"

// =========================================================================
// DOCUMENTACIÓN PARA ESTUDIANTES
// =========================================================================
/*
 * CLASE: ComportamientoTecnico
 * 
 * DESCRIPCIÓN:
 * Esta clase implementa el comportamiento del agente Técnico en el mundo Belkan.
 * El técnico colabora con el ingeniero para resolver el problema de instalación de tuberías
 */

   /**
    * @brief Representa el estado del Tecnico en el mapa.
    */
   struct EstadoT{
	ubicacion site;
	bool zapatillas;
	
	// Operador de igualdad
	bool operator==(const EstadoT &st) const{
		return site == st.site and zapatillas == st.zapatillas;
        }
        
        // Obligatorio para poder usar std::map con EstadoT
        bool operator<(const EstadoT &st) const {
	    if (site.f != st.site.f) return site.f < st.site.f;
	    if (site.c != st.site.c) return site.c < st.site.c;
	    if (site.brujula != st.site.brujula) return site.brujula < st.site.brujula;
		return zapatillas < st.zapatillas;
        }
   };
   
  /**
   * @brief Nodo del árbol de búsqueda.
   */
   struct NodoT{
	EstadoT estado;
	list<Action> secuencia;
	int g; // Coste real de energía acumulada hasta aquí
        int h; // Heurística: Energía mínima estimada hasta la meta
        
        // La f es la suma de ambos. A* se guía por esto.
        int f() const { return g + h; }
    
	bool operator==(const NodoT &node) const{
	   return estado == node.estado;
	}
	
	bool operator<(const NodoT &node) const{
	    if (estado.site.f < node.estado.site.f) return true;
	    else if (estado.site.f == node.estado.site.f and estado.site.c < node.estado.site.c) return true;
	    else if (estado.site.f == node.estado.site.f and estado.site.c == node.estado.site.c and estado.site.brujula <
	node.estado.site.brujula) return true;
	    else if (estado.site.f == node.estado.site.f and estado.site.c == node.estado.site.c and estado.site.brujula ==
	node.estado.site.brujula and estado.zapatillas < node.estado.zapatillas) return true;
	    else return false;
	}
};

	// Comparador para que la Priority Queue ordene por menor 'f'
	struct ComparaF_NodoT {
	    bool operator()(const NodoT &a, const NodoT &b) const {
		return a.f() > b.f(); 
	    }
	};





/*************************************************************************************/

class ComportamientoTecnico : public Comportamiento {
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================
  
  /**
   * @brief Constructor para niveles 0, 1 y 6 (sin mapa completo)
   * @param size Tamaño del mapa (si es 0, se inicializa más tarde)
   */
  ComportamientoTecnico(unsigned int size = 0) : Comportamiento(size) {
    last_action = IDLE;
    tiene_zapatillas = false;
    giro45Izq = 0;
    
     fil= -1;
     col= -1;
     brujula = norte;

    // Variables de estado Nivel 0

	 prioridad_derecha = false;
	 
	  for (int i=0; i<100; i++)
            for (int j=0; j<100; j++)
         matriz_visitados[i][j] = 0;
	     
    
  }


/*************************************************************************************/


  /**
   * @brief Constructor para niveles 2, 3, 4 y 5 (con mapa completo conocido)
   * @param mapaR Mapa de terreno conocido
   * @param mapaC Mapa de cotas conocido
   */
  ComportamientoTecnico(std::vector<std::vector<unsigned char>> mapaR, 
                       std::vector<std::vector<unsigned char>> mapaC): 
                       Comportamiento(mapaR, mapaC) {
    // Inicializar Variables de Estado
    
    last_action = IDLE;
    tiene_zapatillas = false;
    
    hayPlan = false;

    
    giro45Izq = 0;
    turnos_esperando = 0;
    
    fase_t = AGUARDAR_LLAMADA;


     f_meta_t = -1;
     c_meta_t = -1;


  }
  
/*************************************************************************************/

  ComportamientoTecnico(const ComportamientoTecnico &comport): Comportamiento(comport) {}
  ~ComportamientoTecnico() {}

  /**
   * @brief Bucle principal de decisión del técnico.
   * Estudia los sensores y decide la siguiente acción.
   * 
   * EJEMPLO DE USO:
   * Action accion = think(sensores);
   * return accion; // El motor ejecutará esta acción
   */
   
   
/*************************************************************************************/


  Action think(Sensores sensores);


/*************************************************************************************/


  ComportamientoTecnico *clone() {
    return new ComportamientoTecnico(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================
  
/**
 * @brief Comportamiento del técnico para el Nivel 0.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_0(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_1(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_2(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_3(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_4(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_5(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_6(Sensores sensores);
  
    /**
 * @brief Comportamiento del técnico para el Nivel E (especial tutorial parte 2).
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_E(Sensores sensores);
  



protected:
  // =========================================================================
  // FUNCIONES PROPORCIONADAS
  // =========================================================================

  /**
   * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
   * IMPORTANTE: Esta función ya está implementada. Actualiza mapaResultado y mapaCotas
   * con la información de los 16 sensores.
   */
  void ActualizarMapa(Sensores sensores);

  /**
   * @brief Determina si una casilla es transitable para el técnico.
   * NOTA: El técnico puede tener reglas de transitabilidad diferentes al ingeniero.
   * @param f Fila de la casilla.
   * @param c Columna de la casilla.
   * @param tieneZapatillas Indica si el agente posee las zapatillas.
   * @return true si la casilla es transitable.
   */
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * REGLA PARA TÉCNICO: Desnivel máximo siempre 1 (independiente de zapatillas).
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const ubicacion &actual);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  ubicacion Delante(const ubicacion &actual) const;

  /**
   * @brief Comprueba si una celda es de tipo transitable por defecto.
   * @param c Carácter que representa el tipo de superficie.
   * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
   */
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

private:
  Action last_action;
  bool tiene_zapatillas;
  int giro45Izq;
  
  static constexpr int COST_INFINITE   = 999999;
  
  int fil;
  int col;
  Orientacion brujula;
  
	// Variables de estado Nivel 0

	bool prioridad_derecha;
  	int matriz_visitados[100][100];
  
  
  //Nivel 2
  bool hayPlan;            // Indica si hay una plan que ejecutar
  list<Action> plan;       // Almacena el plan a realizar.
  
  // Nivel 3
  int turnos_esperando;
  
  // Nivel 5
  // --- VARIABLES NIVEL 5 ---
// 1. El Semáforo del Técnico
enum FaseOperacionTecnico {
    AGUARDAR_LLAMADA,      // Tu antiguo ESPERANDO_JEFE
    TRASLADO_A_POSICION,   // Tu antiguo IR_AL_TAJO
    SINCRONIZAR_NIVELADO,  // Tu antiguo AJUSTAR_TERRENO_T
    EJECUTAR_INSTALACION   // Tu antiguo INSTALAR_T
};

// El técnico empieza su día de trabajo esperando órdenes
FaseOperacionTecnico fase_t;

// Coordenadas donde el ingeniero nos pidió ir
    int f_meta_t;
    int c_meta_t;
  
  
  EstadoT estadoDestino;

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
int VeoCasillaInteresanteT(char i, char c, char d, bool zap, bool pref_dcha, const Sensores &sensores) ;
  
/**
 * @brief Determina si casilla viable por altura
 * @param casilla tipo de terreno
 * @param dif diferencia de altura entre casillas
 * @param zap indica si estoy en posesión de las zapatillas
 * @ 'P' si no es accesible por altura y casilla en otro caso
 */
 
 char ViablePorAlturaT (char casilla, int dif);
   
   
   
 /**
  * @brief Filtra el tipo de casilla en la posición indicada (Nivel 0).
  * @details Prioriza casillas de interés básico como calles, zapatillas o plantas.
  * @param sensores Objeto con la información actual de los sensores.
  * @param pos Índice del cono de visión a consultar.
  * @param zap Booleano que indica si el técnico lleva zapatillas.
  * @return Carácter que representa el tipo de terreno filtrado.
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
  * @brief Consulta el valor del mapa de calor para una ubicación específica.
  * @param ub Estructura con la fila y columna a consultar.
  * @return Número de veces que el técnico ha visitado esa casilla.
  */
  int leerCalor(const ubicacion &ub);

 /**
  * @brief Filtra el tipo de casilla considerando terrenos complejos (Nivel 1).
  * @details Evalúa si terrenos como agua o bosque son transitables según el equipo actual.
  * @param sensores Referencia a los sensores del agente.
  * @param pos Índice del cono de visión.
  * @param tiene_zapatillas Indica si el agente posee el objeto zapatillas.
  * @return Carácter del terreno procesado.
  */
  char FiltrarCasillaNivel1(const Sensores &sensores, int pos, bool tiene_zapatillas);

 /**
  * @brief Decide hacia qué casilla dirigirse basándose en el análisis del Nivel 1.
  * @param i Carácter de la casilla a la izquierda.
  * @param c Carácter de la casilla al centro.
  * @param d Carácter de la casilla a la derecha.
  * @param zap Indica si tiene zapatillas.
  * @param pref_dcha Booleano para desempatar giros (preferencia derecha).
  * @param sensores Referencia a los sensores para consultas adicionales.
  * @return Índice de la dirección elegida (1: Izq, 2: Frente, 3: Dcha).
  */
  int VeoCasillaNivel1(char i, char c, char d, bool zap, bool pref_dcha, const Sensores &sensores);

 /**
  * @brief Calcula el coste de oportunidad de explorar una casilla específica.
  * @details Combina el número de visitas previas con la penalización del tipo de terreno.
  * @param visitas Cantidad de veces que se ha pasado por la casilla.
  * @param terreno Carácter que representa el tipo de suelo (S, T, B, A, etc.).
  * @return Valor numérico que representa el coste (a menor valor, más deseable es la casilla).
  */
  long int CalcularCosteExploracion(int visitas, char terreno);
 
  
  // =========================================================================
  // Nivel 2
  // =========================================================================
  
  
  list <Action> B_Anchura (const EstadoT &inicio, const EstadoT &fin, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);

  list <Action> B_Anchura_V2(const EstadoT &inicio, const EstadoT &fin, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  
 /**
  * @brief Comprueba si un nodo ya existe en una lista de nodos.
  * @details Útil en algoritmos de búsqueda para evitar ciclos y procesar nodos repetidos en la frontera.
  * @param st Nodo que se desea buscar.
  * @param lista Lista de nodos (frontera o explorados) donde buscar.
  * @return true si el nodo se encuentra en la lista, false en caso contrario.
  */
  bool Find(const NodoT &st, const list<NodoT> &lista);

 /**
  * @brief Calcula la siguiente ubicación teórica basada en la posición y orientación actual.
  * @details Determina la fila y columna resultantes de avanzar un paso hacia adelante.
  * @param st Estado actual del técnico (posición y brújula).
  * @return Nuevo objeto EstadoT con las coordenadas actualizadas.
  */
  EstadoT NextCasillaTécnico(const EstadoT &st);

 /**
  * @brief Verifica si una casilla es físicamente transitable para el Técnico.
  * @details Evalúa límites del mapa, obstáculos (Muros, Precipicios) y restricciones de altura.
  * @param st Estado que representa la casilla a evaluar.
  * @param terreno Matriz del tipo de terreno detectado.
  * @param altura Matriz de cotas para verificar desniveles.
  * @return true si la casilla es transitable, false si es un obstáculo o inaccesible.
  */
  bool CasillaAccesibleTécnico(const EstadoT &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);

 /**
  * @brief Genera el estado resultante tras aplicar una acción específica.
  * @details Simula el efecto de WALK, TURN_SR, etc., verificando si la acción es legal en el entorno.
  * @param accion La acción a aplicar (giro o avance).
  * @param st Estado inicial desde el que se parte.
  * @param terreno Información del mapa de superficie.
  * @param altura Información del mapa de alturas.
  * @return El nuevo estado resultante tras la transición.
  */
  EstadoT applyT(Action accion, const EstadoT &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);

  
  // =========================================================================
  // Nivel 3
  // =========================================================================
  
 /**
  * @brief Calcula el coste de realizar una acción WALK entre dos estados.
  * @details Evalúa el esfuerzo según el tipo de terreno (hierba, agua, bosque) y el equipo actual.
  * @param actual Estado de origen.
  * @param siguiente Estado de destino tras avanzar.
  * @param terreno Matriz de tipos de superficie.
  * @param altura Matriz de cotas para considerar penalizaciones por desnivel.
  * @return Valor entero que representa el coste de energía o tiempo.
  */
  int Coste_Walk(const EstadoT &actual, const EstadoT &siguiente, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);

 /**
  * @brief Calcula el coste de realizar una acción de giro (TURN_SR o TURN_SL).
  * @details El coste de rotar puede variar dependiendo de si el terreno es resbaladizo o difícil.
  * @param actual Estado actual del técnico.
  * @param terreno Matriz de tipos de superficie.
  * @return Valor entero del coste de rotación.
  */
  int Coste_Turn(const EstadoT &actual, const vector<vector<unsigned char>> &terreno);

 /**
  * @brief Implementación del algoritmo de búsqueda A* para el Técnico.
  * @details Encuentra la secuencia de acciones óptima hacia el objetivo usando costes reales y heurística.
  * @param inicio Posición y orientación inicial.
  * @param fin Posición de destino.
  * @param terreno Mapa de superficies.
  * @param altura Mapa de elevaciones.
  * @return Lista de acciones (WALK, TURN, etc.) para llegar al destino.
  */
  list<Action> A_Estrella_Nivel3(const EstadoT &inicio, const EstadoT &fin, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);

 /**
  * @brief Calcula la estimación del coste restante (heurística) hasta el objetivo.
  * @param actual Estado evaluado.
  * @param fin Estado meta.
  * @return Valor estimado del coste restante.
  */
  int CalcularHeuristica(const EstadoT &actual, const EstadoT &fin);
  
  // =========================================================================
  // Nivel 4
  // =========================================================================
  
  
  // =========================================================================
  // Nivel 5
  // =========================================================================
  
  
 /**
  * @brief Estado inicial: El técnico espera parado a recibir una orden del ingeniero.
  * @param sensores Información actual del entorno.
  * @return Action IDLE mientras no reciba un mensaje.
  */
  Action FaseAguardar(Sensores sensores);

 /**
  * @brief El técnico se desplaza siguiendo el plan generado por el A*.
  * @param sensores Información actual del entorno.
  * @return La siguiente acción de movimiento de la lista 'plan'.
  */
  Action FaseTraslado(Sensores sensores);

 /**
  * @brief El técnico ajusta su orientación para quedar frente al ingeniero o la casilla de obra.
  * @param sensores Información actual del entorno.
  * @return Action de giro o IDLE si ya está alineado.
  */
  Action FaseSincronizar(Sensores sensores);

 /**
  * @brief El técnico colabora en la acción final de instalación.
  * @details En esta fase, el técnico simplemente espera o confirma la acción del ingeniero.
  * @param sensores Información actual del entorno.
  * @return Action resultante de la colaboración.
  */
  Action FaseEjecutar(Sensores sensores);
  

  // =========================================================================
  // Nivel 6
  // =========================================================================

  
};

#endif
