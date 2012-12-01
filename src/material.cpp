#include "material.hpp"
#include "primitive.hpp"
#include "light.hpp"

PhongMaterial::PhongMaterial(const Colour& ks, double shininess,
                             double transparency, double refractiveIndex)
  : m_transparency(transparency), m_refractiveIndex(refractiveIndex),
    m_ks(ks), m_shininess(shininess), m_bump(false), m_bumpMap()
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::bump(const std::string& filename)
{
  if (!m_bumpMap.loadPng(filename)) {
    std::cerr << "No bump map file found: " << std::endl;
    return;
  }

  m_bump = true;
}

Colour PhongMaterial::getColour(const Vector3D& normal, const Vector3D& viewDirection,
                                const std::list<Light*>& lights, const Colour& ambient, 
                                const Point3D& poi, const Primitive* primitive) const
{
  // Bump normal if need be.
  Vector3D norm = normal;
  if (m_bump) {
    norm = bumpNormal(norm, primitive, poi);
  }

  // Get diffuse coefficients
  Colour kd = getDiffuse(primitive, poi);

  // First add ambient light.
  Colour c = kd * ambient;

  for (std::list<Light*>::const_iterator it = lights.begin(); it != lights.end(); it++) {
    Light* light = (*it);

    Vector3D lightDirection = light->position - poi; // Note this needs to point towards the light.
    lightDirection.normalize();

    double dist = lightDirection.length();
    Vector3D r = -lightDirection + 2 * (lightDirection.dot(norm)) * norm;

    Colour contribution = (kd + m_ks * ( pow(r.dot(viewDirection), m_shininess) / norm.dot(lightDirection) )) *
                          light->colour * lightDirection.dot(norm) * 
                          (1 / (light->falloff[0] + light->falloff[1] * dist +
                                light->falloff[2] * dist * dist)); 

    // Ignore negative contributions
    if (contribution.R() >= 0 && contribution.G() >= 0 && contribution.B() >= 0) {
      c = c + contribution;
    }
  }

  return c;
}

Vector3D PhongMaterial::bumpNormal(const Vector3D& n, const Primitive* primitive, const Point3D& p) const
{
  Point2D mapCoords = primitive->textureMapCoords(p);

  int x = mapCoords[0] * (double)m_bumpMap.width();
  int y = mapCoords[1] * (double)m_bumpMap.height();
  
  if (x < 0 || x > m_bumpMap.width() || y < 0 || y > m_bumpMap.height()) {
    std::cerr << "Bad Texture Map Coordinates Returned: " << mapCoords[0] << " " << x
              << " " << mapCoords[1] << " " << y << std::endl;
  }

  if (x == 0 || x == m_bumpMap.width() - 1 || y == 0 || y == m_bumpMap.height() - 1) {
    return n;
  }

  double x_gradient = 2 * (m_bumpMap(x - 1, y, 0) - m_bumpMap(x + 1, y, 0));
  double y_gradient = 2 * (m_bumpMap(x, y - 1, 0) - m_bumpMap(x, y + 1, 0));

  return Vector3D(n[0] + x_gradient, n[1] + y_gradient, n[2]);
}

/*
  *************** BasicMaterial ***************
*/

BasicMaterial::BasicMaterial(const Colour& kd, const Colour& ks, double shininess,
                              double transparency, double refractiveIndex) 
  : PhongMaterial(ks, shininess, transparency, refractiveIndex), 
    m_kd(kd)
{
}

BasicMaterial::~BasicMaterial()
{
}

Colour BasicMaterial::getDiffuse(const Primitive* primitive, const Point3D& p) const
{
  (void)primitive;
  (void)p;

  return m_kd;
}

/*
  *************** TextureMap ***************
*/

TextureMap::TextureMap(const std::string& filename, const Colour& ks, double shininess,
                       double transparency, double refractiveIndex) 
  : PhongMaterial(ks, shininess, transparency, refractiveIndex), 
    m_textureMap()
{
  if (!m_textureMap.loadPng(filename)) {
    std::cerr << "No texture map file found: " << filename << std::endl;
  }
}

TextureMap::~TextureMap()
{
}

Colour TextureMap::getDiffuse(const Primitive* primitive, const Point3D& p) const
{
  Point2D mapCoords = primitive->textureMapCoords(p);

  int x = mapCoords[0] * (double)m_textureMap.width();
  int y = mapCoords[1] * (double)m_textureMap.height();
  
  if (x < 0 || x > m_textureMap.width() || y < 0 || y > m_textureMap.height()) {
    std::cerr << "Bad Texture Map Coordinates Returned: " << mapCoords[0] << " " << x
              << " " << mapCoords[1] << " " << y << std::endl;
    return Colour(0.0);
  }

  return Colour(m_textureMap(x, y, 0),
                m_textureMap(x, y, 1),
                m_textureMap(x, y, 2));
}
