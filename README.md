# Autonomous AI Agents — Reactive & Deliberative Map Navigation

Cooperative autonomous artificial intelligence agents (**Engineer** and **Technician**) developed in C++ for 2D grid-world exploration, terrain-aware navigation, resource management, and optimal pathfinding in a simulated environment.

## Repository Structure & My Contributions

- **`/Comportamientos_Agentes` (Implemented by me):**
  - **Reactive Agent Behavior:** Sensor-driven decision-making for unknown map exploration, obstacle avoidance, battery recharging, and terrain adaptation without prior map knowledge.
  - **Deliberative Agent & Search Algorithms:** Goal-directed planning and optimal pathfinding algorithms (**BFS**, **Dijkstra / Uniform-Cost Search**, and **A\***) accounting for movement energy costs across different terrain types and orientation states.
- **`/src`, `/bin_src`, `/include` & `/lib`:** Simulation engine, perception/action loop, OpenGL visualization, and core headers.
- **`/mapas` & `/ply`:** Benchmark grid maps (`.map`) and 3D polygonal models (`.ply`) for the graphical simulator.
- **Automated Testing & Evaluation:**
  - `run_tests.sh` & `test0.sh`: Bash scripts for automated headless benchmarking and terminal output verification across maps and scenarios.
  - `Autoevaluacion_practica2.pdf`: Experimental evaluation report analyzing exploration coverage, energy consumption, and path optimality.

## Tech Stack

- **Languages:** C++ (C++17), Bash
- **Libraries & Build System:** CMake (`CMakeLists.txt`), OpenGL / GLUT
- **Core Concepts:** Autonomous Agents, Reactive vs. Deliberative Architectures, State-Space Search, Dijkstra, A* Heuristic Search

## Prerequisites (Linux / Ubuntu)

Install the build toolchain and OpenGL/GLUT dependencies (or run `./install.sh`):

```bash
sudo apt update
sudo apt install build-essential cmake libgl1-mesa-dev freeglut3-dev
```

## How to Build & Run

1. Clone the repository:
   ```bash
   git clone [https://github.com/Manucantero/ai-autonomous-agents.git](https://github.com/Manucantero/ai-autonomous-agents.git)
   cd ai-autonomous-agents
   ```

2. Build both targets (GUI `practica2` and headless `practica2SG`):
   ```bash
   mkdir -p build && cd build
   cmake ..
   make
   cp practica2 practica2SG ..
   cd ..
   ```

3. Run a specific scenario (GUI or headless terminal mode):
   ```bash
   # Run with OpenGL graphical interface (practica2)
   ./practica2 -m ./mapas/mapa100.map -n 1 -i 47 54 0 -t 48 54 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000

   # Run in headless terminal-only mode (practica2SG)
   ./practica2SG -m ./mapas/mapa100.map -n 1 -i 47 54 0 -t 48 54 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
   ```

   **CLI Parameters:**
   - `-m`: Path to the `.map` environment file.
   - `-n`: Scenario / level identifier.
   - `-i`: Initial position and orientation of the **Engineer** agent (`row col orientation`).
   - `-t`: Initial position and orientation of the **Technician** agent (`row col orientation`).
   - `-seed`: Random number generator seed for reproducible runs.
   - `-Tiempo`, `-Ambiental`, `-Energia`: Maximum simulation steps, environmental steps, and initial energy budget.

4. Run the automated headless test scripts:
   ```bash
   chmod +x run_tests.sh test0.sh
   ./run_tests.sh
   ```
