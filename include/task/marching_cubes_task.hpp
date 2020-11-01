#pragma once
#include "helpers/render_task.hpp"
#include "helpers/factory.hpp"

#define GENERATE_ON_CPU

namespace irmc {

	static constexpr Vec3u32 dim = Vec3u32(32);

	struct MarchingInfo {

		Vec3f32 invScale;
		f32 isoLevel;

		Vec3u32 dim;
		f32 t;

	};

	class MarchingCubesTask : public igx::RenderTask {

		igx::FactoryContainer &factory;

		igx::SceneGraph *sceneGraph;

		igx::PipelineRef shader;
		igx::PipelineLayoutRef shaderLayout;

		igx::DescriptorsRef descriptors;
		igx::GPUBufferRef marchingInfo;

	public:

		MarchingCubesTask(igx::FactoryContainer &factory);

		void prepareCommandList(igx::CommandList *cl) final override;
		void update(f64 dt) final override;

		void switchToScene(igx::SceneGraph *sceneGraph) final override;
	};

}