
#include "FluidSimulator.h"
#include <cstdlib>
#include <unordered_map>
#include <variant>
#include <string>
#include <iostream>
#include <vector>

void replaceBrackets(std::string& input) {
    for (char& c : input) {
        if (c == '(') {
            c = '<'; 
        } else if (c == ')') {
            c = '>';
        }
    }
}

using FluidSimulatorVariant = std::variant<FluidSimulator<double, double, double>>;

// Main entry point
int main(int argc, char* argv[]) {
    std::unordered_map<std::string, int> params = { {"double, double, double", 0} };
    std::vector<FluidSimulatorVariant> arr = { FluidSimulator<double, double, double>() };

    std::string p_type;
    std::string v_type;
    std::string v_flow_type;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--p-type=") == 0) {
            p_type = arg.substr(9);
        } else if (arg.find("--v-type=") == 0) {
            v_type = arg.substr(9);
        } else if (arg.find("--v-flow-type=") == 0) {
            v_flow_type = arg.substr(14);
        }
    }

    std::string args_str = p_type + " " + v_type + " " + v_flow_type;
    replaceBrackets(args_str);

    const auto& it = params.find(args_str);
    if (it == params.end()) {
        std::cout << "No suitable params" << std::endl;
        return 1;
    }

    std::visit([](auto& simulator) { simulator.runSimulation(500); }, arr[it->second]);

    return 0;
}
