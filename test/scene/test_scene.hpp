#pragma once
#include "helpers/scene_graph.hpp"
#include "geometry/tetron.hpp"
#include "task/marching_cubes_task.hpp"

namespace irmc {

	class TestScene : public igx::SceneGraph {

		f64 time{};
		List<u64> dynamicObjects;

	public:

		TestScene(igx::ui::GUI &gui, igx::FactoryContainer &factory);

		void update(f64 dt) override;

	};

}