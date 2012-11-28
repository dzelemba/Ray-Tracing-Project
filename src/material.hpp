#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"
#include "image.hpp"
#include <list>

class Primitive;
class Light;

class PhongMaterial {
 public:
  PhongMaterial(const Colour& ks, double shininess,
                double transparency, double refractiveIndex);
  virtual ~PhongMaterial();

  Colour getColour(const Vector3D& normal, const Vector3D& viewDirection,
                   const std::list<Light*>& lights, const Colour& ambient,
                   const Point3D& poi, const Primitive* primitive) const;

  void bump(const std::string& filename);

  const double m_transparency;
  const double m_refractiveIndex;

 protected:
  virtual Colour getDiffuse(const Primitive* primitive, const Point3D& p) const = 0;

  Colour m_ks;
  double m_shininess;

 private:
  Vector3D bumpNormal(const Vector3D& n, const Primitive* primitive, const Point3D& p) const;

  bool m_bump;
  Image m_bumpMap;
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
