#include "pathtracer.h"

#include "scene/light.h"
#include "scene/sphere.h"
#include "scene/triangle.h"


using namespace CGL::SceneObjects;

namespace CGL {

PathTracer::PathTracer() {
  gridSampler = new UniformGridSampler2D();
  hemisphereSampler = new UniformHemisphereSampler3D();

  tm_gamma = 2.2f;
  tm_level = 1.0f;
  tm_key = 0.18;
  tm_wht = 5.0f;
}

PathTracer::~PathTracer() {
  delete gridSampler;
  delete hemisphereSampler;
}

void PathTracer::set_frame_size(size_t width, size_t height) {
  sampleBuffer.resize(width, height);
  sampleCountBuffer.resize(width * height);
}

void PathTracer::clear() {
  bvh = NULL;
  scene = NULL;
  camera = NULL;
  sampleBuffer.clear();
  sampleCountBuffer.clear();
  sampleBuffer.resize(0, 0);
  sampleCountBuffer.resize(0, 0);
}

void PathTracer::write_to_framebuffer(ImageBuffer &framebuffer, size_t x0,
                                      size_t y0, size_t x1, size_t y1) {
  sampleBuffer.toColor(framebuffer, x0, y0, x1, y1);
}

Vector3D
PathTracer::estimate_direct_lighting_hemisphere(const Ray &r,
                                                const Intersection &isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // For this function, sample uniformly in a hemisphere.

  // Note: When comparing Cornel Box (CBxxx.dae) results to importance sampling, you may find the "glow" around the light source is gone.
  // This is totally fine: the area lights in importance sampling has directionality, however in hemisphere sampling we don't model this behaviour.

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // w_out points towards the source of the ray (e.g.,
  // toward the camera if this is a primary ray)
  const Vector3D hit_p = r.o + r.d * isect.t;
  const Vector3D w_out = w2o * (-r.d);

  // This is the same number of total samples as
  // estimate_direct_lighting_importance (outside of delta lights). We keep the
  // same number of samples for clarity of comparison.
  int num_samples = scene->lights.size() * ns_area_light;
  Vector3D L_out;

  // TODO (Part 3): Write your sampling loop here
  // TODO BEFORE YOU BEGIN
  // UPDATE `est_radiance_global_illumination` to return direct lighting instead of normal shading 
  float p = 1 / (2 * PI);
  int i = 0;
  while (i < num_samples) {
      Vector3D w = hemisphereSampler->get_sample(), d = o2w * w, o = (EPS_F * d) + hit_p;
      Ray rod = Ray(o, d);
      Intersection isect_i;
      r.min_t = EPS_F;
      if (bvh->intersect(rod, &isect_i)) {
        //Vector3D emission = isect_i.bsdf->get_emission(), f = isect.bsdf->f(w_out, w);
        L_out += isect_i.bsdf->get_emission() * isect.bsdf->f(w_out, w) * w.z / p;
      }
      i++;
  }
  L_out /= num_samples;
  return L_out;
}

Vector3D
PathTracer::estimate_direct_lighting_importance(const Ray &r,
                                                const Intersection &isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // To implement importance sampling, sample only from lights, not uniformly in
  // a hemisphere.

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // w_out points towards the source of the ray (e.g.,
  // toward the camera if this is a primary ray)
  const Vector3D hit_p = r.o + r.d * isect.t;
  const Vector3D w_out = w2o * (-r.d);
  Vector3D L_out;
  for (SceneLight* lamp : this->scene->lights ) {
    //number of samples to loop through
    int n_s = (lamp->is_delta_light()) ? 1 : ns_area_light;
    Vector3D wi, L_samp = Vector3D(0, 0, 0);
    double distance, pdf;
    int i = 0;
    while (i < n_s) {
      Vector3D rad = lamp->sample_L(hit_p, &wi, &distance, &pdf), direction = w2o * wi;
      if (direction.z >= 0) {
        Vector3D o = (EPS_F * wi) + hit_p;
        Ray owi = Ray(o, wi);
        owi.min_t = EPS_F * 1.1;
        owi.max_t = (distance - (EPS_F * 1.1));
        Intersection isect_new;
        if (!(bvh->intersect(owi, &isect_new))) {
          L_samp += rad * isect.bsdf->f(w_out, direction) * cos_theta(direction) / pdf;
        }
      }
      i++;
    }
    // Normalize
    if (!lamp->is_delta_light()) {
      L_samp /= ns_area_light;
    }
    L_out += L_samp;
  }
  return L_out;
}

Vector3D PathTracer::zero_bounce_radiance(const Ray &r,
                                          const Intersection &isect) {
  // TODO: Part 3, Task 2
  // Returns the light that results from no bounces of light
  return isect.bsdf->get_emission();
}

Vector3D PathTracer::one_bounce_radiance(const Ray &r,
                                         const Intersection &isect) {
  // TODO: Part 3, Task 3
  // Returns either the direct illumination by hemisphere or importance sampling
  // depending on `direct_hemisphere_sample`
  Vector3D fin = (direct_hemisphere_sample) ? estimate_direct_lighting_hemisphere(r, isect) : estimate_direct_lighting_importance(r, isect);
  return fin;
}

Vector3D PathTracer::at_least_one_bounce_radiance(const Ray &r,
                                                  const Intersection &isect) {
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  Vector3D hit_p = r.o + r.d * isect.t;
  Vector3D w_out = w2o * (-r.d);

  Vector3D L_out(0, 0, 0);

  // TODO: Part 4, Task 2
  // Returns the one bounce radiance + radiance from extra bounces at this point.
  // Should be called recursively to simulate extra bounces.


  return L_out;
}

Vector3D PathTracer::est_radiance_global_illumination(const Ray &r) {
  Intersection isect;
  Vector3D L_out;

  // You will extend this in assignment 3-2.
  // If no intersection occurs, we simply return black.
  // This changes if you implement hemispherical lighting for extra credit.

  // The following line of code returns a debug color depending
  // on whether ray intersection with triangles or spheres has
  // been implemented.
  //
  // REMOVE THIS LINE when you are ready to begin Part 3.
  
  if (!bvh->intersect(r, &isect))
    return envLight ? envLight->sample_dir(r) : L_out;

  L_out = (isect.t == INF_D) ? debug_shading(r.d) : normal_shading(isect.n);

  
  //Part 3.2 (zero illumination)
  Vector3D Z_out = zero_bounce_radiance(r, isect);
  // TODO (Part 3): Return the direct illumination.
  //Part 3.3 
  Vector3D D_out = estimate_direct_lighting_hemisphere(r, isect);

  // TODO (Part 4): Accumulate the "direct" and "indirect"
  // parts of global illumination into L_out rather than just direct
  //Part 3.4
  Vector3D I_out =  at_least_one_bounce_radiance(r, isect);

  return I_out + D_out;
}

void PathTracer::raytrace_pixel(size_t x, size_t y) {
  // TODO (Part 1.2):
  // Make a loop that generates num_samples camera rays and traces them
  // through the scene. Return the average Vector3D.
  // You should call est_radiance_global_illumination in this function.

  // TODO (Part 5):
  // Modify your implementation to include adaptive sampling.
  // Use the command line parameters "samplesPerBatch" and "maxTolerance"

  int num_samples = ns_aa;          // total samples to evaluate
  Vector2D origin = Vector2D(x, y); // bottom left corner of the pixel
  // sampleBuffer.update_pixel(Vector3D(0.2, 1.0, 0.8), x, y);
  // sampleCountBuffer[x + y * sampleBuffer.w] = num_samples;

  double w = this->sampleBuffer.w, h = this->sampleBuffer.h;
  double sig = 0, sig_2 = 0;
  int samp = 0;
  Vector3D fin = Vector3D();

  if (num_samples == 1) {
    Ray ray = this->camera->generate_ray((x + 0.5) / w, (y + 0.5) / h);
    ray.depth = this->max_ray_depth;
    fin += est_radiance_global_illumination(ray);
    samp += 1;
  }
  else {
    int samp;
    for (samp = 0; samp < num_samples; samp++) {
      //&& samp in following if statement?
      if (samp % samplesPerBatch == 0 && samp > 0) {
         //double mean = sig / double(i), var = (sig_2 - (sig * sig) / double(i)) / (i - 1.0);
         if (1.96 * sqrt((sig_2 - (sig * sig) / double(samp)) / (samp - 1.0) / double(samp)) <= maxTolerance * sig / double(samp)) {
           break;
         }
      }
      Vector2D s_v = this->gridSampler->get_sample();
      Ray s_r = this->camera->generate_ray((x + s_v.x) / w, (y + s_v.y) / h);
      s_r.depth = this->max_ray_depth;
      Vector3D curr = this->est_radiance_global_illumination(s_r);
      fin += curr;
      sig += curr.illum();
      sig_2 += (curr.illum() * curr.illum());
    }
  }
  fin = fin / (double) samp;
  sampleBuffer.update_pixel(fin, x, y);
  sampleCountBuffer[x + y * w] = samp;
}

void PathTracer::autofocus(Vector2D loc) {
  Ray r = camera->generate_ray(loc.x / sampleBuffer.w, loc.y / sampleBuffer.h);
  Intersection isect;

  bvh->intersect(r, &isect);

  camera->focalDistance = isect.t;
}

} // namespace CGL
