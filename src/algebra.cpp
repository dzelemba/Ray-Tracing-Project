//---------------------------------------------------------------------------
//
// CS488 -- Introduction to Computer Graphics
//
// algebra.hpp/algebra.cpp
//
// Classes and functions for manipulating points, vectors, matrices, 
// and colours.  You probably won't need to modify anything in these
// two files.
//
// University of Waterloo Computer Graphics Lab / 2003
//
//---------------------------------------------------------------------------

#include "algebra.hpp"

bool equal(const double d1, const double d2) {
  return d1 > d2 - epsilon && d1 < d2 + epsilon;
}

double Vector3D::normalize()
{
  double denom = 1.0;
  double x = (v_[0] > 0.0) ? v_[0] : -v_[0];
  double y = (v_[1] > 0.0) ? v_[1] : -v_[1];
  double z = (v_[2] > 0.0) ? v_[2] : -v_[2];

  if(x > y) {
    if(x > z) {
      if(1.0 + x > 1.0) {
        y = y / x;
        z = z / x;
        denom = 1.0 / (x * sqrt(1.0 + y*y + z*z));
      }
    } else { /* z > x > y */ 
      if(1.0 + z > 1.0) {
        y = y / z;
        x = x / z;
        denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
      }
    }
  } else {
    if(y > z) {
      if(1.0 + y > 1.0) {
        z = z / y;
        x = x / y;
        denom = 1.0 / (y * sqrt(1.0 + z*z + x*x));
      }
    } else { /* x < y < z */
      if(1.0 + z > 1.0) {
        y = y / z;
        x = x / z;
        denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
      }
    }
  }

  if(1.0 + x + y + z > 1.0) {
    v_[0] *= denom;
    v_[1] *= denom;
    v_[2] *= denom;
    return 1.0 / denom;
  }

  return 0.0;
}

/*
 * Define some helper functions for matrix inversion.
 */

static void swaprows(Matrix4x4& a, size_t r1, size_t r2)
{
  std::swap(a[r1][0], a[r2][0]);
  std::swap(a[r1][1], a[r2][1]);
  std::swap(a[r1][2], a[r2][2]);
  std::swap(a[r1][3], a[r2][3]);
}

static void dividerow(Matrix4x4& a, size_t r, double fac)
{
  a[r][0] /= fac;
  a[r][1] /= fac;
  a[r][2] /= fac;
  a[r][3] /= fac;
}

static void submultrow(Matrix4x4& a, size_t dest, size_t src, double fac)
{
  a[dest][0] -= fac * a[src][0];
  a[dest][1] -= fac * a[src][1];
  a[dest][2] -= fac * a[src][2];
  a[dest][3] -= fac * a[src][3];
}

/*
 * invertMatrix
 *
 * I lifted this code from the skeleton code of a raytracer assignment
 * from a different school.  I taught that course too, so I figured it
 * would be okay.
 */
Matrix4x4 Matrix4x4::invert() const
{
  /* The algorithm is plain old Gauss-Jordan elimination 
     with partial pivoting. */

  Matrix4x4 a(*this);
  Matrix4x4 ret;

  /* Loop over cols of a from left to right, 
     eliminating above and below diag */

  /* Find largest pivot in column j among rows j..3 */
  for(size_t j = 0; j < 4; ++j) { 
    size_t i1 = j; /* Row with largest pivot candidate */
    for(size_t i = j + 1; i < 4; ++i) {
      if(fabs(a[i][j]) > fabs(a[i1][j])) {
        i1 = i;
      }
    }

    /* Swap rows i1 and j in a and ret to put pivot on diagonal */
    swaprows(a, i1, j);
    swaprows(ret, i1, j);

    /* Scale row j to have a unit diagonal */
    if(a[j][j] == 0.0) {
      // Theoretically throw an exception.
      return ret;
    }

    dividerow(ret, j, a[j][j]);
    dividerow(a, j, a[j][j]);

    /* Eliminate off-diagonal elems in col j of a, doing identical 
       ops to b */
    for(size_t i = 0; i < 4; ++i) {
      if(i != j) {
        submultrow(ret, i, j, a[i][j]);
        submultrow(a, i, j, a[i][j]);
      }
    }
  }

  return ret;
}

