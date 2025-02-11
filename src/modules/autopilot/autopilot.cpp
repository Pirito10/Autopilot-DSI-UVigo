#include <unistd.h>
#include <chrono>
#include <thread>
#include <zenohc.hxx>

using namespace zenohc;

// Valores mínimos y máximos de los resultados. Esperamos los valores ideales entre -30º y 30º, y damos un margen de 15º
#define MIN_VALUE -45.0f
#define MAX_VALUE 45.0f

// Estructura para almacenar constantes y variables internas de cada PID
struct PIDConfig
{
    // Constantes
    float Kp;
    float Ki;
    float Kd;

    // Variables internas
    float integral;
    float previous_error;
    std::chrono::steady_clock::time_point previous_time;
};

// Inicialización de las configuraciones de PID para roll y pitch
PIDConfig rollPIDConfig = {1.0, 0.1, 0.05, 0.0, 0.0, std::chrono::steady_clock::now()};
PIDConfig pitchPIDConfig = {1.0, 0.1, 0.05, 0.0, 0.0, std::chrono::steady_clock::now()};

// Función para calcular el PID de un eje
float calcularPID(float angulo_actual, float angulo_deseado, PIDConfig &config)
{
    // Calculamos el error (componente proporcional)
    float error = angulo_deseado - angulo_actual;

    // Calculamos el tiempo transcurrido
    auto current_time = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed_time = current_time - config.previous_time;
    float dt = elapsed_time.count();

    // Calculamos la componente integral
    config.integral += error * dt;

    // Calculamos la componente derivativa
    float derivative = (error - config.previous_error) / dt;

    // Calculamos la salida del PID
    float output = (config.Kp * error) + (config.Ki * config.integral) + (config.Kd * derivative);

    // Actualizamos las variables para la siguiente iteración
    config.previous_error = error;
    config.previous_time = current_time;

    return output;
}

// Función para normalizar los resultados entre -1 y 1
float normalize(float value)
{
    // Aseguramos que el resultado esté dentro del rango esperado
    value = std::clamp(value, MIN_VALUE, MAX_VALUE);

    return 2.0f * (value - MIN_VALUE) / (MAX_VALUE - MIN_VALUE) - 1.0f;
}

// Función para procesar los datos recibidos y publicar el resultado
void process(float roll_actual, float pitch_actual, float roll_deseado, float pitch_deseado, Publisher &pub)
{
    // Calculamos el PID para los ejes roll y pitch
    float u_roll = calcularPID(roll_actual, roll_deseado, rollPIDConfig);
    float u_pitch = calcularPID(pitch_actual, pitch_deseado, pitchPIDConfig);

    // Asignamos los valores normalizados a un string y los publicamos
    std::string data = std::to_string(normalize(u_roll)) + "," + std::to_string(normalize(u_pitch));
    pub.put(data);

    // printf("Publicando: PID Roll -> %f | PID Pitch -> %f\n", normalize(u_roll), normalize(u_pitch));
}

int main()
{
    // Variables para almacenar los datos recibidos
    float roll_actual = 0, pitch_actual = 0, roll_deseado = 0, pitch_deseado = 0;

    // Configuración y apertura de sesión Zenoh
    Config config;
    auto session = expect<Session>(open(std::move(config)));

    // Creamos el publisher (tópico pos_deseada)
    auto pub = expect<Publisher>(session.declare_publisher("pid"));

    // Nos suscribimos al tópico pos_actual
    auto sub_pos_actual = expect<Subscriber>(session.declare_subscriber("pos_actual", [&](const Sample &sample)
                                                                        {
                                                                             {
                                                                                 // Leemos los datos del tópico pos_deseada
                                                                                 std::string pos_actual = std::string(sample.get_payload().as_string_view());
                                                                                 std::sscanf(pos_actual.c_str(), "%f,%f", &roll_actual, &pitch_actual);
                                                                             }
                                                                             // Procesamos los datos y publicamos el resultado
                                                                             process(roll_actual, pitch_actual, roll_deseado, pitch_deseado, pub); }));

    // Nos suscribimos al tópico pos_deseada
    auto sub_pos_deseada = expect<Subscriber>(session.declare_subscriber("pos_deseada", [&](const Sample &sample)
                                                                         {
                                                                             {
                                                                                 // Leemos los datos del tópico pos_deseada
                                                                                 std::string pos_deseada = std::string(sample.get_payload().as_string_view());
                                                                                 std::sscanf(pos_deseada.c_str(), "%*f,%f,%f", &roll_deseado, &pitch_deseado);
                                                                             }
                                                                             // Procesamos los datos y publicamos el resultado
                                                                             process(roll_actual, pitch_actual, roll_deseado, pitch_deseado, pub); }));

    // Nos suscribimos al tópico sub_constantes_pid
    auto sub_constantes_pid = expect<Subscriber>(session.declare_subscriber("constantes_pid", [&](const Sample &sample)

                                                                            {
                                                                                 // Leemos los datos del tópico sub_constantes_pid
                                                                                 std::string constantes_pid = std::string(sample.get_payload().as_string_view());
                                                                                 std::sscanf(constantes_pid.c_str(), "%f,%f,%f,%f,%f,%f", &rollPIDConfig.Kp, &rollPIDConfig.Ki, &rollPIDConfig.Kd, &pitchPIDConfig.Kp, &pitchPIDConfig.Ki, &pitchPIDConfig.Kd); }));

    // Mantenemos el programa en ejecución
    std::this_thread::sleep_for(std::chrono::hours(24));
}
