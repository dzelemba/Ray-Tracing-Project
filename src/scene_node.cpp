#include "scene_node.hpp"
#include <iostream>
#include <cfloat>
#include "transforms.hpp"
#include "scene.hpp"

const Scene* SceneNode::m_scene = NULL;

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::rotate(char axis, double angle)
{
	set_transform(m_trans * rotation(angle, axis) );
}

void SceneNode::scale(const Vector3D& amount)
{
	set_transform(m_trans * scaling(amount));
}

void SceneNode::translate(const Vector3D& amount)
{
	set_transform(m_trans * translation(amount));
}

bool SceneNode::intersect(const Point3D& eye, const Vector3D& ray, double offset) const
{
  IntersectionPoint poi;
  return intersect(eye, ray, offset, poi);
}

bool SceneNode::intersect(const Point3D& eye, const Vector3D& ray, const double offset, Colour& c) const
{
  IntersectionPoint poi;
  if (intersect(eye, ray, offset, poi)) {
    c = poi.m_owner->getColour(eye, poi);
    return true;
  }

  return false;
}

bool SceneNode::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                                         IntersectionPoint& poi) const
{
  SegmentList segments; 
  intersect(eye, ray, offset, segments);

  if (segments.getMin(offset, poi)) {
    poi.calcPOI(eye, ray);
    return true; 
  }
  
  return false;
}

void SceneNode::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                                          SegmentList& tVals) const
{
  const Point3D transEye = m_invtrans * eye;
  const Vector3D transRay = m_invtrans * ray;

  if (!m_children.empty()) {
    SegmentList childSegments;
    m_children.front()->intersect(transEye, transRay, offset, childSegments);
    tVals.insert(childSegments);
    std::list<SceneNode*>::const_iterator it = m_children.begin();
    it++;
    for ( ; it != m_children.end(); it++) {
      childSegments.clear();
      (*it)->intersect(transEye, transRay, offset, childSegments);
      combineSegments(childSegments, tVals);
    }

    tVals.transformNormals(m_invtrans.transpose());
  }
}

void SceneNode::combineSegments(SegmentList& s1, SegmentList& s2) const
{
  s2.insert(s1);
}

Mesh* SceneNode::getBoundingBox()
{
  std::cerr << "Error! BoundingBox requested from SceneNode" << std::endl;
  return NULL;
}

/*
  ************ IntersectionNode ****************
*/

IntersectionNode::IntersectionNode(const std::string& name)
  : SceneNode(name)
{}

IntersectionNode:: ~IntersectionNode()
{
}

void IntersectionNode::combineSegments(SegmentList& s1, SegmentList& s2) const
{
  s2.intersect(s1);
}

/*
  ************ DifferenceNode ****************
*/

DifferenceNode::DifferenceNode(const std::string& name)
  : SceneNode(name)
{}

DifferenceNode::~DifferenceNode()
{
}

void DifferenceNode::combineSegments(SegmentList& s1, SegmentList& s2) const
{
  s2.remove(s1);
}
/*
  ************ GeometryNode ****************
*/

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}


void GeometryNode::intersect(const Point3D& eye, const Vector3D& ray, const double offset,
                                            SegmentList& tVals) const
{
  const Point3D transEye = m_invtrans * eye;
  const Vector3D transRay = m_invtrans * ray;

  std::list<IntersectionPoint> tValues;
  if (m_primitive->filteredIntersect(transEye, transRay, offset, tValues)) { 
    std::list<IntersectionPoint>::iterator it = tValues.begin();
    std::list<IntersectionPoint>::iterator next = tValues.begin();
    next++;
    for (; it != tValues.end(); it++, it++, next++, next++) {
      // Filter out points that map to an alpha value of 0.
      if (!m_material->hasZeroAlpha(m_primitive, transEye + it->m_t * transRay)) {
        it->calcPrimitivePOI(transEye, transRay);
        next->calcPrimitivePOI(transEye, transRay);
        tVals.insert(*it, *next, this);
      } 
    }

    tVals.transformNormals(m_invtrans.transpose());
  }
}

Mesh* GeometryNode::getBoundingBox()
{
  Mesh* m = m_primitive->getBoundingBox();

 // std::cerr << "Before: " << *m << std::endl;
  m->transform(m_trans);
  //std::cerr << "After: " << *m << std::endl;
  //std::cerr << "trans: " << m_trans << std::endl;

  return m;
}

