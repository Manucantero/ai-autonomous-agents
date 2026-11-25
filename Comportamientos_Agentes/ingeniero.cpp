#include "ingeniero.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoIngeniero::think(Sensores sensores)
{
  Action accion = IDLE;

  // Decisión del agente según el nivel
  switch (sensores.nivel)
  {
  case 0:
    accion = ComportamientoIngenieroNivel_0(sensores);
    break;
  case 1:
    accion = ComportamientoIngenieroNivel_1(sensores);
    break;
  case 2:
    accion = ComportamientoIngenieroNivel_2(sensores);
    break;
  case 3:
    accion = ComportamientoIngenieroNivel_3(sensores);
    break;
  case 4:
    accion = ComportamientoIngenieroNivel_4(sensores);
    break;
  case 5:
    accion = ComportamientoIngenieroNivel_5(sensores);
    break;
  case 6:
    accion = ComportamientoIngenieroNivel_6(sensores);
    break;
  }

  return accion;
}


// =========================================================================
// Nivel 0
// =========================================================================
  
/**
 * Decide hacia dónde mirar o moverse en el Nivel 0.
 * Prioriza la Meta, luego las Zapatillas y por último el mapa de calor.
 */
int ComportamientoIngeniero::VeoCasillaInteresanteI(char i, char c, char d, bool zap, bool pref_dcha, const Sensores &sensores) {
// --- PRIORIDAD 1: LA META ('U') ---
    // Si la meta está a la vista, ignoramos el resto de estímulos.
    if (c == 'U') return 2; 
    
    // Si la meta está en los laterales, consultamos el sesgo (pref_dcha) para decidir el orden de comprobación y evitar comportamientos deterministas en esquinas.
    if (pref_dcha) { 
        if (d == 'U') return 3; 
        if (i == 'U') return 1; 
    } else { 
        if (i == 'U') return 1; 
        if (d == 'U') return 3; 
    }

    // --- PRIORIDAD 2: EQUIPAMIENTO ('D') ---
    // Si no tenemos zapatillas, el objetivo secundario es recolectarlas para mejorar el coste de movimiento.
    if (!zap) {
        if (c == 'D') return 2;
        if (pref_dcha) { if (d == 'D') return 3; if (i == 'D') return 1; }
        else           { if (i == 'D') return 1; if (d == 'D') return 3; }
    }

    // --- CÁLCULO DE COORDENADAS PARA EL MAPA DE CALOR ---
    // Para no dar vueltas en el mismo sitio, necesitamos saber qué hay en el mapa global.

    ubicacion actual = {sensores.posF, sensores.posC, sensores.rumbo};
    ubicacion frente = Delante(actual);
    
    // Calculamos la posición a la izquierda rotando el rumbo -45º ( +7 en el enum circular ).
    ubicacion aux_izq = actual;
    aux_izq.brujula = (Orientacion)((actual.brujula + 7) % 8);
    ubicacion frente_izq = Delante(aux_izq);

    // Calculamos la posición a la derecha rotando el rumbo +45º ( +1 en el enum circular ).
    ubicacion aux_dcha = actual;
    aux_dcha.brujula = (Orientacion)((actual.brujula + 1) % 8);
    ubicacion frente_dcha = Delante(aux_dcha);

    // --- PRIORIDAD 3: EXPLORACIÓN (EVITAR EL CALOR) ---
    // Obtenemos el nivel de "visitas" de las tres casillas adyacentes.
    // Si una casilla es un muro o precipicio ('P'), le asignamos un coste prohibitivo.
    int calor_i = (i != 'P') ? leerCalor(frente_izq) : COST_INFINITE;
    int calor_c = (c != 'P') ? leerCalor(frente) : COST_INFINITE;
    int calor_d = (d != 'P') ? leerCalor(frente_dcha) : COST_INFINITE;

    // Si estamos en un callejón sin salida (tres muros delante), forzamos un giro.
    if (i == 'P' && c == 'P' && d == 'P') return 0;

    // --- DECISIÓN FINAL: BÚSQUEDA DEL MÍNIMO ---
    // Aplicamos una política de "ir hacia lo frío". Si el centro es igual o mejor que los lados, mantenemos la inercia y seguimos recto para evitar zigzagueos innecesarios.
    if (calor_c <= calor_i && calor_c <= calor_d) return 2;
    
    // Si los lados son más atractivos que el frente, usamos pref_dcha para desempatar.
    if (pref_dcha) {
        return (calor_d <= calor_i) ? 3 : 1;
    } else {
        return (calor_i <= calor_d) ? 1 : 3;
    }
}


/*************************************************************************************/


/**
 * Comprueba si el salto de altura es físicamente posible.
 */
char ComportamientoIngeniero::ViablePorAlturaI(char casilla, int dif, bool zap) { 
    // Si el desnivel es de 1, paso siempre. Si es de 2, solo si tengo zapatillas.
    if (abs(dif) <= 1 || (zap && abs(dif) <= 2)) {
        return casilla; 
    } else {
        // Si es demasiado alto o profundo, lo trato como un muro ('P').
        return 'P'; 
    }
}

/*************************************************************************************/

/**
 * Filtro para el Nivel 0: Solo me interesan caminos, zapatillas y metas.
 */
char ComportamientoIngeniero::FiltrarCasillaNivel0(Sensores sensores, int pos, bool zap) {
    // Si hay otro agente, es como si hubiera una pared.
    if (sensores.agentes[pos] != '_') return 'P';

    char terreno = sensores.superficie[pos];
    
    // Solo piso lo que conozco y es seguro en Nivel 0.
    if (terreno != 'C' && terreno != 'D' && terreno != 'U') return 'P';

    // Finalmente chequeo si me voy a matar por la altura.
    return ViablePorAlturaI(terreno, sensores.cota[pos] - sensores.cota[0], zap);
}


/*************************************************************************************/

/**
 * Lee las visitas de la matriz de forma segura.
 */
int ComportamientoIngeniero::leerCalor(const ubicacion &ub) {
    // Control de seguridad: coordenada está dentro del mapa 100x100.
    if (ub.f >= 0 && ub.f < 100 && ub.c >= 0 && ub.c < 100) {
        return matriz_visitados[ub.f][ub.c];
    }
    // Si por algún error miro fuera del mapa, devuelvo infinito para no ir allí.
    return COST_INFINITE; 
}


/*************************************************************************************/




// =========================================================================
// Nivel 1
// =========================================================================


/**
 * Asigna un valor de prioridad (deseo) a cada tipo de terreno.
 * Se utiliza para calcular qué tan atractiva es una casilla antes de considerar las visitas.
 */
int ComportamientoIngeniero::PuntuacionCasilla(unsigned char casilla) {
    switch (casilla) {
        case '?': return 50000; // Máxima prioridad: incentiva al agente a descubrir lo desconocido.
        
        case 'C': return 30000; // Prioridad alta: el asfalto es la superficie óptima de desplazamiento.
        case 'X': 
        case 'U': return 29000; // Prioridad alta: casillas de recarga y metas son objetivos clave.
        case 'S': return 20000; // Prioridad media: los senderos son aceptables pero secundarios a la calle.
        case 'H': return 5000;  // Prioridad baja: la hierba penaliza el movimiento, se evita si hay alternativas.
        case 'A': return -20000; // Penalización: el agua es costosa y peligrosa para la batería.
        
        // Prioridad dinámica: si no hay zapatillas, son el objetivo principal (40000), si ya las tiene, son neutras.
        case 'D': return tiene_zapatillas ? 0 : 40000; 
        
        default:  return -50000; // Penalización máxima: muros y precipicios son infranqueables.
    }
}


