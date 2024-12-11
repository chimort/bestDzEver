#include "FluidSimulator.h"


int main() {
    // Define the Fixed types with 32 bits and 16 fractional bits
    using FixedType = FAST_FIXED<64, 16>;
    //template class FluidSimulator<Fixed<32, 16>, Fixed<32, 16>, Fixed<32, 16>, 36, 84>;
    // Create an instance of FluidSimulator with Fixed<32, 16> for Ptype, VType, and VFlowType
    FluidSimulator<FAST_FIXED<64, 16>, Fixed<64, 16>, FAST_FIXED<32, 8>, 36, 84> simulator;

    size_t simulationSteps = 10000;

    // Run the simulation
    simulator.runSimulation(simulationSteps);

    return 0;
}