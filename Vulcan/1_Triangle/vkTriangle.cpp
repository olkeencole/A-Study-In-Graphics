#define VK_USE_PLATFORM_WIN32_KH
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
//#include "glm/glm.hpp"
#include <string>
#include <iostream> 
#include "Math.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
#define ArrayCount(value) ( sizeof(value) / sizeof(value[0] ) ) //Length of an Array

typedef uint16_t  u16; 
typedef uint32_t  u32;

GLFWwindow *window; 
VkInstance instance; 

u32 physicalDeviceCount = 0;
VkPhysicalDevice physicalDevice =   VK_NULL_HANDLE;
VkDevice     device; 

VkQueue      graphicsQueue; 
VkQueue      presentQueue; 

VkSurfaceKHR surface;
VkSwapchainKHR swapChain;

VkImage *swapChainImages;
u32 chainImageCount = 0;

VkFormat swapChainImageFormat; 
VkExtent2D swapChainExtent;

u32 imageViewCount = 0;
VkImageView *imageViews; 

VkDeviceMemory vertexBufferMemory; 
VkBuffer vertexBuffer; 

VkDeviceMemory indexBufferMemory; 
VkBuffer indexBuffer; 

VkRenderPass renderPass; 
VkPipelineLayout pipelineLayout; 
VkPipeline graphicsPipeline;

VkFramebuffer *frameBuffers; 

VkCommandPool commandPool;
VkCommandBuffer *commandBuffers; 

VkSemaphore imageAvailableSemaphore; 
VkSemaphore renderFinishedSemaphore; 

struct Vertex {
	Vector2 pos; 
	Vector3 color; 
};

const Vertex vertices[] = {
	{ {0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    { {0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
    { {-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

const Vertex verticesRect[] = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f},   {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f},  {1.0f, 0.0f, 1.0f}}
};

const u16 indices[] = {
   0, 1, 2, 2, 3, 0
};

const int WindowWidth   = 800; 
const int WindowHeight  = 600;

const char* validationLayers[] = {
 "VK_LAYER_LUNARG_standard_validation"
};

const int deviceExtensionCount = 1;
const char* deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const int validationLayerCount = 1;
VkDebugReportCallbackEXT callback;
//VkDestroyDebugReportCallbackEXT destroyCallback; 

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif


struct QueueFamilyIndices {
	int graphicsFamily = -1;
	int presentFamily  = -1;

	bool IsComplete() { 
		return graphicsFamily >= 0 && presentFamily >= 0;
	}
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice *physicalDevice) {

	QueueFamilyIndices familyResult; 
	VkQueueFamilyProperties *indices;
	u32 queueFamilyCount = 0; 

	// Get a collection of the queue family resources available
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamilyCount, nullptr);
	indices = new VkQueueFamilyProperties[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(*physicalDevice, &queueFamilyCount, indices);
	VkBool32 presentSupport = false;
	//Find the right one
	if(queueFamilyCount > 0) {
		for (int i = 0; i < queueFamilyCount; ++i)
		{
			VkQueueFamilyProperties family = indices[i];
			if(family.queueFlags & VK_QUEUE_GRAPHICS_BIT )	{
				familyResult.graphicsFamily = i;
			}	

			//Why is present support not in the queueFlags?
			vkGetPhysicalDeviceSurfaceSupportKHR(*physicalDevice, i, surface, &presentSupport);
			if(presentSupport){
				familyResult.presentFamily = i;
			}

			if(familyResult.IsComplete() )
				break;
		}
	}

	cout << "Family Index: " << familyResult.graphicsFamily << endl;
	return familyResult;
}


bool CheckValidationLayer(){
	//Get LayerProperties
	u32 count = 0;
	VkLayerProperties *layerProperties = NULL;
	vkEnumerateInstanceLayerProperties(&count, nullptr);
	
	layerProperties = new VkLayerProperties[count];
	vkEnumerateInstanceLayerProperties(&count, layerProperties);


	bool layerFound = false;
	for (int i = 0; i < count; ++i)
	{
		VkLayerProperties layer = layerProperties[i];
		//cout << "Layer Property: " << layer.layerName << endl;
		
		for (int i = 0; i < validationLayerCount; ++i) {
			if( strcmp( layer.layerName,  validationLayers[i] ) == 0) {
				layerFound = true;
				cout << "Layer Found: "  << layer.layerName << endl;
				break;
			}
		}
	}

	return layerFound;
}



static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugReportFlagsEXT      flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData) 
{

    std::cerr << "validation layer: " << msg << std::endl;

    return VK_FALSE;
}

void RecreateSwapChain(); 

static void OnResize(GLFWwindow* window, int width, int height){

	RecreateSwapChain();
}

void InitWindow(){
	
	glfwInit(); 
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); 

	window = glfwCreateWindow(WindowWidth, WindowHeight, "Vulkan Window", nullptr, nullptr); 

	//glfwSetWindowUserPointer(window, this); 
	glfwSetWindowSizeCallback(window, OnResize);
}



void InitializeDebugging(){

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType       = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT; 
	createInfo.flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = &DebugCallback; //Actually function

	auto vkDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT ) 
																vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	
	//NB: we are calling the extension function here, not the callback (I guess)
    if(vkDebugReportCallback != nullptr ) 
    {
		if(  vkDebugReportCallback(instance, &createInfo, nullptr, &callback ) != VK_SUCCESS) {
			cout << "Failed to Setup  a debug callback" << endl;
		}
	} else {
		cout << "Callback: " << VK_ERROR_EXTENSION_NOT_PRESENT << endl;
	}
}

bool CheckExtensionSupport(VkPhysicalDevice *device){

//Diffs between int and u32?
	u32 extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(*device, nullptr, &extensionCount, nullptr);

	VkExtensionProperties *availableExtensions = new VkExtensionProperties[extensionCount];
	vkEnumerateDeviceExtensionProperties(*device, nullptr, &extensionCount, availableExtensions); 

	//Now check and see if are requested property list is in the available list
	int requiredExtensionsFound = 0;
	for (int i = 0; i < extensionCount; ++i)
	{
		for (int x = 0; x < deviceExtensionCount; ++x)
		{
			//cout << "Extension Name: " << availableExtensions[i].extensionName << endl; 
			if( strcmp(availableExtensions[i].extensionName,  &deviceExtensions[x]) == 0){
				requiredExtensionsFound++;
				cout << "Extension Found: " << availableExtensions[i].extensionName  << endl;
			}
		}
	}

	return (requiredExtensionsFound >= deviceExtensionCount);
}


 struct SwapChainSupport{
 	VkSurfaceCapabilitiesKHR capabilities; 
 	VkSurfaceFormatKHR *surfaceFormats; 
 	u32 formatCount;
 	VkPresentModeKHR *presentModes;
 	u32 presentModeCount;
 }; 

SwapChainSupport QuerySwapChainSupport(VkPhysicalDevice physDevice){
	SwapChainSupport details; 

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &details.capabilities); 

	u32 formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR( physDevice, surface, &formatCount, nullptr);
	if(formatCount != 0)
	{
		details.surfaceFormats = new VkSurfaceFormatKHR[formatCount];
		vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, details.surfaceFormats);
		details.formatCount = formatCount;
	}

	u32 presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, nullptr);
	if(presentModeCount != 0){
		details.presentModes = new VkPresentModeKHR[presentModeCount];
		vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, details.presentModes);
		details.presentModeCount = presentModeCount;
	}

	return details;
}