/*************************************************************************************/


/**
 * Determina si una casilla es transitable en el Nivel 1.
 * Filtra por presencia de otros agentes, tipo de terreno y capacidades físicas (altura).
 */
char ComportamientoIngeniero::FiltrarCasillaNivel1(const Sensores &sensores, int pos, bool tiene_zapatillas) {
    // Bloqueo por colisión: si la casilla está ocupada por otro agente, se marca como pared.
    if (sensores.agentes[pos] != '_') return 'P';
    
    char terreno = sensores.superficie[pos];
    int dif_cota = sensores.cota[pos] - sensores.cota[0];

    // Restricción de terreno: muros, precipicios y bosques se consideran obstáculos totales.
    if (terreno == 'M' || terreno == 'P' || terreno == 'B') return 'P';

    // Validación de salto: el límite de altura es 2 con equipamiento (zapatillas) y 1 sin él.
    int limite_altura = tiene_zapatillas ? 2 : 1;
    if (abs(dif_cota) > limite_altura) return 'P';

    return terreno; 
}


/*************************************************************************************/

 /**
  * Evalúa las opciones de movimiento (Izquierda, Frente, Derecha) en el Nivel 1.
  * Combina la búsqueda de objetos, el mapa de calor y el coste del terreno.
  */
  int ComportamientoIngeniero::VeoCasillaNivel1(char i, char c, char d, bool zap, bool pref_dcha, const Sensores &sensores) {
    // Lógica de recolección: si se detectan zapatillas y no se poseen, se prioriza esa dirección.
    if (!zap) {
        if (c == 'D') return 2;
        if (pref_dcha) { if (d == 'D') return 3; if (i == 'D') return 1; }
        else           { if (i == 'D') return 1; if (d == 'D') return 3; }
    }

    // Proyección de coordenadas: calculamos la posición global de las 3 casillas frente al agente.
    ubicacion actual = {sensores.posF, sensores.posC, sensores.rumbo};
    ubicacion frente = Delante(actual);
    
    // Rotaciones de brújula (+7 es -45º, +1 es +45º) para obtener las casillas laterales.
    ubicacion aux_izq = actual; aux_izq.brujula = (Orientacion)((actual.brujula + 7) % 8);
    ubicacion frente_izq = Delante(aux_izq);
    
    ubicacion aux_dcha = actual; aux_dcha.brujula = (Orientacion)((actual.brujula + 1) % 8);
    ubicacion frente_dcha = Delante(aux_dcha);

    // Escape: si las tres opciones frontales son muros ('P'), se fuerza un giro (retorno 0).
    if (i == 'P' && c == 'P' && d == 'P') return 0;

    // Consulta de memoria: obtenemos las visitas de cada casilla viable para evitar bucles.
    int v_i = (i != 'P') ? leerCalor(frente_izq) : COST_INFINITE;
    int v_c = (c != 'P') ? leerCalor(frente) : COST_INFINITE;
    int v_d = (d != 'P') ? leerCalor(frente_dcha) : COST_INFINITE;

    // Cálculo de coste final: integra el tipo de suelo con la frecuencia de paso.
    long int coste_i = CalcularCosteExploracion(v_i, i);
    long int coste_c = CalcularCosteExploracion(v_c, c);
    long int coste_d = CalcularCosteExploracion(v_d, d);

    // Selección de mínima resistencia: se elige la dirección con menor coste acumulado.
    if (coste_c <= coste_i && coste_c <= coste_d) return 2; 
    
    // Desempate: en caso de costes laterales similares, se usa la preferencia de giro.
    if (pref_dcha) {
        return (coste_d <= coste_i) ? 3 : 1;
    } else {
        return (coste_i <= coste_d) ? 1 : 3;
    }
}

/*************************************************************************************/


 /**
  * Calcula el peso logístico de una casilla. 
  * Aplica un sistema de recompensas para zonas nuevas y penalizaciones por re-visita.
  */
  long int ComportamientoIngeniero::CalcularCosteExploracion(int visitas, char terreno) {
// Si la casilla es inaccesible o está fuera de límites, el coste es infinito.
    if (terreno == 'P' || visitas >= COST_INFINITE) return COST_INFINITE; 

    int puntos = PuntuacionCasilla(terreno);
    
    // Política de exploración: las casillas nunca visitadas tienen coste negativo (actúan como imanes).
    if (visitas == 0) {
        return -puntos - 5000; 
    } else {
        // Política de fatiga: cada visita añade 4000 de coste para forzar al agente a buscar rutas alternativas.
        return (visitas * 4000) - puntos; 
    }
  }


