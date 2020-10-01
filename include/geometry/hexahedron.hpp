#pragma once
#include <types/scene_object_types.hpp>

namespace irmc {

	union Hexahedron {

		using Vert8 = Array<Vec3f32, 8>;
		using Vert2x2x2 = Array<Array<Array<Vec3f32, 2>, 2>, 2>;

		Vert8 positions;
		Vert2x2x2 pos;	//pos[isRight][isTop][isFront]

		//Helpers for a hexahedron

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
		inline Vec3f32 &operator[](usz i) { return positions[i]; }

		//C++ defaults

		~Hexahedron() = default;
		Hexahedron(const Hexahedron&) = default;
		Hexahedron(Hexahedron&&) = default;
		Hexahedron &operator=(const Hexahedron&) = default;
		Hexahedron &operator=(Hexahedron&&) = default;

	};

}