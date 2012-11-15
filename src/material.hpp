#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"

class Material {
public:
  virtual ~Material();
  virtual void apply_gl() const = 0;

  virtual Colour getCoefficient(const Vector3D& normal, const Vector3D& light, const Vector3D& view) const = 0;
  virtual Colour getAmbient(const Colour& ambient) = 0;
  virtual double getReflection() const = 0;
protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double reflection);
  virtual ~PhongMaterial();

  virtual void apply_gl() const;

  Colour getCoefficient(const Vector3D& normal, const Vector3D& light, const Vector3D& view) const;
  Colour getAmbient(const Colour& ambient);

  double getReflection() const;
private:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;
  double m_reflection;
};


#endif