VkSurfaceFormatKHR ChooseSwapSurfaceFormat(VkSurfaceFormatKHR *availableFormats, u32 formatCount){
	if(formatCount == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED ){
		cout << availableFormats[0].format <<endl;
		return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
	}

	for (int i = 0; i < formatCount; ++i)
	{
		 cout << availableFormats[i].format << endl;
		 if(availableFormats[i].format     == VK_FORMAT_B8G8R8A8_UNORM &&
		    availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
		 	return availableFormats[i]; 
		 }
	}

	return availableFormats[0];
}

VkPresentModeKHR ChoosePresentMode(VkPresentModeKHR *availableModes, u32 modeCount){
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR; 

	for (int i = 0; i < modeCount; ++i)
	{
		if(availableModes[i] == VK_PRESENT_MODE_MAILBOX_KHR ){
			return availableModes[i];
		}
		else if( availableModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			bestMode =  availableModes[i];
	}

	return bestMode;
}

VkExtent2D ChooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities){
	//extents are the size of the images in the swap chain

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	VkExtent2D extent = {width, height};

	extent.width  = max (capabilities.minImageExtent.width, min(capabilities.maxImageExtent.width, extent.width )  );
	extent.height = max (capabilities.minImageExtent.height, min(capabilities.maxImageExtent.height, extent.height )  );

	return extent;
}

bool IsDeviceSuitable(VkPhysicalDevice *device ){
	VkPhysicalDeviceProperties properties; 
	VkPhysicalDeviceFeatures features; 

	vkGetPhysicalDeviceProperties(*device, &properties);
	vkGetPhysicalDeviceFeatures(  *device, &features); 

	//Check for certain features
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = CheckExtensionSupport(device);

	bool swapChainAdequate = false;
	SwapChainSupport swapChainSupport = QuerySwapChainSupport(*device);
	swapChainAdequate = swapChainSupport.surfaceFormats != nullptr && swapChainSupport.presentModes != nullptr;

	cout << "Devices: " << properties.deviceName << endl;
	ChooseSwapSurfaceFormat(swapChainSupport.surfaceFormats, swapChainSupport.formatCount);
	cout << "Present Mode: " << ChoosePresentMode(swapChainSupport.presentModes, swapChainSupport.presentModeCount) << endl;

	if(!swapChainAdequate){
		cout << "Current SwapChainSupport Not Adequate." << endl;
	}

	return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}


