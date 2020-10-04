#include "test_scene.hpp"
#include "geometry/tetrahedron.hpp"
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
			Material{ { 0, 0, 0 },		{ 0, 0, 0 },				{ 0, 0, 0 },	.25f,	.5f,	1 }
		);

		for(u32 i = 0; i < u32(dynamicObjects.size()); ++i)
			dynamicObjects[i] = addGeometry(Triangle{}, (i / 12) % 6);

		update(0);
	}

	void TestScene::update(f64 dt) {

		static constexpr Array<Vec3f32, 6> dirFromSide = {
			Vec3f32(-1,  0,  0),
			Vec3f32(0,  0,  -1),
			Vec3f32(1,  0,  0),
			Vec3f32(0,  1,  0),
			Vec3f32(0,  -1,  0),
			Vec3f32(0,  0,  1)
		};

		f32 tf = f32(sin(time * 0.5f) * 0.5 + 0.5) * 2;

		usz j = 0;

		for (usz i = 0; i < 6; ++i) {

			Vec3f32 center = dirFromSide[i] * tf;

			Tetrahedron cubeDivision(Tetrahedron::HexahedronDivision(i), center);

			//for(const Tetrahedron &div0 : cubeDivision.splitTetra())
				for(const Hexahedron &div1 : cubeDivision.splitHexa())
					for (const igx::Triangle &tri : div1.triangulate()) {
						SceneGraph::update(dynamicObjects[j], tri);
						++j;
					}
		}

		time += dt;

		SceneGraph::update(dt);
	}

}