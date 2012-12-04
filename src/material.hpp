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

  // Texture mapped images might have an alpha channel
  // So we should avoid hits where this is the case.
  virtual bool hasZeroAlpha(const Primitive* primitive, const Point3D& p) const = 0;

  const double m_transparency;
  const double m_refractiveIndex;

  Vector3D bumpNormal(const Vector3D& n, const Primitive* primitive, const Point3D& p) const;
 protected:
  virtual Colour getDiffuse(const Primitive* primitive, const Point3D& p) const = 0;

  Colour m_ks;
  double m_shininess;

 private:

  bool m_bump;
  Image m_bumpMap;
};

class BasicMaterial : public PhongMaterial {
 public:
  BasicMaterial(const Colour& kd, const Colour& ks, double shininess,
           double transparency, double refractiveIndex);
  virtual ~BasicMaterial();

  bool hasZeroAlpha(const Primitive* primitive, const Point3D& p) const;

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

  bool hasZeroAlpha(const Primitive* primitive, const Point3D& p) const;

 protected:
  Colour getDiffuse(const Primitive* primitive, const Point3D& p) const;

 private:
  bool getMapCoords(const Primitive* primitve, const Point3D& p, int coords[2]) const;  

  Image m_textureMap;
};


#endif
