#include "tecnico.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>
#include <map>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoTecnico::think(Sensores sensores) {
  Action accion = IDLE;


  // Decisión del agente según el nivel
  switch (sensores.nivel) {
    case 0: accion = ComportamientoTecnicoNivel_0(sensores); break;
    case 1: accion = ComportamientoTecnicoNivel_1(sensores); break;
    case 2: accion = ComportamientoTecnicoNivel_2(sensores); break;
    case 3: accion = ComportamientoTecnicoNivel_3(sensores); break;
    //case 3: accion = ComportamientoTecnicoNivel_E(sensores); break;
    case 4: accion = ComportamientoTecnicoNivel_4(sensores); break;
    case 5: accion = ComportamientoTecnicoNivel_5(sensores); break;
    case 6: accion = ComportamientoTecnicoNivel_6(sensores); break;
  }

  return accion;
}



/**********************************************************************************************/

/**
 * LÓGICA NIVEL 0 DEL TÉCNICO: Comportamiento Reactivo Básico.
 * Al igual que el ingeniero, el técnico se mueve por instinto buscando 
 * elementos de interés inmediato y evitando obstáculos simples.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores) {
    Action accion = IDLE;
    
    // Registro lo que veo en el mapa global para ir completando la misión.
    ActualizarMapa(sensores);
    
    // MAPA DE CALOR: Registro que acabo de pasar por esta casilla para 
    // evitar volver inmediatamente y fomentar que me mueva a zonas nuevas.
    if (sensores.posF < 100 && sensores.posC < 100) {
        matriz_visitados[sensores.posF][sensores.posC]++;
    }
    
    // GESTIÓN DE EQUIPO Y META:
    // Si piso zapatillas ('D'), las recojo automáticamente.
    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;
    // Si llego a la planta potabilizadora ('U'), me detengo.
    if (sensores.superficie[0] == 'U') return IDLE;

    // PROTOCOLO DE CHOQUE:
    // Si me doy contra un muro o un agente, invierto mi preferencia de giro
    // (si iba a la derecha, ahora prefiero izquierda) y roto para desbloquearme.
    if (sensores.choque) {
        prioridad_derecha = !prioridad_derecha; 
        accion = prioridad_derecha ? TURN_SR : TURN_SL;
        last_action = accion;
        return accion;
    }

    // FILTRADO DE VISIÓN:
    // Analizo las casillas a mi izquierda (1), frente (2) y derecha (3).
    // Uso 'FiltrarCasillaNivel0' para identificar caminos ('C'), zapatillas ('D') o plantas ('U').
    char i = FiltrarCasillaNivel0(sensores, 1, tiene_zapatillas);
    char c = FiltrarCasillaNivel0(sensores, 2, tiene_zapatillas);
    char d = FiltrarCasillaNivel0(sensores, 3, tiene_zapatillas);

    // TOMA DE DECISIÓN:
    // 'VeoCasillaInteresanteT' me dice cuál de las tres direcciones es más atractiva.
    int pos = VeoCasillaInteresanteT(i, c, d, tiene_zapatillas, prioridad_derecha, sensores);
   
    switch(pos) {
        case 2: accion = WALK;    break; // El camino está despejado al frente.
        case 1: accion = TURN_SL; break; // Hay algo interesante a la izquierda.
        case 3: accion = TURN_SR; break; // Hay algo interesante a la derecha.
        default: accion = TURN_SR; break; // En caso de duda, giro para buscar.
    }
   
    last_action = accion;
    return accion;
}

/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Carácter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoTecnico::es_camino(unsigned char c) const {
  return (c == 'C' || c == 'D' || c == 'U');
}


/**
 * LÓGICA NIVEL 1 DEL TÉCNICO: Exploración por Coste y Fatiga.
 * Aquí el técnico ya no solo busca objetos, sino que intenta descubrir la mayor
 * parte del mapa posible evitando terrenos lentos o peligrosos (agua, bosque).
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_1(Sensores sensores) {
    Action accion = IDLE;

    // 1. ACTUALIZACIÓN DEL MAPA: Fundamental para ganar puntos por descubrimiento.
    // Vuelco la información de los sensores de superficie al mapa resultado.
    ActualizarMapa(sensores);
    
    // 2. MAPA DE CALOR: Incremento las visitas de la casilla actual.
    if (sensores.posF < 100 && sensores.posC < 100) {
        matriz_visitados[sensores.posF][sensores.posC]++;
    }

    // 3. OBJETOS Y CHOQUES: Control de equipamiento y colisiones inesperadas.
    if (sensores.superficie[0] == 'D') tiene_zapatillas = true;

    if (sensores.choque) {
        prioridad_derecha = !prioridad_derecha;
        accion = prioridad_derecha ? TURN_SR : TURN_SL;
        last_action = accion;
        return accion;
    }

    // 4. VISIÓN AVANZADA:
    // Filtro las casillas laterales y frontal considerando terrenos más complejos.
    // El técnico evalúa si puede pasar por bosques o agua basándose en 'tiene_zapatillas'.
    char i = FiltrarCasillaNivel1(sensores, 1, tiene_zapatillas);
    char c = FiltrarCasillaNivel1(sensores, 2, tiene_zapatillas);
    char d = FiltrarCasillaNivel1(sensores, 3, tiene_zapatillas);

    // 5. MOVIMIENTO BASADO EN COSTE:
    // 'VeoCasillaNivel1' calcula qué dirección tiene el menor coste (menos visitas + mejor suelo).
    int pos = VeoCasillaNivel1(i, c, d, tiene_zapatillas, prioridad_derecha, sensores);

    switch(pos) {
        case 2: return WALK;
        case 1: return TURN_SL;
        case 3: return TURN_SR;
        default: return TURN_SR;
    }
}

/*************************************************************************************/


