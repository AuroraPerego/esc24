#ifndef GOOD_PARTICLE
#define GOOD_PARTICLE
#include <string>
#include <vector>

struct GoodParticle {             // alignof 8
    GoodParticle() :
     pid(211),
     name("Pion") {};
    GoodParticle(int id, std::string n) :
     pid(id),
     name(n) {};
    double x, y, z;                // 8*3
    double px, py, pz;             // 8*3
    double mass;                    // 4
    double energy;                  // 4
    bool collX, collY, collZ;      // 1*3
    const int pid;                 // 4
    const std::string name;        // 32 (8)
};

#include <vector>
#include <string>

class ParticleSoA {
public:
    // Constructor to reserve space for vectors and fill const vectors (pid, name)
    ParticleSoA(int N) : size_(N) {
        // Reserve space for N elements in each vector
        x_.reserve(N);
        y_.reserve(N);
        z_.reserve(N);
        px_.reserve(N);
        py_.reserve(N);
        pz_.reserve(N);
        mass_.reserve(N);
        energy_.reserve(N);
        collX_.reserve(N);
        collY_.reserve(N);
        collZ_.reserve(N);

        // Initialize pid and name vectors (these are const so we need to fill them)
        pid_.resize(N);
        name_.resize(N);

        // Example initialization for pid and name (this could be customized)
        for (int i = 0; i < N; ++i) {
            pid_[i] = 211;  // Example PID (e.g., pion)
            name_[i] = "Pion";  // Example name (e.g., pion)
        }
    }

    const size_t size() const { return size_; }

    // Methods to access the vectors by reference
    std::vector<double>& x() { return x_; }
    std::vector<double>& y() { return y_; }
    std::vector<double>& z() { return z_; }
    std::vector<double>& px() { return px_; }
    std::vector<double>& py() { return py_; }
    std::vector<double>& pz() { return pz_; }
    std::vector<double>& mass() { return mass_; }
    std::vector<double>& energy() { return energy_; }
    std::vector<bool>& collX() { return collX_; }
    std::vector<bool>& collY() { return collY_; }
    std::vector<bool>& collZ() { return collZ_; }

    // Const getters for pid and name, since these vectors are const
    const std::vector<int>& getPid() const { return pid_; }
    const std::vector<std::string>& getName() const { return name_; }

private:
    const size_t size_;
    // Member variables
    std::vector<double> x_;
    std::vector<double> y_;
    std::vector<double> z_;
    std::vector<double> px_;
    std::vector<double> py_;
    std::vector<double> pz_;
    std::vector<double> mass_;
    std::vector<double> energy_;
    std::vector<bool> collX_;
    std::vector<bool> collY_;
    std::vector<bool> collZ_;
    std::vector<int> pid_;  // Particle ID (const as specified)
    std::vector<std::string> name_;  // Particle name (const as specified)
};

#endif
