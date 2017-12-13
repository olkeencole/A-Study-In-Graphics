#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <string>
#include <iostream> 

typedef uint8_t  u8; 
typedef uint32_t u32;

int main(int argc, char* args){

	const int WindowWidth  =  800; 
	const int WindowHeight = 600;

	glfwInit(); 
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow * window = glfwCreateWindow(WindowWidth, WindowHeight, "Vulkan Window", nullptr, nullptr); 

	u32 extensionCount = 0; 
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr); 

	std::cout << extensionCount << " extensions supported." << std::endl; 





	

	glfwDestroyWindow(window); 
	glfwTerminate();

	return 0; 

}