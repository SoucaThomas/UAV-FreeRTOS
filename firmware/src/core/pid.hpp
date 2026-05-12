#ifndef PID_HPP
#define PID_HPP

#include "utils.hpp"
class PID {
 private:
  float kp, ki, kd;
  float integral;
  float prevError;
  float outputMin, outputMax;
  float integralMax;

 public:
  PID(float kp, float ki, float kd, float outputMin, float outputMax)
      : kp(kp),
        ki(ki),
        kd(kd),
        integral(0.0f),
        prevError(0.0f),
        outputMin(outputMin),
        outputMax(outputMax),
        integralMax(outputMax * 0.5f) {}

  float compute(float target, float current, float dt) {
    float error = target - current;

    // P term
    float p = kp * error;

    // I term
    integral += error * dt;
    integral = clamp(integral, -integralMax, integralMax);
    float i = ki * integral;

    // D term
    float derivate = (error - prevError) / dt;
    float d = kd * derivate;

    prevError = error;

    float output = p + i + d;
    output = clamp(output, outputMin, outputMax);

    return output;
  }

  void reset() {
    integral = 0.0f;
    prevError = 0.0f;
  }

  void setGains(float newKp, float newKi, float newKd) {
    kp = newKp;
    ki = newKi;
    kd = newKd;
  }
};

#endif