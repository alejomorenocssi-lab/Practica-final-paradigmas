# Práctica 2
**Curso:** SI1002-7 Lenguajes y Paradigmas de Computación, 2026-2, Universidad EAFIT

## Integrantes
- David Alejandro Castaño Moreno
- Brayan Grisales Perez

## Cómo compilar y ejecutar
```bash
g++ -std=c++17 -Wall -Wextra -o practice main.cpp
./practice
```

## Respuestas a las preguntas de la defensa

### Pregunta 1: ¿Qué pasa si el destructor de TransportMode no es virtual?
Nuestra `Network` guarda sus modos como `std::unique_ptr<TransportMode>`, así que cuando la red se destruye, cada `unique_ptr` borra un objeto derivado (un `MetroLine`, un `TouristCable`, etc.) a través de un puntero a la clase base.

Si `TransportMode()` no es virtual, borrar un objeto derivado a través de un puntero base es comportamiento indefinido según el estándar de C++. En la práctica solo se ejecuta el destructor de la base: el destructor de la clase derivada se omite, y lo que esa clase posea no se libera correctamente.

En este programa el daño sería fácil de pasar por alto, porque las clases derivadas solo tienen un `long long`. Pero el comportamiento sigue siendo indefinido, y la primera clase derivada que posea un recurso (un buffer, un archivo, otro puntero inteligente) lo dejaría sin liberar. Declarar el destructor virtual hace que C++ consulte en tiempo de ejecución el tipo real del objeto y ejecute los destructores en el orden correcto, de la clase más derivada hasta la base. Por eso es un requisito de correctitud y no una formalidad.

> *Nota de aclaración:* Aunque la guía menciona al final que el README debe tener la respuesta a las preguntas de defensa, no especifica un número estricto. Por eso respondimos las 3 preguntas clave abordadas en el diseño conceptual del proyecto (el destructor virtual, la clase abstracta frente a la enumeración con `switch`, y el STL frente al bucle escrito a mano).

### Pregunta 2: ¿Por qué es preferible una clase abstracta con funciones virtuales puras a una enumeración con un switch?
Con una enumeración y un `switch`, el conocimiento sobre cada modo queda disperso: un `switch` para la tarifa, otro para la descripción, otro para la política de integración, otro para la tarifa plana del estudiante. Agregar un modo nuevo obliga a editar todos esos lugares, y el compilador no avisa si se olvida uno; el programa simplemente se comporta mal.

Con una clase abstracta, todo lo que hace diferente a un modo vive en una sola clase. Agregar un modo nuevo significa escribir una clase y no tocar nada más. Además, declarar `baseFare()` y `serviceType()` como virtuales puras traslada el error a tiempo de compilación: una clase que olvide implementarlas sigue siendo abstracta y no se puede instanciar.

Nuestro código lo muestra: `TouristCable` y `FeederRoute` se diferencian de los demás modos solo por sobrescribir algunos ganchos, y ni `chargeItinerary`, ni `Passenger`, ni los reportes comprueban de qué clase es un modo. Solo le hacen preguntas al objeto (`allowsIntegration()`, `opensIntegrationWindow()`, `allowsStudentFlatFare()`), y cada objeto responde por sí mismo.

La contrapartida es que el polimorfismo facilita agregar nuevos tipos, pero hace más difícil agregar una nueva operación que afecte a todos los tipos, porque hay que editar cada clase. Para este problema, donde lo esperable es que aparezcan modos y perfiles nuevos, el polimorfismo es la mejor opción.

### Pregunta 3: ¿Qué aporta el STL cuando se reemplaza un bucle escrito a mano por un algoritmo y una lambda?
Un bucle escrito a mano mezcla dos cosas: cómo recorrer y qué hacer con cada elemento. Un algoritmo como `std::accumulate`, `std::count_if` o `std::for_each` expresa la intención en su nombre, y la lambda aporta solo la parte que varía. Eso significa menos estado mutable, ningún error de índices o de inicialización del acumulador, y un código que se lee como lo que hace: sumar, contar, agrupar.

Además permite inyectar el criterio desde afuera. `namesMatching` recibe una `std::function<bool(const TransportMode&)>`, así que la red no necesita conocer el criterio: `main` le pasa "más de 8 estaciones" y mañana podría pasarle otro sin cambiar la función.

En nuestro código, los cinco reportes usan algoritmos y no tienen bucles. El único bucle explícito del programa que escribimos está en `chargeItinerary`, y se mantiene porque ahí el bucle es el algoritmo mismo: arrastra estado (la ventana de integración) de un tramo al siguiente.
g++ -std=c++17 -Wall -Wextra -o practice main.cpp
./practice
Respuestas a las preguntas de la defensa
Pregunta 1: ¿Qué pasa si el destructor de TransportMode no es virtual?
Nuestra Network guarda sus modos como std::unique_ptr<TransportMode>, así que
cuando la red se destruye, cada unique_ptr borra un objeto derivado (un MetroLine, un
TouristCable, etc.) a través de un puntero a la clase base.[project_readme.md](https://github.com/user-attachments/files/34581007/project_readme.md)