// This is ugly...
bool solve3x2System(const Vector3D& A1, const Vector3D& A2, const Vector3D& B, Point2D& x)
{
  // Make copies of everthing.
  Vector3D a1 = A1, a2 = A2, b = B;

  // Make sure A[0,0] and A[1,1] aren't 0.
  if (a1[0] > -epsilon && a1[0] < epsilon) {
    if (a1[2] != 0) {
      double t1 = a1[0], t2 = a2[0], t3 = b[0];
      a1[0] = a1[2]; a2[0] = a2[2]; b[0] = b[2];
      a1[2] = t1; a2[2] = t2; b[2] = t3;
    } else if (a1[1] != 0) {
      double t1 = a1[0], t2 = a2[0], t3 = b[0];
      a1[0] = a1[1]; a2[0] = a2[1]; b[0] = b[1];
      a1[1] = t1; a2[1] = t2; b[1] = t3;
    } else {
      return false;
    }
  }

  if (a2[1] > -epsilon && a2[1] < epsilon) {
    if (a2[2] != 0) {
      double t1 = a1[1], t2 = a2[1], t3 = b[1];
      a1[1] = a1[2]; a2[1] = a2[2]; b[1] = b[2];
      a1[2] = t1; a2[2] = t2; b[2] = t3;
    } else if (a2[0] != 0) {
      double t1 = a1[0], t2 = a2[0], t3 = b[0];
      a1[0] = a1[1]; a2[0] = a2[1]; b[0] = b[1];
      a1[1] = t1; a2[1] = t2; b[1] = t3;
    } else {
      return false;
    } 
  }

  // Check again
  if (a1[0] == 0) {
    return false;
  }

  // Now we can solve...
  if (a1[1] != 0) {
    a2[1] = (a1[0] / a1[1]) * a2[1] - a2[0];
    b[1] = (a1[0] / a1[1]) * b[1] - b[0];
    a1[1] = 0;
  }

  // Check again since we modified this row
  if (a2[1] > -epsilon && a2[1] < epsilon) {
    if (a2[2] != 0) {
      double t1 = a1[1], t2 = a2[1], t3 = b[1];
      a1[1] = a1[2]; a2[1] = a2[2]; b[1] = b[2];
      a1[2] = t1; a2[2] = t2; b[2] = t3;
    } else {
      return false;
    } 
  }

  if (a1[2] < -epsilon || a1[2] > epsilon) {
    a2[2] = (a1[0] / a1[2]) * a2[2] - a2[0];
    b[2] = (a1[0] / a1[2]) * b[2] - b[0];
    a1[2] = 0;
  }

  if (a2[2] < -epsilon || a2[2] > epsilon) {
    b[2] = (a2[1] / a2[2]) * b[2] - b[1];
    a2[2] = 0;
  }

  if (b[2] > epsilon || b[2] < -epsilon) {
    return false;
  }

  x[1] = b[1] / a2[1];
  x[0] = (b[0] - a2[0] * x[1]) / a1[0];

  return true;
}

void tests() {
  // Basic
  {
    Vector3D a1(1, 0, 0);
    Vector3D a2(0, 1, 0);
    Vector3D b(2, 3, 0);

    Point2D x;
    solve3x2System(a1, a2, b, x);
    std::cerr << x << std::endl;
  }
  // Basic
  {
    Vector3D a1(1, 0, 0);
    Vector3D a2(0, 0, 1);
    Vector3D b(2, 0, 3);

    Point2D x;
    solve3x2System(a1, a2, b, x);
    std::cerr << x << std::endl;
  }
  // Basic
  {
    Vector3D a1(0, 1, 0);
    Vector3D a2(0, 0, 1);
    Vector3D b(0, 2, 3);

    Point2D x;
    solve3x2System(a1, a2, b, x);
    std::cerr << x << std::endl;
  }
  // Basic
  {
    Vector3D a1(0, 0, 1);
    Vector3D a2(0, 1, 0);
    Vector3D b(0, 3, 2);

    Point2D x;
    solve3x2System(a1, a2, b, x);
    std::cerr << x << std::endl;
  }
  // Basic
  {
    Vector3D a1(0, 0, 1);
    Vector3D a2(1, 0, 0);
    Vector3D b(3, 0, 2);

    Point2D x;
    solve3x2System(a1, a2, b, x);
    std::cerr << x << std::endl;
  }
  // Basic
  {
    Vector3D a1(0, 1, 0);
    Vector3D a2(1, 0, 0);
    Vector3D b(3, 2, 0);

    Point2D x;
    solve3x2System(a1, a2, b, x);
    std::cerr << x << std::endl;
  }


  // Less Basic
  {
    Vector3D a1(1, 1, 0);
    Vector3D a2(1, -1, 0);
    Vector3D b(10, 6, 0);

    Point2D x;
    solve3x2System(a1, a2, b, x);
    std::cerr << x << std::endl;
  }
  // Fail
  {
    Vector3D a1(1, 1, 1);
    Vector3D a2(1, -1, 1);
    Vector3D b(10, 6, 2);

    Point2D x;
    if (solve3x2System(a1, a2, b, x)) {
      std::cerr << x << std::endl;
    }
  }
  // From Ray Tracer
  {
    Vector3D a1(0.809017, -0.309017, 0.5);
    Vector3D a2(-0.467086, 0.178411, 0.866025);
    Vector3D b(-1.61803, 0.618034, -1);

    Point2D x;
    if (solve3x2System(a1, a2, b, x)) {
      std::cerr << x << std::endl;
    }
  }
}
