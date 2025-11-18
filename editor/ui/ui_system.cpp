#include "ui_system.h"

#include "world/scene/scene_manager.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include <GLFW/glfw3.h>


UISystem::UISystem(World& world) : SystemBase(world),
	sceneManager(*world.get<SceneManager>()){
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void) io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	io.Fonts->AddFontFromFileTTF("../../editor/ui/Roboto-Regular.ttf", 18.0f);

	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::StyleColorsDark();
	style.WindowRounding = 8.0f;
	style.FrameRounding  = 6.0f;
	style.ChildRounding  = 6.0f;
	style.PopupRounding  = 6.0f;
	style.ScrollbarRounding = 4.0f;
	style.GrabRounding = 4.0f;
	style.WindowPadding = ImVec2(8, 8);
	style.FramePadding = ImVec2(6, 4);
	style.ItemSpacing  = ImVec2(6, 6);
	style.ItemInnerSpacing = ImVec2(4, 4);

	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void UISystem::tick() {
	ImGui_ImplOpenGL3_NewFrame(), ImGui_ImplGlfw_NewFrame(), ImGui::NewFrame();

	dockSpace();
	mainMenuBar();

	ImGui::Begin("FPS");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();

	submit([] {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	});
}

void UISystem::dockSpace() {
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
									ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
									ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
									ImGuiWindowFlags_NoBackground;
	ImGui::Begin("DockSpace DockSpace", nullptr, window_flags);
	ImGuiID dock_id = ImGui::GetID("DockSpace");
	ImGuiID dock_id_root = dock_id;
	if (ImGui::DockBuilderGetNode(dock_id_root) == nullptr) {
		ImGui::DockBuilderRemoveNode(dock_id_root);
		ImGui::DockBuilderAddNode(dock_id_root, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dock_id_root, viewport->WorkSize);

		ImGuiID dock_id_inspector = ImGui::DockBuilderSplitNode(dock_id_root, ImGuiDir_Right, 0.15f, nullptr, &dock_id_root);
		ImGuiID dock_id_asset     = ImGui::DockBuilderSplitNode(dock_id_root, ImGuiDir_Down, 0.20f, nullptr, &dock_id_root);
		ImGuiID dock_id_hierarchy = ImGui::DockBuilderSplitNode(dock_id_root, ImGuiDir_Left, 0.15f, nullptr, &dock_id_root);

		ImGui::DockBuilderDockWindow("Hierarchy", dock_id_hierarchy);
		ImGui::DockBuilderDockWindow("Inspector", dock_id_inspector);
		ImGui::DockBuilderDockWindow("Project", dock_id_asset);
		ImGui::DockBuilderDockWindow("Main View", dock_id_root);
		ImGui::DockBuilderFinish(dock_id_root);
	}
	ImGui::DockSpace(dock_id, ImVec2(0.0f, 0.0f));
	ImGui::End();
}

void UISystem::mainMenuBar() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
			}
			if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
			}
			if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
			}
			if (ImGui::MenuItem("Exit", "Alt+F4")) {
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
			}
			if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows")) {
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}



UISystem::~UISystem() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