/**
 * Si detecto cerca al ingeniero, intento moverme a una casilla libre para dejarle paso.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_2(Sensores sensores) {
    // 1. DETECCIÓN DEL INGENIERO:
    bool ing_cerca = false;
    for (int i = 1; i <= 15 && !ing_cerca; ++i) {
        ing_cerca = (sensores.agentes[i] == 'i');
    }

    // 2. PROTOCOLO DE ESPERA:
    if (!ing_cerca) {
        return IDLE;
    }

    // 3. SEGURIDAD ANTE BLOQUEO FRONTAL:
    if (sensores.agentes[2] == 't') {
        return TURN_SR;
    }

    // 4. ANÁLISIS DEL TERRENO:
    char terreno_frente = sensores.superficie[2];
    char agente_frente = sensores.agentes[2];
    int desnivel = sensores.cota[2] - sensores.cota[0];

    // Un muro (M) o un precipicio (P) siempre son obstáculos.
    // El bosque (B) solo es un obstáculo si NO tengo zapatillas.
    bool es_bosque_intransitable = (terreno_frente == 'B' && !tiene_zapatillas);
    bool obstaculo = (terreno_frente == 'M' || terreno_frente == 'P' || es_bosque_intransitable);
    
    // Verifico si el salto es demasiado alto.
    bool muy_alto = (abs(desnivel) > 1);

    // 5. ACCIÓN DE APARTARSE:
    // Si el frente es transitable y está libre, avanzo.
    if (!obstaculo && !muy_alto && agente_frente == '_') {
        return WALK;
    }

    // 6. SALIDA DE EMERGENCIA:
    return TURN_SR;
}

/*************************************************************************************/


/**
 * Aquí utilizo un algoritmo de búsqueda para encontrar la ruta más rápida
 * hacia la Belkanita, gestionando posibles bloqueos en el camino.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_3(Sensores sensores) {
    Action accion = IDLE;

    // 1. GENERACIÓN DEL PLAN:
    // Si no tengo una ruta calculada, defino los puntos de inicio y fin.
    if (!hayPlan) {
        EstadoT inicio, fin;
        
        // Cargo mi situación actual: posición, rumbo y si llevo zapatillas.
        inicio.site.f = sensores.posF;
        inicio.site.c = sensores.posC;
        inicio.site.brujula = sensores.rumbo;
        inicio.zapatillas = tiene_zapatillas; 

        // Mi destino es la posición de la Belkanita detectada.
        fin.site.f = sensores.BelPosF;
        fin.site.c = sensores.BelPosC;

        // Ejecuto mi A* especializado para el nivel 3.
        plan = A_Estrella_Nivel3(inicio, fin, mapaResultado, mapaCotas);

        // Muestro la línea del plan en el simulador para debuggear la ruta.
        VisualizaPlan(inicio.site, plan);
        
        // Activo la bandera de plan si la búsqueda tuvo éxito.
        hayPlan = !plan.empty();
    }

    // 2. EJECUCIÓN Y CONTROL DE ATASCOS:
    if (hayPlan && !plan.empty()) {
        Action accion_plan = plan.front();
        
        // Si el plan me pide avanzar pero hay alguien delante ('_'), espero.
        if (accion_plan == WALK && sensores.agentes[2] != '_') {
            turnos_esperando++;
            
            // Si espero más de 5 turnos, asumo que el camino está bloqueado permanentemente.
            if (turnos_esperando > 5) {
                hayPlan = false; // Cancelo el plan para recalcular una ruta alternativa.
                turnos_esperando = 0;
            }
            return IDLE;
        }
        
        // Si la casilla está libre, ejecuto la acción y la elimino de la lista.
        turnos_esperando = 0; 
        plan.pop_front();
        return accion_plan;
    }

    // 3. FINALIZACIÓN:
    // Si he llegado al final de mi lista de acciones, limpio la bandera para el siguiente objetivo.
    if (plan.empty()) {
        hayPlan = false;
    }

    return accion;
}


/*************************************************************************************/


Action ComportamientoTecnico::ComportamientoTecnicoNivel_4(Sensores sensores) {
  return IDLE;
}


/*************************************************************************************/


Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores) {
    switch (fase_t) {
        case AGUARDAR_LLAMADA:    return FaseAguardar(sensores);
        case TRASLADO_A_POSICION: return FaseTraslado(sensores);
        case SINCRONIZAR_NIVELADO: return FaseSincronizar(sensores);
        case EJECUTAR_INSTALACION: return FaseEjecutar(sensores);
        default: return IDLE;
    }
}


/*************************************************************************************/


Action ComportamientoTecnico::ComportamientoTecnicoNivel_6(Sensores sensores) {
  return IDLE;
}



 list<Action> AvanzaASaltosDeCaballo() {
    list<Action> secuencia;
    secuencia.push_back(WALK);
    secuencia.push_back(WALK);
    secuencia.push_back(TURN_SR);   
    secuencia.push_back(TURN_SR);    
    secuencia.push_back(WALK);
    return secuencia;     
  }
  