Colour GeometryNode::getColour(const Point3D& eye, const IntersectionPoint& poi, 
                               const double refractiveIndex, int recursiveDepth) const
{
  Colour c(0.0);

  // Normalize the normal
  // Check if we're inside an object and flip the normal if we are.
  Vector3D norm = poi.m_normal;
  if (refractiveIndex != 1.0) {
    norm = -1 * norm;
  }
  norm.normalize();

  Vector3D viewDirection = eye - poi.m_poi;
  viewDirection.normalize();

  // Add contribution of reflection + refraction.
  double transparency = m_material->m_transparency;
  if (transparency > 0) {
    double reflectance = getReflectiveRatio(viewDirection, norm, refractiveIndex);
    double transmittance = 1.0 - reflectance;

    Colour c1(0.0);
    if (reflectance > epsilon) {
     c1 = c1 + reflectance * reflectionContribution(viewDirection, norm, poi.m_poi, refractiveIndex, recursiveDepth);
    }

    if (transmittance > epsilon) {
      c1 = c1 +  transmittance * refractionContribution(viewDirection, norm, poi.m_poi,
                                                        refractiveIndex, recursiveDepth);  
    }

    c = c + transparency * c1;
  }

  // Now add contributions of all light sources.
  double materialCoeff = 1.0 - transparency;
  if (materialCoeff > 0) {
    c = c + materialCoeff * getLightContribution(poi, viewDirection, norm);
  }
  
  return c;
}

double GeometryNode::getReflectiveRatio(const Vector3D& viewDirection, const Vector3D& normal,
                                        const double refractiveIndex) const
{
  double n1 = refractiveIndex;
  double n2 = m_material->m_refractiveIndex;

  // n2 == 0 signifies fully reflective material
  if (n2 == 0) {
    return 1.0;
  }

  double cosInc = viewDirection.dot(normal); 
  double indexRatio = n1 / n2; 
  double sinT2 = indexRatio * indexRatio * (1.0 - cosInc * cosInc);

  // Check for Total Internal Reflection
  if (sinT2 > 1.0) {
    return 1.0; 
  }

  // Use Fresnel equation to determine the ratio between reflectance and transmittance.
  double cosT = sqrt(1.0 - sinT2);
  double rOrth = (n1 * cosInc - n2 * cosT) / (n1 * cosInc + n2 * cosT);
  double rPar = (n2 * cosInc - n1 * cosT) / (n2 * cosInc + n1 * cosT);

  return (rOrth * rOrth + rPar * rPar) / 2.0;
}

Colour GeometryNode::reflectionContribution(const Vector3D& viewDirection, const Vector3D& normal,
                                            const Point3D& poi, const double refractiveIndex,
                                            int recursiveDepth) const
{
  if (recursiveDepth < 5) {
    Vector3D mirrorDirection = -1 * viewDirection + 2 * viewDirection.dot(normal) * normal;
    IntersectionPoint objPOI;
    
    if (m_scene->root->intersect(poi, mirrorDirection, epsilon,  objPOI)) {
      return objPOI.m_owner->getColour(poi, objPOI, refractiveIndex, recursiveDepth + 1);
    }
  }

  return Colour(0.0);
}

// Assumptions:
//  - No refractive objects inside other refractive objects.
//  - Materials can't have refractiveIndex = 1.
//  - No single plane refractive objects
Colour GeometryNode::refractionContribution(const Vector3D& viewDirection, const Vector3D& normal,
                                            const Point3D& poi, const double refractiveIndex,
                                            int recursiveDepth) const
{
  double n1 = refractiveIndex;
  double n2 = m_material->m_refractiveIndex;
  double cosInc = viewDirection.dot(normal); 
  double indexRatio = n1 / n2; 
  double sinT2 = indexRatio * indexRatio * (1.0 - cosInc * cosInc);

  Vector3D transDirection = -indexRatio * viewDirection +
                             (indexRatio * cosInc - sqrt(1.0 - sinT2)) * normal;
  IntersectionPoint objPOI;
  if (m_scene->root->intersect(poi, transDirection, epsilon, objPOI)) {
    return objPOI.m_owner->getColour(poi, objPOI, n1 == 1.0 ? n2 : 1.0, recursiveDepth);
  } else {
    return m_scene->getBackground(poi, transDirection);
  }
}

Colour GeometryNode::getLightContribution(const IntersectionPoint& poi, const Vector3D& viewDirection, 
                                          const Vector3D& normal) const
{
  // Determine which lights are visible
  std::list<Light*> lights; 
  for (std::list<Light*>::const_iterator it = m_scene->lights.begin(); it != m_scene->lights.end(); it++) {
    Light* light = (*it);

    Vector3D lightDirection = light->position - poi.m_poi; // Note this needs to point towards the light.
    lightDirection.normalize();

    // Check if we get a contribution from this light (i.e. check if any objects are in the way)
    // Ignore transparent objects.
    // TODO: Add supprt for non-fully transparent objects.
    // TODO: If we ever start doing CSG with Refractive materials this won't work.
    SegmentList segments;
    m_scene->root->intersect(poi.m_poi, lightDirection, epsilon, segments);
    std::list<Segment> segs;
    segments.getValidSegments(epsilon, segs);
    bool exit = false;
    for (std::list<Segment>::const_iterator it = segs.begin(); it != segs.end(); it++) {
      if (it->m_start.m_owner->m_material->m_transparency == 0) {
        exit = true;
        break;
      }
    }
    if (exit) {
      break;
    }

    lights.push_back(light);
  }

  return m_material->getColour(normal, viewDirection, lights, m_scene->ambient,
                               poi.m_primitivePOI, m_primitive);
}
