
#include <pybind11/pybind11.h>

namespace py = pybind11;

// Step 1: Define a class representing a particle
class Particle {
  float energy_;
  float mass_;
public:
  Particle() = default;
  Particle(float e, float m) : energy_(e), mass_(m) {}

  // Step 1.1: Define getters and setters for the data members
  void setEnergy(float e) { energy_ = e; }
  float getEnergy() const { return energy_; }

  void setMass(float m) { mass_ = m; }
  float getMass() const { return mass_; }

  // Step 2: Method to return the energy of the particle
  float calculateEnergy(float momentum, float c = 299792458.0f) {
      // Using E^2 = (pc)^2 + (mc^2)^2 formula
      return sqrt((momentum * c) * (momentum * c) + (mass_ * c * c) * (mass_ * c * c));
  }

  // Step 2: Method to return the invariant mass of the particle
  float calculateInvariantMass(float momentum, float c = 299792458.0f) {
      // Using m^2c^4 = E^2 - (pc)^2, rearranging to m = sqrt(E^2/c^4 - p^2/c^2)
      float E = calculateEnergy(momentum, c);
      return sqrt((E * E) / (c * c * c * c) - (momentum * momentum) / (c * c));
  }
};

// Step 3: Write a function returning the distance between two particles
double distance(Particle a, Particle b) { return a.getEnergy() - b.getEnergy();};

PYBIND11_MODULE(HEP, m) {
  // Step 4: Bind the Particle class
  py::class_<Particle>(m, "Particle")
    .def(py::init<>())
    .def(py::init<float, float>())
    .def("getEnergy", &Particle::getEnergy)
    .def("setEnergy", &Particle::setEnergy);

  // Step 5: Bind the distance function
  m.def("distance", &distance);

}
