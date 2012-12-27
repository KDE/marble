//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Gerhard HOLTKAMP
//

/***********************************************************************
   3-Dim Vector and Matrix Definitions and Operations

      Linux C++ version

  Author: Gerhard HOLTKAMP                   15-APR-2010
 ***********************************************************************/

//#include <iostream>
#include <cmath>
using namespace std;

#include "attlib.h"

/*********************************************************************/
double atan20 (double y, double x)
{
 /* redefine atan2 so that it does'nt crash when both x and y are 0 */
 double result;

 if ((x == 0) && (y == 0)) result = 0;
 else result = atan2 (y, x);

  return result;
}


 Vec3::Vec3(double x, double y, double z)
		 {assign(x, y, z);}

 Vec3::Vec3 (const Vec3& c)
  {
   for (int j = 0; j<3; ++j) v[j] = c.v[j];
  }


 void Vec3::assign (double x, double y, double z)
  {
   v[0]=x; v[1]=y; v[2]=z;
  }

 double& Vec3::operator [] (unsigned index)
  {
   return (index < 3) ? * (v + index) : *v;
  }

 Vec3& Vec3::operator = (const Vec3& c)
  {
   for (int j = 0; j<3; ++j) v[j] = c.v[j];
   return *this;
  }

 Vec3& Vec3::operator += (const Vec3& c)
  {
   for (int j = 0; j<3; ++j) v[j] = v[j] + c.v[j];
   return *this;
  }

 Vec3& Vec3::operator -= (const Vec3& c)
  {
   for (int j = 0; j<3; ++j) v[j] = v[j] - c.v[j];
   return *this;
  }

 Vec3& Vec3::operator *= (const Vec3& c)   // cross product
  {
   Vec3 result;
   result.v[0] = v[1] * c.v[2] - v[2] * c.v[1];
   result.v[1] = v[2] * c.v[0] - v[0] * c.v[2];
   result.v[2] = v[0] * c.v[1] - v[1] * c.v[0];
   for (int j = 0; j<3; ++j) v[j] = result.v[j];
   return *this;
  }

 Vec3& Vec3::operator *= (double r)
  {
   for (int j = 0; j<3; ++j) v[j] = r * v[j];
   return *this;
  }

 Vec3& Vec3::operator /= (double r)
  {
   double q;

   if (r < 1E-100) q = 0.0;
   else q = 1.0 / r;
   for (int j = 0; j<3; ++j) v[j] = q * v[j];
   return *this;
  }

 double abs(const Vec3& c)    // absolute value
  {
   double result=0;

   for (int j=0; j<3; ++j) result += c.v[j]*c.v[j];
   return sqrt(result);
  }

 double dot (const Vec3& c1, const Vec3& c2)  // dot product
  {
   double result=0;

   for (int j = 0; j<3; ++j) result += c1.v[j]*c2.v[j];
   return result;
  }

 Vec3 operator + (const Vec3& c1, const Vec3& c2)
  {
   Vec3 result;

   for (int j = 0; j<3; ++j) result.v[j] = c1.v[j] + c2.v[j];
   return result;
  }

 Vec3 operator - (const Vec3& c1, const Vec3& c2)
  {
   Vec3 result;

   for (int j = 0; j<3; ++j) result.v[j] = c1.v[j] - c2.v[j];

   return result;
  }

 Vec3 operator * (double r, const Vec3& c1)
  {
   Vec3 result;

   for (int j = 0; j<3; ++j) result.v[j] = r * c1.v[j];
   return result;
  }

 Vec3 operator * (const Vec3& c1, double r)
  {
   Vec3 result;

   for (int j = 0; j<3; ++j) result.v[j] = c1.v[j] * r;
   return result;
  }

 Vec3 operator * (const Vec3& c1, const Vec3& c2)  // cross product
  {
   Vec3 result;

   result.v[0] = c1.v[1] * c2.v[2] - c1.v[2] * c2.v[1];
   result.v[1] = c1.v[2] * c2.v[0] - c1.v[0] * c2.v[2];
   result.v[2] = c1.v[0] * c2.v[1] - c1.v[1] * c2.v[0];
   return result;
  }

 Vec3 operator / (const Vec3& c1, double r)
  {
   double q;
   Vec3 result;

   if (r < 1E-100) q = 0.0;
   else q = 1.0 / r;
   for (int j = 0; j<3; ++j) result.v[j] = q * c1.v[j];
   return result;
  }

 Vec3 vnorm (const Vec3& c)   // norm vector
  {
   int j;
   double q=0;
   Vec3 result;

   for (j=0; j<3; ++j) q += c.v[j]*c.v[j];
   q =  sqrt(q);
   if (q < 1E-100) q = 0.0;
   else q = 1.0 / q;
   for (j=0; j<3; ++j) result.v[j] = c.v[j]*q;
   return result;
  }

 Vec3 carpol (const Vec3& c)    // Cartesian -> Polar
  /* Convert vector from cartesian coordinates c = [x, y, z]
     into polar coordinates v = [rho, phi, theta]
     where rho is the radius, phi the "azimuth" and theta the
     "declination" (equatorial plane = 0, pole = 90ø)
  */
  {
   double  x, y, z, r;
   Vec3 result;

   x = c.v[0];
   y = c.v[1];
   z = c.v[2];
   r = x*x + y*y;
   result.v[0] = sqrt (r + z*z);
   result.v[1] = atan20 (y, x);
   if (result.v[1] < 0) result.v[1] += 2.0*M_PI;
   r = sqrt (r);
   result.v[2] = atan20 (z, r);

   return result;
  }

 Vec3 polcar (const Vec3& c)    // Polar -> Cartesian
  /* Convert vector from polar coordinates c = [rho, phi, theta]
     where rho is the radius, phi the "azimuth" and theta the
     "declination" (equatorial plane = 0, pole = 90ø)
     into vector v = [x, y, z] of cartesian coordinates.
  */
  {
   double  r;
   Vec3 result;

   r = c.v[0] * cos(c.v[2]);
   result.v[0] = r * cos(c.v[1]);
   result.v[1] = r * sin(c.v[1]);
   result.v[2] = c.v[0] * sin(c.v[2]);

   return result;
  }

 ostream& operator << (ostream& os, const Vec3& c)
  {
   os << "[" << c.v[0] << "," << c.v[1] << "," << c.v[2] << "]";
   return os;
  }

