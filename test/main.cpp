#include "rt/raytracing_interface.hpp"
#include "task/marching_cubes_task.hpp"
#include "scene/test_scene.hpp"

//Create window and wait for exit

int main() {

	using namespace oic;

	ignis::Graphics g("Igxrt marching caves", 1, "Igx", 1);

	igx::FactoryContainer factory(g);
	igx::ui::GUI gui(g);

	irmc::TestScene testScene(gui, factory);

	igx::rt::RaytracingInterface viewportInterface(g, gui, factory, testScene);

	#ifndef GENERATE_ON_CPU
		viewportInterface.addPrepass(new irmc::MarchingCubesTask(factory));
	#endif

	g.pause();

	System::viewportManager()->create(
		ViewportInfo(
			g.appName, {}, {}, 0, &viewportInterface, ViewportInfo::HANDLE_INPUT
		)
	);

	System::viewportManager()->waitForExit();

	return 0;
}