void FindPhysicalDevice(){

	vkEnumeratePhysicalDevices( instance, &physicalDeviceCount, nullptr);
	if( physicalDeviceCount == 0){
		cout << "No Device Found" << endl;
	}

	//Need to look up about constants and using ints with arrays
	//VkPhysicalDevice devices[ (const int) physicalDeviceCount]; ; 
	VkPhysicalDevice *devices = new VkPhysicalDevice[physicalDeviceCount];
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, devices);

	for (int i = 0; i < physicalDeviceCount; ++i)
	{
		if( IsDeviceSuitable( &devices[i])  ) {
			physicalDevice = devices[i];
			break;
		}
	}

	if( physicalDevice  == VK_NULL_HANDLE){
		cout << "No Suitable Device Found" << endl;
	}
}

void CreateLogicalDevice(){

//Need to make queueCreates for each queue Family that we need
	QueueFamilyIndices indices = findQueueFamilies(	&physicalDevice); 
	const u32 queueCount = 2;
	VkDeviceQueueCreateInfo queueCreateInfos[queueCount] = {};
	int queueIDs[] = {indices.graphicsFamily, indices.presentFamily };

	for(int i = 0; i < queueCount; i++) {

		int queueID = queueIDs[i];

		//Setting up for queue creation
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueID;// indices.graphicsFamily;  //Is this the chief reason why we found the queue family? 
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos[i] = queueCreateInfo;
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	// Setup for the Device Creation
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos; 
	deviceCreateInfo.queueCreateInfoCount = 1; //we can have more than one? 
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;  // this likely needs to be global
	deviceCreateInfo.enabledExtensionCount = deviceExtensionCount;//extension count
	deviceCreateInfo.ppEnabledExtensionNames = &deviceExtensions;

	if(enableValidationLayers){

		deviceCreateInfo.enabledLayerCount   = (u32) validationLayerCount;
		deviceCreateInfo.ppEnabledLayerNames = validationLayers;
	}
	else
	{
		deviceCreateInfo.enabledLayerCount =1;
	}

	//Creating Device
	if( vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device ) != VK_SUCCESS) {
		cout << "Creating Logical Device Failed" << endl;
	}

	//Detect Error
	vkGetDeviceQueue( device, indices.graphicsFamily, 0, &graphicsQueue );
	vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);

}	



// typedef VkFlags VkWin32SurfaceCreateFlagsKHR;
// typedef struct VkWin32SurfaceCreateInfoKHR
// {
//     VkStructureType                 sType;
//     const void*                     pNext;
//     VkWin32SurfaceCreateFlagsKHR    flags;
//     HINSTANCE                       hinstance;
//     HWND                            hwnd;
// } VkWin32SurfaceCreateInfoKHR;



void CreateSurface() {
//	VkWin32SurfaceCreateInfoKHR    surfCreateInfo; 
	// surfCreateInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR; 
	//  surfCreateInfo.hwnd     = glfwGetWin32Window(window); 
	// surfCreateInfo.hinstance = GetModuleHandle(nullptr);

	// vkCreateWin32SurfaceKHR CreateWin32Surface = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
	// if(CreateWin32Surface != NULL ){
	// 	VK_Result result = CreateWin32Surface(instance,  & surfCreateInfo, nullptr, &surface);
	// 	if(result != VK_SUCCESS)
	// 		cout << "Failed to Create Win32 Surface" << endl;
	// }

	if( glfwCreateWindowSurface(instance,window, nullptr, &surface) != VK_SUCCESS) 
	 	cout <<  "Failed ot create surface" << endl;
}

void CreateSwapChain(){

	SwapChainSupport chainSupport = QuerySwapChainSupport(physicalDevice);

	u32 imageCount = 0;
	VkExtent2D extents         = ChooseSwapExtent(chainSupport.capabilities);
	VkSurfaceFormatKHR format  = ChooseSwapSurfaceFormat(chainSupport.surfaceFormats, chainSupport.formatCount); 
	VkPresentModeKHR   mode    = ChoosePresentMode(chainSupport.presentModes, chainSupport.presentModeCount);  

	imageCount = chainSupport.capabilities.minImageCount; 
	if(chainSupport.capabilities.maxImageCount > 0 && imageCount > chainSupport.capabilities.maxImageCount)
		imageCount = chainSupport.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType           = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface 	       = surface;
	createInfo.minImageCount   = imageCount; 
	createInfo.imageFormat     = format.format; 
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.imageArrayLayers= 1;
	createInfo.imageExtent     = extents;
	createInfo.imageUsage      = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //DIRECTLY TO THE SCREEN

	//How the swapchain will work across different queue families
	QueueFamilyIndices family = findQueueFamilies(&physicalDevice);

	u32 indices[] = { (u32) family.graphicsFamily, (u32) family.presentFamily}; 
	if( family.graphicsFamily != family.presentFamily){

		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = indices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = chainSupport.capabilities.currentTransform; 
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = mode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = nullptr;

	if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS){
		cout << "Swapchain Was Not Created" << endl;
	}

	vkGetSwapchainImagesKHR(device, swapChain, &chainImageCount, nullptr);
	if(chainImageCount == 0){
		cout << "Chain Image count is zer0" <<endl;
	}
	swapChainImages = new VkImage[chainImageCount]; 
	vkGetSwapchainImagesKHR(device, swapChain, &chainImageCount, swapChainImages);

	swapChainExtent = extents;
	swapChainImageFormat = format.format; 
}

