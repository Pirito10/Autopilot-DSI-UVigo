#include <unistd.h>
#include <chrono>
#include <thread>
#include <Navio2/RCOutput_Navio2.h>
#include <Common/Util.h>
#include <zenohc.hxx>

using namespace zenohc;

// Asignación para cada motor
#define MOTOR_RL 0
#define MOTOR_FR 1
#define MOTOR_RR 2
#define MOTOR_FL 3

// Pulsos de potencia para el armado, mínima y máxima
#define ARM_POWER 800
#define MIN_POWER 1150.0f
#define MAX_POWER 1924.0f

// Variable para controlar el armado de los motores
bool armed = false;

// Puntero al módulo PWM
std::unique_ptr<RCOutput> pwm = std::unique_ptr<RCOutput>{new RCOutput_Navio2()};

// Función para armar los motores
void arm(float throttle, float switchC, float switchD)
{
    // Comprobamos que no se esté ejecutando ArduPilot
    if (check_apm())
    {
        fprintf(stderr, "ArduPilot está activado, por favor, desactívalo\n");
        return;
    }

    // Inicializamos los motores
    if (!(pwm->initialize(MOTOR_RL)) || !(pwm->initialize(MOTOR_FR)) || !(pwm->initialize(MOTOR_RR)) || !(pwm->initialize(MOTOR_FL)))
    {
        fprintf(stderr, "Error al inicializar los motores\n");
        return;
    }

    // Establecemos la frecuencia de los motores a 50Hz
    pwm->set_frequency(MOTOR_RL, 50);
    pwm->set_frequency(MOTOR_FR, 50);
    pwm->set_frequency(MOTOR_RR, 50);
    pwm->set_frequency(MOTOR_FL, 50);

    // Habilitamos los motores
    if (!(pwm->enable(MOTOR_RL)) || !(pwm->enable(MOTOR_FR)) || !(pwm->enable(MOTOR_RR)) || !(pwm->enable(MOTOR_FL)))
    {
        fprintf(stderr, "Error al habilitar los motores\n");
        return;
    }

    // Variable con el momento actual
    auto startTime = std::chrono::high_resolution_clock::now();

    // Ponemos los motores a potencia de armado durante 2 segundos
    while (std::chrono::high_resolution_clock::now() - startTime < std::chrono::seconds(2))
    {
        pwm->set_duty_cycle(MOTOR_RL, ARM_POWER);
        pwm->set_duty_cycle(MOTOR_FR, ARM_POWER);
        pwm->set_duty_cycle(MOTOR_RR, ARM_POWER);
        pwm->set_duty_cycle(MOTOR_FL, ARM_POWER);
        usleep(100000);
    }

    printf("\nPreparado para volar, pon los interruptores superiores hacia delante para apagar los motores\n");
}

// Función para dar potencia a los motores
void power(float motores_PWM[4])
{
    // Aseguramos que las potencias estén dentro del rango permitido
    for (int i = 0; i < 4; i++)
    {
        motores_PWM[i] = std::clamp(motores_PWM[i], MIN_POWER, MAX_POWER);
    }

    // Le damos la potencia correspondiente a cada motor
    pwm->set_duty_cycle(MOTOR_RL, motores_PWM[0]);
    pwm->set_duty_cycle(MOTOR_FR, motores_PWM[1]);
    pwm->set_duty_cycle(MOTOR_RR, motores_PWM[2]);
    pwm->set_duty_cycle(MOTOR_FL, motores_PWM[3]);
}

// Función para procesar los datos recibidos
void process(float motores_PWM[4], float throttle, float switchC, float switchD)
{
    // Comprobamos si los motores están armados
    if (!armed)
    {
        // Si la secuencia es correcta (cero aceleración, ambos switches hacia atrás), armamos los motores
        if (throttle == 0 && switchC == -1 && switchD == -1)
        {
            arm(throttle, switchC, switchD);
            armed = true;
        }
    }
    else
    {
        // Si ambos switches están hacia delante, apagamos los motores
        if (switchC == 1 && switchD == 1)
        {
            printf("\nMotores apagados, vuelve a realizar la secuencia de armado para encenderlos\n");
            armed = false;
        }
        else
        {
            // Le damos la correspondiente potencia a los motores
            power(motores_PWM);
        }
    }
}

int main(int argc, char *argv[])
{
    // Ejecución obligatoria como administrador
    if (getuid())
    {
        fprintf(stderr, "Ejecuta el programa como administrador: sudo %s\n", argv[0]);
        return 1;
    }

    // Delay para que inicien correctamente los demás módulos
    sleep(1);

    // Variables para almacenar los datos recibidos
    float throttle, switchC, switchD, motores_PWM[4];

    // Configuración y apertura de sesión Zenoh
    Config config;
    auto session = expect<Session>(open(std::move(config)));

    // Nos suscribimos al tópico armado_motores
    auto sub_armado_motores = expect<Subscriber>(session.declare_subscriber("armado_motores", [&](const Sample &sample)
                                                                            {
                                                                                 // Leemos los datos del tópico armado_motores
                                                                                 std::string armado_motores = std::string(sample.get_payload().as_string_view());            
                                                                                 std::sscanf(armado_motores.c_str(), "%f,%f,%f", &throttle, &switchC, &switchD); }));

    // Nos suscribimos al tópico motores
    auto sub_motores = expect<Subscriber>(session.declare_subscriber("motores", [&](const Sample &sample)
                                                                     {
                                                                             {
                                                                                 // Leemos los datos del tópico motores
                                                                                 std::string motores = std::string(sample.get_payload().as_string_view());
                                                                                 std::sscanf(motores.c_str(), "%f,%f,%f,%f", &motores_PWM[0], &motores_PWM[1], &motores_PWM[2], &motores_PWM[3]);
                                                                             }
                                                                             // Procesamos los datos y publicamos el resultado
                                                                             process(motores_PWM, throttle, switchC, switchD); }));

    printf("\nNo aceleres y pon los interruptores superiores del mando hacia atrás para armar los motores\n");

    // Mantenemos el programa en ejecución
    std::this_thread::sleep_for(std::chrono::hours(24));
}