/*************************************************************************************/

Action ComportamientoTecnico::ComportamientoTecnicoNivel_E(Sensores sensores){
	Action accion = IDLE;
	if (!hayPlan){
		// Invocar al método de búsqueda
		EstadoT inicio, fin;
		inicio.site.f = sensores.posF;
		inicio.site.c = sensores.posC;
		inicio.site.brujula = sensores.rumbo;
		inicio.zapatillas = tiene_zapatillas;
		
		fin.site.f = sensores.BelPosF;
		fin.site.c = sensores.BelPosC;
		
		plan = B_Anchura_V2(inicio, fin, mapaResultado, mapaCotas);
		VisualizaPlan(inicio.site,plan);
		hayPlan = plan.size() != 0 ;
	}
	if (hayPlan and plan.size()>0){
		accion = plan.front();
		plan.pop_front();
	}
	if (plan.size()== 0){
		hayPlan = false;
	}
	return accion;
}

  
// =========================================================================
// Nivel 0
// =========================================================================

 /**
  * TOMA DE DECISIÓN PRIORIZADA:
  * Este método es el cerebro reactivo del Técnico. Decide a dónde ir
  * evaluando objetivos (Meta, Zapatillas) y luego el Mapa de Calor.
  */
  int ComportamientoTecnico::VeoCasillaInteresanteT(char i, char c, char d, bool zap, bool pref_dcha, const Sensores &sensores) {
    // 1. PRIORIDAD MÁXIMA: La Planta Potabilizadora ('U').
    // Si la veo al frente, voy directo. Si no, miro a los lados según mi preferencia de giro.
    if (c == 'U') return 2;
    if (pref_dcha) { 
        if (d == 'U') return 3; 
        if (i == 'U') return 1; 
    } else { 
        if (i == 'U') return 1; 
        if (d == 'U') return 3; 
    }

    // 2. PRIORIDAD SECUNDARIA: Equipamiento.
    // Si no tengo zapatillas y las veo, cambio mi ruta para recogerlas ('D').
    if (!zap) {
        if (c == 'D') return 2;
        if (pref_dcha) { 
            if (d == 'D') return 3; 
            if (i == 'D') return 1; 
        } else { 
            if (i == 'D') return 1; 
            if (d == 'D') return 3; 
        }
    }

    // --- CÁLCULO DE COORDENADAS PARA EL MAPA DE CALOR ---
    // Determino qué casillas del mapa global corresponden a mi visión izquierda, centro y derecha.
    ubicacion actual = {sensores.posF, sensores.posC, sensores.rumbo};
    ubicacion frente = Delante(actual);
    
    // Calculo la posición a mi izquierda rotando la brújula -1 (o +7) posiciones.
    ubicacion aux_izq = actual; 
    aux_izq.brujula = (Orientacion)((actual.brujula + 7) % 8);
    ubicacion frente_izq = Delante(aux_izq);
    
    // Calculo la posición a mi derecha rotando la brújula +1 posición.
    ubicacion aux_dcha = actual; 
    aux_dcha.brujula = (Orientacion)((actual.brujula + 1) % 8);
    ubicacion frente_dcha = Delante(aux_dcha);

    // 3. CONSULTA DEL MAPA DE CALOR:
    // Obtengo cuántas veces he pasado por cada una de esas tres casillas.
    // Si una casilla es un muro o precipicio ('P'), le asigno coste infinito para evitarla.
    int calor_i = (i != 'P') ? leerCalor(frente_izq) : COST_INFINITE;
    int calor_c = (c != 'P') ? leerCalor(frente) : COST_INFINITE;
    int calor_d = (d != 'P') ? leerCalor(frente_dcha) : COST_INFINITE;

    // Si estoy rodeado de obstáculos insuperables, retorno 0 (bloqueo).
    if (i == 'P' && c == 'P' && d == 'P') return 0;

    // 4. ELECCIÓN DE LA RUTA MENOS TRANSITADA (Casilla más "fría"):
    // Si el frente es lo menos visitado (o igual que los lados), prefiero seguir recto.
    if (calor_c <= calor_i && calor_c <= calor_d) return 2;
    
    // Si el frente está muy visto, elijo el lado menos visitado respetando mi 'prioridad_derecha'.
    if (pref_dcha) {
        return (calor_d <= calor_i) ? 3 : 1;
    } else {
        return (calor_i <= calor_d) ? 1 : 3;
    }
}

/*************************************************************************************/
 /**
  * Control de altura estricto para el Técnico.
  */
  char ComportamientoTecnico::ViablePorAlturaT (char casilla, int dif) { 
    // El técnico es menos ágil: solo salta desniveles de 1 como máximo.
    if (abs(dif) <= 1 ) {
        return casilla;
    } else {
        return 'P'; // Demasiado alto o bajo, lo tratamos como muro.
    }
}
/*************************************************************************************/
 
 /**
  * En los primeros niveles, solo me muevo por suelo seguro.
  */
  char ComportamientoTecnico::FiltrarCasillaNivel0(Sensores sensores, int pos, bool zap) {
    // Si hay un obstáculo dinámico (otro agente) en esa posición, es como un muro.
    if (sensores.agentes[pos] != '_') return 'P';

    char terreno = sensores.superficie[pos];
    
    // Solo considero "bueno" el Camino, las Zapatillas o la Meta. 
    // Bosques y agua se descartan en Nivel 0 para evitar penalizaciones.
    if (terreno != 'C' && terreno != 'D' && terreno != 'U') return 'P';

    return ViablePorAlturaT(terreno, sensores.cota[pos] - sensores.cota[0]);
}