struct ShaderInfo{
	char* shaderBuffer;
	size_t size; 
}; 

 ShaderInfo ReadShaderFile( char *path){
	
	char* buffer; 
	std::ifstream file( path, std::ios::ate | std::ios::binary);
	if( !file.is_open() ){
		cout << "Could Not Open File" << endl;
	}

	size_t fileSize = (size_t) file.tellg(); /// Learn more about ifstream. This was pretty easy
	buffer = new char[fileSize];
	file.seekg(0);  //go back to beginning
	file.read( buffer, fileSize);
	file.close();
	return {buffer, fileSize};
 }

 VkShaderModule CreateShaderModule(char *shader, size_t shaderSize){

 	VkShaderModuleCreateInfo createInfo = {}; 
 	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; 
 	createInfo.codeSize = shaderSize; 
 	createInfo.pCode = reinterpret_cast<const u32*>(shader);
 	VkShaderModule shaderModule;

 	if( vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS){
 		cout << "Failed to create shader module. \n";
 	}
 	return shaderModule;
 }

//Framebuffer attachments - how color and depth buffers and contents are handled in rendering operations 
void CreateRenderPass(){

	VkAttachmentDescription colorAttachment = {}; // Single color buffer attachments represented by one images from the swap chain
		colorAttachment.format  = swapChainImageFormat; 
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

		colorAttachment.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR; //before rendering - clear
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // after - preserve existing contents of the attachment

		colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE; //not using stencil
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;   //dont' care prev layout
		colorAttachment.finalLayout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //images presented to the swap chain


 //subpasses
	VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {}; //Attachment Descriptions, References and then the subpasses
		subpass.pipelineBindPoint 	 = VK_PIPELINE_BIND_POINT_GRAPHICS; 
		subpass.colorAttachmentCount = 1; //layout location?
		subpass.pColorAttachments    = &colorAttachmentRef;  


	VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments    = &colorAttachment; 
		renderPassInfo.subpassCount    = 1; 
		renderPassInfo.pSubpasses      = &subpass;

VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL; //implicit subpass before the actuals subpass
	dependency.dstSubpass = 0; // must always be higher to prevent cycles int he dependency graph

	dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //WHICH STAGE TO WAIT FOR 
	dependency.srcAccessMask = 0;

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; //PREVENT TRANSITION TIL ALLOWED

	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies   = &dependency;


	if( vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS ) {
		cout << "Failed to create Render Pass" << endl;
	}
}


