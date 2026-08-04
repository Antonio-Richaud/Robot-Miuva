#include "pca9685.h"
#include "i2c_master.h"

#define PCA9685_ADDRESS_7BIT 0x40u
#define PCA9685_WRITE_BYTE   ((PCA9685_ADDRESS_7BIT << 1) | 0u)

bool pca9685_probe(void)
{
    bool acknowledged;

    if (!i2c_master_start())
    {
        return false;
    }

    acknowledged = i2c_master_write(PCA9685_WRITE_BYTE);
    (void)i2c_master_stop();

    return acknowledged;
}
