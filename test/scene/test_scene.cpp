#include "test_scene.hpp"
#include "geometry/marching_hexahedra.hpp"
#include <input/keyboard.hpp>

namespace igx::rt {

	using namespace irmc;

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
	}

	//Simple hills

	static constexpr f32 pi2 = f32(PI_CONST * 2);

	//#define USE_TETRAHEDRA

	#ifndef USE_TETRAHEDRA

	static constexpr u16 gridPoints = 16;
	static constexpr f32 scale = 1.f / gridPoints;

	#endif

	f32 currentT = 0;

	inline f32 testVolume(const Vec3f32 &p) {
		f32 hills = cos(p.x * pi2) * cos(p.z * pi2) * cos(currentT * pi2 * 0.05f);
		return hills - p.y;	
	}

	static constexpr MarchingHexahedra<testVolume> marchingHexahedra;

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

		for (usz i = 0; i < 6; ++i) {

			auto cubeDivision = Tetrahedron(Tetrahedron::HexahedronDivision(i));

			for(const Tetrahedron &div0 : cubeDivision.splitTetra())
				for(const Tetrahedron &div1 : div0.splitTetra())
					for(const Tetrahedron &div2 : div1.splitTetra())
						for(const Tetrahedron &div3 : div2.splitTetra())
							for(const Tetrahedron &div4 : div3.splitTetra())
								for(const Tetrahedron &div5 : div4.splitTetra())
									for(const Tetrahedron &div6 : div5.splitTetra())
										for(const Hexahedron &div7 : div6.splitHexa())
											for(const Triangle &tri : marchingHexahedra.triangulate(div7))

												if (dynamicObjects.size() < tris + 1) {
													dynamicObjects.push_back(addGeometry(tri, 0));
													++tris;
												}
												else {
													SceneGraph::update(dynamicObjects[tris], tri);
													++tris;
												}
		}

		#else

		for(u8 i = 0; i < gridPoints; ++i)
			for(u8 j = 0; j < gridPoints; ++j)
				for (u8 k = 0; k < gridPoints; ++k) {

					Vec3f32 left(i, j, k);

					Hexahedron hex = Cube{ left * scale, (left + 1) * scale };

					for(const Triangle &tri : marchingHexahedra.triangulate(hex))

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

		List<u64> deletedIndices;

		for (usz i = dynamicObjects.size() - 1; i >= tris && i != usz_MAX; --i)
			deletedIndices.push_back(dynamicObjects[i]);

		SceneGraph::del(deletedIndices);

		dynamicObjects.resize(tris);

		//

		time += dt;

		SceneGraph::update(dt);
	}

}