#include "core.h"

namespace ikhanchoi {

/*---------*/
/* Manager */
/*---------*/

void Manager::setManager(std::weak_ptr<Manager> manager) {
    if (auto sharedManager = manager.lock())
        this->manager[typeid(*sharedManager)] = manager;
    else
        throw std::invalid_argument("Error: (Manager::setManager) Manager pointer is expired or null.");
}

std::weak_ptr<Manager> Manager::getManager(std::type_index managerType) {
    if (manager.find(managerType) == manager.end())
        throw std::runtime_error("Error: (Manager::getManager) Manager type not found.");
    return manager[managerType];
}

Handle Manager::create(const std::type_index& concrete, const std::string& name) {
    if (pool.find(concrete) == pool.end())
        throw std::runtime_error("Error: (Manager::create) Type not registered.");
    std::unique_ptr<Object> object = factory[concrete](0, name);
    if (!object)
        throw std::runtime_error("Error: (Manager::create) Failed to create object.");
    Handle handle = pool[concrete]->add(object.get());
    object->setId(handle.index);
    return handle;
}

void Manager::destroy(const Handle& handle) {
	if (pool.find(handle.type) == pool.end())
        throw std::runtime_error("Error: (Manager::destroy) Module not found.");
    pool[handle.type]->remove(handle);
}



/*---------*/
/* Context */
/*---------*/

Context::Context(std::string name, int width, int height)
		: name(std::move(name)), width(width), height(height) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindow = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
	if (glfwWindow == nullptr) {
		std::cerr << "Error: (Window) Failed to create window." << '\n';
		glfwTerminate();
	}
	glfwMakeContextCurrent(glfwWindow);
	//glewExperimental = GL_TRUE;
	glewInit();


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

Context::~Context() {
	try {
		for (auto& [type, manager] : manager)
			manager.reset();
		glfwDestroyWindow(glfwWindow);
		glfwTerminate();
	} catch (const std::exception& e) {
		std::cerr << "Error: (Context::~Context) " << e.what() << std::endl;
	}

}

}