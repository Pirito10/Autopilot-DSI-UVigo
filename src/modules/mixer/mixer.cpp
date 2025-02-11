#include <unistd.h>
#include <thread>
#include <zenohc.hxx>

using namespace zenohc;

// Asignación para cada motor
#define MOTOR_RL 0
#define MOTOR_FR 1
#define MOTOR_RR 2
#define MOTOR_FL 3

// Ratios para el reparto de potencia
#define THROTTLE_RATIO 0.6f // 60% para el acelerador
#define PID_RATIO 0.25f     // 25% combinado para ángulos roll y pitch
#define YAW_RATIO 0.15f     // 15% para ángulo yaw

// Valores mínimos y máximos de potencia
#define MIN_POWER 1150
#define MAX_POWER 1924

// Función para procesar los datos recibidos y publicar el resultado
void process(float throttle, float yaw, float roll, float pitch, Publisher &pub)
{
    roll = abs(roll) > 0.05 ? roll : 0;
    pitch = abs(pitch) > 0.05 ? pitch : 0;
    yaw = abs(yaw) > 0.05 ? yaw : 0;
    // Inicializamos las variables
    float roll_motores[4], pitch_motores[4], yaw_motores[4], motores[4];

    // Obtenemos el valor de roll para cada motor, invirtiendo derecha/izquierda
    roll_motores[MOTOR_RL] = roll;
    roll_motores[MOTOR_FR] = -roll;
    roll_motores[MOTOR_RR] = -roll;
    roll_motores[MOTOR_FL] = roll;

    // Obtenemos el valor de pitch para cada motor, invirtiendo delante/atrás
    pitch_motores[MOTOR_RL] = pitch;
    pitch_motores[MOTOR_FR] = -pitch;
    pitch_motores[MOTOR_RR] = pitch;
    pitch_motores[MOTOR_FL] = -pitch;

    // Obtenemos el valor de yaw para cada motor, invirtiendo diagonalmente
    yaw_motores[MOTOR_RL] = -yaw;
    yaw_motores[MOTOR_FR] = -yaw;
    yaw_motores[MOTOR_RR] = yaw;
    yaw_motores[MOTOR_FL] = yaw;

    // Recorremos cada motor
    for (int i = 0; i < 4; i++)
    {
        // Eliminamos las contribuciones negativas
        roll_motores[i] = roll_motores[i] > 0 ? roll_motores[i] : 0;
        pitch_motores[i] = pitch_motores[i] > 0 ? pitch_motores[i] : 0;
        yaw_motores[i] = yaw_motores[i] > 0 ? yaw_motores[i] : 0;

        // Si la suma de la contribución de roll y pitch es superior a su ratio combinado, limitamos cada contribución a la mitad del máximo ratio permitido
        if (roll_motores[i] + pitch_motores[i] > PID_RATIO)
        {
            roll_motores[i] = PID_RATIO / 2;
            pitch_motores[i] = PID_RATIO / 2;
        }

        // Sumamos cada valor correspondientemente reescalado a su porcentaje de potencia
        motores[i] = (roll_motores[i]) + (pitch_motores[i]) + (yaw_motores[i] * YAW_RATIO) + (throttle * THROTTLE_RATIO);

        // Convertimos el resultado en pulsos PWM
        motores[i] = MIN_POWER + (motores[i] * (MAX_POWER - MIN_POWER));
    }

    // Asignamos los valores a un string y los publicamos
    std::string data = std::to_string(motores[MOTOR_RL]) + "," + std::to_string(motores[MOTOR_FR]) + "," + std::to_string(motores[MOTOR_RR]) + "," + std::to_string(motores[MOTOR_FL]);
    pub.put(data);

    // printf("Publicando: FR -> %f | FL -> %f | RL -> %f | RR -> %f\n", motores[MOTOR_FR], motores[MOTOR_FL], motores[MOTOR_RL], motores[MOTOR_RR]);
}

int main()
{
    // Variables para almacenar los datos recibidos
    float throttle, yaw, roll, pitch;

    // Configuración y apertura de sesión Zenoh
    Config config;
    auto session = expect<Session>(open(std::move(config)));

    // Creamos el publisher (tópico motores)
    auto pub = expect<Publisher>(session.declare_publisher("motores"));

    // Nos suscribimos al tópico pos_deseada
    auto sub_pos_deseada = expect<Subscriber>(session.declare_subscriber("pos_deseada", [&](const Sample &sample)
                                                                         {
                                                                             {
                                                                                 // Leemos los datos del tópico pos_deseada
                                                                                 std::string pos_deseada = std::string(sample.get_payload().as_string_view());
                                                                                 std::sscanf(pos_deseada.c_str(), "%f,%*f,%*f,%f", &throttle, &yaw);
                                                                             }
                                                                             // Procesamos los datos y publicamos el resultado
                                                                             process(throttle, yaw, roll, pitch, pub); }));

    // Nos suscribimos al tópico pid
    auto sub_pid = expect<Subscriber>(session.declare_subscriber("pid", [&](const Sample &sample)
                                                                 {
                                                                     {
                                                                         // Leemos los datos del tópico pid
                                                                         std::string pid = std::string(sample.get_payload().as_string_view());
                                                                         std::sscanf(pid.c_str(), "%f,%f", &roll, &pitch);
                                                                     }
                                                                     // Procesamos los datos y publicamos el resultado
                                                                     process(throttle, yaw, roll, pitch, pub); }));

    // Mantenemos el programa en ejecución
    std::this_thread::sleep_for(std::chrono::hours(24));
}