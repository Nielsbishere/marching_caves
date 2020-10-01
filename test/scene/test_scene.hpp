#pragma once
#include "helpers/scene_graph.hpp"
#include "geometry/tetrahedron.hpp"

namespace igx::rt {

	class TestScene : public SceneGraph {

		f64 time{};
		Array<u64, irmc::Tetrahedron::localTriangles.size()> dynamicObjects;

	public:

		TestScene(ui::GUI &gui, FactoryContainer &factory);

		void update(f64 dt) override;

	};

}