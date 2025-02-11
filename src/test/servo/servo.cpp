#include <unistd.h>
#include <Navio2/PWM.h>
#include <Navio2/RCOutput_Navio2.h>
#include <Common/Util.h>
#include <stdlib.h>
#include <memory>
#include <iostream>
#include <chrono>

#define SERVO_MIN 1000 /*mS*/
#define SERVO_MID 1300 /*mS*/
#define SERVO_MAX 1400 /*mS*/

#define PWM_OUTPUT_MOTOR_0 0
#define PWM_OUTPUT_MOTOR_1 1
#define PWM_OUTPUT_MOTOR_2 2
#define PWM_OUTPUT_MOTOR_3 3

auto pwm = std::unique_ptr<RCOutput>{new RCOutput_Navio2()};

int power_motor_0 = SERVO_MIN;
int power_motor_1 = SERVO_MIN;
int power_motor_2 = SERVO_MIN;
int power_motor_3 = SERVO_MIN;

bool armed = false;

int main(int argc, char *argv[])

{
    // Before start we check if ardupilot is disabled
    if (check_apm())
    {
        printf("Ardupilot running, please disable it.\n");
        return 1;
    }

    // We have to launch it as sudo.
    if (getuid())
    {
        fprintf(stderr, "Not root. Please launch like this: sudo %s\n", argv[0]);
    }

    // Initializing the 4 motors
    if (!(pwm->initialize(PWM_OUTPUT_MOTOR_1)) || !(pwm->initialize(PWM_OUTPUT_MOTOR_2)) ||
        !(pwm->initialize(PWM_OUTPUT_MOTOR_3)) || !(pwm->initialize(PWM_OUTPUT_MOTOR_0)))
    {
        printf("Failed to initialize\n");
        return 1;
    }

    pwm->set_frequency(PWM_OUTPUT_MOTOR_1, 50);
    pwm->set_frequency(PWM_OUTPUT_MOTOR_2, 50);
    pwm->set_frequency(PWM_OUTPUT_MOTOR_3, 50);
    pwm->set_frequency(PWM_OUTPUT_MOTOR_0, 50);

    if (!(pwm->enable(PWM_OUTPUT_MOTOR_1)) || !(pwm->enable(PWM_OUTPUT_MOTOR_2)) ||
        !(pwm->enable(PWM_OUTPUT_MOTOR_3)) || !(pwm->enable(PWM_OUTPUT_MOTOR_0)))
    {
        printf("Failed to initialize engines");
        return 1;
    }

    // Arming sequence
    std::cout << "Starting Arming...\n";
    auto duration = std::chrono::seconds(2);
    auto one_sec = std::chrono::seconds(1);

    auto start1 = std::chrono::high_resolution_clock::now();
    while (std::chrono::high_resolution_clock::now() - start1 < duration)
    {
        // printf("Estoy en el bucle1\n");
        /*First, motors at low level during 2 seconds*/
        pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_1, SERVO_MIN);
        pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_2, SERVO_MIN);
        pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_3, SERVO_MIN);
        pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_0, SERVO_MIN);
        usleep(1000);
    }

    auto start2 = std::chrono::high_resolution_clock::now();
    while (std::chrono::high_resolution_clock::now() - start2 < duration)
    {
        /*Then, motors at high level during 2 seconds*/
        // printf("Estoy en el bucle2\n");
        pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_1, SERVO_MID);
        pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_2, SERVO_MID);
        pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_3, SERVO_MID);
        pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_0, SERVO_MID);
        usleep(1000);
    }

    auto start3 = std::chrono::high_resolution_clock::now();
    while (std::chrono::high_resolution_clock::now() - start3 < one_sec)
    {
        // printf("Estoy en el bucle3\n");
        pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_1, SERVO_MIN);
        pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_2, SERVO_MIN);
        pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_3, SERVO_MIN);
        pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_0, SERVO_MIN);
        usleep(1000);
    }

    /*2 second before running*/
    // sleep(2);
    armed = true;
    std::cout << "Ready to fly\n";

    /*Here we have to implement the post-armed logic*/
    while (1)
    {
        if (armed)
        {
            pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_0, power_motor_0);
            pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_1, power_motor_1);
            pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_2, power_motor_2);
            pwm->set_duty_cycle(PWM_OUTPUT_MOTOR_3, power_motor_3);
        }

        usleep(1000);
    }
}
