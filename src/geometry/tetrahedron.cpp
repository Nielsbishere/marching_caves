#include "geometry/tetrahedron.hpp"

namespace irmc {

	//Cube: x | (y << 1) | (z << 2)
	//Where bool x,y,z
	//0,0,0 = 0
	//1,0,0 = 1
	//0,1,0 = 2
	//1,1,0 = 3
	//0,0,1 = 4
	//1,0,1 = 5
	//0,1,1 = 6
	//1,1,1 = 7

	const Array<u8, 72> Tetrahedron::indices = {

		0,4,2, 4,5,2, 0,5,4, 5,0,2,		//BACK_1
		5,0,2, 5,1,2, 1,0,2, 0,1,5,		//BACK_2
		
		1,5,2, 1,2,3, 1,5,3, 5,3,2,		//RIGHT_1
		5,3,2, 5,7,2, 5,3,7, 7,3,2,		//RIGHT_2

		4,5,2, 4,6,2, 4,5,6, 5,6,2,		//FRONT_1
		5,6,2, 5,6,7, 6,7,2, 5,7,2		//FRONT_2
	};

	const Array<Vec3f32, 6> Tetrahedron::dirFromSide = {
		Vec3f32(-1,  0,  0),
		Vec3f32(0,  0,  -1),
		Vec3f32(1,  0,  0),
		Vec3f32(0,  1,  0),
		Vec3f32(0,  -1,  0),
		Vec3f32(0,  0,  1)
	};

	static inline constexpr Vec3f32 processIndex(u8 i) {
		return Vec3f32(f32(i & 1), f32((i >> 1) & 1), f32(i >> 2));
	}

	static inline constexpr Vec3f32 processIndex(u8 i, const Hexahedron &hex) {
		Vec3f32 d = processIndex(i);
		return hex.get(d.x, d.y, d.z);
	}

	static inline constexpr Array<igx::Triangle, 24> processIndices() {

		Array<igx::Triangle, 24> tris;

		for(usz i = 0; i < tris.size(); ++i)
			tris[i] = igx::Triangle{ 
				processIndex(Tetrahedron::indices[3 * i]), 
				processIndex(Tetrahedron::indices[3 * i + 1]), 
				processIndex(Tetrahedron::indices[3 * i + 2]) 
			};

		return tris;
	}

	const Array<igx::Triangle, 24> Tetrahedron::localTriangles = processIndices();

	Array<igx::Triangle, 24> Tetrahedron::getTriangles(const Hexahedron &hex) {

		Array<igx::Triangle, 24> tris;

		for(usz i = 0; i < tris.size(); ++i)
			tris[i] = igx::Triangle{ 
				processIndex(indices[3 * i], hex), 
				processIndex(indices[3 * i + 1], hex), 
				processIndex(indices[3 * i + 2], hex) 
			};

		return tris;
	}

	Array<igx::Triangle, 4> Tetrahedron::getTriangles(const Hexahedron &hex, Side side) {

		Array<igx::Triangle, 4> tris;

		for(usz i = 4 * u32(side), j = i + 4; i < j; ++i)
			tris[i] = igx::Triangle{ 
				processIndex(indices[3 * i], hex), 
				processIndex(indices[3 * i + 1], hex), 
				processIndex(indices[3 * i + 2], hex) 
			};

		return tris;
	}


	Tetrahedron::Tetrahedron(const igx::Cube &cube, Side side): hex(cube), side(side) {}
	Tetrahedron::Tetrahedron(const Hexahedron &hex, Side side): hex(hex), side(side) {}

	Array<igx::Triangle, 4> Tetrahedron::getTriangles() const {
		return getTriangles(hex, side);
	}

}