// =========================================================================
// Nivel 2
// =========================================================================

 /**
  * Valida si el agente puede desplazarse físicamente a la siguiente casilla.
  * Evalúa las restricciones de terreno (obstáculos) y las limitaciones de salto por altura.
  */
  bool ComportamientoIngeniero::CasillaAccesibleIngeniero(const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {

// Calculamos primero a qué coordenadas llegaría el agente según su orientación.
    EstadoI next = NextCasillaIngeniero(st);
    bool check1 = false, check2 = false, check3 = false;
    
    // Validación de obstáculos: Se prohíbe el paso por Muros ('M') y Precipicios ('P').
    check1 = terreno[next.site.f][next.site.c] != 'P' and terreno[next.site.f][next.site.c] != 'M';
    
    // Restricción adicional: El ingeniero no puede atravesar el Bosque ('B').
    check2 = terreno[next.site.f][next.site.c] != 'B';
    
    // Regla de física por altura: Si el agente calza zapatillas, el límite de salto es 2, de lo contrario, el desnivel máximo permitido es de 1 unidad.
    int max_desnivel = st.zapatillas ? 2 : 1; 
    
    // Verificamos el valor absoluto de la diferencia de altura entre la casilla actual y la de destino.
    check3 = abs(altura[next.site.f][next.site.c] - altura[st.site.f][st.site.c]) <= max_desnivel;
    
    // El movimiento solo es posible si se cumplen todas las condiciones de terreno y altura.
    return check1 && check2 && check3;
}

/*************************************************************************************/


  EstadoI ComportamientoIngeniero::NextCasillaIngeniero(const EstadoI &st) {
    EstadoI siguiente = st;
    
    // Modificamos fila (f) y columna (c) basándonos en la orientación actual (brujula).
   switch (st.site.brujula)
   {
   case norte:
	siguiente.site.f = st.site.f - 1;
	break;
   case noreste:
	siguiente.site.f = st.site.f - 1;
	siguiente.site.c = st.site.c + 1;
	break;
   case este:
	siguiente.site.c = st.site.c + 1;
	break;
   case sureste:
	siguiente.site.f = st.site.f + 1;
	siguiente.site.c = st.site.c + 1;
	break;
   case sur:
	siguiente.site.f = st.site.f + 1;
	break;
   case suroeste:
	siguiente.site.f = st.site.f + 1;
	siguiente.site.c = st.site.c - 1;
	break;
   case oeste:
	siguiente.site.c = st.site.c - 1;
	break;
   case noroeste:
	siguiente.site.f = st.site.f - 1;
	siguiente.site.c = st.site.c - 1;
   }
   
   return siguiente;
}


/*************************************************************************************/

 /**
  * Aplica una acción a un estado determinado y devuelve el estado resultante.
  * Es la función de transición del modelo del mundo para el ingeniero.
  */
  EstadoI ComportamientoIngeniero::applyI(Action accion, const EstadoI &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {
    EstadoI next = st;
    switch(accion) {
        case WALK:
            // Intentamos avanzar una casilla. Solo se actualiza la posición si la casilla de enfrente es accesible según las reglas de terreno y altura.
            if (CasillaAccesibleIngeniero(st, terreno, altura)) {
                next = NextCasillaIngeniero(st);
            }
            break;

        case JUMP: {
            // El salto es un movimiento doble en la dirección actual.
            EstadoI intermedio = NextCasillaIngeniero(st);
            EstadoI destino = NextCasillaIngeniero(intermedio);

            // Comprobamos que el destino esté dentro de los límites del mapa.
            if (destino.site.f >= 0 && destino.site.f < terreno.size() &&
                destino.site.c >= 0 && destino.site.c < terreno[0].size()) {

                // Validación de la casilla sobre la que saltamos:
                // El ingeniero no puede sobrevolar Muros, Precipicios ni Bosques.
                char terr_inter = terreno[intermedio.site.f][intermedio.site.c];
                bool intermedia_ok = (terr_inter != 'P' && terr_inter != 'M' && terr_inter != 'B');

                // Validación de la casilla de aterrizaje:
                // No se puede aterrizar en Muro, Precipicio ni Bosque.
                char terr_dest = terreno[destino.site.f][destino.site.c];
                bool destino_ok = (terr_dest != 'P' && terr_dest != 'M' && terr_dest != 'B');

                // Validación física por altura:
                // Se calcula el desnivel entre el suelo de origen y el de destino final.
                int max_desnivel = st.zapatillas ? 2 : 1;
                int desnivel_salto = abs((int)altura[destino.site.f][destino.site.c] - (int)altura[st.site.f][st.site.c]);
                bool altura_ok = (desnivel_salto <= max_desnivel);

                // El salto solo se ejecuta si la trayectoria es limpia, el destino es seguro y la altura viable.
                if (intermedia_ok && destino_ok && altura_ok) {
                    next = destino;
                }
            }
            break;
        }
        
        case TURN_SR:
            // Giro suave a la derecha
            next.site.brujula = (Orientacion) ((next.site.brujula + 1) % 8);
            break;
            
        case TURN_SL:
            // Giro suave a la izquierda
            next.site.brujula = (Orientacion) ((next.site.brujula + 7) % 8);
            break;
            
        default: break;
    }
    return next;
}


/*************************************************************************************/
  list<Action> ComportamientoIngeniero::BusquedaCaminoMasCorto(const EstadoI &inicio, const EstadoI &fin, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {

    // Cola de prioridad para expandir siempre el nodo con menor coste acumulado.
    priority_queue<NodoI, vector<NodoI>, ComparaCosteNodoI> frontier;
    map<EstadoI, int> explored; 
    
    // Configuración del nodo inicial.
    NodoI start;
    start.estado = inicio;
    start.coste = 0;
	
     // Insertamos el punto de partida en la frontera y lo marcamos como visitado con coste 0.
    frontier.push(start);
    explored[inicio] = 0;

    while (!frontier.empty()) {

        NodoI current = frontier.top();
        frontier.pop();

        // Condición de victoria: si la posición actual coincide con el destino, devolvemos la secuencia de acciones.
        if (current.estado.site.f == fin.site.f &&
            current.estado.site.c == fin.site.c) {
            return current.secuencia;
        }

        // Si ya hemos encontrado un camino más corto hacia este mismo estado anteriormente, ignoramos este nodo.
        if (explored[current.estado] < current.coste)
            continue;

        // ================= WALK =================
        NodoI child = current;
        child.estado = applyI(WALK, current.estado, terreno, altura);
 
       // Si el estado ha cambiado (el movimiento ha sido posible y no hemos chocado).
        if (!(child.estado == current.estado)) {

        // Lógica de equipamiento: si pisamos una casilla con zapatillas, el estado del agente se actualiza.
            if (terreno[child.estado.site.f][child.estado.site.c] == 'D')
                child.estado.zapatillas = true;
            else
                child.estado.zapatillas = current.estado.zapatillas;

            child.coste = current.coste + 1;
            child.secuencia.push_back(WALK);

           // Si el estado es nuevo o hemos llegado a él por un camino más económico, lo añadimos a la frontera.
            if (!explored.count(child.estado) || explored[child.estado] > child.coste) {
                explored[child.estado] = child.coste;
                frontier.push(child);
            }
        }

        // ================= JUMP =================
        child = current;
        child.estado = applyI(JUMP, current.estado, terreno, altura);

        if (!(child.estado == current.estado)) {

         // Actualización de estado de zapatillas tras el aterrizaje.
            if (terreno[child.estado.site.f][child.estado.site.c] == 'D')
                child.estado.zapatillas = true;
            else
                child.estado.zapatillas = current.estado.zapatillas;

            child.coste = current.coste + 1;
            child.secuencia.push_back(JUMP);

            if (!explored.count(child.estado) || explored[child.estado] > child.coste) {
                explored[child.estado] = child.coste;
                frontier.push(child);
            }
        }

        // ================= TURN_SR =================
        child = current;
        child.estado = applyI(TURN_SR, current.estado, terreno, altura);
        child.estado.zapatillas = current.estado.zapatillas;// Mantener estado de objetos al girar.
        child.coste = current.coste + 1;
        child.secuencia.push_back(TURN_SR);

         // Los giros siempre son posibles, así que solo validamos si el nuevo estado es óptimo.
        if (!explored.count(child.estado) || explored[child.estado] > child.coste) {
            explored[child.estado] = child.coste;
            frontier.push(child);
        }

        // ================= TURN_SL =================
        child = current;
        child.estado = applyI(TURN_SL, current.estado, terreno, altura);
        child.estado.zapatillas = current.estado.zapatillas;
        child.coste = current.coste + 1;
        child.secuencia.push_back(TURN_SL);

        if (!explored.count(child.estado) || explored[child.estado] > child.coste) {
            explored[child.estado] = child.coste;
            frontier.push(child);
        }
    }
    
    // Si la frontera se vacía sin alcanzar el destino, devolvemos una lista vacía.
    return list<Action>();
}



// =========================================================================
// Nivel 3
// =========================================================================

 /**
  * Implementación de Búsqueda en Anchura (BFS) avanzada para el Ingeniero.
  * Explora el espacio de estados por niveles, garantizando el camino mínimo en número de acciones.
  */
  list<Action> ComportamientoIngeniero::B_Anchura_V2(const EstadoI &inicio, const EstadoI &fin, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){
    
    NodoI current_node;
    list<NodoI> frontier; // Cola de nodos pendientes (FIFO)
    set<NodoI> explored;  // Conjunto de nodos ya visitados para evitar ciclos
    list<Action> path;    // Lista final de acciones
    
    // Inicialización del nodo raíz con el estado inicial
    current_node.estado = inicio;
    frontier.push_back(current_node);

    // Comprobación inmediata de si el inicio es ya el destino
    bool SolutionFound = (current_node.estado.site.f == fin.site.f and 
                          current_node.estado.site.c == fin.site.c);
                        
    while (!SolutionFound and !frontier.empty()) {
        // Extraemos el primer nodo de la lista de pendientes y lo registramos como explorado
        frontier.pop_front();
        explored.insert(current_node);
        
        // --- GESTIÓN DE OBJETOS ---
        // Si el estado actual está sobre una casilla de zapatillas, actualizamos el equipamiento del ingeniero
        if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D') {
            current_node.estado.zapatillas = true;
        }
        
        // --- EXPANSIÓN DEL HIJO: WALK ---
        NodoI child_Walk = current_node;
        child_Walk.estado = applyI(WALK, current_node.estado, terreno, altura);

        // Verificamos si avanzar nos lleva a la solución
        if (child_Walk.estado.site.f == fin.site.f and 
            child_Walk.estado.site.c == fin.site.c) {
            child_Walk.secuencia.push_back(WALK);
            current_node = child_Walk;
            SolutionFound = true; 
        }
        // Si no es solución y no ha sido explorado, se añade a la cola para futuras expansiones
        else if (explored.find(child_Walk) == explored.end()) {
            child_Walk.secuencia.push_back(WALK);
            frontier.push_back(child_Walk);
        }
    
        // Si aún no hay solución, expandimos los giros
        if (!SolutionFound){
            // --- EXPANSIÓN DEL HIJO: TURN_SR  ---
            NodoI child_TurnSR = current_node;
            child_TurnSR.estado = applyI(TURN_SR, current_node.estado, terreno, altura);
            
            if (explored.find(child_TurnSR) == explored.end()) {
                child_TurnSR.secuencia.push_back(TURN_SR);
                frontier.push_back(child_TurnSR);
            }
            
            // --- EXPANSIÓN DEL HIJO: TURN_SL  ---
            NodoI child_TurnSL = current_node;
            child_TurnSL.estado = applyI(TURN_SL, current_node.estado, terreno, altura);
            
            if (explored.find(child_TurnSL) == explored.end()) {
                child_TurnSL.secuencia.push_back(TURN_SL);
                frontier.push_back(child_TurnSL);
            }
        }
    
        // --- SELECCIÓN DEL SIGUIENTE NODO ---
        // Obtenemos el próximo nodo de la frontera, saltando aquellos que ya hayamos explorado
        if (!SolutionFound and !frontier.empty()) {
            current_node = frontier.front();
            while(explored.find(current_node) != explored.end() and !frontier.empty()){
                frontier.pop_front();
                if (!frontier.empty()){
                    current_node = frontier.front();
                }
            }
        }
    }
    
    // Si salimos del bucle con éxito, extraemos la secuencia de acciones acumulada
    if (SolutionFound)
        path = current_node.secuencia;
        
    return path;
}



// =========================================================================
// Nivel 4
// ========================================================================

 /**
  * Implementación de A* para el trazado de tuberías.
  * Busca el camino de menor impacto económico para conectar el inicio con una planta 'U'.
  */
  list<Paso> ComportamientoIngeniero::A_Estrella_Tuberias(const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &mapaCotas, int f_ini, int c_ini, int max_eco) {
    
    // 0. LOCALIZACIÓN DE OBJETIVOS
    // Escaneamos el mapa para encontrar todas las plantas 'U' y usarlas en la heurística.
    vector<pair<int,int>> plantas;
    for(int i=0; i<terreno.size(); ++i) {
        for(int j=0; j<terreno[0].size(); ++j) {
            if(terreno[i][j] == 'U') plantas.push_back({i,j});
        }
    }

    // Estructuras de control de A*: prioridad por (g + h) y registro de impacto mínimo por estado.
    priority_queue<NodoTuberia> frontier;
    map<pair<pair<int,int>, int>, int> explored_impact;

    // 1. GENERACIÓN DEL NODO RAÍZ (Punto de inicio)
    // Probamos las 3 posibilidades en la casilla inicial: bajar terreno (-1), dejarlo igual (0) o subirlo (1).
    for (int op : {-1, 0, 1}) {
        // Restricciones físicas: no se puede cavar bajo el nivel 1, subir sobre el 9, ni tocar el agua ('A').
        if (op == 1 && (mapaCotas[f_ini][c_ini] >= 9 || terreno[f_ini][c_ini] == 'A')) continue;
        if (op == -1 && (mapaCotas[f_ini][c_ini] <= 1 || terreno[f_ini][c_ini] == 'A')) continue;

        NodoTuberia inicial;
        inicial.f = f_ini; inicial.c = c_ini;
        inicial.altura_efectiva = (int)mapaCotas[f_ini][c_ini] + op;
        inicial.g = 0; 
        
        // El coste inicial solo incluye la alteración del terreno (DIG/RAISE).
        inicial.impacto_acumulado = CosteAlteracionTerreno(terreno[f_ini][c_ini], op);
        inicial.secuencia.push_back({f_ini, c_ini, op});
        inicial.h = DistanciaU(f_ini, c_ini, plantas); // Heurística hasta la 'U' más cercana.
        
        if (inicial.impacto_acumulado <= max_eco) {
            frontier.push(inicial);
        }
    }

    // 2. BÚCLE PRINCIPAL DE EXPLORACIÓN
    while (!frontier.empty()) {
        NodoTuberia current = frontier.top();
        frontier.pop();

        // COMPROBACIÓN DE OBJETIVO: ¿Hemos llegado a U?
        if (terreno[current.f][current.c] == 'U') {
            return current.secuencia;
        }

        // CONTROL DE EXPLORADOS (PODA): 
        // El estado incluye (fila, columna, altura_efectiva). Si ya pasamos por aquí 
        // con un coste menor, esta rama no es óptima y la descartamos.
        pair<pair<int,int>, int> estado = {{current.f, current.c}, current.altura_efectiva};
        if (explored_impact.count(estado) && explored_impact[estado] <= current.impacto_acumulado) {
            continue;
        }
        explored_impact[estado] = current.impacto_acumulado;

        // 3. EXPANSIÓN A CASILLAS ADYACENTES (N, E, S, O)
        int df[] = {-1, 0, 1, 0}, dc[] = {0, 1, 0, -1};
        for (int i = 0; i < 4; i++) {
            int nf = current.f + df[i], nc = current.c + dc[i];
            
            // Verificamos límites del mapa y terrenos intransitables.
            if (nf < 0 || nf >= terreno.size() || nc < 0 || nc >= terreno[0].size()) continue;
            if (terreno[nf][nc] == 'M' || terreno[nf][nc] == 'P' || terreno[nf][nc] == 'B') continue;

            // Para cada casilla vecina, probamos las 3 operaciones de nivelación.
            for (int nop : {-1, 0, 1}) {
                if (nop == 1 && (mapaCotas[nf][nc] >= 9 || terreno[nf][nc] == 'A')) continue;
                if (nop == -1 && (mapaCotas[nf][nc] <= 1 || terreno[nf][nc] == 'A')) continue;

                int n_alt_efectiva = (int)mapaCotas[nf][nc] + nop;
                int dif = current.altura_efectiva - n_alt_efectiva;

                // REGLA DE GRAVEDAD: 
                // El agua solo fluye si la casilla actual es IGUAL o MÁS ALTA (+1) que la siguiente.
                if (dif >= 0 && dif <= 1) {
                    
                    // CÁLCULO ECONÓMICO DEL TRAMO:
                    // impacto anterior + coste instalación (ambas casillas) + coste nivelación nueva casilla.
                    int costo_tramo_install = CosteInstall(terreno[current.f][current.c]) + CosteInstall(terreno[nf][nc]);
                    int costo_alteracion_hijo = CosteAlteracionTerreno(terreno[nf][nc], nop);
                    int n_imp = current.impacto_acumulado + costo_tramo_install + costo_alteracion_hijo;
                    
                    // Si el presupuesto lo permite, generamos el nuevo nodo.
                    if (n_imp <= max_eco) {
                        NodoTuberia hijo;
                        hijo.f = nf; hijo.c = nc;
                        hijo.altura_efectiva = n_alt_efectiva;
                        hijo.g = current.g + 1; 
                        hijo.impacto_acumulado = n_imp;
                        hijo.secuencia = current.secuencia;
                        hijo.secuencia.push_back({nf, nc, nop});
                        hijo.h = DistanciaU(nf, nc, plantas);
                        
                        frontier.push(hijo);
                    }
                }
            }
        }
    }
    return {}; // No se encontró ningún camino viable dentro del presupuesto.
  }
/*************************************************************************************/

 /**
  * Calcula el coste económico de modificar la elevación de una casilla.
  * El coste varía según si se excava (DIG) o se rellena (RAISE) y el tipo de suelo.
  */
  int ComportamientoIngeniero::CosteAlteracionTerreno(char tipo_terreno, int op) {
    // Si la operación es 0, no hay modificación física y el coste es nulo.
    if (op == 0) return 0; 
    
    // Escenario de elevación del terreno (RAISE).
    if (op == 1) { 
        switch(tipo_terreno) {
            case 'A': return 1000000; // El agua se considera inviable para elevar 
            case 'H': return 55;      // La hierba es costosa de rellenar.
            case 'S': return 30;      // El sendero es moderado.
            case 'C': case 'U': return 10; // La calle o planta son los más económicos de nivelar.
            default:  return 40;      // Coste estándar para otros terrenos transitables.
        }
    } 
    // Escenario de excavación del terreno (DIG).
    else if (op == -1) { 
        switch(tipo_terreno) {
            case 'A': return 1000000; // El agua no se puede excavar
            case 'H': return 65;      // Excavar en hierba es la operación más cara de este bloque.
            case 'S': return 40;      // Coste intermedio para senderos.
            case 'C': case 'U': return 25; // Excavar bajo asfalto es relativamente barato.
            default:  return 50;      // Coste estándar de excavación.
        }
    }
    return 0;
  }
  

 /**
  * Determina el coste fijo de instalación de la infraestructura sobre el terreno.
  * Representa el coste de materiales y mano de obra por cada unidad de superficie.
  */
  int ComportamientoIngeniero::CosteInstall(char tipo_terreno) {
    switch(tipo_terreno) {
        case 'A': return 50;           // Instalar sobre/bajo agua es lo más costoso en materiales.
        case 'H': return 45;           // La hierba requiere preparación adicional.
        case 'S': return 25;           // Coste moderado en senderos.
        case 'C': case 'U': return 15; // Son las zonas más sencillas de instalar.
        default:  return 30;           // Coste base para el resto de superficies.
    }
  }
/*************************************************************************************/

 /**
  * Función heurística para el algoritmo A*.
  * Calcula la Distancia Manhattan a la planta potabilizadora ('U') más cercana.
  */
  int ComportamientoIngeniero::DistanciaU(int f, int c, const vector<pair<int,int>> &plantas) {
    int min_dist = 1000; // Valor inicial alto para la búsqueda del mínimo.
    
    // Iteramos sobre todas las plantas 'U' detectadas previamente en el mapa.
    for (const auto& p : plantas) {
        // Calculamos la distancia Manhattan: |f1 - f2| + |c1 - c2|.
        // Es la métrica ideal para trazados que solo permiten movimientos ortogonales (N,S,E,O).
        int d = abs(f - p.first) + abs(c - p.second);
        
        // Actualizamos si encontramos una planta más próxima que las anteriores.
        if (d < min_dist) min_dist = d;
    }
    return min_dist;
}


/*************************************************************************************/


// =========================================================================
// Nivel 5
// =========================================================================


// 1. CALCULANDO PLAN
Action ComportamientoIngeniero::FaseGenerarRed(Sensores sensores) {

// Llamamos a tu función del Nivel 4 con los parámetros exactos
    lista_tuberias = A_Estrella_Tuberias(
        mapaResultado,
        mapaCotas,
        sensores.BelPosF,
        sensores.BelPosC,
        sensores.max_ecologico
    );

    // 3. Visualizar tuberías (muy recomendable)
    VisualizaRedTuberias(lista_tuberias);

   fase_actual = DESPLAZAMIENTO_BELKANITA;     
   return IDLE; // Gastamos este turno en "pensar"

}


Action ComportamientoIngeniero::FaseDesplazamientoBelkanita(Sensores sensores) {
    // 1. ¿Ya estamos en la Belkanita?
    if (sensores.posF == sensores.BelPosF && sensores.posC == sensores.BelPosC) {
        fase_actual = PREPARAR_BELKANITA;
        hayPlan = false; // Limpiamos el plan de movimiento por precaución
        return IDLE;     
    }

    // 2. Control de choques
    if (sensores.choque) hayPlan = false;

    // 3. Preparar estados para el 
    EstadoI inicio = {sensores.posF, sensores.posC, sensores.rumbo, tiene_zapatillas};
    EstadoI meta = {sensores.BelPosF, sensores.BelPosC, sensores.rumbo, false};

    // 4. Calcular ruta si no tenemos o si cambió el destino
    if (!hayPlan || (estadoDestino.site.f != meta.site.f || estadoDestino.site.c != meta.site.c)) {
        plan = BusquedaCaminoMasCorto(inicio, meta, mapaResultado, mapaCotas);
        if (!plan.empty()) {
            hayPlan = true;
            estadoDestino = meta;
        }
    }

    // 5. Ejecutar la acción
    if (hayPlan && !plan.empty()) {
        Action accion = plan.front();
        plan.pop_front();
        return accion;
    }
    return IDLE;
}

Action ComportamientoIngeniero::FasePrepararBelkanita(Sensores sensores) {
    if (!lista_tuberias.empty()) {
        int op_inicio = lista_tuberias.front().op; 
        
        // Ejecutamos la operación necesaria y la marcamos como hecha (op = 0)
        if (op_inicio == 1) {
            lista_tuberias.front().op = 0; 
            return RAISE;
        } else if (op_inicio == -1) {
            lista_tuberias.front().op = 0; 
            return DIG;
        }
    }
    
    // Si no había que hacer nada (op == 0), avanzamos a llamar al técnico
    fase_actual = SOLICITAR_TECNICO;
    return IDLE;
}

Action ComportamientoIngeniero::FaseSolicitarTecnico(Sensores sensores) {
    if (lista_tuberias.empty()) return IDLE; // Fin de la obra
    
    // Si la siguiente tubería a instalar es justo donde estoy parado ahora, 
    // la descarto de la lista general (porque ya estoy encima)
    if (sensores.posF == lista_tuberias.front().fil && sensores.posC == lista_tuberias.front().col) {
        lista_tuberias.pop_front();
        if (lista_tuberias.empty()) return IDLE; // Por si era la última
    }
    
    // Guardamos la información del tramo al que vamos a ir ahora
    tramo_objetivo = lista_tuberias.front();
    
    // El grito COME hace que el técnico vaya a mi posición ACTUAL.
    // Mientras él viene aquí, yo me muevo a la siguiente baldosa.
    fase_actual = DESPLAZAMIENTO_TRAMO; 
    hayPlan = false; // Forzamos recálculo de ruta
    return COME; 
}


Action ComportamientoIngeniero::FaseDesplazamientoTramo(Sensores sensores) {
    // 1. ¿Hemos llegado al tramo objetivo?
    if (sensores.posF == tramo_objetivo.fil && sensores.posC == tramo_objetivo.col) {
        fase_actual = ESPERAR_ALINEACION; 
        hayPlan = false;
        return IDLE; 
    }

    // 2. Control de choques
    if (sensores.choque) hayPlan = false;

    EstadoI inicio = {sensores.posF, sensores.posC, sensores.rumbo, tiene_zapatillas};
    EstadoI meta = {tramo_objetivo.fil, tramo_objetivo.col, sensores.rumbo, false};

    // 3. Calculamos la ruta con tu Dijkstra
    if (!hayPlan || (estadoDestino.site.f != meta.site.f || estadoDestino.site.c != meta.site.c)) {
        plan = BusquedaCaminoMasCorto(inicio, meta, mapaResultado, mapaCotas);
        if (!plan.empty()) {
            hayPlan = true;
            estadoDestino = meta;
        }
    }

    // 4. Ejecutar el movimiento
    if (hayPlan && !plan.empty()) {
        Action accion = plan.front();
        plan.pop_front();
        return accion;
    }
    return IDLE;
}

Action ComportamientoIngeniero::FaseEsperarAlineacion(Sensores sensores) {
    // Verificamos si en la baldosa pegada a nosotros (índice 2) hay un técnico 't'
    if (sensores.agentes[2] == 't') { 
        if (sensores.enfrente) {      
            // Si nos está mirando cara a cara, pasamos a nivelar
            fase_actual = NIVELAR_TERRENO;
        }
        return IDLE; // Nos quedamos quietos esperando
    } else {
        return TURN_SR; // Rotamos hasta tenerlo en la casilla de enfrente
    }
}


Action ComportamientoIngeniero::FaseNivelarTerreno(Sensores sensores) {
    // Control de seguridad: si perdemos contacto visual, volvemos a alinearnos
    if (sensores.agentes[2] != 't' || !sensores.enfrente) {
        fase_actual = ESPERAR_ALINEACION;
        return IDLE;
    }

    // Preparamos la fase para el siguiente turno
    fase_actual = COLOCAR_TUBERIA;

    // Ejecutamos la acción guardada en nuestro plan sin mirar el mapa de nuevo
    if (tramo_objetivo.op == 1) {
        tramo_objetivo.op = 0; 
        return RAISE;
    } else if (tramo_objetivo.op == -1) {
        tramo_objetivo.op = 0; 
        return DIG;
    } else {
        // Si no había que nivelar, gastamos 1 turno en IDLE para sincronizarnos
        return IDLE;
    }
}



Action ComportamientoIngeniero::FaseColocarTuberia(Sensores sensores) {
    if (sensores.enfrente) {
        // Reiniciamos el ciclo volviendo a llamar al técnico
        fase_actual = SOLICITAR_TECNICO; 
        return INSTALL;
    } else {
        // Por si algo fue mal, volvemos a asegurar la alineación
        fase_actual = ESPERAR_ALINEACION;
        return IDLE;
    }
}





// =========================================================================
// Nivel 6
// =========================================================================

/*************************************************************************************/

/**
 * Lógica para el Nivel 0: Un comportamiento reactivo básico.
 * El objetivo es no chocar y llegar a la meta si se ve.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores) {
    Action accion = IDLE;
    ActualizarMapa(sensores); // Ponemos en el mapa lo que vemos ahora mismo.
    
    // Sumo una visita a la casilla donde estoy para el mapa de calor.
    if (sensores.posF < 100 && sensores.posC < 100) {
        matriz_visitados[sensores.posF][sensores.posC]++;
    }
    
    // Si piso zapatillas, me las pongo. Si llego a la meta, me paro.
    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
    if (sensores.superficie[0] == 'U') return IDLE; 

    // CONTROL DE CHOQUES: Si me doy contra algo (como un agente en una esquina),
    // cambio de lado mi "preferencia de giro" y doy media vuelta para no bloquearme.
    if (sensores.choque) {
        prioridad_derecha = !prioridad_derecha; 
        accion = prioridad_derecha ? TURN_SR : TURN_SL;
        last_action = accion;
        return accion;
    }

    // Miro a los lados y al frente, pero filtrando solo lo que me interesa (C, D, U).
    char i = FiltrarCasillaNivel0(sensores, 1, tiene_zapatillas);
    char c = FiltrarCasillaNivel0(sensores, 2, tiene_zapatillas);
    char d = FiltrarCasillaNivel0(sensores, 3, tiene_zapatillas);

    // Decido cuál es la mejor casilla de las tres.
    int pos = VeoCasillaInteresanteI(i, c, d, tiene_zapatillas, prioridad_derecha, sensores);
   
    switch(pos) {
        case 2: accion = WALK;    break; // De frente.
        case 1: accion = TURN_SL; break; // Giro izquierda.
        case 3: accion = TURN_SR; break; // Giro derecha.
        default: accion = TURN_SL; break; // Si estoy atrapado, giro para buscar salida.
    }
   
    last_action = accion;
    return accion;
}
/*************************************************************************************/

/**
 * Lógica para el Nivel 1: Aquí ya exploramos en serio.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores) {
    Action accion = IDLE;
    // ¡OJO! Aquí es donde rellenamos el mapa resultado para que nos den los puntos.
    ActualizarMapa(sensores);
    
    // Control de mapa de calor (visitas).
    if (sensores.posF < 100 && sensores.posC < 100) {
        matriz_visitados[sensores.posF][sensores.posC]++;
    }

    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

    // Si choco, giro para salir del callejón.
    if (sensores.choque) {
        prioridad_derecha = !prioridad_derecha;
        accion = TURN_SL;
        last_action = accion;
        return accion;
    }

    // Filtro la visión con las reglas del Nivel 1 (incluye agua, bosques, etc.)
    char i = FiltrarCasillaNivel1(sensores, 1, tiene_zapatillas);
    char c = FiltrarCasillaNivel1(sensores, 2, tiene_zapatillas);
    char d = FiltrarCasillaNivel1(sensores, 3, tiene_zapatillas);

    // Uso la lógica de coste (visitas + tipo de terreno) para decidir el movimiento.
    int pos = VeoCasillaNivel1(i, c, d, tiene_zapatillas, prioridad_derecha, sensores);

    switch(pos) {
        case 2: return WALK;
        case 1: return TURN_SL;
        case 3: return TURN_SR;
        default: return TURN_SL;
    }
}
/*************************************************************************************/


// Niveles avanzados (Uso de búsqueda)
/**
 * @brief Comportamiento del ingeniero para el Nivel 2 (búsqueda).
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_2(Sensores sensores) {
    Action accion = IDLE;

    // 1. ESCUDO ANTI-BLOQUEOS: Si nos chocamos, el plan ya no sirve.
    if (sensores.choque) {
        hayPlan = false;
    }

    if (!hayPlan) {
        EstadoI inicio, fin;
        
        inicio.site.f = sensores.posF;
        inicio.site.c = sensores.posC;
        inicio.site.brujula = sensores.rumbo;
        
        inicio.zapatillas = (mapaResultado[sensores.posF][sensores.posC] == 'D');

        fin.site.f = sensores.BelPosF;
        fin.site.c = sensores.BelPosC;

        // Calculamos el plan
        plan = BusquedaCaminoMasCorto(inicio, fin, mapaResultado, mapaCotas);
        VisualizaPlan(inicio.site, plan);
        
        hayPlan = !plan.empty();
    }

    if (hayPlan && !plan.empty()) {
        accion = plan.front();
        plan.pop_front();
    }

    if (plan.empty()) {
        hayPlan = false;
    }

    return accion;
}

/*************************************************************************************/


/**

 * El ingeniero actúa como asistente del Técnico. Su prioridad es apartarse 
 * del camino para que el Técnico pueda maniobrar sin colisiones.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_3(Sensores sensores) {
    // 1. DETECCIÓN DEL TÉCNICO:
    // Escaneamos el cono de visión (15 casillas frontales) buscando al agente 't'.
    bool tecnico_cerca = false;
    for (int i = 1; i <= 15 && !tecnico_cerca; ++i) {
      tecnico_cerca = (sensores.agentes[i] == 't');
    }

    // 2. PROTOCOLO DE DESPEJE:
    // Si el Técnico está cerca, el Ingeniero debe moverse para no estorbar.
    if (tecnico_cerca) {
        char terreno_frente = sensores.superficie[2];
        char agente_frente = sensores.agentes[2];
        int desnivel = sensores.cota[2] - sensores.cota[0];

        // Analizamos si la casilla de enfrente es apta para "hacerse a un lado".
        bool obstaculo = (terreno_frente == 'M' || terreno_frente == 'P' || terreno_frente == 'B');
        bool muy_alto = (abs(desnivel) > 1); // Desniveles que requieren saltar o son intransitables.

        // Si el frente está libre de obstáculos físicos y de otros agentes ('_')...
        if (!obstaculo && !muy_alto && agente_frente == '_') {
            return WALK; // El ingeniero camina para liberar la zona actual.
        } else {
            // Si el camino frontal está bloqueado, rota para buscar una vía de escape.
            return TURN_SR;
        }
    }

    // 3. ESTADO DE ESPERA:
    // Si no hay rastro del Técnico, el Ingeniero permanece en IDLE. 
    return IDLE;
}

/*************************************************************************************/


/**
 * En este nivel, el ingeniero debe encontrar la ruta óptima para una tubería
 * que conecte la Belkanita con una planta U, respetando la gravedad
 * y el impacto ecológico
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_4(Sensores sensores)
{
    // 1. LLAMADA AL ALGORITMO DE BÚSQUEDA A*
    list<Paso> planTuberias = A_Estrella_Tuberias(
        mapaResultado, 
        mapaCotas,         // Información de elevación necesaria para la gravedad
        sensores.BelPosF,  // Fila de inicio (Belkanita)
        sensores.BelPosC,  // Columna de inicio (Belkanita)
        sensores.max_ecologico
    );

    // 2. VALIDACIÓN Y VISUALIZACIÓN
    // Si el algoritmo encuentra un camino válido que no supere el coste máximo:
    if (!planTuberias.empty()) {
        // VisualizaRedTuberias: Dibuja la red en la interfaz del simulador para su evaluación.
        VisualizaRedTuberias(planTuberias);
        
        // PintaPlan: Marca el recorrido en el mapa interno del agente.
        PintaPlan(planTuberias);
    }
    
    // 3. FINALIZACIÓN
    return IDLE; 
}
    
    



/*************************************************************************************/


Action ComportamientoIngeniero::ComportamientoIngenieroNivel_5(Sensores sensores) {
    Action accion = IDLE;

    switch (fase_actual) {
        case GENERAR_RED_TUBERIAS:     return FaseGenerarRed(sensores);
        case DESPLAZAMIENTO_BELKANITA: return FaseDesplazamientoBelkanita(sensores);
        case PREPARAR_BELKANITA:       return FasePrepararBelkanita(sensores);
        case SOLICITAR_TECNICO:        return FaseSolicitarTecnico(sensores);
        case DESPLAZAMIENTO_TRAMO:     return FaseDesplazamientoTramo(sensores);
        case ESPERAR_ALINEACION:       return FaseEsperarAlineacion(sensores);
        case NIVELAR_TERRENO:          return FaseNivelarTerreno(sensores);
        case COLOCAR_TUBERIA:          return FaseColocarTuberia(sensores);
        default:                       return IDLE;
    }
}


/*************************************************************************************/


Action ComportamientoIngeniero::ComportamientoIngenieroNivel_6(Sensores sensores)
{
  return IDLE;
}


// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Carácter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoIngeniero::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}


/*************************************************************************************/


void ComportamientoIngeniero::ActualizarMapa(Sensores sensores)
{
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo)
  {
  case norte:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF - j][sensores.posC + i] = sensores.superficie[pos];
        mapaCotas[sensores.posF - j][sensores.posC + i] = sensores.cota[pos++];
      }
    break;
  case noreste:
    mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[1];
    mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[1];
    mapaResultado[sensores.posF - 1][sensores.posC + 1] = sensores.superficie[2];
    mapaCotas[sensores.posF - 1][sensores.posC + 1] = sensores.cota[2];
    mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[3];
    mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[3];
    mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[4];
    mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[4];
    mapaResultado[sensores.posF - 2][sensores.posC + 1] = sensores.superficie[5];
    mapaCotas[sensores.posF - 2][sensores.posC + 1] = sensores.cota[5];
    mapaResultado[sensores.posF - 2][sensores.posC + 2] = sensores.superficie[6];
    mapaCotas[sensores.posF - 2][sensores.posC + 2] = sensores.cota[6];
    mapaResultado[sensores.posF - 1][sensores.posC + 2] = sensores.superficie[7];
    mapaCotas[sensores.posF - 1][sensores.posC + 2] = sensores.cota[7];
    mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[8];
    mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[8];
    mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[9];
    mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[9];
    mapaResultado[sensores.posF - 3][sensores.posC + 1] = sensores.superficie[10];
    mapaCotas[sensores.posF - 3][sensores.posC + 1] = sensores.cota[10];
    mapaResultado[sensores.posF - 3][sensores.posC + 2] = sensores.superficie[11];
    mapaCotas[sensores.posF - 3][sensores.posC + 2] = sensores.cota[11];
    mapaResultado[sensores.posF - 3][sensores.posC + 3] = sensores.superficie[12];
    mapaCotas[sensores.posF - 3][sensores.posC + 3] = sensores.cota[12];
    mapaResultado[sensores.posF - 2][sensores.posC + 3] = sensores.superficie[13];
    mapaCotas[sensores.posF - 2][sensores.posC + 3] = sensores.cota[13];
    mapaResultado[sensores.posF - 1][sensores.posC + 3] = sensores.superficie[14];
    mapaCotas[sensores.posF - 1][sensores.posC + 3] = sensores.cota[14];
    mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[15];
    mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[15];
    break;
  case este:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF + i][sensores.posC + j] = sensores.superficie[pos];
        mapaCotas[sensores.posF + i][sensores.posC + j] = sensores.cota[pos++];
      }
    break;
  case sureste:
    mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[1];
    mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[1];
    mapaResultado[sensores.posF + 1][sensores.posC + 1] = sensores.superficie[2];
    mapaCotas[sensores.posF + 1][sensores.posC + 1] = sensores.cota[2];
    mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[3];
    mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[3];
    mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[4];
    mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[4];
    mapaResultado[sensores.posF + 1][sensores.posC + 2] = sensores.superficie[5];
    mapaCotas[sensores.posF + 1][sensores.posC + 2] = sensores.cota[5];
    mapaResultado[sensores.posF + 2][sensores.posC + 2] = sensores.superficie[6];
    mapaCotas[sensores.posF + 2][sensores.posC + 2] = sensores.cota[6];
    mapaResultado[sensores.posF + 2][sensores.posC + 1] = sensores.superficie[7];
    mapaCotas[sensores.posF + 2][sensores.posC + 1] = sensores.cota[7];
    mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[8];
    mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[8];
    mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[9];
    mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[9];
    mapaResultado[sensores.posF + 1][sensores.posC + 3] = sensores.superficie[10];
    mapaCotas[sensores.posF + 1][sensores.posC + 3] = sensores.cota[10];
    mapaResultado[sensores.posF + 2][sensores.posC + 3] = sensores.superficie[11];
    mapaCotas[sensores.posF + 2][sensores.posC + 3] = sensores.cota[11];
    mapaResultado[sensores.posF + 3][sensores.posC + 3] = sensores.superficie[12];
    mapaCotas[sensores.posF + 3][sensores.posC + 3] = sensores.cota[12];
    mapaResultado[sensores.posF + 3][sensores.posC + 2] = sensores.superficie[13];
    mapaCotas[sensores.posF + 3][sensores.posC + 2] = sensores.cota[13];
    mapaResultado[sensores.posF + 3][sensores.posC + 1] = sensores.superficie[14];
    mapaCotas[sensores.posF + 3][sensores.posC + 1] = sensores.cota[14];
    mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[15];
    mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[15];
    break;
  case sur:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF + j][sensores.posC - i] = sensores.superficie[pos];
        mapaCotas[sensores.posF + j][sensores.posC - i] = sensores.cota[pos++];
      }
    break;
  case suroeste:
    mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[1];
    mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[1];
    mapaResultado[sensores.posF + 1][sensores.posC - 1] = sensores.superficie[2];
    mapaCotas[sensores.posF + 1][sensores.posC - 1] = sensores.cota[2];
    mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[3];
    mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[3];
    mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[4];
    mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[4];
    mapaResultado[sensores.posF + 2][sensores.posC - 1] = sensores.superficie[5];
    mapaCotas[sensores.posF + 2][sensores.posC - 1] = sensores.cota[5];
    mapaResultado[sensores.posF + 2][sensores.posC - 2] = sensores.superficie[6];
    mapaCotas[sensores.posF + 2][sensores.posC - 2] = sensores.cota[6];
    mapaResultado[sensores.posF + 1][sensores.posC - 2] = sensores.superficie[7];
    mapaCotas[sensores.posF + 1][sensores.posC - 2] = sensores.cota[7];
    mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[8];
    mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[8];
    mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[9];
    mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[9];
    mapaResultado[sensores.posF + 3][sensores.posC - 1] = sensores.superficie[10];
    mapaCotas[sensores.posF + 3][sensores.posC - 1] = sensores.cota[10];
    mapaResultado[sensores.posF + 3][sensores.posC - 2] = sensores.superficie[11];
    mapaCotas[sensores.posF + 3][sensores.posC - 2] = sensores.cota[11];
    mapaResultado[sensores.posF + 3][sensores.posC - 3] = sensores.superficie[12];
    mapaCotas[sensores.posF + 3][sensores.posC - 3] = sensores.cota[12];
    mapaResultado[sensores.posF + 2][sensores.posC - 3] = sensores.superficie[13];
    mapaCotas[sensores.posF + 2][sensores.posC - 3] = sensores.cota[13];
    mapaResultado[sensores.posF + 1][sensores.posC - 3] = sensores.superficie[14];
    mapaCotas[sensores.posF + 1][sensores.posC - 3] = sensores.cota[14];
    mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[15];
    mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[15];
    break;
  case oeste:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
        mapaResultado[sensores.posF - i][sensores.posC - j] = sensores.superficie[pos];
        mapaCotas[sensores.posF - i][sensores.posC - j] = sensores.cota[pos++];
      }
    break;
  case noroeste:
    mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[1];
    mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[1];
    mapaResultado[sensores.posF - 1][sensores.posC - 1] = sensores.superficie[2];
    mapaCotas[sensores.posF - 1][sensores.posC - 1] = sensores.cota[2];
    mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[3];
    mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[3];
    mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[4];
    mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[4];
    mapaResultado[sensores.posF - 1][sensores.posC - 2] = sensores.superficie[5];
    mapaCotas[sensores.posF - 1][sensores.posC - 2] = sensores.cota[5];
    mapaResultado[sensores.posF - 2][sensores.posC - 2] = sensores.superficie[6];
    mapaCotas[sensores.posF - 2][sensores.posC - 2] = sensores.cota[6];
    mapaResultado[sensores.posF - 2][sensores.posC - 1] = sensores.superficie[7];
    mapaCotas[sensores.posF - 2][sensores.posC - 1] = sensores.cota[7];
    mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[8];
    mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[8];
    mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[9];
    mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[9];
    mapaResultado[sensores.posF - 1][sensores.posC - 3] = sensores.superficie[10];
    mapaCotas[sensores.posF - 1][sensores.posC - 3] = sensores.cota[10];
    mapaResultado[sensores.posF - 2][sensores.posC - 3] = sensores.superficie[11];
    mapaCotas[sensores.posF - 2][sensores.posC - 3] = sensores.cota[11];
    mapaResultado[sensores.posF - 3][sensores.posC - 3] = sensores.superficie[12];
    mapaCotas[sensores.posF - 3][sensores.posC - 3] = sensores.cota[12];
    mapaResultado[sensores.posF - 3][sensores.posC - 2] = sensores.superficie[13];
    mapaCotas[sensores.posF - 3][sensores.posC - 2] = sensores.cota[13];
    mapaResultado[sensores.posF - 3][sensores.posC - 1] = sensores.superficie[14];
    mapaCotas[sensores.posF - 3][sensores.posC - 1] = sensores.cota[14];
    mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[15];
    mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[15];
    break;
  }
}

