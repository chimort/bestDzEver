#pragma once

#include <array>
#include <iostream>
#include <random>
#include <cassert>       
#include <algorithm>     
#include <tuple>         
#include <utility>       
#include "Fixed.h"
#include "deltas.h"


template<typename T, size_t N = 36, size_t M = 84>
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

template<typename Ptype, typename VType, typename VFlowType, size_t N = 36, size_t M = 84>
class FluidSimulator {
public:

    FluidSimulator() = default;
    void runSimulation(size_t T);

private:
    char field[N][M + 1] = {
        "####################################################################################",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                       .........                                  #",
        "#..............#            #           .........                                  #",
        "#..............#            #           .........                                  #",
        "#..............#            #           .........                                  #",
        "#..............#            #                                                      #",
        "#..............#            #                                                      #",
        "#..............#            #                                                      #",
        "#..............#            #                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............#                                                      #",
        "#..............#............################                     #                 #",
        "#...........................#....................................#                 #",
        "#...........................#....................................#                 #",
        "#...........................#....................................#                 #",
        "##################################################################                 #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "#                                                                                  #",
        "####################################################################################",
    };
    Ptype rho_[256] {};
    Ptype g_;
    int dirs[N][M]{};
    Ptype p[N][M]{}, old_p[N][M]{};

    VectorField<VType, N, M> velocity;
    VectorField<VFlowType, N, M> velocity_flow;
    int last_use[N][M] {};
    int UT = 0;
    std::mt19937 random_generator_;

    struct ParticleParams {
        char type;
        Ptype cur_p;
        std::array<VType, deltas.size()> v;

        void swap_with(FluidSimulator& fs, int x, int y) {
            std::swap(fs.field[x][y], type);
            std::swap(fs.p[x][y], cur_p);
            std::swap(fs.velocity.v[x][y], v);
        }
    };
    
    void readInputFile(const std::string& filename);

    std::tuple<Ptype, bool, std::pair<int, int>> propagate_flow(int x, int y, Ptype lim);
    double random01();
    void propagate_stop(int x, int y, bool force = false);
    Ptype move_prob(int x, int y); 
    bool propagate_move(int x, int y, bool is_first);

};

template<typename Ptype, typename VType, typename VFlowType, size_t N, size_t M>
std::tuple<Ptype, bool, std::pair<int, int>> 
FluidSimulator<Ptype, VType, VFlowType, N, M>::propagate_flow(int x, int y, Ptype lim)  
{
    this->last_use[x][y] = this->UT - 1;
    Ptype ret = 0;
    for (auto [dx, dy] : deltas) {
        int nx = x + dx, ny = y + dy;
        if (this->field[nx][ny] != '#' && this->last_use[nx][ny] < this->UT) {
            auto cap = this->velocity.get(x, y, dx, dy);
            auto flow = this->velocity_flow.get(x, y, dx, dy);
            if (flow == cap) {
                continue;
            }
            // assert(v >= velocity_flow.get(x, y, dx, dy));
            auto vp = std::min(lim, cap - flow);
            if (this->last_use[nx][ny] == this->UT - 1) {
                this->velocity_flow.add(x, y, dx, dy, vp);
                this->last_use[x][y] = this->UT;
                // cerr << x << " " << y << " -> " << nx << " " << ny << " " << vp << " / " << lim << "\n";
                return {vp, 1, {nx, ny}};
            }
            auto [t, prop, end] = this->propagate_flow(nx, ny, vp);
            ret += t;
            if (prop) {
                this->velocity_flow.add(x, y, dx, dy, t);
                this->last_use[x][y] = this->UT;
                // cerr << x << " " << y << " -> " << nx << " " << ny << " " << t << " / " << lim << "\n";
                return {t, prop && end != std::make_pair(x, y), end};
            }
        }
    }
    this->last_use[x][y] = this->UT;
    return {ret, 0, {0, 0}};
};