/*************************************************************************************/

 /**
  * Consulta la matriz de visitas asegurándose de no salirse de los límites del mapa (100x100).
  */
  int ComportamientoTecnico::leerCalor(const ubicacion &ub) {
    if (ub.f >= 0 && ub.f < 100 && ub.c >= 0 && ub.c < 100) {
        return matriz_visitados[ub.f][ub.c];
    }
    // Si la coordenada está fuera del mapa, devuelvo un valor altísimo para no ir allí.
    return COST_INFINITE; 
}

// =========================================================================
// Nivel 1
// =========================================================================

 /**
 * Asigno un valor numérico a cada tipo de casilla. Cuanto más alto es el valor,
 * más "ganas" tiene el Técnico de ir hacia allí.
 */
  int ComportamientoTecnico::PuntuacionCasilla(unsigned char casilla) {
    switch (casilla) {
        case '?': return 50000; // PRIORIDAD MÁXIMA
        case 'C': return 30000; //  Terreno estándar muy favorable.
        case 'X': 
        case 'U': return 29000; //  Muy importantes.
        case 'S': return 20000; //  Aceptable.
        case 'H': return 5000;  // 
        case 'A': return -20000; // El técnico intenta evitarla.
        
        // Si tengo zapatillas, el bosque es genial para explorar (20000).
        // Si no las tengo, lo evito (-30000).
        case 'B': return tiene_zapatillas ? 20000 : -30000;        

        // Si veo zapatillas ('D') y no las llevo, tienen un valor altísimo (40000).
        // Si ya las tengo, la casilla no tiene valor especial (0).
        case 'D': return tiene_zapatillas ? 0 : 40000; 
        default:  return -50000; // Evitar
    }
}



/**********************************************************************************************/

 /**
  * FILTRO DE VISIÓN AVANZADO (Nivel 1):
  * Determina si una casilla detectada por los sensores es físicamente transitable.
  */
  char ComportamientoTecnico::FiltrarCasillaNivel1(const Sensores &sensores, int pos, bool tiene_zapatillas) {
    // Si hay un agente bloqueando, la casilla es infranqueable
    if (sensores.agentes[pos] != '_') return 'P';
    
    char terreno = sensores.superficie[pos];
    int dif_cota = sensores.cota[pos] - sensores.cota[0];

    // Muros (M) y precipicios (P) son límites físicos del mapa.
    if (terreno == 'M' || terreno == 'P') return 'P'; 

    if (terreno == 'B' && !tiene_zapatillas) return 'P'; 

    // Si el desnivel es mayor, la consideramos un obstáculo insuperable.
    int limite_altura = 1; 
    if (abs(dif_cota) > limite_altura) return 'P';

    return terreno;
}

/**********************************************************************************************/

 /**
  * Compara las tres opciones de movimiento (Izq, Centro, Dcha) y elige la que
  * ofrece el mejor equilibrio entre descubrimiento y facilidad de paso.
  */
  int ComportamientoTecnico::VeoCasillaNivel1(char i, char c, char d, bool zap, bool pref_dcha, const Sensores &sensores) {
    // 1. Si detectamos zapatillas y las necesitamos, ignoramos el resto de la lógica.
    if (!zap) {
        if (c == 'D') return 2;
        if (pref_dcha) { if (d == 'D') return 3; if (i == 'D') return 1; }
        else           { if (i == 'D') return 1; if (d == 'D') return 3; }
    }

    // 2. Localizamos las coordenadas globales de lo que estamos viendo.
    ubicacion actual = {sensores.posF, sensores.posC, sensores.rumbo};
    ubicacion frente = Delante(actual);
    ubicacion aux_izq = actual; aux_izq.brujula = (Orientacion)((actual.brujula + 7) % 8);
    ubicacion frente_izq = Delante(aux_izq);
    ubicacion aux_dcha = actual; aux_dcha.brujula = (Orientacion)((actual.brujula + 1) % 8);
    ubicacion frente_dcha = Delante(aux_dcha);

    // Si no podemos pasar, giramos por defecto.
    if (i == 'P' && c == 'P' && d == 'P') return 0;

    // 3. Consultamos el mapa de calor (cuántas veces hemos pasado por allí).
    int v_i = (i != 'P') ? leerCalor(frente_izq) : COST_INFINITE;
    int v_c = (c != 'P') ? leerCalor(frente) : COST_INFINITE;
    int v_d = (d != 'P') ? leerCalor(frente_dcha) : COST_INFINITE;

    // 4. Calculamos el "Coste de Exploración": combina visitas + tipo de terreno.
    long int coste_i = CalcularCosteExploracion(v_i, i);
    long int coste_c = CalcularCosteExploracion(v_c, c);
    long int coste_d = CalcularCosteExploracion(v_d, d);

    // 5. Elegimos el camino con el coste más bajo (el más eficiente).
    if (coste_c <= coste_i && coste_c <= coste_d) return 2; 
    
    if (pref_dcha) {
        return (coste_d <= coste_i) ? 3 : 1;
    } else {
        return (coste_i <= coste_d) ? 1 : 3;
    }
}

