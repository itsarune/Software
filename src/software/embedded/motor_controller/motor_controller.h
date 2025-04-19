#pragma once

#include "software/embedded/motor_controller/motor_board.h"
#include "software/embedded/motor_controller/motor_fault_indicator.h"
#include "software/embedded/motor_controller/motor_index.h"

class MotorController
{
    public:
    virtual void reset() = 0;

    virtual MotorFaultIndicator checkDriverFault(MotorIndex motor) = 0;

    virtual double readThenWriteValue(const MotorIndex motor, const uint8_t read_addr, const uint8_t write_addr,
            const uint8_t write_data) = 0;
};
