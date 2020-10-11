#pragma once
#include "hexahedron.hpp"

namespace irmc {

	//Marching cubes LUT from http://paulbourke.net/geometry/polygonise/

	static constexpr u16 mcEdgeTable[256] = {
		0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
		0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
		0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
		0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
		0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
		0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
		0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
		0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
		0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
		0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
		0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
		0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
		0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
		0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
		0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
		0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
		0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
		0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
		0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
		0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
		0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
		0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
		0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
		0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
		0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
		0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
		0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
		0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
		0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
		0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
		0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
		0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0 
	};

	//Transformed triTable to a u64[] as a (nibble[3])[5] where 0x000 signifies no tri
	//
	//2560 byte lookup table instead of 10240, potentially better cache (also less loc)
	static constexpr u64 mcTriTable[256] = {

		0x000'000'000'000'000,  0x000'000'000'000'083,  0x000'000'000'000'019,  0x000'000'000'981'183,
		0x000'000'000'000'12A,  0x000'000'000'12A'083,  0x000'000'000'029'92A,  0x000'000'A98'2A8'283,
		0x000'000'000'000'3B2,  0x000'000'000'8B0'0B2,  0x000'000'000'23B'190,  0x000'000'98B'19B'1B2,
		0x000'000'000'BA3'3A1,  0x000'000'8BA'08A'0A1,  0x000'000'BA9'3B9'390,  0x000'000'000'A8B'98A,

		0x000'000'000'000'478,  0x000'000'000'734'430,  0x000'000'000'847'019,  0x000'000'731'471'419,
		0x000'000'000'847'12A,  0x000'000'12A'304'347,  0x000'000'847'902'92A,  0x000'794'273'297'2A9,
		0x000'000'000'3B2'847,  0x000'000'204'B24'B47,  0x000'000'23B'847'901,  0x000'921'9B2'94B'47B,
		0x000'000'784'3BA'3A1,  0x000'7B4'104'14B'1BA,  0x000'B03'9BA'90B'478,  0x000'000'9BA'4B9'47B,

		0x000'000'000'000'954,  0x000'000'000'083'954,  0x000'000'000'150'054,  0x000'000'315'835'854,
		0x000'000'000'954'12A,  0x000'000'495'12A'308,  0x000'000'402'542'52A,  0x000'348'354'325'2A5,
		0x000'000'000'23B'954,  0x000'000'495'08B'0B2,  0x000'000'23B'015'054,  0x000'485'28B'258'215,
		0x000'000'954'A13'A3B,  0x000'8BA'8A1'081'495,  0x000'B03'5BA'50B'540,  0x000'000'A8B'58A'548,

		0x000'000'000'579'978,  0x000'000'573'953'930,  0x000'000'157'017'078,  0x000'000'000'357'153,
		0x000'000'A12'957'978,  0x000'573'530'950'A12,  0x000'A52'857'825'802,  0x000'000'357'253'2A5,
		0x000'000'3B2'789'795,  0x000'27B'920'972'957,  0x000'157'178'018'23B,  0x000'000'715'B17'B21,
		0x000'A3B'A13'857'958,  0xBA0'10A'7B0'509'570,  0x570'807'A50'B03'BA0,  0x000'000'000'7B5'BA5,


		0x000'000'000'000'A65,  0x000'000'000'5A6'083,  0x000'000'000'5A6'901,  0x000'000'5A6'198'183,
		0x000'000'000'261'165,  0x000'000'308'126'165,  0x000'000'026'906'965,  0x000'328'526'582'598,
		0x000'000'000'A65'23B,  0x000'000'A65'B20'B08,  0x000'000'5A6'23B'019,  0x000'98B'9B2'192'5A6,
		0x000'000'513'653'63B,  0x000'5B6'051'0B5'08B,  0x000'059'065'036'3B6,  0x000'000'B98'69B'659,

		0x000'000'000'478'5A6,  0x000'000'65A'473'430,  0x000'000'847'5A6'190,  0x000'794'173'197'A65,
		0x000'000'478'651'612,  0x000'347'304'526'125,  0x000'026'065'905'847,  0x269'596'329'794'739,
		0x000'000'A65'784'3B2,  0x000'27B'420'472'5A6,  0x000'5A6'23B'478'019,  0x5A6'7B4'94B'9B2'921,
		0x000'5B6'351'3B5'847,  0x04B'7B4'10B'5B6'51B,  0x847'B63'036'065'059,  0x000'7B9'479'69B'659,

		0x000'000'000'64A'A49,  0x000'000'083'49A'4A6,  0x000'000'640'A60'A01,  0x000'61A'864'816'831,
		0x000'000'264'124'149,  0x000'264'249'129'308,  0x000'000'000'426'024,  0x000'000'426'824'832,
		0x000'000'B23'A64'A49,  0x000'4A6'49A'28B'082,  0x000'61A'064'016'3B2,  0x8B1'21B'481'61A'641,
		0x000'B63'913'936'964,  0x641'914'B61'810'8B1,  0x000'000'064'360'3B6,  0x000'000'000'B68'648,

		0x000'000'89A'78A'7A6,  0x000'67A'09A'0A7'073,  0x000'180'178'1A7'A67,  0x000'000'173'A71'A67,
		0x000'867'189'168'126,  0x739'093'679'291'269,  0x000'000'602'706'780,  0x000'000'000'672'732,
		0x000'867'A89'A68'23B,  0x9A7'67A'097'27B'207,  0x23B'67A'1A7'178'180,  0x000'671'A61'B17'B21,
		0x136'B63'916'867'896,  0x000'000'000'B67'091,  0x000'B60'3B0'706'780,  0x000'000'000'000'7B6,


		0x000'000'000'000'76B,  0x000'000'000'B76'308,  0x000'000'000'B76'019,  0x000'000'B76'831'819,
		0x000'000'000'6B7'A12,  0x000'000'6B7'308'12A,  0x000'000'6B7'2A9'290,  0x000'A98'A83'2A3'6B7,
		0x000'000'000'627'723,  0x000'000'620'760'708,  0x000'000'019'237'276,  0x000'876'198'186'162,
		0x000'000'137'A17'A76,  0x000'108'187'17A'A76,  0x000'6A7'0A9'07A'037,  0x000'000'8A9'7A8'76A,

		0x000'000'000'B86'684,  0x000'000'046'306'36B,  0x000'000'901'846'86B,  0x000'B36'931'963'946,
		0x000'000'2A1'6B8'684,  0x000'046'06B'30B'12A,  0x000'2A9'029'46B'4B8,  0x463'B36'943'A32'A93,
		0x000'000'462'842'823,  0x000'000'000'462'042,  0x000'438'246'234'190,  0x000'000'246'142'194,
		0x000'6A1'846'861'813,  0x000'000'604'A06'A10,  0xA93'039'6A3'438'463,  0x000'000'000'6A4'A94,

		0x000'000'000'76B'495,  0x000'000'B76'495'083,  0x000'000'76B'540'501,  0x000'315'354'834'B76,
		0x000'000'76B'A12'954,  0x000'495'083'12A'6B7,  0x000'402'42A'54A'76B,  0xB76'A52'325'354'348,
		0x000'000'549'762'723,  0x000'687'062'086'954,  0x000'540'150'376'362,  0x158'485'218'687'628,
		0x000'137'176'A16'954,  0x954'870'107'176'16A,  0x37A'6A7'03A'4A5'40A,  0x000'48A'54A'7A8'76A,

		0x000'000'B89'6B9'695,  0x000'095'056'063'36B,  0x000'56B'015'05B'0B8,  0x000'000'531'635'6B3,
		0x000'B56'9B8'95B'12A,  0x12A'569'096'06B'0B3,  0x025'A52'805'B56'B85,  0x000'A53'2A3'635'6B3,
		0x000'382'562'528'589,  0x000'000'062'960'956,  0x628'382'568'180'158,  0x000'000'000'216'156,
		0x896'569'386'16A'136,  0x000'560'950'A06'A10,  0x000'000'000'56A'038,  0x000'000'000'000'A56,


		0x000'000'000'75B'B5A,  0x000'000'830'B75'B5A,  0x000'000'190'5AB'5B7,  0x000'831'981'AB7'A75,
		0x000'000'751'B71'B12,  0x000'72B'175'127'083,  0x000'2B7'902'927'975,  0x982'328'592'72B'752,
		0x000'000'375'235'25A,  0x000'A25'875'852'820,  0x000'3A2'537'5A3'901,  0x752'A25'872'921'982,
		0x000'000'000'375'135,  0x000'000'175'071'087,  0x000'000'537'935'903,  0x000'000'000'597'987,

		0x000'000'AB8'5A8'584,  0x000'B30'5AB'5B0'504,  0x000'A45'8AB'84A'019,  0x314'941'B34'A45'AB4,
		0x000'458'2B8'285'251,  0x51B'2B1'45B'0B3'04B,  0xB85'458'2B5'059'025,  0x000'000'000'2B3'945,
		0x000'384'345'352'25A,  0x000'000'420'524'5A2,  0x019'458'385'35A'3A2,  0x000'942'192'524'5A2,
		0x000'000'351'853'845,  0x000'000'000'105'045,  0x000'035'905'853'845,  0x000'000'000'000'945,

		0x000'000'9AB'49B'4B7,  0x000'9AB'9B7'497'083,  0x000'74B'140'1B4'1AB,  0xAB4'74B'1A4'348'314,
		0x000'912'92B'9B4'4B7,  0x083'2B1'91B'9B7'974,  0x000'000'240'B42'B74,  0x000'324'834'B42'B74,
		0x000'749'237'279'29A,  0x207'870'A27'974'9A7,  0x40A'1A0'74A'3A2'37A,  0x000'000'000'874'1A2,
		0x000'000'713'417'491,  0x000'871'081'417'491,  0x000'000'000'743'403,  0x000'000'000'000'487,

		0x000'000'000'AB8'9A8,  0x000'000'B9A'39B'309,  0x000'000'8AB'0A8'01A,  0x000'000'000'B3A'31A,
		0x000'000'9B8'1B9'12B,  0x000'2B9'129'39B'309,  0x000'000'000'80B'02B,  0x000'000'000'000'32B,
		0x000'000'A89'28A'238,  0x000'000'000'092'9A2,  0x000'1A8'018'28A'238,  0x000'000'000'000'1A2,
		0x000'000'000'918'138,  0x000'000'000'000'091,  0x000'000'000'000'038,  0x000'000'000'000'000,
	};

