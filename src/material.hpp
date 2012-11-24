#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"
#include "image.hpp"

class Primitive;

class PhongMaterial {
 public:
  PhongMaterial(const Colour& ks, double shininess,
                double transparency, double refractiveIndex);
  virtual ~PhongMaterial();

  void calcDiffuse(const Primitive* primitive, const Point3D& p);
  Colour getCoefficient(const Vector3D& normal, const Vector3D& light,
                        const Vector3D& view) const;
  Colour getAmbient(const Colour& ambient) const;

  const double m_transparency;
  const double m_refractiveIndex;

 protected:
  virtual Colour getDiffuse(const Primitive* primitive, const Point3D& p) const = 0;

  Colour m_ks;
  double m_shininess;

 private:
  Colour m_tempDiffuse;
};

class BasicMaterial : public PhongMaterial {
 public:
  BasicMaterial(const Colour& kd, const Colour& ks, double shininess,
           double transparency, double refractiveIndex);
  virtual ~BasicMaterial();

 protected:
  Colour getDiffuse(const Primitive* primitive, const Point3D& p) const;

 private:
  Colour m_kd;
};

class TextureMap : public PhongMaterial {
 public:
  TextureMap(const std::string& filename, const Colour& ks, double shininess,
             double transparency, double refractiveIndex);
  virtual ~TextureMap();

 protected:
  Colour getDiffuse(const Primitive* primitive, const Point3D& p) const;

 private:
  Image m_textureMap;
};


#endif
