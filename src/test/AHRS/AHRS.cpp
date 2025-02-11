#include "Common/MPU9250.h"
#include "Common/Util.h"
#include <memory>

int main(int argc, char *argv[])
{

    if (check_apm()) {
        return 1;
    }

    auto sensor = std::unique_ptr <InertialSensor>{ new MPU9250() };

    if (!sensor->probe()) {
        printf("Sensor not enabled\n");
        return EXIT_FAILURE;
    }

    sensor->initialize();

    float ax, ay, az;
    float gx, gy, gz;
    float mx, my, mz;

    while(1) {
        sensor->update();
        sensor->read_accelerometer(&ax, &ay, &az);
        sensor->read_gyroscope(&gx, &gy, &gz);
        sensor->read_magnetometer(&mx, &my, &mz);
        printf("Acc: %+7.3f %+7.3f %+7.3f | ", ax, ay, az);
        printf("Gyr: %+7.3f %+7.3f %+7.3f | ", gx, gy, gz);
        printf("Mag: %+7.3f %+7.3f %+7.3f\n", mx, my, mz);

       usleep(100000);
    }
}
