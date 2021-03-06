#pragma once
#include "convexify.h"

namespace Cy
{
	C_NefPolyhedron make_nef(Array<Vertex> vertices, Array<Polygon> polygons);


	struct BlenderData
	{
		bool valid;
		Array<Material> materials;
		Array<BlenderMesh> meshes;
		Array<Object> objects;
		Array<Texture> textures;
	};

	BlenderData extractData(PyObject* blenderData);
}