/**********************************************************************************************/


/**
 * Busca minimizar las visitas repetidas y maximizar la recompensa del terreno.
 */
long int ComportamientoTecnico::CalcularCosteExploracion(int visitas, char terreno) {
    // Si la casilla es un muro o hemos pasado demasiadas veces, el coste es infinito.
    if (terreno == 'P' || visitas >= COST_INFINITE) return COST_INFINITE; 

    int puntos = PuntuacionCasilla(terreno);
    
    if (visitas == 0) {
        // Si nunca hemos estado ahí, restamos puntos al coste haciendo que la casilla sea más atractiva.
        return -puntos - 5000; 
    } else {
        // Cada visita suma 4000 al coste.
        // Con el tiempo, una casilla de calle muy cómoda acabará siendo más "cara"
        // que una de hierba por la que nunca hemos pasado.
        return (visitas * 4000) - puntos; 
    }
}

// =========================================================================
// Nivel E/2
// =========================================================================


/**
 * ¿Puedo pisar la siguiente casilla?
 */
bool ComportamientoTecnico::CasillaAccesibleTécnico(const EstadoT &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){
    // Primero calculamos a dónde queremos ir
    EstadoT next = NextCasillaTécnico(st);
    
    // 1. LÍMITES DEL MUNDO: No puedo caminar fuera del mapa.
    if (next.site.f < 0 || next.site.f >= terreno.size() || 
        next.site.c < 0 || next.site.c >= terreno[0].size()) {
        return false;
    }

    // 2. OBSTÁCULOS FIJOS: Muros (M) y Precipicios (P) son infranqueables.
    bool check1 = (terreno[next.site.f][next.site.c] != 'P' && terreno[next.site.f][next.site.c] != 'M');
    
    // 3. EQUIPAMIENTO: Solo me meto en el Bosque ('B') si llevo las zapatillas puestas.
    bool check2 = (terreno[next.site.f][next.site.c] != 'B' || st.zapatillas);
    
 
    bool check3 = abs((int)altura[next.site.f][next.site.c] - (int)altura[st.site.f][st.site.c]) <= 1;
    
    // Si cumplo las tres condiciones, la casilla es accesible.
    return check1 && check2 && check3;
}
/*************************************************************************************/

 /**
  * @brief Cálculo de la siguiente coordenada.
  */
  EstadoT ComportamientoTecnico::NextCasillaTécnico(const EstadoT &st){
   EstadoT siguiente = st;
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
  * @brief  ¿He estado ya aquí?
  * Se usa durante la búsqueda (A*) para no procesar el mismo nodo dos veces.
  */
  bool ComportamientoTecnico::Find(const NodoT & st, const list<NodoT> &lista){
    auto it = lista.begin();
    // Recorro la lista buscando si el estado del nodo coincide con el que busco.
    while (it != lista.end() and !((*it) == st)){
        it++;
    }
    // Si el iterador no llegó al final, es que lo encontré.
    return (it != lista.end());
  }


/*************************************************************************************/

/**
 * @brief Aplicar acción al estado.
 * Es la función que usa el algoritmo de búsqueda para expandir el árbol de soluciones.
 */
EstadoT ComportamientoTecnico::applyT(Action accion, const EstadoT & st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){
    EstadoT next = st;
    switch(accion){
        case WALK:
            // Solo cambio la posición si la casilla es accesible.
            if (CasillaAccesibleTécnico(st, terreno, altura)){
                next = NextCasillaTécnico(st);
            }
            break;
        
        case TURN_SR: // Giro a la derecha (sentido horario).
            next.site.brujula = (Orientacion) ((next.site.brujula + 1) % 8);
            break;

        case TURN_SL: // Giro a la izquierda (sentido antihorario).
            next.site.brujula = (Orientacion) ((next.site.brujula + 7) % 8);
            break;
    }
    return next;
}

/************************************************************************************/


 list <Action> ComportamientoTecnico::B_Anchura (const EstadoT &inicio, const EstadoT &fin, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){
    
    NodoT current_node;
    list<NodoT> frontier;
    list<NodoT> explored;
    list<Action> path;
    
    current_node.estado = inicio;
    frontier.push_back(current_node);
    bool SolutionFound = (current_node.estado.site.f == fin.site.f and 
                          current_node.estado.site.c == fin.site.c);
                        
    while (!SolutionFound and !frontier.empty()) {
        // eliminar el estado actual de la lista de pendientes de explorar y meterlo en la lista de explorados 
    	frontier.pop_front();
    	explored.push_back(current_node);
    	
    	// Generar todos los descendientes del estado actual 
    	
    	// Compruebo si estoy en una casilla que da las zapatillas
    	if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D') {
    	    current_node.estado.zapatillas = true;
    	}
    	
    	//Genero el hijo resultante de aplicar la accion WALK
    	NodoT child_Walk = current_node;
	child_Walk.estado = applyT(WALK, current_node.estado, terreno, altura);

	if (child_Walk.estado.site.f == fin.site.f and 
	    child_Walk.estado.site.c == fin.site.c) {
	    // El hijo generado es solucion
	    child_Walk.secuencia.push_back(WALK);
	    current_node = child_Walk;
	    SolutionFound = true; 
	}
	else if (!Find(child_Walk, frontier) and !Find(child_Walk, explored)) {
	  // Se mete en la lista de frontier después de añadir a secuencia la acción
	  child_Walk.secuencia.push_back(WALK);
	  frontier.push_back(child_Walk);
	}
	
	if (!SolutionFound){
	  // El hijo resultante de aplicar la accion TURN_SR
	  NodoT child_TurnSR = current_node;
	  child_TurnSR.estado = applyT(TURN_SR, current_node.estado, terreno, altura);
	  
	  if (!Find(child_TurnSR, frontier) and !Find(child_TurnSR, explored)) {
	     child_TurnSR.secuencia.push_back(TURN_SR);
	     frontier.push_back(child_TurnSR);
	  }
		
	  // El hijo resultante de aplicar la accion TURN_SL
	  NodoT child_TurnSL = current_node;
	  child_TurnSL.estado = applyT(TURN_SL, current_node.estado, terreno, altura);
	  
	  if (!Find(child_TurnSL, frontier) and !Find(child_TurnSL, explored)) {
	     child_TurnSL.secuencia.push_back(TURN_SL);
	     frontier.push_back(child_TurnSL);
	  }
	
	}
	
	// Paso a evaluar el siguiente nodo en la lista "frontier"
	if (!SolutionFound and !frontier.empty()) {
	  current_node = frontier.front();
          SolutionFound = (current_node.estado.site.f == fin.site.f and current_node.estado.site.c ==fin.site.c);
	}
    }
    
    if (SolutionFound)
        path = current_node.secuencia;
        
    return path;
    
 }


