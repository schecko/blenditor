#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Implicit_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>

// Domain
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::FT FT;
typedef K::Point_3 Point;
typedef FT (Function)(const Point&);
typedef CGAL::Implicit_mesh_domain_3<Function,K> Mesh_domain;

#ifdef CGAL_CONCURRENT_MESH_3
typedef CGAL::Parallel_tag Concurrency_tag;
#else
typedef CGAL::Sequential_tag Concurrency_tag;
#endif

// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain,K,Concurrency_tag>::type Tr;

typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;

// To avoid verbose function and named parameters call
using namespace CGAL::parameters;

// Function, a capsule:
//   a cylinder centered at x=y=0, with z in [-5, -5]
//   with radius 1 and "round ends".
FT capsule_function(const Point& p)
{
  const FT base = CGAL::square(p.x())+CGAL::square(p.y()) - 1;
  const FT z = p.z();
  if(z > FT(5)) return base+CGAL::square(z-5);
  else if(z < FT(-5)) return base+CGAL::square(z+5);
  else return base;
}

struct Field {
  typedef ::FT FT;
  
  FT operator()(const Point& p, const int, const Mesh_domain::Index) const {
    if(p.z() > 2) return 0.025;
    if(p.z() < -3) return 0.01;
    else return 1;
  }
} field;

int main()
{
  Mesh_domain domain(capsule_function,
                     K::Sphere_3(CGAL::ORIGIN, 49.));

  // Mesh criteria
  Mesh_criteria criteria(facet_angle=30, facet_size=0.5,
			 facet_distance=field);
  
  // Mesh generation
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria);

  // Output
  std::ofstream medit_file("out.mesh");
  c3t3.output_to_medit(medit_file);

  return 0;
}

