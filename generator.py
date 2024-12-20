import sys
import re
import itertools

cpp_template = """
#include "FluidSimulator.h"
#include <cstdlib>
#include <unordered_map>
#include <variant>
#include <string>
#include <iostream>
#include <vector>

void replaceBrackets(std::string& input) {
    for (char& c : input) {
        if (c == '(') c = '<';
        else if (c == ')') c = '>';
    }
}

using FluidSimulatorVariant = std::variant<{{types}}>;

int main(int argc, char* argv[]) {
    std::unordered_map<std::string, int> params = { {{params}} };
    std::vector<FluidSimulatorVariant> arr = { {{types_vec}} };

    std::string p_type, v_type, v_flow_type;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--p-type=") == 0) p_type = arg.substr(9);
        else if (arg.find("--v-type=") == 0) v_type = arg.substr(9);
        else if (arg.find("--v-flow-type=") == 0) v_flow_type = arg.substr(14);
    }

    std::string args_str = p_type + " " + v_type + " " + v_flow_type;
    replaceBrackets(args_str);

    auto it = params.find(args_str);
    if (it == params.end()) {
        std::cout << "Неподходящие параметры" << std::endl;
        std::cout << "Круглые скобки нужно либо экранировать, либо писать в ковычнах."
        return 1;
    }

    size_t T = 500;
    size_t tick_for_save = 50;
    std::string input_file = "../input.json";
    std::visit([&](auto& simulator) { 
        simulator.runSimulation(T, tick_for_save, input_file); 
    }, arr[it->second]);
    return 0;
}
"""

def replace_fast_fixed(input_string):
    """Replace FAST_FIXED(N, M) -> FAST_FIXED<N, M>"""
    return re.sub(r'FAST_FIXED\((\d+),\s*(\d+)\)', r'FAST_FIXED<\1,\2>', input_string)



def parse_types(input_string):
    """Parse and normalize types."""
    input_string = replace_fast_fixed(input_string) 
    input_string = re.sub(r'FIXED\((\d+),\s*(\d+)\)', r'FIXED<\1,\2>', input_string)
    type_strings = re.findall(r'FAST_FIXED<\d+,\d+>|FIXED<\d+,\d+>|double|float', input_string)
    return type_strings



def generate_permutations(types):
    """Generate all permutations of three types."""
    return ["{}".format(", ".join(p)) for p in itertools.product(types, repeat=3)]

def generate_code(types):
    """Generate variant, vector, and params."""
    type_combinations = generate_permutations(types)
    types_variant = ", ".join(f"FluidSimulator<{t}>" for t in type_combinations)
    types_vec = ", ".join(f"FluidSimulator<{t}>()" for t in type_combinations)
    params_map = ", ".join(f'{{"{t}", {i}}}' for i, t in enumerate(type_combinations))
    return types_variant, types_vec, params_map

if len(sys.argv) < 2:
    print("Usage: python generate_code.py <TYPES>")
    sys.exit(1)

types_value = sys.argv[1]
parsed_types = parse_types(types_value)

variant, vec, params = generate_code(parsed_types)
rendered_code = cpp_template.replace("{{types}}", variant).replace("{{types_vec}}", vec).replace("{{params}}", params)

with open("main.cpp", "w") as cpp_file:
    cpp_file.write(rendered_code)

print("Generated main.cpp!")
