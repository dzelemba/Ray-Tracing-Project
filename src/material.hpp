#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"

class Material {
 public:
  Material(const Colour& kd, const Colour& ks, double shininess,
           double transparency, double refractiveIndex);
  virtual ~Material();

  Colour getCoefficient(const Vector3D& normal, const Vector3D& light, const Vector3D& view) const;
  Colour getAmbient(const Colour& ambient);


 private:
  Colour m_kd;
  Colour m_ks;

  double m_shininess;

 public:
  const double m_transparency;
  const double m_refractiveIndex;
};


#endif