void CreateGraphicsPipeline(){
	ShaderInfo vertShader = ReadShaderFile("vert.spv");
	ShaderInfo fragShader = ReadShaderFile("frag.spv");

	cout << "Size of Vert: " <<  vertShader.size << endl;
	cout << "Size of Frag: " << fragShader.size << endl;

	VkShaderModule vertModule = CreateShaderModule(vertShader.shaderBuffer, vertShader.size);
	VkShaderModule fragModule = CreateShaderModule(fragShader.shaderBuffer, fragShader.size);

	//Shader Stage
	VkPipelineShaderStageCreateInfo vertCreateInfo = {}; 
	vertCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertCreateInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
	vertCreateInfo.module = vertModule;
	vertCreateInfo.pName  = "main";

	VkPipelineShaderStageCreateInfo fragCreateInfo = {}; 
	fragCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragCreateInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragCreateInfo.module = fragModule;
	fragCreateInfo.pName  = "main";

	VkPipelineShaderStageCreateInfo shaderPipelineCreate[] = {vertCreateInfo, fragCreateInfo};


	//

	VkVertexInputBindingDescription vertexBindingDesc = {}; 
		vertexBindingDesc.binding = 0;
		vertexBindingDesc.stride  = sizeof(Vertex);//
		vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // ass opposed to instanced

//Will need two of these since we have position and color
		//Just like the glVertexAttribute something
	VkVertexInputAttributeDescription vertexPosAttributeDesc = {}; 
		vertexPosAttributeDesc.binding  = 0;
		vertexPosAttributeDesc.location = 0;
		vertexPosAttributeDesc.format   = VK_FORMAT_R32G32_SFLOAT; 
		vertexPosAttributeDesc.offset   = offsetof(Vertex, pos); 

	VkVertexInputAttributeDescription vertexColorAttributeDesc = {}; 
		vertexColorAttributeDesc.binding  = 0;
		vertexColorAttributeDesc.location = 1; 
		vertexColorAttributeDesc.format   = VK_FORMAT_R32G32B32_SFLOAT; 
		vertexColorAttributeDesc.offset   = offsetof(Vertex, color);

	VkVertexInputAttributeDescription attribList[] = {vertexPosAttributeDesc, vertexColorAttributeDesc};

	VkPipelineVertexInputStateCreateInfo vertexInput = {};  //describing format fo the vertex data
		vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInput.vertexBindingDescriptionCount 	= 1;
		vertexInput.pVertexBindingDescriptions 		= &vertexBindingDesc;
		vertexInput.vertexAttributeDescriptionCount = ArrayCount(attribList);// 2; //HARDCODED
		vertexInput.pVertexAttributeDescriptions 	= attribList;


	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {}; // what kind of geomtry should be draw and primitive restart
		inputAssembly.sType    				 = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology 				 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE; 


	VkViewport viewPort = {}; 
		viewPort.x = 0.0f;
		viewPort.y = 0.0f;
		viewPort.width    = (float) swapChainExtent.width;
		viewPort.height   = (float) swapChainExtent.height;
		viewPort.minDepth = 0.0f;
		viewPort.maxDepth = 1.0f;

	VkRect2D  scissor  = {}; 
		scissor.offset = {0,0};
		scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {}; 
		viewportState.sType 		= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports 	= &viewPort;
		viewportState.scissorCount  = 1;
		viewportState.pScissors     = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {}; //takes geo and makes fragments. performans depth and face testing and sicssor
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable        = VK_FALSE; //clamp if outside clip planes rather than discarding
		rasterizer.rasterizerDiscardEnable = VK_FALSE; //TRUE, disregards the framebuffer
		rasterizer.polygonMode 			   = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth 			   = 1.0f;
		rasterizer.cullMode 			   = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE; //decide winding order for normal
		
		rasterizer.depthBiasEnable         = VK_FALSE; //Modify the depth values if you choose.. Good for shadow mapping apparently
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp 		   = 0.0f;
		rasterizer.depthBiasSlopeFactor    = 0.0f;

	VkPipelineMultisampleStateCreateInfo multiSampler = {};
		multiSampler.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multiSampler.sampleShadingEnable   = VK_FALSE;
		multiSampler.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
		multiSampler.minSampleShading      = 1.0f;
		multiSampler.pSampleMask           = nullptr;
		multiSampler.alphaToCoverageEnable = VK_FALSE;
		multiSampler.alphaToOneEnable      = VK_FALSE;
	
	VkPipelineDepthStencilStateCreateInfo depthStencil = {};

	//Blending. Pixel after Rasterization
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {}; //Mix old and new
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable    = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlendState = {}; //combine old and new with bitwise
		colorBlendState.sType 			  = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.logicOpEnable     = VK_FALSE;
		colorBlendState.logicOp           = VK_LOGIC_OP_COPY;
		colorBlendState.attachmentCount   = 1;
		colorBlendState.pAttachments      = &colorBlendAttachment;
		colorBlendState.blendConstants[0] = 0.0f;
		//blendconstants are optional

	//VkPipelineDynamicStateCreateInfo dynamicState = {};

	//VkPipelineLayout           pipelineLayout; 
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {}; //specify uniform values in the shader
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO; 
		pipelineLayoutInfo.setLayoutCount    = 0;
		pipelineLayoutInfo.pSetLayouts       = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = 0;

	if(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
		cout << "Failed to create Pipelline Layout" << endl;
	}


	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderPipelineCreate;

	pipelineInfo.pVertexInputState   = &vertexInput;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState      = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState   = &multiSampler;
	pipelineInfo.pDepthStencilState  = nullptr;
	pipelineInfo.pColorBlendState    = &colorBlendState;
	pipelineInfo.pDynamicState       = nullptr;

	pipelineInfo.layout     = pipelineLayout; 

	pipelineInfo.renderPass = renderPass; 
	pipelineInfo.subpass    = 0;

	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; 
	pipelineInfo.basePipelineIndex  = -1;


	if( vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline ) != VK_SUCCESS){
		cout << "Failed to create pipeline" << endl;
	}
		//and be sure to destroy them 


	vkDestroyShaderModule(device, vertModule, nullptr);
	vkDestroyShaderModule(device, fragModule, nullptr);
}

