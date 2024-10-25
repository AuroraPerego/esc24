#include "Particle.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <random>

// utility class for timing
class Timer {
public:
  Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}

  void reset() { start_time_ = std::chrono::high_resolution_clock::now(); }

  double elapsed() const {
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_time =
        end_time - start_time_;
    return elapsed_time.count();
  }

private:
  std::chrono::high_resolution_clock::time_point start_time_;
};

// function to initialize AoS
void initializeAoS(std::vector<GoodParticle> parts, std::vector<double> x, std::vector<double> y, std::vector<double> z, std::vector<double> p, std::vector<double> m) {
  parts.resize(x.size());
  for (int i = 0; i < x.size(); ++i){
    auto& part = parts[i];
    part.x = x[i];
    part.y = y[i];
    part.z = z[i];
    part.px = p[i] * 1.732;
    part.py = p[i] * 1.732;
    part.pz = p[i] * 1.732;
    part.mass = m[i];
    part.energy = std::sqrt(p[i]*p[i] + m[i]*m[i]);
    part.collX = false;
    part.collY = false;
    part.collZ = false;
  }
}

// function for computation on AoS
void calculateAoS(std::vector<GoodParticle> parts, std::mt19937 gen, const double x_max) {
  for (auto& part : parts){
    auto const t = std::uniform_real_distribution<double>(-2., 2.)(gen);
    std::cout << t << "\n";
    part.x += part.x + part.px / part.mass * t;
    if (part.x < 0 or part.x > x_max)
      part.collX = true;
    else{
      part.collX = false;
      part.px = -part.px;
    }
  }
}

// function to initialize SoA
void initializeSoA(ParticleSoA part, std::vector<double> x, std::vector<double> y, std::vector<double> z, std::vector<double> p, std::vector<double> m) {
  // your code here
  // instead of for loop, just do vector = vector
  for (int i = 0; i < part.size(); ++i){
    part.x() = x;
    part.y() = y;
    part.z() = z;
    part.px() = p;
    part.py() = p;
    part.pz() = p;
    part.mass() = m;
    part.energy() = m;
//    part.collX() = std::vector<bool>(N, 0);
//    part.collY() = std::vector<bool>(N, 0);
//    part.collZ() = std::vector<bool>(N, 0);
  }
}

// function for computation on SoA
void calculateSoA(/*args*/) {
  // your code here
}

int main() {

  std::cout << "sizeof(GoodParticle) " << sizeof(GoodParticle) << std::endl;

  //Uncomment this code for Exercise 1
  std::random_device rd;
  std::mt19937 gen(2);

  std::vector<double> pDist;
  std::vector<double> xDist;
  std::vector<double> yDist;
  std::vector<double> zDist;
  std::vector<double> massDist;

  constexpr int NIter = 1000000;
  constexpr double x_max = 200;
  constexpr int Npart = 1 << 10;

  auto fillVec = [&](std::vector<double>& vec, double min, double max) {
      vec.reserve(Npart);
      std::uniform_real_distribution<double> dist(min, max);
      std::generate_n(std::back_inserter(vec), Npart, [&]{ return dist(gen);
      });
  };


  fillVec(pDist, 10., 100.);
  fillVec(xDist, -100., 100.);
  fillVec(yDist, -100., 100.);
  fillVec(zDist, -300., 300.);
  fillVec(massDist, 10., 100.);

  Timer timer; //starting the clock
  std::vector<GoodParticle> good_particles;
  initializeAoS(good_particles, xDist, yDist, zDist, pDist, massDist);

  calculateAoS(good_particles, gen, x_max);
  std::cout << "AoS Elapsed time: " << timer.elapsed() << " ms " << "\n";

  timer.reset();
  ParticleSoA soa_particles(Npart);
  initializeSoA(soa_particles, xDist, yDist, zDist, pDist, massDist);
  //
  //  calculateSoA(/*args*/);
  //  std::cout << "SoA Elapsed time: " << timer.elapsed() << " ms " << "\n";

  return 0;
}
