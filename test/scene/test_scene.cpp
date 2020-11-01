#include "test_scene.hpp"
#include "geometry/marching_polygons.hpp"
#include "geometry/marching_tetra.hpp"
#include "geometry/paulbourke_polygonise.hpp"
#include <input/keyboard.hpp>

//#define GENERATE_LUT

#define USE_TETRAHEDRA
//#define USE_HEXAHEDRA
#define DEBUG_TETRA

namespace irmc {

	using namespace igx;

	TestScene::TestScene(ui::GUI &gui, FactoryContainer &factory):
		SceneGraph(gui, factory, NAME("Test scene"), VIRTUAL_FILE("textures/qwantani_4k.hdr"))
	{
		Vec3f32 sunDir = Vec3f32{ -0.5f, -2, -1 }.normalize();

		add(
			Material{ { 1, 0.5, 1 },	{ 0.05f, 0.01f, 0.05f },	{ 0, 0, 0 },	0,		1,		1 },
			Material{ { 0, 1, 0 },		{ 0, 0.05f, 0 },			{ 0, 0, 0 },	0,		1,		1 },
			Material{ { 0, 0, 1 },		{ 0, 0, 0.05f },			{ 0, 0, 0 },	0,		1,		1 },
			Material{ { 1, 0, 1 },		{ 0.05f, 0, 0.05f },		{ 0, 0, 0 },	0,		1,		1 },
			Material{ { 1, 1, 0 },		{ 0.05f, 0.05f, 0 },		{ 0, 0, 0 },	0,		1,		1 },
			Material{ { 0, 1, 1 },		{ 0, 0.05f, 0.05f },		{ 0, 0, 0 },	0,		1,		1 },
			Material{ { 0, 0, 0 },		{ 0, 0, 0 },				{ 0, 0, 0 },	1,		0,		1 },
			Material{ { 0, 0, 0 },		{ 0, 0, 0 },				{ 0, 0, 0 },	.25f,	.5f,	1 },

			Light(Vec3f32(-1, -1, -1).normalize(), Vec3f32(1))
		);

		update(0);

		//Original LUT, compressed so the values use less memory, lines of code and registers
		//From paulborke

		#ifdef USE_TETRAHEDRA
			#ifdef GENERATE_LUT
				oic::System::log()->debug('\n' + TetronHelper::compressTetraLut(true));
			#endif
		#else
			#ifdef GENERATE_LUT
				oic::System::log()->debug(compressPaulbourke(true));
			#endif
		#endif

	}

	#ifdef GENERATE_ON_CPU

		//Simple hills

		static constexpr f32 pi2 = f32(PI_CONST * 2);

		#ifndef USE_TETRAHEDRA
			static constexpr Vec3f32 invScale = Vec3f32(1) / dim.cast<Vec3f32>();
		#endif

		f32 currentT = 0;

		inline f32 testVolume(const Vec3f32 &p) {

			Vec2f32 hillXz = (p.xz() * pi2).cos();

			f32 hills = hillXz.prod() * cos(currentT * pi2 * 0.002f);
			return hills - p.y;	
		}

		static constexpr MarchingPolygons<testVolume> marchingPolygons;

		void TestScene::update(f64 dt) {

			static constexpr Array<Vec3f32, 6> dirFromSide = {
				Vec3f32(-1,  0,  0),
				Vec3f32(0,  0,  -1),
				Vec3f32(1,  0,  0),
				Vec3f32(0,  1,  0),
				Vec3f32(0,  -1,  0),
				Vec3f32(0,  0,  1)
			};

			currentT = f32(time);

			usz tris = 0;

			#ifdef USE_TETRAHEDRA

			u8 displayLod = 7;

			for (usz i = 0; i < 6; ++i) {

				for(u8 j = 0; j < TetronHelper::sizeOfLod(displayLod); ++j) {

					Tetron tetra = TetronHelper::getTetra(Tetron::HexonDivision(i), displayLod, j);

					tetra.setCenter(
						tetra.getCenter() + 
						(tetra.getCenter() - .5f).normalize() * 
						(std::cos(currentT * 0.125f) * -.5f + .5f) * 5
					);

					#ifdef DEBUG_TETRA
					
						for (const Triangle& tri : tetra.triangulate())

							if (dynamicObjects.size() < tris + 1) {
								dynamicObjects.push_back(addGeometry(tri, (tris >> 2) % 6));
								++tris;
							} else {
								SceneGraph::update(dynamicObjects[tris], tri);
								++tris;
							}

					#elif defined(USE_HEXAHEDRA)

						for (const Hexahedron &hex : tetra.splitHexa())
							for (const Triangle &tri : marchingPolygons.triangulate(hex))

								if (dynamicObjects.size() < tris + 1) {
									dynamicObjects.push_back(addGeometry(tri, 0));
									++tris;
								} else {
									SceneGraph::update(dynamicObjects[tris], tri);
									++tris;
								}

					#else

						for (const Triangle &tri : marchingPolygons.triangulate(tetra))

							if (dynamicObjects.size() < tris + 1) {
								dynamicObjects.push_back(addGeometry(tri, 0));
								++tris;
							} else {
								SceneGraph::update(dynamicObjects[tris], tri);
								++tris;
							}

					#endif
				}
			}

			#else

			for(u8 i = 0; i < dim.x; ++i)
				for(u8 j = 0; j < dim.y; ++j)
					for (u8 k = 0; k < dim.z; ++k) {

						Vec3f32 left(i, j, k);

						Hexahedron hex = Cube{ left * invScale * 8 - 4, (left + 1) * invScale * 8 - 4 };

						for(const Triangle &tri : marchingPolygons.triangulate(hex))

							if (dynamicObjects.size() < tris + 1) {
								dynamicObjects.push_back(addGeometry(tri, 0));
								++tris;
							}
							else {
								SceneGraph::update(dynamicObjects[tris], tri);
								++tris;
							}
					}

			#endif

			//Clean previously allocated tris we don't need anymore

			oic::System::log()->debug(tris);

			List<u64> deletedIndices;

			for (usz i = dynamicObjects.size() - 1; i >= tris && i != usz_MAX; --i)
				deletedIndices.push_back(dynamicObjects[i]);

			SceneGraph::del(deletedIndices);

			dynamicObjects.resize(tris);

			//

			time += dt;

			SceneGraph::update(dt);
		}

	#else
		void TestScene::update(f64) {}
	#endif

}