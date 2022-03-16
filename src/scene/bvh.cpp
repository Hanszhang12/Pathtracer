#include "bvh.h"

#include "CGL/CGL.h"
#include "triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL {
namespace SceneObjects {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  primitives = std::vector<Primitive *>(_primitives);
  root = construct_bvh(primitives.begin(), primitives.end(), max_leaf_size);
}

BVHAccel::~BVHAccel() {
  if (root)
    delete root;
  primitives.clear();
}

BBox BVHAccel::get_bbox() const { return root->bb; }

void BVHAccel::draw(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->draw(c, alpha);
    }
  } else {
    draw(node->l, c, alpha);
    draw(node->r, c, alpha);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->drawOutline(c, alpha);
    }
  } else {
    drawOutline(node->l, c, alpha);
    drawOutline(node->r, c, alpha);
  }
}

BVHNode *BVHAccel::construct_bvh(std::vector<Primitive *>::iterator start,
                                 std::vector<Primitive *>::iterator end,
                                 size_t max_leaf_size) {

  // TODO (Part 2.1):
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

  // BBox bbox;
  //
  // for (auto p = start; p != end; p++) {
  //   BBox bb = (*p)->get_bbox();
  //   bbox.expand(bb);
  // }
  //
  // BVHNode *node = new BVHNode(bbox);
  // node->start = start;
  // node->end = end;
  //
  // return node;

  BBox bbox;
  int primitive_count = 0;
  for (auto p = start; p != end; p++) {
    primitive_count++;
    BBox bb = (*p)->get_bbox();
    bbox.expand(bb);
  }

  BVHNode *node = new BVHNode(bbox);
  node->start = start;
  node->end = end;
  if (primitive_count <= max_leaf_size) {
    node->start = start;
    node->end = end;
  } else {
    //find the split start + (some integer)
    // double diff = ((bbox.max - bbox.min)/2).x;
    // std::vector<Primitive *>::iterator copied_vector;
    // std::copy(start, end, copied_vector);

    // std::vector<Primitive *>::iterator middle = std::partition(start, end, [=](Primitive *em){ return em->get_bbox().centroid().x < diff; });


    node->l = construct_bvh(start, start + primitive_count/2, max_leaf_size);
    node->r = construct_bvh(start + primitive_count/2, end, max_leaf_size);
  }
  return node;
}


bool BVHAccel::has_intersection(const Ray &ray, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.

  for (auto p : primitives) {
  total_isects++;
  if (p->has_intersection(ray))
    return true;
  }
  return false;

}

bool BVHAccel::intersect(const Ray &ray, Intersection *i, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.
  // double t0 = ray.min_t;
  // double t1 = ray.max_t;

  if (!(node->bb.intersect(ray, ray.min_t, ray.max_t))) {
    return false;
  }

  bool hit1 = false;
  bool hit2 = false;

  if (node->isLeaf()) {
    bool hit3 = false;
    for (auto p = node->start; p != node->end; p++) {
      bool result = (*p)->intersect(ray, i);
      hit3 = hit3 || result;
      if (result) {
        total_isects++;
      }
    }
    return hit3;
  }
  Intersection *i_new1 = i;
  hit1 = intersect(ray, i_new1, node->l);
  hit2 = intersect(ray, i_new1, node->r);
  *i = *i_new1;

  return hit1 || hit2;

  // bool hit = false;
  // for (auto p : primitives) {
  //   total_isects++;
  //   hit = p->intersect(ray, i) || hit;
  // }
  // return hit;


}

} // namespace SceneObjects
} // namespace CGL
