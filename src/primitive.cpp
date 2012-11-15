#include "primitive.hpp"
#include "polyroots.hpp"

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

bool Sphere::intersect(const Point3D& eye, const Vector3D& ray, double offset, double& minT, Vector3D& normal)
{
  return m_unitSphere.intersect(eye, ray, offset, minT, normal);
}

Cube::~Cube()
{
}

bool Cube::intersect(const Point3D& eye, const Vector3D& ray, double offset, double& minT, Vector3D& normal)
{
  return m_unitCube.intersect(eye, ray, offset, minT, normal);
}

NonhierSphere::~NonhierSphere()
{
}

bool NonhierSphere::intersect(const Point3D& eye, const Vector3D& ray, double offset, double& minT, Vector3D& normal)
{
  double A = 0; 
  double B = 0;
  double C = 0;

  // Solving for intersection of sphere and ray
  // Using Maple to expand and collect for t
  // we get a quadratic formula for t with these coefficients.
  for (int i = 0; i < 3; i++) {
   A += ray[i] * ray[i];
   B += 2 * (eye[i] - m_pos[i]) * ray[i];
   C += (eye[i] - m_pos[i]) * (eye[i] - m_pos[i]);
  }
  C -= m_radius * m_radius;

  bool closerObject = false;
  double roots[2];
  int numRoots = quadraticRoots(A, B, C, roots); 
  if (numRoots != 0) {
    for (int i = 0; i < 2; i++) {
      if (roots[i] > offset && roots[i] < minT) {
        minT = roots[i];
        closerObject = true;
      }
    }
  }

  // Now calculate the normal to the point of intersection.
  if (closerObject) {
    Point3D poi = eye + minT * ray;
    normal = poi - m_pos;
  }

  return closerObject;
}

NonhierBox::~NonhierBox()
{
}

// Face: 1 - x, 2 - y, 3 - z
// NormalDir: 1 - Left, 2 - Right
bool NonhierBox::checkPoint(Point3D p, int face, double t, double& minT, Vector3D& normal, int normalDir)
{
  bool inFace = true;

  // Compare other 2 coordinates
  for (int j = 0; j < 3; j++) {
    if (face != j) {
      if (p[j] < m_pos[j] || p[j] > m_pos[j] + m_size) {
        inFace = false;
        break;
      }
    }
  }
  
  if (inFace) {
    minT = t;
    normal = Vector3D();
    normal[face] = normalDir == 1 ? -1.0 : 1.0; 
    return true;
  }

  return false;
}

bool NonhierBox::intersect(const Point3D& eye, const Vector3D& ray, double offset, double& minT, Vector3D& normal)
{
  // Since Nonhier box's are aligned with MCS, the plane equations become very simple.
  // Just {x,y,z} = {m_pos[0] [+ m_size], m_pos[1] [+ m_size], m_pos[2] [+ m_size]}.

  bool found = false;
  for (int i = 0; i < 3; i++) {
    double t1 = (m_pos[i] - eye[i]) / ray[i];
    double t2 = (m_pos[i] + m_size - eye[i]) / ray[i];

    // Check if points are in face and update
    // minT, normal appropriately.
    if (t1 > offset && t1 < minT) {
      found = checkPoint(eye + t1 * ray, i, t1, minT, normal, 1) || found;
    }

    if (t2 > offset && t2 < minT) {
      found = checkPoint(eye + t2 * ray, i, t2, minT, normal, 2) || found;
    }
  }
  return found;
}