void CreateFrameBuffer(){
	frameBuffers = new VkFramebuffer[ imageViewCount];

	for (int i = 0; i < imageViewCount; ++i)
	{
		VkImageView attachments[] { imageViews[i] }; 
	

	VkFramebufferCreateInfo createInfo = {};
		createInfo.sType      = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = renderPass; 
		createInfo.attachmentCount = 1;
		createInfo.pAttachments    = attachments;
		createInfo.width           = swapChainExtent.width;
		createInfo.height          = swapChainExtent.height;
		createInfo.layers          = 1;


		if( vkCreateFramebuffer(device ,&createInfo, nullptr, &frameBuffers[i] ) != VK_SUCCESS){
			cout << "Failed to create frame buffer" << endl;
		}
	}

}

u32 FindMemoryTypeIndex(u32 typeFilter, VkMemoryPropertyFlags properties ){

	VkPhysicalDeviceMemoryProperties memProperties = {};
	vkGetPhysicalDeviceMemoryProperties( physicalDevice, &memProperties);

	cout << "Memory Types: " << memProperties.memoryTypeCount << endl;
	for (int i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		//if typer filter is greater than zero and proerites are an exact match?????
		if( (typeFilter & (1 << i) ) && (memProperties.memoryTypes[i].propertyFlags &  properties) == properties )
			return i;
	}

	cout << "Failed to find Memory Type Index" << endl;
}

void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,  VkBuffer *buffer, VkDeviceMemory *memory) {
	
	VkBufferCreateInfo vBufferCreateInfo = {};
		vBufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; 
		vBufferCreateInfo.size        = size;     //HARDCODE
		vBufferCreateInfo.usage       = usage;
		vBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vBufferCreateInfo.flags       = 0;   

	if(vkCreateBuffer( device, &vBufferCreateInfo, nullptr, buffer ) != VK_SUCCESS ){
		cout << "Failed to create vertex buffer"  << endl;
	}

	VkMemoryRequirements memRequirements = {}; //Requirements dictated by the Buffer type
	 //What the graphics card actually supports
	vkGetBufferMemoryRequirements( device, *buffer, &memRequirements); 

	VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO; 
		allocInfo.allocationSize  = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryTypeIndex(memRequirements.memoryTypeBits, properties);

	if( vkAllocateMemory( device, &allocInfo, nullptr, memory) != VK_SUCCESS) {
		cout << "Failed to allocate memory for vertex buffer" << endl;
	}

	vkBindBufferMemory( device, *buffer, *memory, 0); // offset is zero

}



void CopyBuffers(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

	VkCommandBuffer commandBuffer; 
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; 
	allocInfo.level =  VK_COMMAND_BUFFER_LEVEL_PRIMARY; 
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer); 

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo); //begin recording to the command buffer
		VkBufferCopy regionCopy = {}; 
		regionCopy.srcOffset = 0;
		regionCopy.dstOffset = 0;
		regionCopy.size = size;

		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer,1, &regionCopy );

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {}; 
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1; 
	submitInfo.pCommandBuffers = &commandBuffer; 

	vkQueueSubmit(graphicsQueue,1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

}

void CreateVertexBuffer(){

	VkDeviceSize bufferSize = sizeof( vertices[0] ) * ArrayCount(vertices); // HARDCODED
	VkBuffer stagingBuffer; 
	VkDeviceMemory stagingMemory; 
	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
				  &stagingBuffer, &stagingMemory );


	void *data; 
	vkMapMemory( device, stagingMemory, 0, bufferSize, 0, &data); //pointer to a pointer
		memcpy(data, vertices, bufferSize);
	vkUnmapMemory(device, stagingMemory);

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBuffer, &vertexBufferMemory);

	CopyBuffers(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingMemory, nullptr);
}

