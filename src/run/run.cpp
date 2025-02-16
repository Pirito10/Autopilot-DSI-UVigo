#include <unistd.h>
#include <iostream>
#include <thread>
#include <filesystem>
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

    // Obtenemos la ruta a la raíz del proyecto
    std::filesystem::path projectRoot = std::filesystem::absolute(std::filesystem::path(argv[0])).parent_path().parent_path().parent_path().parent_path().parent_path();

    // Lista de rutas a los ejecutables
    std::vector<std::string> modulos = {
        (projectRoot / "src/modules/build/attitude/attitude").string(),
        (projectRoot / "src/modules/build/radio/radio").string(),
        (projectRoot / "src/modules/build/autopilot/autopilot").string(),
        (projectRoot / "src/modules/build/mixer/mixer").string(),
        (projectRoot / "src/modules/build/motor/motor").string()};

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