/*
**  PROGRAM: Mandelbrot area
**
**  PURPOSE: Program to compute the area of a  Mandelbrot set.
**           Correct answer should be around 1.510659.
**           WARNING: this program may contain errors
**
**  USAGE:   Program runs without input ... just run the executable
**
**  HISTORY: Written:  (Mark Bull, August 2011).
**           No structures or file-scope issues (T. Mattson, Nov. 2023)
*/

#include <omp.h>
#include <iostream>

# define NPOINTS 1000
# define MAXITER 100000

void testpoint(double, double);

int numoutside = 0;

int main(){
   constexpr double eps = 1.e-5;
   double area, error = 1.0e-5;

//   Loop over grid of points in the complex plane which contains the Mandelbrot
//   set, testing each point to see whether it is inside or outside the set.

#pragma omp parallel for
   for (int i=0; i<NPOINTS; ++i) {
     for (int j=0; j<NPOINTS; ++j) {
       const double creal = -2.0+2.5*(double)(i)/(double)(NPOINTS)+eps;
       const double cimag = 1.125*(double)(j)/(double)(NPOINTS)+eps;
       testpoint(creal, cimag);
     }
   }

// Calculate area of set and error estimate. Output the results

area=2.0*2.5*1.125*(double)(NPOINTS*NPOINTS-numoutside)/(double)(NPOINTS*NPOINTS);
   error=area/(double)NPOINTS;

   printf("Area of Mandlebrot set = %12.8f +/- %12.8f\n",area,error);
   printf("Correct answer should be around 1.510659\n");

}

void testpoint(const double creal, const double cimag){

// iterate z=z*z+c, until |z| > 2 when point is known to be outside set
// If loop count reaches MAXITER, point is considered to be inside the set

       double zreal, zimag, temp;
       zreal = creal;  zimag = cimag;

       for (int iter=0; iter<MAXITER; iter++) {
         temp = (zreal*zreal)-(zimag*zimag)+creal;
         zimag = zreal*zimag*2+cimag;
         zreal = temp;
         if ((zreal*zreal+zimag*zimag)>4.0) {
#pragma omp critical
           ++numoutside;
           break;
         }
       }
}