/*******************************************************************/

Mat3::Mat3 (double x)
 {
  int i, j;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) m[i][j] = x;
 }

Mat3::Mat3 (const Mat3& c)
 {
  int i, j;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) m[i][j] = c.m[i][j];
 }

void Mat3::assign (double x11, double x12, double x13,
                   double x21, double x22, double x23,
                   double x31, double x32, double x33)
 {
  m[0][0] = x11;
  m[0][1] = x12;
  m[0][2] = x13;
  m[1][0] = x21;
  m[1][1] = x22;
  m[1][2] = x23;
  m[2][0] = x31;
  m[2][1] = x32;
  m[2][2] = x33;
 }

void Mat3::assign (double x[3][3])
 {
  int i, j;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) m[i][j] = x[i][j];
 }

void Mat3::PutMij (double x, int i, int j)
 {
  if ((i>0) && (i<4) && (j>0) && (j<4)) m[i-1][j-1] = x;
 }

double Mat3::GetMij (int i, int j)
 {
  double result;

  if ((i>0) && (i<4) && (j>0) && (j<4)) result = m[i-1][j-1];
  else result = 0;
  return result;
 }


Mat3& Mat3::operator = (const Mat3& c)
 {
  int i, j;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) m[i][j] = c.m[i][j];

  return (*this);
 }

Mat3& Mat3::operator += (const Mat3& c)
 {
  int i, j;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) m[i][j] = m[i][j] + c.m[i][j];
  return *this;
 }

Mat3& Mat3::operator -= (const Mat3& c)
 {
  int i, j;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) m[i][j] = m[i][j] - c.m[i][j];
  return *this;
 }

Mat3& Mat3::operator *= (const Mat3& c)
 {
  int i, j, k;
  double r;
  Mat3 b;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j)
    {
     r = 0;
     for (k=0; k<3; ++k) r = r + c.m[i][k] * m[k][j];
     b.m[i][j] = r;
    }

   for (i=0; i<3; ++i)
    for (j=0; j<3; ++j) m[i][j] = b.m[i][j];

   return *this;
 }

Mat3& Mat3::operator *= (double r)
 {
  int i, j;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) m[i][j] = r*m[i][j];
  return *this;
 }

Mat3& Mat3::operator /= (double r)
 {
  int i, j;
  double q;

  if (r < 1E-100) q = 0.0;
  else q = 1.0 / r;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) m[i][j] = m[i][j] * q;
  return *this;
 }

Mat3 mxcon (double r)  // constant matrix with all elements of value r
 {
  Mat3 result;
  int i, j;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) result.m[i][j] = r;

  return result;
 }

