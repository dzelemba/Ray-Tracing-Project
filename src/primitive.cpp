#include "primitive.hpp"
#include "polyroots.hpp"
#include <cfloat>

Primitive::~Primitive()
{
}

bool Primitive::filteredIntersect(const Point3D& eye, const Vector3D& ray, const double offset,
                                  std::list<IntersectionPoint>& tValues) const
{
  if (intersect(eye, ray, offset, tValues)) { 
    tValues.sort();

    int numVals = tValues.size();

    // Here we have to eliminate dupicates that occur due to points passing
    // through an edge or a corner. We must be careful not to eliminate duplicates
    // that occur from an intersection on a corner or edge.
    // We will differentiate by checking two points up and down the ray near the point.
    std::list<std::list<IntersectionPoint>::iterator> pointsToRemove;
    std::list<IntersectionPoint>::iterator prev = tValues.begin();
    std::list<IntersectionPoint>::iterator curr = tValues.begin();
    curr++;
    for (; prev != tValues.end(); curr++, prev++) {
      if (equal(curr->m_t, prev->m_t)) {
        if (containsPoint(eye + (curr->m_t + 100 * tightEpsilon) * ray) ||
            containsPoint(eye + (curr->m_t - 100 * tightEpsilon) * ray)) {
          // Now if there's more than 2 we should remove them as well
          for ( ; equal(curr->m_t, prev->m_t) && curr != tValues.end(); curr++, prev++) {
            pointsToRemove.push_back(curr);
          }
        }
      }
    }
    // Now remove
    for (std::list<std::list<IntersectionPoint>::iterator>::iterator it = pointsToRemove.begin();
          it != pointsToRemove.end(); it++) {
      tValues.erase(*it);
    } 

    // Now we should have an even number of points.
    if (tValues.size() % 2 != 0) {
      std::cerr << std::endl << "Error! Number of intersection points is odd: " << numVals << " "
                << " " << tValues.size() << " ";
      for (std::list<IntersectionPoint>::iterator it = tValues.begin(); it != tValues.end(); it++)
        std::cerr << it->m_t << " " << eye + it->m_t * ray << " ";
      std::cerr << std::endl;
      return false;
    }

    return true;
  }

  return false;
}

bool Primitive::checkQuadraticRoots(const Point3D& eye, const Vector3D& ray, const double minValue,
                                    const double A, const double B, const double C,
                                    std::list<IntersectionPoint>& tVals) const
{
  bool pointFound = false;
  
  double roots[2];
  int numRoots = quadraticRoots(A, B, C, roots); 
  if (numRoots != 0) {
    for (int i = 0; i < 2; i++) {
      double t = roots[i];
      Point3D poi = eye + t * ray;
      if (checkPoint(poi)) {
        pointFound = true;
        tVals.push_back(IntersectionPoint(t, getNormal(poi)));
      }
    }
  }

  return pointFound;
}

bool Primitive::checkPoint(const Point3D& poi) const
{
  (void) poi; // Silence Compiler Warnings

  return true;
}

/* 
  ********** NonhierSphere **********
*/
NonhierSphere::~NonhierSphere()
{
}

bool NonhierSphere::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                              std::list<IntersectionPoint>& tVals) const 
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

  checkQuadraticRoots(eye, ray, offset, A, B, C, tVals);

  return tVals.size() != 0;
}

bool NonhierSphere::containsPoint(const Point3D& p) const
{
  return (p - m_pos).length() < m_radius;
}

Vector3D NonhierSphere::getNormal(const Point3D& p) const
{
  return p - m_pos;
}

Point2D NonhierSphere::textureMapCoords(const Point3D& p) const
{
  // We will find the latitude and longitude of the point and
  // use that as our coordinates.

  // Vectors pointing to the north pole and a point on the equator.
  static Vector3D vn(0.0, 1.0, 0.0);
  static Vector3D ve(0.0, 0.0, -1.0);

  Vector3D vp = p - m_pos;
  vp.normalize();

  double phi = acos(vn.dot(vp));
  double lat = phi / M_PI;

  Vector3D proj(vp[0], 0.0, vp[2]);
  proj.normalize();

  double theta = acos(ve.dot(proj));
  if (proj[0] < 0) {
    theta = 2* M_PI - theta;
  }
  double longitude = theta / (2*M_PI);

  return Point2D(longitude, lat);  
}

