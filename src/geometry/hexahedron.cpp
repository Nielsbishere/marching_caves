#include "geometry/hexahedron.hpp"

namespace irmc {

	static inline constexpr void extract(const igx::Cube &cube, Hexahedron::Vert2x2x2 &out) {

		Vec3f32 d = cube.max - cube.min;

		for (u16 i = 0; i < 2; ++i)
			for (u16 j = 0; j < 2; ++j)
				for (u16 k = 0; k < 2; ++k)
					out[i][j][k] = cube.min + d * Vec3f32(i, j, k);
	}

	template<typename T>
	static inline void copy(void *ptr, const T &t) {
		std::memcpy(ptr, &t, sizeof(T));
	}

	Hexahedron::Hexahedron(const igx::Cube &cube) {
		extract(cube, pos);
	}

	Hexahedron::Hexahedron(const Vert8 &positions) { copy(this, positions); }
	Hexahedron::Hexahedron(const Vert2x2x2 &pos) { copy(this, pos); }

	Array<igx::Triangle, 12> Hexahedron::triangulate() const {
		
		//LUT

		static constexpr u32 lut[] = {

			0'0123,		//Back
			0'0246,		//Left
			0'0145,		//Bottom

			0'4567,		//Front
			0'1357,		//Right
			0'2367		//Top
		};

		//Unpack quads

		Array<igx::Triangle, 12> tris;

		for (u8 i = 0; i < 6; ++i) {

			u8 j = i << 1;

			Vec4u8 k = ((Vec4u32(lut[i]) >> Vec4u32(0, 3, 6, 9)) & 7).cast<Vec4u8>();

			const Vec3f32 i0 = get(k[0]);
			const Vec3f32 i1 = get(k[1]);
			const Vec3f32 i2 = get(k[2]);
			const Vec3f32 i3 = get(k[3]);

			tris[j] = { i0, i1, i2 };
			tris[j + 1] = { i2, i3, i1 };
		}

		return tris;
	}

	Hexahedron::Hexahedron(const Hexahedron &other) { copy(this, other); }
	Hexahedron::Hexahedron(Hexahedron &&other) { copy(this, other); }
	Hexahedron &Hexahedron::operator=(const Hexahedron &other) { copy(this, other); return *this; }
	Hexahedron &Hexahedron::operator=(Hexahedron &&other) { copy(this, other); return *this; }

}