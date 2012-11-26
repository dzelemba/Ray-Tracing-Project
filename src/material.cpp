#include "material.hpp"
#include "primitive.hpp"

PhongMaterial::PhongMaterial(const Colour& ks, double shininess,
                             double transparency, double refractiveIndex)
  : m_transparency(transparency), m_refractiveIndex(refractiveIndex),
    m_ks(ks), m_shininess(shininess), m_tempDiffuse(0.0)
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::calcDiffuse(const Primitive* primitive, const Point3D& p)
{
  m_tempDiffuse = getDiffuse(primitive, p);
}

Colour PhongMaterial::getCoefficient(const Vector3D& normal, const Vector3D& light, const Vector3D& view) const
{
  Vector3D r = -light + 2 * (light.dot(normal)) * normal;

  return m_tempDiffuse + m_ks * ( pow(r.dot(view), m_shininess) / normal.dot(light) );
}

Colour PhongMaterial::getAmbient(const Colour& ambient) const
{
  return ambient * (m_tempDiffuse);
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
    std::cerr << "No png file found: " << filename << std::endl;
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
  }

  return Colour(m_textureMap(x, y, 0),
                m_textureMap(x, y, 1),
                m_textureMap(x, y, 2));
}