	//(2016-2017 school year)
	//Added normals and efficient rendering / culling (https://github.com/Nielsbishere/RaspberryCaveGen/blob/master/Source/MarchingCubes/MarchingCubes.cpp)
	//
	//(2020)
	//Added ability to use hexahedra and cleaned up
	//And to inline the density function (this is important because the density function is called A LOT)
	//
	template<f32 (&d)(const Vec3f32&)>
	struct MarchingHexahedra {

		//Simple marching cube cell, except it works for any hexahedron
		//
		struct Cell {

			//Members

			Hexahedron hexa;

			Vec3f32 normals[8]{};
			f32 values[8]{};
			f32 isoLevel{};

			u8 index{};

			//Helpers

			static inline f32 dif(const Vec3f32 &p, const Vec3f32 &axis) {
				return d(p + axis) - d(p - axis);
			}

			static constexpr f32 epsilon = 1e-5f, delta = 1e-4f;

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

			Cell(const Hexahedron &hex, f32 isoLevel): 
				isoLevel(isoLevel)
			{
				//Convert to correct winding order so the LUT is still correct

				hexa[0] = hex.pos[0][0][0];
				hexa[1] = hex.pos[1][0][0];
				hexa[2] = hex.pos[1][0][1];
				hexa[3] = hex.pos[0][0][1];

				hexa[4] = hex.pos[0][1][0];
				hexa[5] = hex.pos[1][1][0];
				hexa[6] = hex.pos[1][1][1];
				hexa[7] = hex.pos[0][1][1];

				//

				for (u8 i = 0; i < 8; ++i) {

					const Vec3f32 &p = hexa[i];

					normals[i] = n(p);
					values[i] = d(p);

					index |= u8(values[i] < isoLevel + 1e-6f) << i;
				}
			}