/*
   Mesh in mesh.cpp 
*/

/* 
  ********** NonhierBox **********
*/

NonhierBox::NonhierBox(const Point3D& pos, double size)
  : m_box(std::vector<Point3D>(), std::vector<std::vector<int> >())
{
  std::vector<Point3D> vertices;
  vertices.push_back(Point3D(pos[0], pos[1], pos[2]));
  vertices.push_back(Point3D(pos[0] + size, pos[1], pos[2]));
  vertices.push_back(Point3D(pos[0] + size, pos[1] + size, pos[2]));
  vertices.push_back(Point3D(pos[0], pos[1] + size, pos[2]));
  vertices.push_back(Point3D(pos[0], pos[1], pos[2] + size));
  vertices.push_back(Point3D(pos[0] + size, pos[1], pos[2] + size));
  vertices.push_back(Point3D(pos[0] + size, pos[1] + size, pos[2] + size));
  vertices.push_back(Point3D(pos[0], pos[1] + size, pos[2] + size));

  std::vector<std::vector<int> > f;
  int faces[6][4] = { {3, 2, 1, 0}, // Front
                      {0, 1, 5, 4}, // Bottom
                      {2, 6, 5, 1}, // Right
                      {7, 3, 0, 4}, // Left
                      {7, 6, 2, 3}, // Top
                      {4, 5, 6, 7}}; // Back
  for (int i = 0; i < 6; i++) f.push_back(std::vector<int> (faces[i], faces[i] + 4));

  std::vector<Vector3D> upVectors;
  upVectors.push_back(Vector3D(0.0, 1.0, 0.0));
  upVectors.push_back(Vector3D(0.0, 0.0, -1.0));
  upVectors.push_back(Vector3D(0.0, 1.0, 0.0));
  upVectors.push_back(Vector3D(0.0, -1.0, 0.0));
  upVectors.push_back(Vector3D(0.0, 0.0, 1.0));
  upVectors.push_back(Vector3D(0.0, -1.0, 0.0));

  m_box = Mesh(vertices, f, upVectors);
}

NonhierBox::~NonhierBox()
{
}

bool NonhierBox::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                           std::list<IntersectionPoint>& tVals) const
{
  return m_box.intersect(eye, ray, offset, tVals);
}

bool NonhierBox::containsPoint(const Point3D& p) const
{
  return m_box.containsPoint(p);
}

Vector3D NonhierBox::getNormal(const Point3D& p) const
{
  return m_box.getNormal(p);
}

Point2D NonhierBox::textureMapCoords(const Point3D& p) const
{
  return m_box.textureMapCoords(p);
}
/* 
  ********** Sphere **********
*/

Sphere::~Sphere()
{
}

bool Sphere::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                       std::list<IntersectionPoint>& tVals) const
{
  return m_unitSphere.intersect(eye, ray, offset, tVals);
}

bool Sphere::containsPoint(const Point3D& p) const
{
  return m_unitSphere.containsPoint(p);
}

Vector3D Sphere::getNormal(const Point3D& p) const
{
  return m_unitSphere.getNormal(p);
}

Point2D Sphere::textureMapCoords(const Point3D& p) const
{
  return m_unitSphere.textureMapCoords(p);
}

/* 
  ********** Cube **********
*/

Cube::~Cube()
{
}

bool Cube::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                     std::list<IntersectionPoint>& tVals) const
{
  return m_unitCube.intersect(eye, ray, offset, tVals);
}

bool Cube::containsPoint(const Point3D& p) const
{
  return m_unitCube.containsPoint(p);
}

Vector3D Cube::getNormal(const Point3D& p) const
{
  return m_unitCube.getNormal(p);
}

Point2D Cube::textureMapCoords(const Point3D& p) const
{
  return m_unitCube.textureMapCoords(p);
}


/* 
  ********** Cone **********
*/

Cone::Cone()
  : m_base(Vector3D(0.0, 0.0, 1.0), Point3D(0.0, 0.0, 1.0), 1.0)
{
}

Cone::~Cone()
{
}

