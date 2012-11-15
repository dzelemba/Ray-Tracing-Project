#include "material.hpp"

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double reflection)
  : m_kd(kd), m_ks(ks), m_shininess(shininess), m_reflection(reflection)
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::apply_gl() const
{
  // Perform OpenGL calls necessary to set up this material.
}

Colour PhongMaterial::getCoefficient(const Vector3D& normal, const Vector3D& light, const Vector3D& view) const
{
  Vector3D r = -light + 2 * (light.dot(normal)) * normal;

  return m_kd + m_ks * ( pow(r.dot(view), m_shininess) / normal.dot(light) );
}

Colour PhongMaterial::getAmbient(const Colour& ambient)
{
  return ambient * (m_kd);
}

double PhongMaterial::getReflection() const
{
  return m_reflection;
}
