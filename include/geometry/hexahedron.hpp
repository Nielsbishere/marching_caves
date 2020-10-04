#pragma once
#include <types/scene_object_types.hpp>

namespace irmc {

	//A hexahedron is defined as 8 points
	//Where each edge is not 0 units
	//
	union Hexahedron {

		using Vert8 = Vec3f32[8];
		using Vert2x2x2 = Vec3f32[2][2][2];

		Vert8 positions{};
		Vert2x2x2 pos;	//pos[isRight][isTop][isFront]

		//Helpers for a hexahedron

		Hexahedron() = default;
		Hexahedron(const igx::Cube &cube);
		Hexahedron(const Vert8 &positions);
		Hexahedron(const Vert2x2x2 &pos);

		inline Vec3f32 &get(bool isRight, bool isTop, bool isFront) {
			return pos[isRight][isTop][isFront];
		}

		inline constexpr const Vec3f32 &get(bool isRight, bool isTop, bool isFront) const {
			return pos[isRight][isTop][isFront];
		}

		inline const Vec3f32 &operator[](usz i) const { return positions[i]; }
		inline const Vec3f32 &get(usz i) const { return positions[i]; }

		inline Vec3f32 &operator[](usz i) { return positions[i]; }
		inline Vec3f32 &get(usz i) { return positions[i]; }

		Array<igx::Triangle, 12> triangulate() const;

		//C++ stuff due to C-Style arrays

		~Hexahedron() = default;
		Hexahedron(const Hexahedron&);
		Hexahedron(Hexahedron&&);
		Hexahedron &operator=(const Hexahedron&);
		Hexahedron &operator=(Hexahedron&&);

	};

}