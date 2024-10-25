#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

class ParticleSoA {
public:
  // Constructor that takes the size and allocates memory
  ParticleSoA(size_t numParticles) : size_(numParticles) {
    // Calculate the size required for the memory buffer
    size_t bufferSize = (6*sizeof(double) + 3*sizeof(float) + 4*sizeof(bool)+256)*numParticles; // remember, it will return a pointer
        /*put your size here */; // note for the std::string you can allocate
                                 // 256 bytes per particle

    buffer = (std::byte*)std::malloc(bufferSize);

    // Set pointers to the beginning of each column
    unsigned char *currentPtr =
        reinterpret_cast<unsigned char *>(buffer); // keep this pointer a char for consistency
                                     // when moving around the pointer

    // assign to each column the correct pointer
    // you can use reinterpret_cast to cast currentPointer to the correct type
    // that stores your variable
    x = reinterpret_cast<double *>(currentPtr);
    y = reinterpret_cast<double *>(x + numParticles);
    z = reinterpret_cast<double *>(y + numParticles);

    px = reinterpret_cast<double *>(z + numParticles);
    py = reinterpret_cast<double *>(px + numParticles);
    pz = reinterpret_cast<double *>(py + numParticles);

    mass = reinterpret_cast<float *>(pz + numParticles);
    energy = reinterpret_cast<float *>(mass + numParticles);

    hit_x = reinterpret_cast<bool *>(energy + numParticles);
    hit_y = reinterpret_cast<bool *>(hit_x + numParticles);
    hit_z = reinterpret_cast<bool *>(hit_y + numParticles);

    id = reinterpret_cast<int*>(hit_z + numParticles);
    name = reinterpret_cast<std::string *>(id + numParticles);
  }
  // defining data members
  double *x;
  double *y;
  double *z;
  double *px;
  double *py;
  double *pz;
  float *mass;
  float *energy;
  bool *hit_x;
  bool *hit_y;
  bool *hit_z;
  int *id;
  std::string *name;

  // Destructor to free the allocated memory
  ~ParticleSoA() { /*REMEMBER TO FREE THE MEMORY! AND REMEMBER TO DESTROY THE
                      OBJECT IF YOU HAVE ANY--> std::string is an object!*/
    for (int i = 0; i < size_; ++i)
      name[i].~basic_string();
    std::free(buffer);
  }

private:
  std::byte *buffer;
  size_t size_;

  // your methods if you need them
};

void initializeSoA(ParticleSoA &particles, const std::vector<double> &pxDist,
                   const std::vector<double> &xDist,
                   const std::vector<double> &yDist,
                   const std::vector<double> &zDist,
                   const std::vector<float> &massDist, int Npart) {

  //particles.name =
  //    new (particles.name) std::string[Npart]; // constructing the string!
  for (int i = 0; i < Npart; ++i) {
    particles.id[i] = i;
    particles.px[i] = pxDist[i];
    particles.py[i] = pxDist[i];
    particles.pz[i] = pxDist[i];
    particles.x[i] = xDist[i];
    particles.y[i] = yDist[i];
    particles.z[i] = zDist[i];
    particles.mass[i] = massDist[i];
    particles.name[i] = *( new (&(particles.name[i])) std::string("Particle" + std::to_string(i)) );
    particles.energy[i] = 0.f;
  }
}

int main() {
  constexpr int N = 17; // number of particle
  ParticleSoA p(N);     // instance of particleSoA
  std::random_device rd;
  std::mt19937 gen(1);

  // creating some random vectors
  std::vector<double> pxDist;
  std::vector<double> xDist;
  std::vector<double> zDist;
  std::vector<double> yDist;
  std::vector<float> massDist;
  std::vector<double> timeDist;

  // utility function for filling the vectors
  auto fillVec = [&](std::vector<double> &vec, double min, double max) {
    vec.reserve(N);
    std::uniform_real_distribution<double> dist(min, max);
    std::generate_n(std::back_inserter(vec), N, [&] { return dist(gen); });
  };
  auto fillVecT = [&](std::vector<float> &vec, float min, float max) {
    vec.reserve(N);
    std::uniform_real_distribution<float> dist(min, max);
    std::generate_n(std::back_inserter(vec), N, [&] { return dist(gen); });
  };

  fillVec(pxDist, 10., 100.);
  fillVec(xDist, -100., 100.);
  fillVec(yDist, -100., 100.);
  fillVec(zDist, -300., 300.);
  fillVecT(massDist, 10.f, 100.f);
  fillVec(timeDist, -10., 10.);

  // Filling the SoA
  initializeSoA(p, pxDist, xDist, yDist, zDist, massDist, N);
  for (int i = 0; i < N; ++i) {
    // printing out to check everything works as expected
    std::cout << " X " << p.x[i] << "\n";
    std::cout << " Name  " << p.name[i] << "\n"; // crashes HERE, if commented the crash becomes
                                                 // double free or corruption (out): 0x000000000146f400
  }

} // destructor called here, no memory leaks