			inline Vec3f32 interpolateEdge(const Vec3f32 (&p)[8], u8 e0, u8 e1) const {

				f32		v0 = values[e0],	v1 = values[e1];
				Vec3f32 p0 = p[e0],			p1 = p[e1];

				f32 isoToP0 = isoLevel - v0;
				f32 p1ToP0 = v1 - v0;

				return p1.lerp(p0, isoToP0 / p1ToP0);
			}

			template<bool isNormal = false>
			inline Vec3f32 interpolate(u8 i) const {

				static constexpr u8 edges[] = {
					001, 012, 023, 030, 045, 056,
					067, 074, 004, 015, 026, 037
				};

				u8 edge = edges[i];

				if constexpr (isNormal)
					return interpolateEdge(normals, edge >> 3, edge & 0x7);

				else return interpolateEdge(hexa.positions, edge >> 3, edge & 0x7);
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

		static inline List<igx::Triangle> triangulate(const Hexahedron &hexa, f32 isoLevel = 0) {
		
			Cell cell = Cell(hexa, isoLevel);

			u16 edge = mcEdgeTable[cell.index];

			if (edge == 0)
				return {};

			//Edge lookups

			Vec3f32 p[12], n[12];

			for (u8 i = 0; i < 12; ++i)
				if (edge & (1 << i)) {
					p[i] = cell.interpolate(i);
					n[i] = cell.interpolate<true>(i);
				}

			//Tri lookups

			u64 tris = mcTriTable[cell.index];

			List<igx::Triangle> res;
			res.reserve(5);

			for (u16 i = 0; i < 5; ++i) {

				u64 tri = (tris >> (12 * i)) & 0xFFF;

				if (tri == 0)
					break;

				u64 p0 = tri & 0xF;
				u8 p1 = u8(tri) >> 4;
				u64 p2 = tri >> 8;

				res.push_back(igx::Triangle(p[p0], p[p1], p[p2], n[p0], n[p1], n[p2]));
			}

			return res;
		}

	};

}