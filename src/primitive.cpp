#include "primitive.hpp"
#include "polyroots.hpp"
#include <cfloat>

// Used to remember stuff about last intersection.
static double epsilon = 0.00001;

Primitive::~Primitive()
{
}

bool Primitive::checkQuadraticRoots(const Point3D& eye, const Vector3D& ray, const double minValue,
                                    const double A, const double B, const double C,
                                    double& minT) const
{
  bool pointFound = false;

  double roots[2];
  int numRoots = quadraticRoots(A, B, C, roots); 
  if (numRoots != 0) {
    for (int i = 0; i < 2; i++) {
      double t = roots[i];
      if (t > minValue && t < minT && checkPoint(eye + t * ray)) {
        minT = t;
        pointFound = true;
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
  ********** Polygon **********
*/

Polygon::Polygon(std::vector<Point3D>& pts, const Vector3D& normal, const Vector3D& up) 
  :  m_verts(pts), m_plane(normal, m_verts.front(), up)
{
}

Polygon::~Polygon()
{
}

bool Polygon::checkPointForLine(const Point3D& p, const Point3D& p1, const Point3D& p2,
                                const Vector3D& normal) const
{
  Vector3D v = p1 - p2;
  Vector3D planeNormal = v.cross(normal);

  return planeNormal.dot(p - p1) < 0;
}

bool Polygon::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                        double& minT, Vector3D& normal) const
{
  double t = m_plane.intersect(eye, ray); 

  if (t > offset && t < minT) {
    Point3D poi = eye + t * ray;

    if (!intersect(poi)) {
      return false;
    }

    minT = t;
    normal = m_plane.m_normal;
    return true;
  }

  return false;
}

bool Polygon::intersect(const Point3D& p) const
{
  double dot = m_plane.m_normal.dot(p - m_plane.m_p);
  if (dot < -epsilon || dot > epsilon) {
    return false;
  }

  std::vector<Point3D>::const_iterator prev = m_verts.begin();
  for (std::vector<Point3D>::const_iterator it = m_verts.begin() + 1; it != m_verts.end(); it++, prev++) {
    if (!checkPointForLine(p, *it, *prev, m_plane.m_normal)) {
      return false;;
    }
  }

  // Also check line formed by first and last.
  if (!checkPointForLine(p, m_verts.front(), m_verts.back(), m_plane.m_normal)) {
    return false;
  }

  return true;
}

Point2D Polygon::textureMapCoords(const Point3D& p) const
{
  // Pick any point to be our "center"
  Point3D center = m_verts.front();  

  // Find max x and y distance based on a coordinate system defined by center
  // and m_up and m_right of the plane.
  Point2D coords;
  double minX = DBL_MAX, maxX = -DBL_MAX;
  double minY = DBL_MAX, maxY = -DBL_MAX;
  for (std::vector<Point3D>::const_iterator it = m_verts.begin(); it != m_verts.end(); it++) {
    Vector3D v = *it - center;
    if (solve3x2System(m_plane.m_up, m_plane.m_right, v, coords)) {
      if (coords[0] > 100000) {
        std::cerr << coords << " " << m_plane.m_up << " " << m_plane.m_right << " " << v << std::endl;
      }
      if (coords[0] < minX) minX = coords[0];
      if (coords[0] > maxX) maxX = coords[0];
      if (coords[1] < minY) minY = coords[1];
      if (coords[1] > maxY) maxY = coords[1];
    } else {
      std::cerr << "Failed to solve system in polygon: " << m_plane.m_up << " "
                << m_plane.m_right << " " << v << std::endl;
    }
  }
  
  // Now get coordinates for our point.
  if (solve3x2System(m_plane.m_up, m_plane.m_right, p - center, coords)) {
    double x = (coords[0] - minX) / (maxX - minX);
    double y = (coords[1] - minY) / (maxY - minY);

    return Point2D(x, y);
  } else {
    std::cerr << "Failed to solve system in polygon: " << m_plane.m_up << " "
              << m_plane.m_right << " " << p - center << std::endl;
  }
  return Point2D(-1, -1);
}
/* 
  ********** Circle **********
*/

Circle::Circle(const Vector3D& normal, const Point3D& center, double radius)
  : m_plane(normal, center), m_center(center), m_radius(radius)
{
}

Circle::Circle(const Vector3D& normal, const Point3D& center, double radius, const Vector3D& up)
  : m_plane(normal, center, up), m_center(center), m_radius(radius)
{
}

Circle::~Circle()
{
}

bool Circle::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                       double& minT, Vector3D& normal) const
{
  double t = m_plane.intersect(eye, ray); 

  if (t > offset && t < minT) {
    Point3D poi = eye + t * ray;

    // Check if point is within circle
    if ((poi - m_center).length() <= m_radius) {
      minT = t;
      normal = m_plane.m_normal;

      return true;
    }
  }

  return false;
}

Point2D Circle::textureMapCoords(const Point3D& p) const
{
  Vector3D v = p - m_center;
  Point2D coords;
  
  if (solve3x2System(m_plane.m_up, m_plane.m_right, v, coords)) {
    return Point2D((coords[0] + 1.0) / 2.0, (coords[1] + 1.0) / 2.0);
  }

  std::cerr << "Circle Texture Map failed to find solution: " << m_plane.m_up << " "
            << m_plane.m_right << " " << v << std::endl;
  return Point2D(-1.0, 1.0);
}
/* 
  ********** NonhierSphere **********
*/
NonhierSphere::~NonhierSphere()
{
}

bool NonhierSphere::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                              double& minT, Vector3D& normal) const 
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

  if (checkQuadraticRoots(eye, ray, offset, A, B, C, minT)) {
    // Now calculate the normal to the point of intersection.
    Point3D poi = eye + minT * ray;
    normal = poi - m_pos;
    
    return true;
  }

  return false;
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
  upVectors.push_back(Vector3D(0.0, 0.0, 1.0));
  upVectors.push_back(Vector3D(0.0, 1.0, 0.0));
  upVectors.push_back(Vector3D(0.0, 1.0, 0.0));
  upVectors.push_back(Vector3D(0.0, 0.0, 1.0));
  upVectors.push_back(Vector3D(0.0, 1.0, 0.0));

  m_box = Mesh(vertices, f, upVectors);
}