Mat3 mxidn ()          // identity matrix
 {
  Mat3 result;
  int i;

  result = mxcon (0);
  for (i=0; i<3; ++i) result.m[i][i] = 1.0;

  return result;
 }

Mat3 mxtrn (const Mat3& m1)  // returns transposed of matrix m1
 {
  Mat3 result;
  int i, j;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) result.m[i][j] = m1.m[j][i];

  return result;
 }

double mxdet (const Mat3& c) // returns determinant of matrix m1
 {
  double result;

  result = c.m[0][0]*c.m[1][1]*c.m[2][2] + c.m[0][1]*c.m[1][2]*c.m[2][0]
         + c.m[0][2]*c.m[1][0]*c.m[2][1] - c.m[0][2]*c.m[1][1]*c.m[2][0]
         - c.m[0][0]*c.m[1][2]*c.m[2][1] - c.m[0][1]*c.m[1][0]*c.m[2][2];

  return result;
 }

Mat3 operator + (const Mat3& c1, const Mat3& c2)
 {
  Mat3 result;
  int i, j;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) result.m[i][j] = c1.m[i][j] + c2.m[i][j];
  return result;
 }

Mat3 operator - (const Mat3& c1, const Mat3& c2)
 {
  Mat3 result;
  int i, j;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) result.m[i][j] = c1.m[i][j] - c2.m[i][j];
  return result;
 }

Mat3 operator * (double r, const Mat3& c1)
 {
  Mat3 result;
  int i, j;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) result.m[i][j] = c1.m[i][j] * r;
  return result;
 }


Mat3 operator * (const Mat3& c1, double r)
 {
  int i, j;
  Mat3 result;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) result.m[i][j] = c1.m[i][j] * r;
  return result;
 }

Mat3 operator * (const Mat3& c1, const Mat3& c2)
 {
  int i, j, k;
  double r;
  Mat3 result;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j)
    {
     r = 0;
     for (k=0; k<3; k++) r = r + c1.m[i][k] * c2.m[k][j];
     result.m[i][j] = r;
    };

  return result;
 }

Mat3 operator / (const Mat3& c1, double r)
 {
  Mat3 result;
  int i, j;
  double q;

  if (r < 1E-100) q = 0.0;
  else q = 1.0 / r;

  for (i=0; i<3; ++i)
   for (j=0; j<3; ++j) result.m[i][j] = c1.m[i][j] * q;
  return result;
 }

Vec3 mxvct (const Mat3& m1, Vec3& v1)
 {
  int i, j;
  double r;
  Vec3 result;

  for (i=0; i<3; ++i)
   {
    r = 0;
    for (j=0; j<3; ++j) r = r + m1.m[i][j] * v1[j];
    result[i] = r;
   };
  return result;
 }

Mat3 xrot (double a)
 {
  //	 Rotation matrix around x-axis with angle a (in radians).
  double c, s;
  Mat3 m1;

  c = cos (a);
  s = sin (a);
  m1.m[0][0] = 1.0;
  m1.m[0][1] = 0.0;
  m1.m[0][2] = 0.0;
  m1.m[1][0] = 0.0;
  m1.m[1][1] = c;
  m1.m[1][2] = s;
  m1.m[2][0] = 0.0;
  m1.m[2][1] = -s;
  m1.m[2][2] = c;

  return m1;
 }

Mat3 yrot (double a)
 {
  //	 Rotation matrix around y-axis with angle a (in radians).
  double c, s;
  Mat3 m1;

  c = cos (a);
  s = sin (a);
  m1.m[0][0] = c;
  m1.m[0][1] = 0.0;
  m1.m[0][2] = -s;
  m1.m[1][0] = 0.0;
  m1.m[1][1] = 1.0;
  m1.m[1][2] = 0.0;
  m1.m[2][0] = s;
  m1.m[2][1] = 0.0;
  m1.m[2][2] = c;

  return m1;
 }

Mat3 zrot (double a)
 {
  //  Rotation matrix around z-axis with angle a (in radians).
  double c, s;
  Mat3 m1;

  c = cos (a);
  s = sin (a);
  m1.m[0][0] = c;
  m1.m[0][1] = s;
  m1.m[0][2] = 0.0;
  m1.m[1][0] = -s;
  m1.m[1][1] = c;
  m1.m[1][2] = 0.0;
  m1.m[2][0] = 0.0;
  m1.m[2][1] = 0.0;
  m1.m[2][2] = 1.0;

  return m1;
 }

