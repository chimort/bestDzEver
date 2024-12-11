#pragma once

#include <array>
#include <iostream>
#include <random>
#include <cassert>       
#include <algorithm>     
#include <tuple>         
#include <utility>  
#include <cstring>

#include "Fixed.h"
#include "FastFixed.h"
#include "FixedConv.h"
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

template<typename Ptype, typename VType, typename VFlowType, size_t N, size_t M>
double FluidSimulator<Ptype, VType, VFlowType, N, M>::random01()
{       
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(random_generator_);
}

template<typename Ptype, typename VType, typename VFlowType, size_t N, size_t M>
void FluidSimulator<Ptype, VType, VFlowType, N, M>::propagate_stop(int x, int y, bool force)
{
    if (!force) {
        bool stop = true;
        for (auto [dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) > 0) {
                stop = false;
                break;
            }
        }
        if (!stop) {
            return;
        }
    }
    last_use[x][y] = UT;
    for (auto [dx, dy] : deltas) {
        int nx = x + dx, ny = y + dy;
        if (field[nx][ny] == '#' || last_use[nx][ny] == UT || velocity.get(x, y, dx, dy) > 0) {
            continue;
        }
        propagate_stop(nx, ny);
    }
}

template<typename Ptype, typename VType, typename VFlowType, size_t N, size_t M>
Ptype FluidSimulator<Ptype, VType, VFlowType, N, M>::move_prob(int x, int y)
{
    Ptype sum = 0;
    for (size_t i = 0; i < deltas.size(); ++i) {
        auto [dx, dy] = deltas[i];
        int nx = x + dx, ny = y + dy;
        if (field[nx][ny] == '#' || last_use[nx][ny] == UT) {
            continue;
        }
        auto v = velocity.get(x, y, dx, dy);
        if (v < 0) {
            continue;
        }
        sum += v;
    }
    return sum;
}

template<typename Ptype, typename VType, typename VFlowType, size_t N, size_t M>
bool FluidSimulator<Ptype, VType, VFlowType, N, M>::propagate_move(int x, int y, bool is_first)
{
    last_use[x][y] = UT - is_first;
    bool ret = false;
    int nx = -1, ny = -1;
    do {
        std::array<Ptype, deltas.size()> tres;
        Ptype sum = 0;
        for (size_t i = 0; i < deltas.size(); ++i) {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if (field[nx][ny] == '#' || last_use[nx][ny] == UT) {
                tres[i] = sum;
                continue;
            }
            auto v = velocity.get(x, y, dx, dy);
            if (v < 0) {
                tres[i] = sum;
                continue;
            }
            sum += v;
            tres[i] = sum;
        }

        if (sum == 0) {
            break;
        }

        Ptype p = sum * random01();
        size_t d = std::ranges::upper_bound(tres, p) - tres.begin();

        auto [dx, dy] = deltas[d];
        nx = x + dx;
        ny = y + dy;
        assert(velocity.get(x, y, dx, dy) > 0 && field[nx][ny] != '#' && last_use[nx][ny] < UT);

        ret = (last_use[nx][ny] == UT - 1 || propagate_move(nx, ny, false));
    } while (!ret);
    last_use[x][y] = UT;
    for (size_t i = 0; i < deltas.size(); ++i) {
        auto [dx, dy] = deltas[i];
        int nx = x + dx, ny = y + dy;
        if (field[nx][ny] != '#' && last_use[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) < 0) {
            propagate_stop(nx, ny);
        }
    }
    if (ret) {
        if (!is_first) {
            ParticleParams pp{};
            pp.swap_with(*this, x, y);
            pp.swap_with(*this, nx, ny);
            pp.swap_with(*this, x, y);
        }
    }
    return ret;
}


template<typename Ptype, typename VType, typename VFlowType, size_t N, size_t M>
void FluidSimulator<Ptype, VType, VFlowType, N, M>::runSimulation(size_t T)
{
    rho_[' '] = 0.01;
    rho_['.'] = 1000;
    g_ = 0.1;
    for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                if (field[x][y] == '#')
                    continue;
                for (auto [dx, dy] : deltas) {
                    dirs[x][y] += (field[x + dx][y + dy] != '#');
                }
            }
        }

    for (size_t i = 0; i < T; ++i) {
        
        Ptype total_delta_p = 0;    
        // Apply external forces
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                if (field[x][y] == '#')
                    continue;
                if (field[x + 1][y] != '#')
                    velocity.add(x, y, 1, 0, g_);
            }
        }

        // Apply forces from p
        memcpy(this->old_p, this->p, sizeof(this->p));
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                if (field[x][y] == '#')
                    continue;
                for (auto [dx, dy] : deltas) {
                    int nx = x + dx, ny = y + dy;
                    if (field[nx][ny] != '#' && old_p[nx][ny] < old_p[x][y]) {
                        auto delta_p = old_p[x][y] - old_p[nx][ny];
                        auto force = delta_p;
                        auto &contr = velocity.get(nx, ny, -dx, -dy);
                        if (contr * rho_[(int) field[nx][ny]] >= force) {
                            contr -= force / rho_[(int) field[nx][ny]];
                            continue;
                        }
                        force -= contr * rho_[(int) field[nx][ny]];
                        contr = 0;
                        velocity.add(x, y, dx, dy, force / rho_[(int) field[x][y]]);
                        p[x][y] -= force / dirs[x][y];
                        total_delta_p -= force / dirs[x][y];
                    }
                }
            }
        }

        // Make flow from velocities
        velocity_flow = {};
        bool prop = false;
        do {
            UT += 2;
            prop = 0;
            for (size_t x = 0; x < N; ++x) {
                for (size_t y = 0; y < M; ++y) {
                    if (field[x][y] != '#' && last_use[x][y] != UT) {
                        auto [t, local_prop, _] = propagate_flow(x, y, 1);
                        if (t > 0) {
                            prop = 1;
                        }
                    }
                }
            }
        } while (prop);

        // Recalculate p with kinetic energy
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                if (field[x][y] == '#')
                    continue;
                for (auto [dx, dy] : deltas) {
                    auto old_v = velocity.get(x, y, dx, dy);
                    auto new_v = velocity_flow.get(x, y, dx, dy);
                    if (old_v > 0) {
                        assert(new_v <= old_v);
                        velocity.get(x, y, dx, dy) = new_v;
                        auto force = (old_v - new_v) * rho_[(int) field[x][y]];
                        if (field[x][y] == '.')
                            force *= 0.8;
                        if (field[x + dx][y + dy] == '#') {
                            p[x][y] += force / dirs[x][y];
                            total_delta_p += force / dirs[x][y];
                        } else {
                            p[x + dx][y + dy] += force / dirs[x + dx][y + dy];
                            total_delta_p += force / dirs[x + dx][y + dy];
                        }
                    }
                }
            }
        }

        UT += 2;
        prop = false;
        for (size_t x = 0; x < N; ++x) {
            for (size_t y = 0; y < M; ++y) {
                if (field[x][y] != '#' && last_use[x][y] != UT) {
                    if (move_prob(x, y) > random01()) {
                        prop = true;
                        propagate_move(x, y, true);
                    } else {
                        propagate_stop(x, y, true);
                    }
                }
            }
        }

        if (prop) {
            std::cout << "Tick " << i << ":\n";
            for (size_t x = 0; x < N; ++x) {
                std::cout << field[x] << "\n";
            }
        }
    }
}
