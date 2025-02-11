#include <unistd.h>
#include <Navio2/RCInput_Navio2.h>
#include <Common/Util.h>
#include <zenohc.hxx>

using namespace zenohc;

// Valores mínimos y máximos de los canales
#define MIN_VALUE 1104.0f
#define MAX_VALUE 1924.0f

// Función para normalizar el eje throttle entre 0 y 1
float normalizeThrottle(int value)
{
    return (value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE);
}

// Función para normalizar los ejes roll y pitch entre -30 y 30 (grados)
float normalizeRollPitch(int value)
{
    return 60.0f * (value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE) - 30.0f;
}

// Función para normalizar el eje yaw entre -1 y 1
float normalizeYaw(int value)
{
    return 2.0f * (value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE) - 1.0f;
}

// Función para normalizar el switch D entre -1 y 1
int normalizeSwitchD(int value)
{
    return (value > 1500) ? 1 : -1;
}

// Función para normalizar el switch C entre 0 y 1
int normalizeSwitchC(int value)
{
    return 2.0f * (value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE) - 1.0f;
}

int main(int argc, char *argv[])
{
    // Comprobamos que no esté activado ArduPilot
    if (check_apm())
    {
        return 1;
    }

    // Configuración y apertura de sesión Zenoh
    Config config;
    auto session = expect<Session>(open(std::move(config)));

    // Creamos los publisher (tópico pos_deseada y armado_motores)
    auto pub_pos_deseada = expect<Publisher>(session.declare_publisher("pos_deseada"));
    auto pub_armado_motores = expect<Publisher>(session.declare_publisher("armado_motores"));

    // Obtenemos un puntero al mando, y lo inicializamos
    std::unique_ptr<RCInput> rcin = std::make_unique<RCInput_Navio2>();
    rcin->initialize();

    // Inicializamos las variables
    float throttle, roll, pitch, yaw, switchD, switchC;
    std::string data_pos_deseada, data_armado_motores;

    while (true)
    {
        // Leemos los 6 canales del mando, y normalizamos los valores
        roll = normalizeRollPitch(rcin->read(0));
        pitch = normalizeRollPitch(rcin->read(1)) * -1; // Invertimos el control del eje pitch
        throttle = normalizeThrottle(rcin->read(2));
        yaw = normalizeYaw(rcin->read(3));
        switchD = normalizeSwitchD(rcin->read(4));
        switchC = normalizeSwitchC(rcin->read(5));

        // Asignamos los valores a un string y los publicamos
        data_pos_deseada = std::to_string(throttle) + "," + std::to_string(roll) + "," + std::to_string(pitch) + "," + std::to_string(yaw) + ",";
        pub_pos_deseada.put(data_pos_deseada);

        data_armado_motores = std::to_string(throttle) + "," + std::to_string(switchC) + "," + std::to_string(switchD);
        pub_armado_motores.put(data_armado_motores);

        // printf("Publicando: Throttle -> %f | Roll -> %f | Pitch -> %f | Yaw -> %f | SwitchC -> %f | SwitchD -> %f\n", throttle, roll, pitch, yaw, switchC, switchD);

        // Leemos y publicamos cada 10 milisegundos
        usleep(10000);
    }
    return 0;
}