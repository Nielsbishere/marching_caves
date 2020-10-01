#include "test_scene.hpp"
#include "geometry/tetrahedron.hpp"

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

		for(usz i = 0; i < dynamicObjects.size(); ++i)
			dynamicObjects[i] = addGeometry(Triangle{}, i & 7);

		update(0);
	}

	static inline Triangle displace(const Triangle &tri, usz i, f64 t) {

		f32 tf = f32(sin(t * 0.5f) * 0.5 + 0.5) * 2;
		Vec3f32 dir = Tetrahedron::dirFromSide[i >> 2] * tf;

		return Triangle {
			tri.p0 + dir,
			tri.p1 + dir,
			tri.p2 + dir
		};
	}

	void TestScene::update(f64 dt) {

		const auto &tris = Tetrahedron::localTriangles;

		usz i{};

		for (const Triangle &tri : tris) {
			const Triangle displaced = displace(tri, i, time);
			SceneGraph::update(dynamicObjects[i], displaced);
			++i;
		}

		time += dt;

		SceneGraph::update(dt);
	}

}