#include <unistd.h>
#include <zenohc.hxx>

using namespace zenohc;

// Estructura para almacenar las constantes de un eje
struct PIDConstants
{
    float Kp;
    float Ki;
    float Kd;
};

PIDConstants rollPIDConstants;
PIDConstants pitchPIDConstants;

// Función para leer los valores del teclado, y almacenarlos en un struct
void getInput(PIDConstants &constants)
{
    std::cin >> constants.Kp >> constants.Ki >> constants.Kd;
}

int main()
{
    // Configuración y apertura de sesión Zenoh
    Config config;
    auto session = expect<Session>(open(std::move(config)));

    // Creamos el publisher (tópico constantes_pid)
    auto pub = expect<Publisher>(session.declare_publisher("constantes_pid"));

    // Leemos del teclado las tres constantes del eje roll
    printf("Introduce las constantes Kp Ki Kd para el eje roll: ");
    getInput(rollPIDConstants);

    // Leemos del teclado las tres constantes del eje pitch
    printf("Introduce las constantes Kp Ki Kd para el eje pitch: ");
    getInput(pitchPIDConstants);

    while (true)
    {
        // Asignamos los valores a un string y los publicamos
        std::string payload = std::to_string(rollPIDConstants.Kp) + "," +
                              std::to_string(rollPIDConstants.Ki) + "," +
                              std::to_string(rollPIDConstants.Kd) + "," +
                              std::to_string(pitchPIDConstants.Kp) + "," +
                              std::to_string(pitchPIDConstants.Ki) + "," +
                              std::to_string(pitchPIDConstants.Kd);
        pub.put(payload);

        printf("Publicando: Kp Roll -> %f Ki Roll -> %f | Kd Roll -> %f | Kp Pitch -> %f | Ki Pitch -> %f | Kd Pitch -> %f\n", rollPIDConstants.Kp, rollPIDConstants.Ki, rollPIDConstants.Kd, pitchPIDConstants.Kp, pitchPIDConstants.Ki, pitchPIDConstants.Kd);

        // Publicamos cada segundo
        sleep(1);
    }
    return 0;
}