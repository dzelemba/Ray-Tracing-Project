#ifndef SCENE_NODE_HPP
#define SCENE_NODE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"
#include "light.hpp"
#include "segmentlist.hpp"

class GeometryNode;
class Scene;

class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  const Matrix4x4& get_transform() const { return m_trans; }
  const Matrix4x4& get_inverse() const { return m_invtrans; }
  
  void set_transform(const Matrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.invert();
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child)
  {
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  // Intersect ray with the scene
  // Returns GeometryNode that is closest to eye. 
  // Also gives point of intersection and normal.
  bool intersect(const Point3D& eye, const Vector3D& ray, double offset) const;
  bool intersect(const Point3D& eye, const Vector3D& ray, double offset, Colour& c) const;
  bool intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                                IntersectionPoint& poi) const;
  
  // Actual function used for the recursion.
  // Keeps track of the minimum t found so far to determine which object to return.
  virtual void intersect(const Point3D& eye, const Vector3D& ray, const double offest,
                                         SegmentList& tVals) const;

  std::string m_name;

  static void setScene(const Scene* scene) { m_scene = scene; }

  void cacheTransforms();
protected:
  virtual void cacheTransforms(const Matrix4x4& rayTrans, const Matrix4x4& normalTrans);
  virtual void combineSegments(SegmentList& s1, SegmentList& s2) const;

  // Useful for picking
  int m_id;

  // Transformations
  Matrix4x4 m_trans;
  Matrix4x4 m_invtrans;

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;

  // Scene this node is in.
  static const Scene* m_scene;
};

class IntersectionNode : public SceneNode {
 public:
  IntersectionNode(const std::string& name);
  virtual ~IntersectionNode();

 protected:
  void combineSegments(SegmentList& s1, SegmentList& s2) const;
};

class DifferenceNode : public SceneNode {
 public:
  DifferenceNode(const std::string& name);
  virtual ~DifferenceNode();

 protected:
  void combineSegments(SegmentList& s1, SegmentList& s2) const;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  const PhongMaterial* get_material() const;
  PhongMaterial* get_material();

  void set_material(PhongMaterial* material)
  {
    m_material = material;
  }

  Colour getColour(const Point3D& eye, const IntersectionPoint& poi,
                   const double refractiveIndex = 1.0, int recursiveDepth = 0) const;

  // Overwritten to do actual intersection
  void intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                                 SegmentList& tVals) const;

protected:
  void cacheTransforms(const Matrix4x4& rayTrans, const Matrix4x4& normalTrans);

  PhongMaterial* m_material;
  Primitive* m_primitive;

private:
  double getReflectiveRatio(const Vector3D& viewDirection, const Vector3D& normal,
                            const double refractiveIndex) const;
  Colour reflectionContribution(const Vector3D& viewDirection, const Vector3D& normal, const Point3D& poi,
                                const double refractiveIndex, int recursiveDepth) const;
  Colour refractionContribution(const Vector3D& viewDirection, const Vector3D& normal, const Point3D& poi,
                                const double refractiveIndex, int recursiveDepth) const;
  Colour getLightContribution(const Point3D& poi, const Vector3D& viewDirection, const Vector3D& normal) const;

  // Total transform going up the hierarchy applied to the poi (Mk^-1 * ... M1^-1 )p
  // Also happens to be the same as the total transformation applied to eye, ray 
  // going down the hierarchy.
  Matrix4x4 m_totalPointTransform;

  // Total transform going up the hierarchy applied to normal (M1^-T * ... Mk^-T)n
  Matrix4x4 m_totalNormalTransform;
};

#endif