Mat3 csmx (double p, double y, double r)
 {
  // Form cosine matrix m from pitch p, yaw y and roll r angles.
  //	  The angles are to be given in radians.
  //	 Roll is rotation about the x-axis, pitch about y, yaw about z.
  //
  Mat3 pitch, yaw, roll, result;

  pitch = yrot (p);
  yaw = 	 zrot (y);
  roll =	 xrot (r);
  result = yaw * pitch;
  result *= roll;

  return result;
 }

void gpyr (const Mat3& m1, double& p, double& y, double& r)
 {
  // Get pitch p, yaw y and roll r angles (in radians) from
  // cosine matrix m1.

  y = asin (m1.m[0][1]);
  r = atan20 (-m1.m[2][1], m1.m[1][1]);
  p = atan20 (-m1.m[0][2], m1.m[0][0]);
 }

void vcpy (Vec3& v, double& p, double& y)
 {
  // Convert direction given by cartesion vector v into a corresponding
  // pitch (p) / yaw (y) sequence. The angles are in radians.

  p = atan20 (-v[2], v[0]);
  y = atan20 (v[1], sqrt (v[0]*v[0] + v[2]*v[2]));
 }

void vcrp (Vec3& v, double& p, double& r)
 {
  // Convert direction given by cartesian vector v into a corresponding
  // roll (r) / pitch (p) sequence. The angles are in radians.

  r = atan20 (v[1], -v[2]);
  p = M_PI / 2.0 - atan20 (v[0], sqrt (v[1]*v[1] + v[2]*v[2]));
 }

void mxevc (const Mat3& m, double& a, Vec3& v)
 {
  // Get eigenvalue a and eigenvector v from cosine matrix m.
  // The eigenangle a is in radians.

  double ri, rj, rk, q1, q2, q3, q4;

  // using the 3-1-3 rotation matrix, first get the corresponding
  // angles ri, rj, rk of rotation about the x-, y-, z-axis.
  ri = atan20 (m.m[2][0], -m.m[2][1]);
  rj = 0.5 * acos (m.m[2][2]);
  rk = atan20 (m.m[0][2], m.m[1][2]);

  //  now convert to quaternions
  q4 = sin (rj);
  q1 = q4 * cos (0.5 * (ri - rk));
  q2 = q4 * sin (0.5 * (ri - rk));
  q4 = cos (rj);
  q3 = q4 * sin (0.5 * (ri + rk));
  q4 = q4 * cos (0.5 * (ri + rk));

  // now get the eigen angle and eigen vector
  v.assign(q1, q2, q3);
  ri = abs (v);
  if (ri == 0)
   {
    // treat singularity for 3-1-3 matrix
    v.assign (0.0, 0.0, 1.0);
    q4 = 0.5 * sqrt (1.0 + m.m[0][0] + m.m[1][1] + m.m[2][2]);
   }
  else v /= ri;

  a = 2.0 * acos (q4);
 }

Mat3 mxrox (double& a, Vec3& v)
 {
  // Convert eigenvalue a (eigen angle in radians) and eigenvector v
  // into a corresponding cosine rotation matrix m.

  double q1, q2, q3, q4, q12, q22, q32, q42;
  Mat3 result;

  // calculate quaternions and their squares
  q4 = sin ( 0.5 * a);
  q1 = v[0] * q4;
  q2 = v[1] * q4;
  q3 = v[2] * q4;
  q4 = cos (0.5 * a);
  q12 = q1 * q1;
  q22 = q2 * q2;
  q32 = q3 * q3;
  q42 = q4 * q4;

  // now get the matrix elements
  result.assign ((q12 - q22 - q32 + q42), (2.0 * (q1*q2 + q3*q4)),
                 (2.0 * (q1*q3 - q2*q4)), (2.0 * (q1*q2 - q3*q4)),
                 (-q12 + q22 - q32 + q42),(2.0 * (q2*q3 + q1*q4)),
                 (2.0 * (q1*q3 + q2*q4)), (2.0 * (q2*q3 - q1*q4)),
                 (-q12 - q22 + q32 + q42));

  return result;
 }


ostream& operator << (ostream& os, const Mat3& c)
 {
  os << "[" << c.m[0][0] << "," << c.m[0][1] << "," << c.m[0][2] << "]" << endl
     << "[" << c.m[1][0] << "," << c.m[1][1] << "," << c.m[1][2] << "]" << endl
     << "[" << c.m[2][0] << "," << c.m[2][1] << "," << c.m[2][2] << "]" << endl;
  return os;
 }