void CreateIndexBuffer(){

	VkDeviceSize bufferSize = sizeof( indices[0] ) * ArrayCount(indices); // HARDCODED
	VkBuffer stagingBuffer; 
	VkDeviceMemory stagingMemory; 
	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
				  &stagingBuffer, &stagingMemory );


	void *data; 
	vkMapMemory( device, stagingMemory, 0, bufferSize, 0, &data); //pointer to a pointer
		memcpy(data, indices, bufferSize);
	vkUnmapMemory(device, stagingMemory);


	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexBuffer, &indexBufferMemory);

	CopyBuffers(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingMemory, nullptr);
}
// Create the Command Buffers or Allocate the Buffers
// Fill out and begin the CommandBuffer
// Initiate and Start the Begin Render Pass
void CreateCommandBuffers(){
	commandBuffers = new VkCommandBuffer[ imageViewCount];

	VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount =  imageViewCount; 
	
	if( vkAllocateCommandBuffers( device, &allocInfo, commandBuffers) != VK_SUCCESS){
		cout << "Failed alloc command buffers" << endl;
	}

	for (int i = 0; i < imageViewCount; ++i)
	{
		/* code */
		VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO; 
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			beginInfo.pInheritanceInfo = nullptr;
			
			vkBeginCommandBuffer( commandBuffers[i], &beginInfo);
	

				VkRenderPassBeginInfo renderPassBeginInfo = {};
					renderPassBeginInfo.sType 	    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
					renderPassBeginInfo.renderPass  = renderPass; //why do we give the renderpass here when we gave it to the framebuffer???
					renderPassBeginInfo.framebuffer = frameBuffers[i];
					renderPassBeginInfo.renderArea.offset = { 0 , 0}; 
					renderPassBeginInfo.renderArea.extent = swapChainExtent;

				VkClearValue clearColor = {0,0, 0 ,1};
					renderPassBeginInfo.clearValueCount = 1;
					renderPassBeginInfo.pClearValues    = &clearColor;
				
		vkCmdBeginRenderPass(commandBuffers[i],  &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE  );

				// Bind the commandbuffers with the graphics pipeline 
				vkCmdBindPipeline( commandBuffers[i] , VK_PIPELINE_BIND_POINT_GRAPHICS,  graphicsPipeline ); //Bind the buffers to the pipeline	
				VkBuffer vBuffers[] = {vertexBuffer};
				VkDeviceSize offsets[] = {0}; 
				vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vBuffers, offsets); //offset and # of bindings 
				//vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0 , VK_INDEX_TYPE_UINT16); 

				vkCmdDraw( commandBuffers[i], ArrayCount(vertices), 1, 0, 0); // HARDCODED vertexCount, instanceCount, offset into vertex buffer, offset for instanced rendering
				//vkCmdDrawIndexed(commandBuffers[i], ArrayCount(indices), 1, 0, 0, 0); //HARDCODED

		vkCmdEndRenderPass( commandBuffers[i] );

		if(vkEndCommandBuffer( commandBuffers[i]) != VK_SUCCESS ) {
			cout << "Failed to record command buffer" << endl;
		}
	}
}


void CreateImageViews(){
	imageViewCount = chainImageCount;
	cout << "Creating " << imageViewCount << " image Views. \n" << swapChainImages[0];
	imageViews = new VkImageView[imageViewCount]; 

	for (int i = 0; i < imageViewCount; ++i)
	 {
	 	VkImageViewCreateInfo createInfo = {};
	 	createInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	 	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	 	createInfo.format   = swapChainImageFormat;
	 	createInfo.image    = swapChainImages[i];

	 	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	 	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	 	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	 	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	 	createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	 	createInfo.subresourceRange.baseMipLevel   = 0;
	 	createInfo.subresourceRange.levelCount     = 1; 
	 	createInfo.subresourceRange.baseArrayLayer = 0;
	 	createInfo.subresourceRange.layerCount     = 1;

	 	if(vkCreateImageView(device, &createInfo, nullptr, &imageViews[i] ) != VK_SUCCESS){
	 		cout << "Failed to Create Image View for: " << i << endl;
	 	}
	 } 
}

//Command pools manage memory used to store the buffers and command buffers are allocated from them
void CreateCommandPool(){
	//command buffers are executed on one of the device family queues
	QueueFamilyIndices queueFamily = findQueueFamilies(	&physicalDevice);
	VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;	
		poolInfo.queueFamilyIndex = queueFamily.graphicsFamily;
		poolInfo.flags 			  = 0;

	if(vkCreateCommandPool( device, &poolInfo, nullptr, &commandPool ) != VK_SUCCESS){
		cout << "Failed to create command pool" << endl;
	}
}