/********************************************************************************************/


 list <Action> ComportamientoTecnico::B_Anchura_V2(const EstadoT &inicio, const EstadoT &fin, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){
    
    NodoT current_node;
    list<NodoT> frontier;
    set<NodoT> explored;
    list<Action> path;
    
    current_node.estado = inicio;
    frontier.push_back(current_node);
    bool SolutionFound = (current_node.estado.site.f == fin.site.f and 
                          current_node.estado.site.c == fin.site.c);
                        
    while (!SolutionFound and !frontier.empty()) {
        // eliminar el estado actual de la lista de pendientes de explorar y meterlo en la lista de explorados 
    	frontier.pop_front();
    	explored.insert(current_node);
    	
    	// Generar todos los descendientes del estado actual 
    	
    	// Compruebo si estoy en una casilla que da las zapatillas
    	if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D') {
    	    current_node.estado.zapatillas = true;
    	}
    	
    	//Genero el hijo resultante de aplicar la accion WALK
    	NodoT child_Walk = current_node;
	child_Walk.estado = applyT(WALK, current_node.estado, terreno, altura);

	if (child_Walk.estado.site.f == fin.site.f and 
	    child_Walk.estado.site.c == fin.site.c) {
	    // El hijo generado es solucion
	    child_Walk.secuencia.push_back(WALK);
	    current_node = child_Walk;
	    SolutionFound = true; 
	}
	else if (explored.find(child_Walk) == explored.end()) {
	  // Se mete en la lista de frontier después de añadir a secuencia la acción
	  child_Walk.secuencia.push_back(WALK);
	  frontier.push_back(child_Walk);
	}
	
	if (!SolutionFound){
	  // El hijo resultante de aplicar la accion TURN_SR
	  NodoT child_TurnSR = current_node;
	  child_TurnSR.estado = applyT(TURN_SR, current_node.estado, terreno, altura);
	  
	  if (explored.find(child_TurnSR) == explored.end()) {
	     child_TurnSR.secuencia.push_back(TURN_SR);
	     frontier.push_back(child_TurnSR);
	  }
		
	  // El hijo resultante de aplicar la accion TURN_SL
	  NodoT child_TurnSL = current_node;
	  child_TurnSL.estado = applyT(TURN_SL, current_node.estado, terreno, altura);
	  
	  if (explored.find(child_TurnSL) == explored.end()) {
	     child_TurnSL.secuencia.push_back(TURN_SL);
	     frontier.push_back(child_TurnSL);
	  }
	
	}
	
	// Paso a evaluar el siguiente nodo en la lista "frontier"
	if (!SolutionFound and !frontier.empty()) {
	  current_node = frontier.front();
          while(explored.find(current_node) != explored.end() and !frontier.empty()){
          	frontier.pop_front();
          	current_node = frontier.front();
          }
	}
    }
    
    if (SolutionFound)
        path = current_node.secuencia;
        
    return path;
    
 }


// =========================================================================
// Nivel 3
// =========================================================================


 /**
  * HEURÍSTICA: Estimación del coste restante.
  * Uso la distancia de Chebyshev porque permite movimientos en 8 direcciones, 
  * asumiendo un coste mínimo de 1 por casilla.
  */
  int ComportamientoTecnico::CalcularHeuristica(const EstadoT &actual, const EstadoT &fin) {
    return std::max(abs(actual.site.f - fin.site.f), abs(actual.site.c - fin.site.c));
  }
