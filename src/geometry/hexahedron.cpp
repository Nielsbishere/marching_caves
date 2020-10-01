#include "geometry/hexahedron.hpp"

namespace irmc {

	static inline constexpr Hexahedron::Vert2x2x2 extract(const igx::Cube &cube) {

		Hexahedron::Vert2x2x2 res;

		Vec3f32 d = cube.max - cube.min;

		for (u16 i = 0; i < 2; ++i)
			for (u16 j = 0; j < 2; ++j)
				for (u16 k = 0; k < 2; ++k)
					res[i][j][k] = cube.min + d * Vec3f32(i, j, k);

		return res;
	}

	Hexahedron::Hexahedron(const igx::Cube &cube):
		pos(extract(cube)) {}

	Hexahedron::Hexahedron(const Vert8 &positions): positions(positions) {}
	Hexahedron::Hexahedron(const Vert2x2x2 &pos): pos(pos) {}

}