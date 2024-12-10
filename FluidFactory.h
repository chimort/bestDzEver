#pragma once

#include "FluidSimulator.h"
#include "Config.h"
#include <memory>
#include <string>
#include <stdexcept>

class FluidSimulatorBase {
public:
    virtual ~FluidSimulatorBase() = default;
    virtual void run(size_t T) = 0;
};

template<typename Config, size_t N, size_t M>
class FluidSimulatorWrapper : public FluidSimulatorBase {
public:
    FluidSimulatorWrapper(const std::string& input_file) 
        : simulator_(std::make_unique<FluidSimulator<Config, N, M>>()) {
        simulator_->readInputFile(input_file); 
    }

    void run(size_t T) override {
        simulator_->runSimulation(T);
    }

private:
    std::unique_ptr<FluidSimulator<Config, N, M>> simulator_;
};
