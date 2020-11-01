#include "task/marching_cubes_task.hpp"
#include "helpers/scene_graph.hpp"

using namespace igx;

namespace irmc {

	MarchingCubesTask::MarchingCubesTask(FactoryContainer &factory):

		RenderTask(
			factory.getGraphics(), 
			NAME("Marching cubes task"),
			Vec4f32(1, 0, 1, 1)
		),

		factory(factory)
	{
		List<RegisterLayout> registers{

			RegisterLayout(
				NAME("TriangleBuffer"), 0, GPUBufferType::STRUCTURED, 0, 0,
				ShaderAccess::COMPUTE, sizeof(igx::Triangle), true
			),

			RegisterLayout(
				NAME("SceneData"), 1, GPUBufferType::STORAGE, 1, 0,
				ShaderAccess::COMPUTE, sizeof(igx::SceneGraphInfo), true
			),

			RegisterLayout(
				NAME("MarchingInfo"), 2, GPUBufferType::UNIFORM, 0, 0,
				ShaderAccess::COMPUTE, sizeof(MarchingInfo)
			)
		};

		MarchingInfo mcInf{
			
			Vec3f32(1) / dim.cast<Vec3f32>(),
			0,

			dim,
			0
		};

		marchingInfo = {
			g, NAME("MarchingInfo"),
			GPUBuffer::Info(
				GPUBufferUsage::UNIFORM, GPUMemoryUsage::CPU_WRITE,
				List<u8>((const u8*)&mcInf, (const u8*)(&mcInf + 1))
			)
		};

		shaderLayout = factory.get(NAME("Marching cube shader layout"), registers);

		descriptors = {
			g, NAME("Marching cube descriptors"),
			Descriptors::Info(shaderLayout, 0, {
				{ 2, GPUSubresource(marchingInfo, GPUBufferType::UNIFORM) }
			})
		};

		shader = factory.get(
			NAME("Marching cube shader"),
			Pipeline::Info(
				Pipeline::Flag::NONE,
				VIRTUAL_FILE("shaders/marching_cubes.comp.spv"),
				{},
				shaderLayout,
				Vec3u32(4)
			)
		);
	}

	void MarchingCubesTask::switchToScene(SceneGraph *_sceneGraph) {

		if (sceneGraph != _sceneGraph) {

			descriptors->updateDescriptor(0, GPUSubresource(_sceneGraph->getBuffer(SceneObjectType::TRIANGLE), GPUBufferType::STORAGE));
			descriptors->updateDescriptor(1, GPUSubresource(_sceneGraph->getSceneInfo(), GPUBufferType::STORAGE));
			descriptors->flush({ { 0, 2 } });

			markNeedCmdUpdate();
			sceneGraph = _sceneGraph;
		}
	}

	void MarchingCubesTask::update(f64 dt) {
		marchingInfo->flush(0, sizeof(MarchingInfo));
		marchingInfo->getBuffer<MarchingInfo>()->t += f32(dt);
	}

	void MarchingCubesTask::prepareCommandList(CommandList *cl) {
		cl->add(
			ClearBuffer(sceneGraph->getSceneInfo(), offsetof(SceneGraphInfo, triangleCount), sizeof(SceneGraphInfo::triangleCount)),
			FlushBuffer(marchingInfo, factory.getDefaultUploadBuffer()),
			BindDescriptors(descriptors),
			BindPipeline(shader),
			Dispatch(dim)
		);
	}

}