void CreateSemaphores(){

	VkSemaphoreCreateInfo semaphoreInfo = {}; 
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if( (vkCreateSemaphore( device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ) || 
		(vkCreateSemaphore( device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS )){
		cout << "Failed to create Semaphores" << endl;
	}
}


void CreateVulkanInstance(VkApplicationInfo *appInfo){

	//Accumulate Necessary Extensions
    u32 extensionCount = 0;
	char ** glfwExtensions; 
    const char ** extTemp; 
    extTemp = glfwGetRequiredInstanceExtensions( &extensionCount);
   

    if(enableValidationLayers)
    {
    	cout << "Enabling Validation Layers" << endl;
    	extensionCount++;
    	glfwExtensions = new char*[extensionCount];
		for (int i = 0; i < extensionCount-1; ++i)
		{
			glfwExtensions[i] = (char*)extTemp[i];
		}
		glfwExtensions[extensionCount-1] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
		
    }else {

    	glfwExtensions = new char*[extensionCount];
		for (int i = 0; i < extensionCount; ++i) {
			glfwExtensions[i] = (char*)extTemp[i];
		}
    }

    //VKInstanceCreateInfo
	VkInstanceCreateInfo instanceInfo = {}; 
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;  
	instanceInfo.pApplicationInfo = appInfo; 

	instanceInfo.enabledExtensionCount   = extensionCount;
	instanceInfo.ppEnabledExtensionNames = glfwExtensions;  


	if(enableValidationLayers) {
		instanceInfo.enabledLayerCount   = (u32) validationLayerCount;
		instanceInfo.ppEnabledLayerNames =  validationLayers;// static_cast<const char* const*>( &validationLayers[0]); 
	}
	else {
		instanceInfo.enabledLayerCount = 0;
	}


	// Create Instance
	VkResult r = vkCreateInstance(&instanceInfo, nullptr, &instance);
	if(  r != VK_SUCCESS ){
		cout << "Creating Instance Failed:" << r <<endl;
	} 

}
void Initialize(){
	InitWindow();

	if(enableValidationLayers && !CheckValidationLayer()){
		cout << "Validation Layers Not Supported" << endl;
	}

	VkApplicationInfo appInfo = {}; 
	appInfo.sType              =  VK_STRUCTURE_TYPE_APPLICATION_INFO;  
	appInfo.pApplicationName   =  "VK Triangle"; 
	appInfo.applicationVersion =  VK_MAKE_VERSION(1,0,0); 
	appInfo.pEngineName        = "No Engine"; 
	appInfo.engineVersion      =  VK_MAKE_VERSION(1,0,0); 
	appInfo.apiVersion         =  VK_API_VERSION_1_0;


	//What extensions do we have? 
	u32 extCount = 0; 
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr); 
	VkExtensionProperties *extensions = new VkExtensionProperties[extCount]; 
	vkEnumerateInstanceExtensionProperties(nullptr, &extCount, extensions); 

	std::cout << extCount << " extensions supported." << std::endl; 


	CreateVulkanInstance(&appInfo);
	InitializeDebugging();
	CreateSurface();
	FindPhysicalDevice();	
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateFrameBuffer();
	CreateCommandPool();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateCommandBuffers();
	CreateSemaphores();

}

void CleanUpSwapChain() {
	
	for (int i = 0; i < imageViewCount; ++i)
	{
		vkDestroyFramebuffer(device, frameBuffers[i], nullptr );
	}

	vkFreeCommandBuffers(device, commandPool, imageViewCount, commandBuffers);

	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr); 
	vkDestroyRenderPass(device, renderPass, nullptr);

	for (int i = 0; i < imageViewCount; ++i)
	{
		vkDestroyImageView(device, imageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(device, swapChain, nullptr);


}

void RecreateSwapChain(){
	CleanUpSwapChain();
	vkDeviceWaitIdle(device); 

	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateFrameBuffer();
	CreateCommandBuffers();
}


void DrawFrame( ){
/* Acquire an image from the swap chain
   Execute the command buffer with that image as attachment in the framebuffer
   Return the image to the swap chain for presentation
  */

	//UpdateAppState
	//queueWaitIdle()

	u32 imageIndex; 
	vkAcquireNextImageKHR( device, swapChain,  (uint64_t) 9223372036854775807, imageAvailableSemaphore, VK_NULL_HANDLE,  &imageIndex); 

	VkSubmitInfo submitInfo = {}; 
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[]      = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; //Which stage to wait for? 
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores    = waitSemaphores;
	submitInfo.pWaitDstStageMask  = waitStages; 

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffers[imageIndex];

	//Specify which semaphores to signal once the commandbuffers have finished execution
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore }; //Queue will flag this semaphore
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores    = signalSemaphores;

	if(vkQueueSubmit ( graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE ) != VK_SUCCESS ){
		cout << "Failed to submit Draw Command Buffer" << endl;
	}

	 //Submit result back to the swapChain and show it on the screen 
	//Time to present

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores; //when render is finished

	VkSwapchainKHR swapChains[] = { swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains   = swapChains; //why array here? 
	presentInfo.pImageIndices = &imageIndex; 

	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(presentQueue, &presentInfo);  //Present it to the screen
	vkQueueWaitIdle(presentQueue);

}


void MainLoop(){
while(!glfwWindowShouldClose(window) ){
		glfwPollEvents();
		DrawFrame();
	}
	vkDeviceWaitIdle(device); 
}



void CleanUp(){
	
	CleanUpSwapChain(); 

	vkDestroyBuffer(device, indexBuffer, nullptr); 
	vkFreeMemory(device, indexBufferMemory, nullptr);

	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);
	vkDestroySemaphore(device, renderFinishedSemaphore, nullptr); 
	vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);

	vkDestroyCommandPool(device, commandPool, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);
	// Need to destroy the debug callback - 
	PFN_vkDestroyDebugReportCallbackEXT DestroyDebugCallback = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT") ;
	if(DestroyDebugCallback != nullptr) {
		DestroyDebugCallback( instance, callback, nullptr);
	}

	vkDestroyDevice( device, nullptr);
	vkDestroyInstance( instance,  nullptr);


	glfwDestroyWindow(window); 
	glfwTerminate();

}

int main(int argc, char* args){


	Initialize(); 
	MainLoop(); 
	CleanUp();

	return 0; 

}