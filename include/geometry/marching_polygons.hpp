#pragma once
#include "hexon.hpp"
#include "tetron.hpp"
#include "geometry/paulbourke_optimized.hpp"

namespace irmc {

	//(2016-2017 school year)
	//Added normals and efficient rendering / culling (https://github.com/Nielsbishere/RaspberryCaveGen/blob/master/Source/MarchingCubes/MarchingCubes.cpp)
	//
	//(2020)
	//Added ability to use hexahedra and cleaned up
	//And to inline the density function (this is important because the density function is called A LOT)
	//
	template<f32 (&d)(const Vec3f32&)>
	struct MarchingPolygons {

		static constexpr f32 delta = 1e-4f, epsilon = 1e-6f;

		//Simple marching cube cell, except it works for any hexahedron
		//
		template<usz verts>
		struct Cell {

			//Members

			Vec3f32 positions[verts]{};
			Vec3f32 normals[verts]{};
			f32 values[verts]{};
			f32 isoLevel{};

			u8 index{};

			//Helpers

			static inline f32 dif(const Vec3f32 &p, const Vec3f32 &axis) {
				return d(p + axis) - d(p - axis);
			}

			//Normal from axis aligned cell
			static inline Vec3f32 n(const Vec3f32 &p) {

				Vec3f32 D(
					dif(p, Vec3f32(delta, 0, 0)),
					dif(p, Vec3f32(0, delta, 0)),
					dif(p, Vec3f32(0, 0, delta))
				);

				if (D == Vec3f32())
					return Vec3f32(0, 1, 0);

				return D.normalize();
			}

			//Member functions

			inline void setPos(u8 i, const Vec3f32 &p) {

				positions[i] = p;
				values[i] = d(p);
				normals[i] = n(p);

				index |= u8(values[i] < isoLevel) << i;
			}

			Cell(const Hexon &hex, f32 isoLevel): 
				isoLevel(isoLevel)
			{
				static_assert(verts == 8, "Cell<8> only has Hexahedron constructor");

				//For transforming hexahedron indices to CCW winded indices
				static constexpr u32 mcCubeIndices[8] = {
					0, 4, 5, 1,
					2, 6, 7, 3
				};

				for (u8 i = 0; i < 8; ++i)
					setPos(i, hex[mcCubeIndices[i]]);
			}

			Cell(const Tetron &tetra, f32 isoLevel): 
				isoLevel(isoLevel)
			{
				static_assert(verts == 4, "Cell<4> only has Tetron constructor");

				for (u8 i = 0; i < 4; ++i)
					setPos(i, tetra[i]);
			}

			inline Vec3f32 interpolateEdge(const Vec3f32 &p0, const Vec3f32 &p1, u8 e0, u8 e1) const {

				f32	v0 = values[e0], v1 = values[e1];

				f32 isoToP0 = isoLevel - v0;
				f32 p1ToP0 = v1 - v0;

				if (std::fabsf(isoToP0) < epsilon || std::fabsf(p1ToP0) < epsilon)
					return p0;

				if (std::fabsf(isoLevel - v1) < epsilon)
					return p1;

				return p1.lerp(p0, isoToP0 / p1ToP0);
			}

			static inline constexpr u8 getEdge(u8 i) {
				return verts == 8 ? mcEdges[i] : mtEdges[i];
			}

			inline Vec3f32 interpolateNormal(u8 i) const {

				u8 edge = getEdge(i);
				u8 e0 = edge >> 3, e1 = edge & 7;

				return interpolateEdge(normals[e0], normals[e1], e0, e1);
			}

			inline Vec3f32 interpolatePos(u8 i) const {

				u8 edge = getEdge(i);
				u8 e0 = edge >> 3, e1 = edge & 7;

				return interpolateEdge(positions[e0], positions[e1], e0, e1);
			}

			//C++ defaults

			Cell() = default;
			~Cell() = default;

			Cell(const Cell&) = default;
			Cell(Cell&&) = default;
			Cell &operator=(const Cell&) = default;
			Cell &operator=(Cell&&) = default;
		};

		//Triangulation

		static inline List<igx::Triangle> triangulate(const Hexon &hexa, f32 isoLevel = 0) {
		
			Cell<8> cell = Cell<8>(hexa, isoLevel);

			u16 edge = (mcEdgeTable[cell.index >> 2] >> (12 * (cell.index & 3))) & 0xFFF;

			if (edge == 0)
				return {};

			//Edge lookups

			Vec3f32 p[12], n[12];

			for (u8 i = 0; i < 12; ++i)
				if (edge & (1 << i)) {
					p[i] = cell.interpolatePos(i);
					n[i] = cell.interpolateNormal(i);
				}

			//Tri lookups

			List<igx::Triangle> res;
			res.reserve(5);

			u64 tris = mcTriTable[cell.index];

			for (u16 i = 0; i < 5; ++i) {

				u16 tri = u16((tris >> (12 * i)) & 0xFFF);

				if (tri == 0)
					break;

				Vec3u16 p012 = (Vec3u16(tri) >> Vec3u16(0, 4, 8)) & 0xF;
				Vec3f32 p0 = p[p012.x], p1 = p[p012.y], p2 = p[p012.z];

				if (p0 == p1 || p1 == p2 || p2 == p0)
					continue;

				res.push_back(igx::Triangle(
					p0, p1, p2,
					n[p012.x], n[p012.y], n[p012.z]
				));
			}

			return res;
		}

		static inline List<igx::Triangle> triangulate(const Tetron &tetra, f32 isoLevel = 0) {
		
			Cell<4> cell = Cell<4>(tetra, isoLevel);

			u8 edge = mtEdgeTable[cell.index];

			if (edge == 0)
				return {};

			//Edge lookups

			Vec3f32 p[6], n[6];

			for (u8 i = 0; i < 6; ++i)
				if (edge & (1 << i)) {
					p[i] = cell.interpolatePos(i);
					n[i] = cell.interpolateNormal(i);
				}

			//Tri lookups

			List<igx::Triangle> res;
			res.reserve(5);

			u32 tris = mtTriTable[cell.index];

			for (u16 i = 0; i < 2; ++i) {

				u16 tri = u16((tris >> (12 * i)) & 0xFFF);

				if (tri == 0)
					break;

				Vec3u16 p012 = (Vec3u16(tri) >> Vec3u16(0, 4, 8)) & 0xF;

				Vec3f32 p0 = p[p012.x], p1 = p[p012.y], p2 = p[p012.z];

				if (p0 == p1 || p1 == p2 || p2 == p0)
					continue;

				res.push_back(igx::Triangle(
					p0, p1, p2,
					n[p012.x], n[p012.y], n[p012.z]
				));
			}

			return res;
		}

	};

}