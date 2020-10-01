#pragma once
#include "types/scene_object_types.hpp"
#include "hexahedron.hpp"

namespace irmc {

	struct Tetrahedron {

		//Subdivided version from
		//https://demonstrations.wolfram.com/ThreePyramidsThatFormACube/
		enum class Side : u32 {
			BACK_1, BACK_2,		//Back = blue
			RIGHT_1, RIGHT_2,	//Right = green
			FRONT_1, FRONT_2	//Front = orange
		};

		static const Array<Vec3f32, 6> dirFromSide;

		Hexahedron hex;
		Side side;

		//Helpers for a single side of a tetrahedron

		Tetrahedron(const igx::Cube &cube, Side side);
		Tetrahedron(const Hexahedron &hex, Side side);

		Array<igx::Triangle, 4> getTriangles() const;

		//C++ defaults

		~Tetrahedron() = default;
		Tetrahedron(const Tetrahedron&) = default;
		Tetrahedron(Tetrahedron&&) = default;
		Tetrahedron &operator=(const Tetrahedron&) = default;
		Tetrahedron &operator=(Tetrahedron&&) = default;

		//Getting all triangles

		static Array<igx::Triangle, 4> getTriangles(const Hexahedron &hex, Side side);
		static Array<igx::Triangle, 24> getTriangles(const Hexahedron &hex);

		static const Array<u8, 72> indices;
		static const Array<igx::Triangle, 24> localTriangles;

	};

}