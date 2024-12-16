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
        if (c == '(') {
            c = '<'; 
        } else if (c == ')') {
            c = '>';
        }
    }
}

using FluidSimulatorVariant = std::variant<{{types}}>;

// Main entry point
int main(int argc, char* argv[]) {
    std::unordered_map<std::string, int> params = { {{params}} };
    std::vector<FluidSimulatorVariant> arr = { {{types_vec}} };

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
"""

def replace_fast_fixed(input_string):
    def replacement(match):
        N, K = match.group(1), match.group(2)
        return f"FAST_FIXED<{N}, {K}>"

    return re.sub(r'FAST_FIXED\((\d+),\s*(\d+)\)', replacement, input_string)

def parse_types(input_string):
    """Парсинг типов из входного значения TYPES."""
    type_strings = re.findall(r'FAST_FIXED\(\d+,\d+\)|FIXED\(\d+,\d+\)|double|float', input_string)
    return [replace_fast_fixed(t) if "FAST_FIXED" in t else t.replace("(", "<").replace(")", ">") for t in type_strings]

def generate_permutations(types):
    """Генерация всех возможных троек типов."""
    if len(types) == 1:
        return [f"{types[0]}, {types[0]}, {types[0]}"]
    return ["{}".format(", ".join(p)) for p in itertools.product(types, repeat=3)]

def generate_code(types):
    # Генерация всех возможных троек
    type_combinations = generate_permutations(types)

    # Генерация для std::variant и std::vector
    types_variant = ", ".join(f"FluidSimulator<{t}>" for t in type_combinations)
    types_vec = ", ".join(f"FluidSimulator<{t}>()" for t in type_combinations)
    params_map = ", ".join(f'{{"{t}", {i}}}' for i, t in enumerate(type_combinations))

    return types_variant, types_vec, params_map

if len(sys.argv) < 2:
    print("Usage: python generate_code.py <TYPES>")
    sys.exit(1)

types_value = sys.argv[1]
parsed_types = parse_types(types_value)

# Генерация C++ кода
variant, vec, params = generate_code(parsed_types)

# Подстановка в шаблон
rendered_code = cpp_template.replace("{{types}}", variant).replace("{{types_vec}}", vec).replace("{{params}}", params)

# Сохранение в файл
with open("main.cpp", "w") as cpp_file:
    cpp_file.write(rendered_code)

print("Сгенерирован файл main.cpp!")
