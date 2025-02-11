#include <RTIMULib.h>
#include <zenohc.hxx>

using namespace zenohc;

int main()
{
    // Configuración y apertura de sesión Zenoh
    Config config;
    auto session = expect<Session>(open(std::move(config)));

    // Creamos el publisher (tópico pos_actual)
    auto pub = expect<Publisher>(session.declare_publisher("pos_actual"));

    // Obtenemos el fichero de configuración
    RTIMUSettings *settings = new RTIMUSettings("/home/pi/dron/modules/attitude/RTIMULib");
    RTIMU *imu = RTIMU::createIMU(settings);

    // Inicializamos el sensor
    imu->IMUInit();
    imu->setSlerpPower(0.02);
    imu->setGyroEnable(true);
    imu->setAccelEnable(true);
    imu->setCompassEnable(true);

    // Inicializamos las variables
    float roll, pitch, yaw;
    std::string data;

    while (true)
    {
        if (imu->IMURead())
        {
            // Leemos los datos y los convertimos de radianes a grados
            RTIMU_DATA imuData = imu->getIMUData();
            roll = imuData.fusionPose.y() * RTMATH_RAD_TO_DEGREE;
            pitch = imuData.fusionPose.x() * RTMATH_RAD_TO_DEGREE;
            yaw = imuData.fusionPose.z() * RTMATH_RAD_TO_DEGREE;

            // Asignamos los valores a un string y los publicamos
            data = std::to_string(roll) + "," + std::to_string(pitch) + "," + std::to_string(yaw);
            pub.put(data);

            // printf("Publicando: Roll -> %f | Pitch -> %f | Yaw -> %f\n", roll, pitch, yaw);
        }

        // Leemos y publicamos cada 10 milisegundos
        usleep(10000);
    }
    return 0;
}