/*************************************************************************************/


bool ComportamientoIngeniero::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]); // Solo 'C', 'D', 'U' son transitables en Nivel 0
}


/*************************************************************************************/


bool ComportamientoIngeniero::EsAccesiblePorAltura(const ubicacion &actual, bool zap)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (zap && desnivel > 2)
    return false;
  if (!zap && desnivel > 1)
    return false;
  return true;
}


/*************************************************************************************/


ubicacion ComportamientoIngeniero::Delante(const ubicacion &actual) const
{
  ubicacion delante = actual;
  switch (actual.brujula)
  {
  case 0:
    delante.f--;
    break; // norte
  case 1:
    delante.f--;
    delante.c++;
    break; // noreste
  case 2:
    delante.c++;
    break; // este
  case 3:
    delante.f++;
    delante.c++;
    break; // sureste
  case 4:
    delante.f++;
    break; // sur
  case 5:
    delante.f++;
    delante.c--;
    break; // suroeste
  case 6:
    delante.c--;
    break; // oeste
  case 7:
    delante.f--;
    delante.c--;
    break; // noroeste
  }
  return delante;
}


/*************************************************************************************/


void ComportamientoIngeniero::PintaPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
    {
      cout << "W ";
    }
    else if (*it == JUMP)
    {
      cout << "J ";
    }
    else if (*it == TURN_SR)
    {
      cout << "r ";
    }
    else if (*it == TURN_SL)
    {
      cout << "l ";
    }
    else if (*it == COME)
    {
      cout << "C ";
    }
    else if (*it == IDLE)
    {
      cout << "I ";
    }
    else
    {
      cout << "-_ ";
    }
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}


/*************************************************************************************/


void ComportamientoIngeniero::PintaPlan(const list<Paso> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    cout << it->fil << ", " << it->col << " (" << it->op << ")\n";
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}


/*************************************************************************************/


void ComportamientoIngeniero::VisualizaPlan(const ubicacion &st,
                                            const list<Action> &plan)
{
  listaPlanCasillas.clear();
  ubicacion cst = st;

  listaPlanCasillas.push_back({cst.f, cst.c, WALK});
  auto it = plan.begin();
  while (it != plan.end())
  {

    switch (*it)
    {
    case JUMP:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
    case WALK:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, WALK});
      break;
    case TURN_SR:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 1) % 8);
      break;
    case TURN_SL:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 7) % 8);
      break;
    }
    it++;
  }
}


/*************************************************************************************/


void ComportamientoIngeniero::VisualizaRedTuberias(const list<Paso> &plan)
{
  listaCanalizacionTuberias.clear();
  auto it = plan.begin();
  while (it != plan.end())
  {
    listaCanalizacionTuberias.push_back({it->fil, it->col, it->op});
    it++;
  }
}
