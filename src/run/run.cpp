#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>

// Función que ejecuta un comando en una terminal
void ejecutarModulo(const std::string &comando)
{
    std::system(comando.c_str());
}

int main(int argc, char *argv[])
{
    // Ejecución obligatoria como administrador
    if (getuid())
    {
        fprintf(stderr, "Ejecuta el programa como administrador: sudo %s\n", argv[0]);
        return 1;
    }

    // Lista de rutas a los ejecutables
    std::vector<std::string> modulos = {
        "/home/pi/dron/modules/build/attitude/attitude",
        "/home/pi/dron/modules/build/radio/radio",
        "/home/pi/dron/modules/build/autopilot/autopilot",
        "/home/pi/dron/modules/build/mixer/mixer",
        "/home/pi/dron/modules/build/motor/motor"};

    // Vector de hilos para mantener los módulos ejecutándose simultáneamente
    std::vector<std::thread> threads;

    // Creamos un hilo para cada módulo
    for (const auto &modulo : modulos)
    {
        threads.emplace_back(ejecutarModulo, modulo);
    }

    // Unimos los hilos para que el programa espere a que terminen
    for (auto &thread : threads)
    {
        thread.join();
    }

    return 0;
}
