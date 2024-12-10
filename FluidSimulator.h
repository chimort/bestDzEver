#pragma once

#include <array>
#include <iostream>
#include <random>
#include "Fixed.h"
#include "deltas.h"
#include "Config.h"

constexpr size_t N = 36;
constexpr size_t M = 84;


template<typename T, size_t N, size_t M>
struct VectorField {
    std::array<T, deltas.size()> v[N][M];

    T& add(int x, int y, int dx, int dy, T dv) {
        return get(x, y, dx, dy) += dv;
    }

    T& get(int x, int y, int dx, int dy) {
        auto it = std::find(deltas.begin(), deltas.end(), std::make_pair(dx, dy));
        assert(it != deltas.end());
        size_t i = std::distance(deltas.begin(), it);
        return v[x][y][i];
    }
};

template<typename Ptype, typename VType, typename VFlowType, size_t N, size_t M>
class FluidSimulator {
public:

    FluidSimulator() = default;
    void runSimulation(size_t T);
    void readInputFile(const std::string& filename);

private:
    Ptype rho_[256] {};
    Ptype g_;
    char field[N][M + 1]{};
    int dirs[N][M]{};
    PType p[N][M]{}, old_p[N][M]{};

    VectorField<VType, N, M> velocity;
    VectorField<VFlowType, N, M> velocity_flow;
    int last_use[N][M] {};
    int UT = 0;
    std::mt19937 random_generator_;

    struct ParticleParams {
        char type;
        PType cur_p;
        std::array<VType, deltas.size()> v;

        void swap_with(int x, int y, FluidSimulator* simulator) {
            std::swap(simulator->field[x][y], type);
            std::swap(simulator->p[x][y], cur_p);
            std::swap(simulator->velocity.v[x][y], v);
        }
    };
    

    std::tuple<Ptype, bool, std::pair<int, int>> propagate_flow(int x, int y, typename Ptype lim);
    Ptype random01();
    void propagate_stop(int x, int y, bool force = false);
    Ptype move_prob(int x, int y); 
    bool propagate_move(int x, int y, bool is_first);

};

template<typename Ptype, typename VType, typename VFlowType, size_t N, size_t M>
std::tuple<Ptype, bool, std::pair<int, int>> propagate_flow(int x, int y, Ptype lim) 
{
    last_use[x][y] = UT - 1;
    Ptype ret = 0;
    for (auto [dx, dy] : deltas) {
        int nx = x + dx, ny = y + dy;
        if (field[nx][ny] != '#' && last_use[nx][ny] < UT) {
            auto cap = velocity.get(x, y, dx, dy);
            auto flow = velocity_flow.get(x, y, dx, dy);
            if (flow == cap) {
                continue;
            }
            // assert(v >= velocity_flow.get(x, y, dx, dy));
            auto vp = min(lim, cap - flow);
            if (last_use[nx][ny] == UT - 1) {
                velocity_flow.add(x, y, dx, dy, vp);
                last_use[x][y] = UT;
                // cerr << x << " " << y << " -> " << nx << " " << ny << " " << vp << " / " << lim << "\n";
                return {vp, 1, {nx, ny}};
            }
            auto [t, prop, end] = propagate_flow(nx, ny, vp);
            ret += t;
            if (prop) {
                velocity_flow.add(x, y, dx, dy, t);
                last_use[x][y] = UT;
                // cerr << x << " " << y << " -> " << nx << " " << ny << " " << t << " / " << lim << "\n";
                return {t, prop && end != pair(x, y), end};
            }
        }
    }
    last_use[x][y] = UT;
    return {ret, 0, {0, 0}};
}