bool Cone::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                     std::list<IntersectionPoint>& tVals) const
{
  double A = 0, B = 0, C = 0;

  for (int i = 0; i < 3; i++) {
    int mult = (i == 2 ? -1 : 1); // Negate Z coordinate
    A += mult * (ray[i] * ray[i]);
    B += mult * (2 * eye[i] * ray[i]);
    C += mult * (eye[i] * eye[i]);
  }

  checkQuadraticRoots(eye, ray, offset, A, B, C, tVals);
  m_base.intersect(eye, ray, offset, tVals);

  return tVals.size() != 0;
}

bool Cone::checkPoint(const Point3D& poi) const
{
  return (poi[2] >= 0.0 && poi[2] <= 1.0);
}

bool Cone::containsPoint(const Point3D& p) const
{
  return checkPoint(p) && (p[0] * p[0] + p[1] * p[1] < p[2] * p[2]); 
}

Vector3D Cone::getNormal(const Point3D& p) const
{
  int region = determineRegion(p);

  if (region == 0) {
    Vector3D v1 = p - Point3D(0.0, 0.0, 0.0);
    Vector3D v2 = Vector3D(-1 * p[1], p[0], 0);
    return  v2.cross(v1);
  } else {
    return m_base.getNormal(p);
  } 
}

Point2D Cone::textureMapCoords(const Point3D& p) const
{
  Point3D poi = p;
  int region = determineRegion(p);

  if (region == 0) {
    // Map point onto base.
    poi[2] = 1.0;
    return m_base.textureMapCoords(poi); 
  } else if (region == 1) {
    return m_base.textureMapCoords(p);
  } 

  return Point2D(-1, -1);
}

int Cone::determineRegion(const Point3D& p) const
{
  if (p[2] > 1.0 - epsilon && p[2] < 1.0 + epsilon) {
    return 1;
  } else {
    return 0;
  }
}

/* 
  ********** Cylinder **********
*/

Cylinder::Cylinder()
  : m_top(Vector3D(0.0, 0.0, 1.0), Point3D(0.0, 0.0, 1.0), 1.0, Vector3D(0.0, 1.0, 0.0)),
    m_bottom(Vector3D(0.0, 0.0, -1.0), Point3D(0.0, 0.0, 0.0), 1.0, Vector3D(0.0, -1.0, 0.0))
{
}

Cylinder::~Cylinder()
{
}

bool Cylinder::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                         std::list<IntersectionPoint>& tVals) const
{
  double A = 0, B = 0, C = 0;

  for (int i = 0; i < 2; i++) {
    A += ray[i] * ray[i];
    B += 2 * eye[i] * ray[i];
    C += eye[i] * eye[i];
  }
  C -= 1.0;

  checkQuadraticRoots(eye, ray, offset, A, B, C, tVals);
  m_top.intersect(eye, ray, offset, tVals);
  m_bottom.intersect(eye, ray, offset, tVals);

  return tVals.size() != 0;
}

bool Cylinder::checkPoint(const Point3D& poi) const
{
  return (poi[2] >= 0.0 && poi[2] <= 1.0);
}

bool Cylinder::containsPoint(const Point3D& p) const
{
  return checkPoint(p) && (p[0] * p[0] + p[1] * p[1] < 1.0); ; 
}

Vector3D Cylinder::getNormal(const Point3D& p) const
{
  int region = determineRegion(p);

  if (region == 0) {
    return Vector3D(p[0], p[1], 0.0);
  } else if (region == 1) { 
    return m_top.getNormal(p);
  } else {
    return m_bottom.getNormal(p);
  }
}

Point2D Cylinder::textureMapCoords(const Point3D& p) const
{
  int region = determineRegion(p);

  if (region == 0) {
    double theta = acos(p[0]);

    if (p[1] < 0) {
      theta = -theta;
    }
  
    // Texture seemed stretched here so cut the range in half.
    double x = (theta + M_PI) / M_PI;
    if (x > 1) x = x -1;

    return Point2D(x, p[2]);
  } else if (region == 1) {
    return m_top.textureMapCoords(p);
  } else if (region == 2) {
    return m_bottom.textureMapCoords(p);
  } else {
    std::cerr << "Unknown Region" << std::endl;
  }

  return Point2D(-1, -1);
}

int Cylinder::determineRegion(const Point3D& p) const
{
  if (p[2] > -epsilon && p[2] < epsilon)  {
    return 2;
  } else if (p[2] > 1.0 - epsilon && p[2] < 1.0 + epsilon) {
    return 1;
  } else {
    return 0;
  }
}
