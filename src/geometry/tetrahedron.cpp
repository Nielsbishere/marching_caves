#include "geometry/tetron.hpp"

namespace irmc {

	template<typename T>
	static inline void copy(void *ptr, const T &t) {
		std::memcpy(ptr, &t, sizeof(T));
	}

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

	static inline constexpr Vec3f32 processIndex(u16 i, const Vec3f32 &center = {}) {
		return Vec3f32(f32(i & 1), f32((i >> 1) & 1), f32(i >> 2)) + center;
	}

	static inline constexpr Vec3f32 processIndex(u16 i, const Vec3f32 &center, const Hexon &hex) {
		Vec3f32 d = processIndex(i);
		return hex.get(d.x, d.y, d.z) + center;
	}

	//https://demonstrations.wolfram.com/ThreePyramidsThatFormACube/
	template<typename ...args>
	static constexpr inline void divideHexon(Tetron::HexonDivision side, const Vec3f32 &center, Tetron::Vert4 &out, const args &...arg) {

		//LUT hexahedron to points

		static constexpr u16 divs[6] = {
			0'0425, 0'0125,		//Blue
			0'1523, 0'5723,		//Green
			0'4526, 0'5726		//Orange
		};

		static constexpr Vec4u16 shifts = { 0, 3, 6, 9 };

		Vec4u16 points = (Vec4u16(divs[usz(side)]) >> shifts) & 7;

		out[0] = processIndex(points[0], center, arg...);
		out[1] = processIndex(points[1], center, arg...);
		out[2] = processIndex(points[2], center, arg...);
		out[3] = processIndex(points[3], center, arg...);
	}

	Tetron::Tetron(const Vert4 &vertices) { copy(this, vertices); }

	Tetron::Tetron(const igx::Cube &cube, HexonDivision side, const Vec3f32 &center): 
		Tetron(Hexon(cube), side, center) {}

	Tetron::Tetron(const Hexon &hex, HexonDivision side, const Vec3f32 &center) {
		divideHexon(side, center, vertices, hex);
	}

	Tetron::Tetron(HexonDivision side, const Vec3f32 &center) {
		divideHexon(side, center, vertices);
	}

	igx::Triangle Tetron::triangle(usz a, usz b, usz c) const {
		return igx::Triangle{ vertices[a], vertices[b], vertices[c] };
	}

	Array<igx::Triangle, 4> Tetron::triangulate() const {
		return {
			triangle(0, 1, 2), triangle(1, 2, 3),
			triangle(0, 2, 3), triangle(0, 1, 3)
		};
	}

	//Optimized and overcomplicated version of
	//https://gist.github.com/KitVanDeBunt/96cbd4526cbfa9c6fddbd29713d7a984
	//
	Array<Tetron, 2> Tetron::splitTetra() const {

		static constexpr u32 edges[4] = {
			0'010203, 0'101213,
			0'202123, 0'303132
		};

		static constexpr u32 notEdges[4] = {
			0'231312, 0'230302,
			0'130301, 0'120201
		};

		//Calculate longest point and the edges and points that aren't part of the edge

		f32 longestEdgel = 0;

		u8 longestNotEdge = 0;
		u8 longestEdge = 0;

		static constexpr Vec3u32 shift = Vec3u32(0, 6, 12), mask = 077;

		for (u8 i = 0; i < 4; ++i) {
			
			Vec3u32 edgei = (Vec3u32(edges[i]) >> shift) & mask;
			Vec3u32 notEdgei = (Vec3u32(notEdges[i]) >> shift) & mask;

			for (u8 j = 0; j < 3; ++j) {

				u8 edgej = u8(edgei[j]);
				f32 l = edgeSqLength(edgej);

				if (l > longestEdgel) {
					longestEdgel = l;
					longestEdge = edgej;
					longestNotEdge = u8(notEdgei[j]);
				}
			}
		}

		//Unpack

		const Vec3f32 e0 = get(longestEdge & 7);
		const Vec3f32 e1 = get(longestEdge >> 3);

		const Vec3f32 n0 = get(longestNotEdge & 7);
		const Vec3f32 n1 = get(longestNotEdge >> 3);

		//Interp

		Array<Tetron, 2> tetra;

		const Vec3f32 cut = e0.lerp(e1, .5f);

		tetra[0][0] = n0;
		tetra[0][1] = e0;
		tetra[0][2] = n1;
		tetra[0][3] = cut;

		tetra[1][0] = n0;
		tetra[1][1] = e1;
		tetra[1][2] = n1;
		tetra[1][3] = cut;

		return tetra;
	}

	//Optimized and overcomplicated version of
	//https://gist.github.com/KitVanDeBunt/96cbd4526cbfa9c6fddbd29713d7a984#file-subdevidetetrahedra-cpp-L423
	//
	Array<Hexon, 4> Tetron::splitHexa() const {

		//Every hexahedra gets:
		//1 edge point of the tetrahedron
		//1 center point of the tetrahedron
		//3 halfway points from edges
		//3 center corners

		const Vec3f32 center = getCenter();

		Array<Hexon, 4> hexahedra;

		for (u8 i = 0; i < 4; ++i) {

			Hexon &hex = hexahedra[i];
			
			u8 i1 = (i + 1) & 3;
			u8 i2 = (i + 2) & 3;
			u8 i3 = (i + 3) & 3;
			
			hex[0] = get(i);

			hex[1] = cut(i, i1);
			hex[2] = cut(i, i2);
			hex[4] = cut(i, i3);

			hex[3] = cutThree(i, i1, i2);
			hex[5] = cutThree(i, i1, i3);
			hex[6] = cutThree(i, i2, i3);

			hex[7] = center;
		}

		return hexahedra;
	}

	void Tetron::setCenter(const Vec3f32 &center) {

		Vec3f32 oldCenter = getCenter();

		for (usz i = 0; i < 4; ++i)
			vertices[i] = vertices[i] - oldCenter + center;

	}

	Tetron::Tetron(const Tetron &other) { copy(this, other); }
	Tetron::Tetron(Tetron &&other) { copy(this, other); }
	Tetron &Tetron::operator=(const Tetron &other) { copy(this, other); return *this; }
	Tetron &Tetron::operator=(Tetron &&other) { copy(this, other); return *this; }
}