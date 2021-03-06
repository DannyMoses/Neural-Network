#pragma once // too lazy to do the standard indef right now

#define PI 3.14159 // Yes. I defined pi. Deal with it
#define ENCODER_PULSE_PER_REV 250 // the pulses per revolution of the encoders(andymark ones in this case)

#define DRIVE_WHEEL_DIAMETER 8.0 // the diamter of our wheels(in inches)
#define DRIVE_WHEEL_CIRCUMFERENCE PI * DRIVE_WHEEL_DIAMETER
#define DRIVE_GEAR_RATIO 5.95 / 1 // 5.95:1 gear ratio
// the distance per pulse in inches of the encoders
#define DRIVE_DISTANCE_PER_PULSE DRIVE_WHEEL_CIRCUMFERENCE / (ENCODER_PULSE_PER_REV * DRIVE_GEAR_RATIO)
