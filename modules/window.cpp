#include <iostream>

#include "window.h"

namespace ikhanchoi {

void Window::initialize() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
	if (window == nullptr) {
		std::cerr << "Error: (Window) Failed to create window." << '\n';
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	//glewExperimental = GL_TRUE;
	glewInit();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
/*
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, width, height);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPos(window, width / 2, height / 2);
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_ESCAPE) {
				glfwSetWindowShouldClose(window, true);
			}
		}
	});
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (action == GLFW_PRESS) {
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				double x, y;
				glfwGetCursorPos(window, &x, &y);
				std::cout << "Mouse clicked at: (" << x << ", " << y << ")" << std::endl;
			}
		}
	});
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		std::cout << "Mouse scrolled: (" << xoffset << ", " << yoffset << ")" << std::endl;
	});
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		win->setWidth(width);
		win->setHeight(height);
		glViewport(0, 0, width, height);
	});
	glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		win->terminate();
	});
	glfwSetDropCallback(window, [](GLFWwindow* window, int count, const char** paths) {
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		for (int i = 0; i < count; i++) {
			std::cout << "Dropped file: " << paths[i] << std::endl;
		}
	});
	glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int codepoint) {
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		std::cout << "Character input: " << (char)codepoint << std::endl;
	});
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		std::cout << "Mouse moved: (" << xpos << ", " << ypos << ")" << std::endl;
	});
	glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focused) {
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (focused) {
			std::cout << "Window focused" << std::endl;
		} else {
			std::cout << "Window unfocused" << std::endl;
		}
	});
	glfwSetWindowIconifyCallback(window, [](GLFWwindow* window, int iconified) {
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (iconified) {
			std::cout << "Window iconified" << std::endl;
		} else {
			std::cout << "Window restored" << std::endl;
		}
	});
	glfwSetWindowMaximizeCallback(window, [](GLFWwindow* window, int maximized) {
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (maximized) {
			std::cout << "Window maximized" << std::endl;
		} else {
			std::cout << "Window restored" << std::endl;
		}
	});
	glfwSetWindowRefreshCallback(window, [](GLFWwindow* window) {
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		std::cout << "Window refreshed" << std::endl;
	});
	glfwSetWindowPosCallback(window, [](GLFWwindow* window, int xpos, int ypos) {
		auto win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		std::cout << "Window moved: (" << xpos << ", " << ypos << ")" << std::endl;
	});
	glfwSetWindowUserPointer(window, this);
	glfwSetWindowSizeLimits(window, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetWindowAspectRatio(window, 16, 9);
	glfwSetWindowOpacity(window, 1.0f);
	glfwSetWindowMonitor(window, nullptr, 0, 0, width, height, 60);
*/
}

void Window::terminate() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::show() {
	ImGui_ImplOpenGL3_NewFrame(), ImGui_ImplGlfw_NewFrame(), ImGui::NewFrame();
	ImGui::Begin("Hello, world!");
	ImGui::Checkbox("Show demo Window", &showDemoWindow);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	resourceManager->show();
//		entityManager->show();
//		componentManager->show();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

}