/************************************************************************************/

 /**
  * El Técnico se cansa más subiendo (+5) y ahorra bajando (-2).
  */
  int ComportamientoTecnico::Coste_Walk(const EstadoT &actual, const EstadoT &siguiente, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {
    char casilla_actual = terreno[actual.site.f][actual.site.c];
    int desnivel = (int)altura[siguiente.site.f][siguiente.site.c] - (int)altura[actual.site.f][actual.site.c];

    int mod_altura = (desnivel > 0) ? 5 : (desnivel < 0 ? -2 : 0);

    // El coste base depende del terreno: Agua (60), Hierba (6), Suelo (3).
    if (casilla_actual == 'A') return 60 + mod_altura;
    if (casilla_actual == 'H') return 6 + mod_altura;
    if (casilla_actual == 'S') return 3 + mod_altura;
    
    return 1; // Otros terrenos (como calle) son óptimos.
}

/************************************************************************************/

 /**
  * COSTE DE GIRO: Rotar sobre sí mismo también consume.
  */
  int ComportamientoTecnico::Coste_Turn(const EstadoT &actual, const vector<vector<unsigned char>> &terreno) {
    char casilla_actual = terreno[actual.site.f][actual.site.c];
    if (casilla_actual == 'A') return 5;
    if (casilla_actual == 'H') return 2;
    return 1; // Girar en suelo o calle es muy barato.
  }

/************************************************************************************/


list<Action> ComportamientoTecnico::A_Estrella_Nivel3(const EstadoT &inicio, const EstadoT &fin, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {
    
    priority_queue<NodoT, vector<NodoT>, ComparaF_NodoT> frontier;
    map<EstadoT, int> explored; // Guardamos el estado y la 'g' (energía) mínima con la que llegamos a él
    
    NodoT current_node;
    current_node.estado = inicio;
    current_node.g = 0;
    // La Heurística: Distancia máxima en filas o columnas (Chebyshev) * 1 (coste mínimo)
    current_node.h = 0;
    
    frontier.push(current_node);
    
while (!frontier.empty()) {
    current_node = frontier.top(); 
    frontier.pop();

    // 1. COMPROBAR META
    if (current_node.estado.site.f == fin.site.f && current_node.estado.site.c == fin.site.c) {
        return current_node.secuencia; 
    }

    // 2. ACTUALIZAR EQUIPO EN EL ACTO
    // Si la casilla donde estoy parado tiene zapatillas, mi estado actual las tiene.
    if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D') {
        current_node.estado.zapatillas = true;
    }

    // 3. REGISTRO EN EXPLORED (Ahora con el estado actualizado correctamente)
    // Gracias a tu operator<, esto diferencia (f,c,brujula, false) de (f,c,brujula, true)
    if (explored.find(current_node.estado) != explored.end() && explored[current_node.estado] <= current_node.g) {
        continue; 
    }
    explored[current_node.estado] = current_node.g;

    // --- GENERACIÓN DE HIJOS ---

    // HIJO WALK
    NodoT child_Walk = current_node; 
    // Al hacer esto, child_Walk ya hereda las zapatillas actualizadas de current_node
    child_Walk.estado = applyT(WALK, child_Walk.estado, terreno, altura);
    
    if (!(child_Walk.estado == current_node.estado)) {
        child_Walk.secuencia.push_back(WALK);
        child_Walk.g = current_node.g + Coste_Walk(current_node.estado, child_Walk.estado, terreno, altura);
        child_Walk.h = CalcularHeuristica(child_Walk.estado, fin);
        
        // Poda preventiva: si ya conocemos un camino mejor a este nuevo estado, no lo encolamos
        if (explored.find(child_Walk.estado) == explored.end() || explored[child_Walk.estado] > child_Walk.g) {
            frontier.push(child_Walk);
        }
    }

    // HIJO TURN_SR
    NodoT child_TurnSR = current_node;
    child_TurnSR.estado = applyT(TURN_SR, child_TurnSR.estado, terreno, altura);
    child_TurnSR.secuencia.push_back(TURN_SR);
    child_TurnSR.g = current_node.g + Coste_Turn(current_node.estado, terreno);
    child_TurnSR.h = CalcularHeuristica(child_TurnSR.estado, fin);
    
    if (explored.find(child_TurnSR.estado) == explored.end() || explored[child_TurnSR.estado] > child_TurnSR.g) {
        frontier.push(child_TurnSR);
    }

    // HIJO TURN_SL
    NodoT child_TurnSL = current_node;
    child_TurnSL.estado = applyT(TURN_SL, child_TurnSL.estado, terreno, altura);
    child_TurnSL.secuencia.push_back(TURN_SL);
    child_TurnSL.g = current_node.g + Coste_Turn(current_node.estado, terreno);
    child_TurnSL.h = CalcularHeuristica(child_TurnSL.estado, fin);
    
    if (explored.find(child_TurnSL.estado) == explored.end() || explored[child_TurnSL.estado] > child_TurnSL.g) {
        frontier.push(child_TurnSL);
    }
}
    return list<Action>(); // Falla si no encuentra camino
}



// =========================================================================
// Nivel 4
// =========================================================================


// =========================================================================
// Nivel 5
// =========================================================================

Action ComportamientoTecnico::FaseAguardar(Sensores sensores) {
    if (sensores.venpaca) {
        // Guardamos las coordenadas que nos dicta el sensor
        f_meta_t = sensores.GotoF;
        c_meta_t = sensores.GotoC;
        fase_t = TRASLADO_A_POSICION;
        hayPlan = false; // Forzamos nueva ruta
    }
    return IDLE;
}

Action ComportamientoTecnico::FaseTraslado(Sensores sensores) {
    // 1. ¿Estamos ya en el sitio?
    int distancia_restante = abs(sensores.posF - f_meta_t) + abs(sensores.posC - c_meta_t);

    if (distancia_restante == 0) {
        hayPlan = false;
        
        // Si ya estamos en el sitio, buscamos al ingeniero para encararlo
        if (sensores.agentes[2] == 'i') {
            if (sensores.enfrente) {
                fase_t = SINCRONIZAR_NIVELADO;
            }
            return IDLE;
        } else {
            return TURN_SR; // Giramos buscando al jefe
        }
    }

    // 2. Si no hemos llegado, navegamos
    if (sensores.choque) hayPlan = false;

    EstadoT inicio = {sensores.posF, sensores.posC, sensores.rumbo, tiene_zapatillas};
    // El técnico suele ir orientado al norte por defecto en la meta si no importa el rumbo
    EstadoT meta = {f_meta_t, c_meta_t, norte, false}; 

    if (!hayPlan || (estadoDestino.site.f != meta.site.f || estadoDestino.site.c != meta.site.c)) {
        
        int f_ing = -1, c_ing = -1;
        unsigned char celda_original = ' ';

        // --- Lógica de esquivado usando la función DELANTE ---
        if (sensores.agentes[2] == 'i') {
            // Obtenemos la casilla de enfrente donde está el ingeniero
		ubicacion posicion_actual = {sensores.posF, sensores.posC, sensores.rumbo};
		ubicacion casilla_ing = Delante(posicion_actual);
            f_ing = casilla_ing.f;
            c_ing = casilla_ing.c;

            // Si el ingeniero no está justo en nuestra meta, lo marcamos como muro temporalmente
            if (f_ing != meta.site.f || c_ing != meta.site.c) {
                celda_original = mapaResultado[f_ing][c_ing];
                mapaResultado[f_ing][c_ing] = 'M'; 
            } else {
                f_ing = -1; // No lo marcamos si está en el destino
            }
        }

        // Calculamos plan con A* (o la función que uses en el técnico)
        plan = A_Estrella_Nivel3(inicio, meta, mapaResultado, mapaCotas);

        // Restauramos el mapa
        if (f_ing != -1) {
            mapaResultado[f_ing][c_ing] = celda_original;
        }

        if (!plan.empty()) {
            hayPlan = true;
            estadoDestino = meta;
        }
    }

    // 3. Ejecutar movimiento con precaución
    if (hayPlan && !plan.empty()) {
        Action sig_accion = plan.front();
        
        // Pausa táctica: si vamos a caminar y el ingeniero se ha cruzado, esperamos
        if (sig_accion == WALK && sensores.agentes[2] == 'i') {
            hayPlan = false; 
            return IDLE; 
        }

        plan.pop_front();
        return sig_accion;
    }

    return IDLE;
}

Action ComportamientoTecnico::FaseSincronizar(Sensores sensores) {
    // Si perdemos al ingeniero de vista, volvemos a buscarlo
    if (sensores.agentes[2] != 'i' || !sensores.enfrente) {
        fase_t = TRASLADO_A_POSICION;
        return IDLE;
    }
    
    // Turno de espera activa: el ingeniero está preparando el suelo
    fase_t = EJECUTAR_INSTALACION;
    return IDLE;
}

Action ComportamientoTecnico::FaseEjecutar(Sensores sensores) {
    if (sensores.enfrente && sensores.agentes[2] == 'i') {
        // ¡Instalación simultánea!
        fase_t = AGUARDAR_LLAMADA;
        return INSTALL;
    } else {
        // Si el ingeniero se movió antes de tiempo, volvemos a navegación
        fase_t = TRASLADO_A_POSICION;
        return IDLE;
    }
}


// =========================================================================
// Nivel 6
// =========================================================================

/************************************************************************************/

// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoTecnico::ActualizarMapa(Sensores sensores) {
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo) {
    case norte:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
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
        for (int i = -j; i <= j; i++) {
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
        for (int i = -j; i <= j; i++) {
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
        for (int i = -j; i <= j; i++) {
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



/**
 * @brief Determina si una casilla es transitable para el técnico.
 * En esta práctica, si el técnico tiene zapatillas, el bosque ('B') es transitable.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable.
 */
bool ComportamientoTecnico::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas) {
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size()) return false;
  return es_camino(mapaResultado[f][c]);  // Solo 'C', 'S', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el técnico: desnivel máximo siempre 1.
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoTecnico::EsAccesiblePorAltura(const ubicacion &actual) {
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size()) return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (desnivel > 1) return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoTecnico::Delante(const ubicacion &actual) const {
  ubicacion delante = actual;
  switch (actual.brujula) {
    case 0: delante.f--; break;                        // norte
    case 1: delante.f--; delante.c++; break;     // noreste
    case 2: delante.c++; break;                     // este
    case 3: delante.f++; delante.c++; break;     // sureste
    case 4: delante.f++; break;                        // sur
    case 5: delante.f++; delante.c--; break;     // suroeste
    case 6: delante.c--; break;                     // oeste
    case 7: delante.f--; delante.c--; break;     // noroeste
  }
  return delante;
}


/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::PintaPlan(const list<Action> &plan)
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



/**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa 2D.
 *
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::VisualizaPlan(const ubicacion &st,
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


