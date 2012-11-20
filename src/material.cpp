#include "material.hpp"

Material::Material(const Colour& kd, const Colour& ks, double shininess,
                   double transparency, double refractiveIndex)
  : m_kd(kd), m_ks(ks), m_shininess(shininess),
    m_transparency(transparency), m_refractiveIndex(refractiveIndex)
{
}

Material::~Material()
{
}

Colour Material::getCoefficient(const Vector3D& normal, const Vector3D& light, const Vector3D& view) const
{
  Vector3D r = -light + 2 * (light.dot(normal)) * normal;

  return m_kd + m_ks * ( pow(r.dot(view), m_shininess) / normal.dot(light) );
}

Colour Material::getAmbient(const Colour& ambient)
{
  return ambient * (m_kd);
}
