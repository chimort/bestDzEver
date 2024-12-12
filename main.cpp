// main.cpp

#include "FluidSimulator.h"
//#include "Fixed.h"


int main() {
    // Определение типов FIXED и FAST_FIXED
    using Ptype = FixedPoint<32, 8>;
    using VType = FixedPoint<32, 16>;
    using VFlowType = FixedPoint<32, 8>;

    // Создание экземпляра FluidSimulator
    FluidSimulator<VFlowType, VType, float, 36, 84> simulator;


    size_t simulationSteps = 1000;

    // Запуск симуляции
    simulator.runSimulation(simulationSteps);

    return 0;
}