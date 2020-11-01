#pragma once
#include "types/scene_object_types.hpp"
#include "hexon.hpp"

namespace irmc {

	//A tetrahedron is defined as 4 points where one edge is bigger than any of the others
	//And where each edge is not 0 units
	//
	struct Tetron {

		using Vert4 = Vec3f32[4];

		//Subdivided from hexahedron
		enum class HexonDivision : u32 {
			BACK_1, BACK_2,		//Back = blue
			RIGHT_1, RIGHT_2,	//Right = green
			FRONT_1, FRONT_2	//Front = orange
		};

		Vert4 vertices{};

		//Helpers for a single side of a tetrahedron

		Tetron() = default;
		Tetron(const Vert4 &vertices);
		Tetron(HexonDivision side, const Vec3f32 &center = {});										//Unit cube into tetrahedron
		explicit Tetron(const igx::Cube &cube, HexonDivision side, const Vec3f32 &center = {});		//Cube into tetrahedron
		Tetron(const Hexon &hex, HexonDivision side, const Vec3f32 &center = {});				//Hexahedron into tetrahedron

		//Cut a tetrahedron into two smaller ones, isFront is which part should be used for the new tetrahedron
		Tetron(const Tetron &tet, bool isFront);

		//C++ stuff due to C-Style arrays

		~Tetron() = default;
		Tetron(const Tetron&);
		Tetron(Tetron&&);
		Tetron &operator=(const Tetron&);
		Tetron &operator=(Tetron&&);

		Array<igx::Triangle, 4> triangulate() const;
		igx::Triangle triangle(usz a, usz b, usz c) const;

		//edgeSqLength(013) <- gets squared length from [1] to [3]
		//
		inline f32 edgeSqLength(u8 i) const {
			return (vertices[i & 7] - vertices[i >> 3]).squaredMagnitude();
		}

		inline const Vec3f32 &operator[](u8 i) const { return vertices[i]; }
		inline const Vec3f32 &get(u8 i) const { return vertices[i]; }

		inline Vec3f32 &operator[](u8 i) { return vertices[i]; }
		inline Vec3f32 &get(u8 i) { return vertices[i]; }

		Array<Tetron, 2> splitTetra() const;
		Array<Hexon, 4> splitHexa() const;

		inline Vec3f32 getCenter() const {
			return (get(0) + get(1) + get(2) + get(3)) * 0.25f;
		}

		void setCenter(const Vec3f32 &center);

		//Cut between two points (with a percentage of where)
		inline Vec3f32 cut(u8 a, u8 b, f32 perc = 0.5f) const {
			return get(a).lerp(get(b), perc);
		}

		//Cut exactly in between three points
		inline Vec3f32 cutThree(u8 a, u8 b, u8 c) const {
			return (get(a) + get(b) + get(c)) * (1 / 3.f);
		}

	};

}