NonhierBox::~NonhierBox()
{
}

bool NonhierBox::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                           double& minT, Vector3D& normal) const
{
  return m_box.intersect(eye, ray, offset, minT, normal);
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
                       double& minT, Vector3D& normal) const
{
  return m_unitSphere.intersect(eye, ray, offset, minT, normal);
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
                     double& minT, Vector3D& normal) const
{
  return m_unitCube.intersect(eye, ray, offset, minT, normal);
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
                     double& minT, Vector3D& normal) const
{
  double A = 0, B = 0, C = 0;

  for (int i = 0; i < 3; i++) {
    int mult = (i == 2 ? -1 : 1); // Negate Z coordinate
    A += mult * (ray[i] * ray[i]);
    B += mult * (2 * eye[i] * ray[i]);
    C += mult * (eye[i] * eye[i]);
  }

  bool pointFound = false;
  if (checkQuadraticRoots(eye, ray, offset, A, B, C, minT)) {
    // Calculate normal
    Point3D poi = eye + minT * ray;
    Vector3D v1 = poi - Point3D(0.0, 0.0, 0.0);
    Vector3D v2 = Vector3D(-1 * poi[1], poi[0], 0);
    normal = v2.cross(v1);

    pointFound = true;
  }

  if (m_base.intersect(eye, ray, offset, minT, normal)) {
    pointFound = true;
  }

  return pointFound;
}

bool Cone::checkPoint(const Point3D& poi) const
{
  return (poi[2] >= 0.0 && poi[2] <= 1.0);
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
  : m_top(Vector3D(0.0, 0.0, 1.0), Point3D(0.0, 0.0, 1.0), 1.0),
    m_bottom(Vector3D(0.0, 0.0, -1.0), Point3D(0.0, 0.0, 0.0), 1.0)
{
}

Cylinder::~Cylinder()
{
}

bool Cylinder::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                         double& minT, Vector3D& normal) const
{
  double A = 0, B = 0, C = 0;

  for (int i = 0; i < 2; i++) {
    A += ray[i] * ray[i];
    B += 2 * eye[i] * ray[i];
    C += eye[i] * eye[i];
  }
  C -= 1.0;

  bool pointFound = false;
  if (checkQuadraticRoots(eye, ray, offset, A, B, C, minT)) {
    // Calculate normal
    Point3D poi = eye + minT * ray;
    normal = Vector3D(poi[0], poi[1], 0.0);

    pointFound = true;
  }

  if (m_top.intersect(eye, ray, offset, minT, normal)) {
    pointFound = true;
  }

  if (m_bottom.intersect(eye, ray, offset, minT, normal)) {
    pointFound = true;
  }

  return pointFound;
}

bool Cylinder::checkPoint(const Point3D& poi) const
{
  return (poi[2] >= 0.0 && poi[2] <= 1.0);
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
