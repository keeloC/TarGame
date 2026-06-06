# Yeamaracuyea - Sandbox 2D en C++

Un videojuego 2D de minería, exploración y construcción desarrollado desde cero utilizando C++ y la biblioteca gráfica SFML. El proyecto destaca por la implementación manual de lógicas de físicas, generación procedural de terreno mediante funciones trigonométricas y un sistema de inventario interactivo.

## 🚀 Características Principales

*   **Generación Procedural Matemática:** El mundo no es estático; se genera dinámicamente utilizando ondas seno y coseno para crear relieves naturales (montañas y valles), implementado mediante un sistema de dos pasadas (Two-Pass Generation) para colocar la vegetación correctamente usando mapas de altura.
*   **Físicas y Colisiones Propias:** Sistema de gravedad, saltos, sprint y detección de colisiones calculada iterativamente contra la matriz del mundo, evitando que el jugador atraviese los bloques sólidos.
*   **Minería y Sistema de Herramientas:** Interacción con el entorno mediante herramientas con atributos específicos. El puño, la pala y el pico tienen diferentes velocidades de minado (`Velminado`) y niveles de capacidad (`Nivelminado`) para romper distintos tipos de bloques (tierra, piedra, madera, hojas).
*   **Inventario y Drag & Drop:** 
    *   Barra de acceso rápido (Hotbar) interactiva.
    *   Inventario principal expansivo.
    *   Sistema fluido de arrastrar y soltar (Drag & Drop) programado manualmente para mover objetos entre la barra rápida, el inventario y la mesa de trabajo.
    *   Apilamiento (stacking) visual de bloques recogidos con contadores numéricos actualizados en tiempo real.
*   **Alquimia / Crafteo:** Lógica de combinación de objetos. Combinar recursos específicos en los *slots* de alquimia genera nuevas herramientas (ej. 3 maderas crean una pala, 3 piedras crean un pico).
*   **Optimización de Renderizado:** Implementación de *culling* espacial básico. El bucle de renderizado calcula matemáticamente qué porción del mapa está visible en la cámara actual del jugador y solo dibuja esos bloques, ahorrando drásticamente recursos del sistema.

## 🛠️ Tecnologías y Dependencias

*   **Lenguaje:** C++
*   **Biblioteca Gráfica:** [SFML](https://www.sfml-dev.org/) (Simple and Fast Multimedia Library)
*   **Matemáticas:** `<math.h>` para algoritmos de trigonometría en la generación del mundo.
*   **Estructuras de Datos:** `<map>` y *arrays* bidimensionales para la gestión de datos del mundo y crafteos.

## 🎮 Controles del Juego

| Acción | Tecla / Ratón |
| :--- | :--- |
| **Moverse a la Izquierda** | `A` |
| **Moverse a la Derecha** | `D` |
| **Saltar** | `Espacio` |
| **Correr (Sprint)** | Mantener `Shift Izquierdo` |
| **Abrir/Cerrar Inventario** | `E` |
| **Seleccionar Hotbar** | `1` al `9` |
| **Minar Bloque** | Mantener `Clic Izquierdo` |
| **Mover Objetos** | `Clic Izquierdo` (Arrastrar) |

## 🏗️ Estructura del Código

El proyecto se basa en una arquitectura de clases ligeras y matrices para la gestión del estado:
*   `mundo[FILAS][COLS]`: Matriz principal que almacena los IDs de los bloques (0=Aire, 1=Tierra, 2=Piedra, 3=Madera, 4=Hojas, 5=Pasto).
*   `Slot`: Gestiona el ID del ítem y su cantidad para los sistemas de almacenamiento.
*   `Arrastre`: Controla la máquina de estados cuando el usuario hace clic y mueve un objeto por la interfaz gráfica.

## 👨‍💻 Autor

**Keelo.C**  
*Desarrollo de lógicas, físicas y generación procedural.*
