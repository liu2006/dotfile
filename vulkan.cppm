module;
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <vulkan/vulkan_raii.hpp>
export module initVulkan;
import vertexInput;
const std::vector<const char *> validationLayers{"VK_LAYER_KHRONOS_validation"};
const std::vector<const char *> requiredDeviceExtensions{vk::KHRSwapchainExtensionName};
constexpr int MAX_FRAMES_IN_FLIGHT{2};
constexpr bool enableValidationLayers{
#ifdef NDEBUG
    false
#else
    true
#endif
};

export class Vulkan
{
    friend class SDLGuard;
    friend class Instance;
    friend class DebugMessenger;
    friend class Surface;
    friend class PhysicalDevice;
    friend class Device;
    friend class SwapChain;
    friend class Pipeline;
    friend class CommandPool;
    friend class CommandBuffers;
    friend class SyncObjects;
    friend class BaseBuffer;
    friend class VertexBuffer;
    friend class Draw;

private:
    vk::raii::Context context;
    vk::raii::Instance instance{nullptr};
    vk::raii::DebugUtilsMessengerEXT debugMessenger{nullptr};
    vk::raii::SurfaceKHR surface{nullptr};
    vk::raii::PhysicalDevice physicalDevice{nullptr};
    vk::raii::Device device{nullptr};
    vk::raii::Queue graphicsQueue{nullptr};
    uint32_t graphicsQueueIndex;
    vk::raii::SwapchainKHR swapChain{nullptr};
    vk::SurfaceFormatKHR swapChainFormat;
    vk::Extent2D swapChainExtent;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::raii::ImageView> swapChainImageViews;
    vk::raii::PipelineLayout pipelineLayout{nullptr};
    vk::raii::Pipeline graphicsPipeline{nullptr};
    vk::raii::CommandPool commandPool{nullptr};
    std::vector<vk::raii::CommandBuffer> commandBuffers;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
    std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
    std::vector<vk::raii::Fence> inFlightFences;

    vk::raii::Buffer vertexBuffer{nullptr};
    vk::raii::DeviceMemory vertexBufferMemory{nullptr};
    uint32_t frameIndex{};
    bool framebufferResized{};

public:
    Vulkan() = default;
    ~Vulkan() = default;
    Vulkan(const Vulkan &) = delete;
    Vulkan &operator=(const Vulkan &) = delete;
};
export class SDLGuard
{
private:
    SDL_Window *window;
    uint32_t width{1920};
    uint32_t height{1080};
    const char *title{"vulkan"};
    SDL_Event event;
    bool running{true};

public:
    SDL_Window *getWindow() { return window; };
    template <typename T> void mainLoop(Vulkan *vulkan, T &&func)
    {
        SDL_ShowWindow(window);
        while (running) {
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                    vulkan->framebufferResized = true;
                default:
                    break;
                }
            }
            std::invoke(std::forward<T>(func));
        }
        vulkan->device.waitIdle();
    }
    SDLGuard()
    {
        SDL_Init(SDL_INIT_VIDEO);
        window =
            SDL_CreateWindow(title, width, height,
                             SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
    }
    ~SDLGuard()
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    SDLGuard(const SDLGuard &) = delete;
    SDLGuard &operator=(const SDLGuard &) = delete;
    SDLGuard(SDLGuard &&) = delete;
    SDLGuard &operator=(SDLGuard &&) = delete;
};

export class Instance
{
private:
    uint32_t sdlExtensionCount{};
    char const *const *sdlExtensions{SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount)};
    std::vector<const char *> extensions{sdlExtensions, sdlExtensions + sdlExtensionCount};
    std::vector<vk::ExtensionProperties> extensionProperties;
    std::vector<const char *> layers;
    std::vector<vk::LayerProperties> layerProperties;

public:
    explicit Instance(Vulkan *vulkan)
        : extensionProperties{vulkan->context.enumerateInstanceExtensionProperties()},
          layerProperties{vulkan->context.enumerateInstanceLayerProperties()}
    {
        vk::ApplicationInfo appInfo{.pApplicationName = "Vulkan",
                                    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                                    .pEngineName = "No engine",
                                    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                                    .apiVersion = vk::ApiVersion14};
        if (enableValidationLayers) {
            extensions.push_back(vk::EXTDebugUtilsExtensionName);
            layers.assign(validationLayers.begin(), validationLayers.end());
        }
        auto unsupportedExtension =
            std::ranges::find_if(extensions, [this](const auto &extension) {
                return std::ranges::none_of(
                    this->extensionProperties, [extension](const auto &extensionProperty) {
                        return std::strcmp(extensionProperty.extensionName, extension) == 0;
                    });
            });
        if (unsupportedExtension != extensions.end()) {
            throw std::runtime_error(
                std::format("Required extension not supported: {}", *unsupportedExtension));
        }
        auto unsupportedLayer = std::ranges::find_if(layers, [this](const auto &layer) {
            return std::ranges::none_of(
                this->layerProperties, [layer](const auto &layerProperty) {
                    return std::strcmp(layerProperty.layerName, layer) == 0;
                });
        });
        if (unsupportedLayer != layers.end()) {
            throw std::runtime_error(
                std::format("Required layer not supported: {}", *unsupportedLayer));
        }
        vk::InstanceCreateInfo instanceInfo{
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = static_cast<uint32_t>(layers.size()),
            .ppEnabledLayerNames = layers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data()};
        vulkan->instance = vk::raii::Instance{vulkan->context, instanceInfo};
    }
    ~Instance() = default;
    Instance(const Instance &) = delete;
    Instance &operator=(const Instance &) = delete;
};

export class DebugMessenger
{
private:
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL
    debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                  vk::DebugUtilsMessageTypeFlagsEXT type,
                  const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
    {
        if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
            std::cerr << std::format("Validation layer type: {}, message: {}\n",
                                     to_string(type), pCallbackData->pMessage);
        }
        return vk::False;
    }

public:
    explicit DebugMessenger(Vulkan *vulkan)
    {
        if (!enableValidationLayers)
            return;
        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags{
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose};
        vk::DebugUtilsMessageTypeFlagsEXT typeFlags{
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance};
        vk::DebugUtilsMessengerCreateInfoEXT debugInfo{
            .messageSeverity = severityFlags,
            .messageType = typeFlags,
            .pfnUserCallback = &debugCallback,
        };
        vulkan->debugMessenger = vk::raii::DebugUtilsMessengerEXT{vulkan->instance, debugInfo};
    }

    ~DebugMessenger() = default;
    DebugMessenger(const DebugMessenger &) = delete;
    DebugMessenger &operator=(const DebugMessenger &) = delete;
};

export class Surface
{
private:
    VkSurfaceKHR _surface;

public:
    Surface(Vulkan *vulkan, SDL_Window *window)
    {
        if (!SDL_Vulkan_CreateSurface(window, *vulkan->instance, nullptr, &_surface)) {
            throw std::runtime_error("Could not create vulkan surface");
        }
        vulkan->surface = vk::raii::SurfaceKHR{vulkan->instance, _surface};
    }
    ~Surface() = default;
    Surface(const Surface &) = delete;
    Surface &operator=(const Surface &) = delete;
};

export class PhysicalDevice
{
private:
    std::vector<vk::raii::PhysicalDevice> physicalDevices;
    bool supportVulkan13;
    bool supportGraphics;
    bool supportDeviceExtensions;
    bool supportFeatures;

public:
    explicit PhysicalDevice(Vulkan *vulkan)
        : physicalDevices{vulkan->instance.enumeratePhysicalDevices()}
    {
        if (physicalDevices.empty())
            throw std::runtime_error("Could not find physical device");
        auto physicalDevice = std::ranges::find_if(physicalDevices, [&](const auto
                                                                            &physicalDevice) {
            supportVulkan13 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;
            auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
            supportGraphics = std::ranges::any_of(queueFamilyProperties, [](const auto &qfp) {
                return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
            });
            auto availableDeviceExtensions =
                physicalDevice.enumerateDeviceExtensionProperties();
            supportDeviceExtensions = std::ranges::all_of(
                requiredDeviceExtensions,
                [&availableDeviceExtensions](const auto &requiredDeviceExtension) {
                    return std::ranges::any_of(
                        availableDeviceExtensions,
                        [requiredDeviceExtension](const auto &availableDeviceExtension) {
                            return std::strcmp(availableDeviceExtension.extensionName,
                                               requiredDeviceExtension) == 0;
                        });
                });
            auto features = physicalDevice.template getFeatures2<
                vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
                vk::PhysicalDeviceVulkan13Features,
                vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
            supportFeatures =
                features.template get<vk::PhysicalDeviceVulkan11Features>()
                    .shaderDrawParameters &&
                features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
                features.template get<vk::PhysicalDeviceVulkan13Features>().synchronization2 &&
                features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
                    .extendedDynamicState;
            return supportVulkan13 && supportGraphics && supportDeviceExtensions &&
                   supportFeatures;
        });
        if (physicalDevice == physicalDevices.end()) {
            throw std::runtime_error("Could not find suitable GPU");
        }
        vulkan->physicalDevice = *physicalDevice;
    }
    ~PhysicalDevice() = default;
    PhysicalDevice(const PhysicalDevice &) = delete;
    PhysicalDevice &operator=(const PhysicalDevice &) = delete;
};
export class Device
{
private:
    float queuePriority{0.5f};

public:
    explicit Device(Vulkan *vulkan)
    {
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties{
            vulkan->physicalDevice.getQueueFamilyProperties()};
        auto graphicsQueueFamilyProperty =
            std::ranges::find_if(queueFamilyProperties, [](const auto &qfp) {
                return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
            });
        vulkan->graphicsQueueIndex = static_cast<uint32_t>(
            std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
        vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
                           vk::PhysicalDeviceVulkan13Features,
                           vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
            featureChain{{},
                         {.shaderDrawParameters = true},
                         {.synchronization2 = true, .dynamicRendering = true},
                         {.extendedDynamicState = true}};
        vk::DeviceQueueCreateInfo queueInfo{.queueFamilyIndex = vulkan->graphicsQueueIndex,
                                            .queueCount = 1,
                                            .pQueuePriorities = &queuePriority};
        vk::DeviceCreateInfo deviceInfo{
            .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queueInfo,
            .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size()),
            .ppEnabledExtensionNames = requiredDeviceExtensions.data()};
        vulkan->device = vk::raii::Device{vulkan->physicalDevice, deviceInfo};
        vulkan->graphicsQueue = vk::raii::Queue{vulkan->device, vulkan->graphicsQueueIndex, 0};
    }
    ~Device() = default;
    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;
};

export class SwapChain
{
private:
    vk::SurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<vk::SurfaceFormatKHR> availableFormats;
    std::vector<vk::PresentModeKHR> availablePresentModes;
    int width{}, height{};
    uint32_t minImageCount;

public:
    SwapChain(Vulkan *vulkan, SDL_Window *window)
        : surfaceCapabilities{
              vulkan->physicalDevice.getSurfaceCapabilitiesKHR(*vulkan->surface)},
          availableFormats{vulkan->physicalDevice.getSurfaceFormatsKHR(*vulkan->surface)},
          availablePresentModes{
              vulkan->physicalDevice.getSurfacePresentModesKHR(*vulkan->surface)},
          minImageCount{std::max(3u, surfaceCapabilities.minImageCount)}
    {
        vulkan->swapChainFormat = [&]() {
            if (availableFormats.empty())
                throw std::runtime_error("Could not suitable surface format");
            auto format = std::ranges::find_if(availableFormats, [](const auto &format) {
                return format.format == vk::Format::eB8G8R8Srgb &&
                       format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
            });
            return (format != availableFormats.end()) ? *format : availableFormats[0];
        }();
        auto swapChainPresentMode = [&]() {
            if (std::ranges::none_of(availablePresentModes, [](const auto &presentMode) {
                    return presentMode == vk::PresentModeKHR::eFifo;
                })) {
                throw std::runtime_error("Could not find suitable surface present mode");
            }
            auto presentMode =
                std::ranges::find_if(availablePresentModes, [](const auto &presentMode) {
                    return presentMode == vk::PresentModeKHR::eMailbox;
                });
            return (presentMode != availablePresentModes.end()) ? *presentMode
                                                                : vk::PresentModeKHR::eFifo;
        }();
        vulkan->swapChainExtent = [this, window]() {
            if (this->surfaceCapabilities.currentExtent.width != UINT32_MAX) {
                return this->surfaceCapabilities.currentExtent;
            }
            SDL_GetWindowSizeInPixels(window, &width, &height);
            return vk::Extent2D{
                std::clamp<uint32_t>(width, this->surfaceCapabilities.minImageExtent.width,
                                     this->surfaceCapabilities.maxImageExtent.width),
                std::clamp<uint32_t>(height, this->surfaceCapabilities.minImageExtent.height,
                                     this->surfaceCapabilities.maxImageExtent.height)};
        }();
        auto swapChainMinImageCount = [this]() {
            if (minImageCount > this->surfaceCapabilities.maxImageCount &&
                this->surfaceCapabilities.maxImageCount > 0) {
                minImageCount = this->surfaceCapabilities.maxImageCount;
            }
            return minImageCount;
        }();
        vk::SwapchainCreateInfoKHR swapChainInfo{
            .surface = *vulkan->surface,
            .minImageCount = swapChainMinImageCount,
            .imageFormat = vulkan->swapChainFormat.format,
            .imageColorSpace = vulkan->swapChainFormat.colorSpace,
            .imageExtent = vulkan->swapChainExtent,
            .imageArrayLayers = 1,
            .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
            .imageSharingMode = vk::SharingMode::eExclusive,
            .preTransform = surfaceCapabilities.currentTransform,
            .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
            .presentMode = swapChainPresentMode,
            .clipped = true};
        vulkan->swapChain = vk::raii::SwapchainKHR{vulkan->device, swapChainInfo};
        vulkan->swapChainImages = vulkan->swapChain.getImages();
        vk::ImageViewCreateInfo imageViewInfo{
            .viewType = vk::ImageViewType::e2D,
            .format = vulkan->swapChainFormat.format,
            .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1},
        };
        for (auto &image : vulkan->swapChainImages) {
            imageViewInfo.image = image;
            vulkan->swapChainImageViews.emplace_back(vulkan->device, imageViewInfo);
        }
    }
    ~SwapChain() = default;
    SwapChain(const SwapChain &) = delete;
    SwapChain &operator=(const SwapChain &) = delete;
};

export class Pipeline
{
private:
    std::vector<vk::DynamicState> dynamicStates{vk::DynamicState::eViewport,
                                                vk::DynamicState::eScissor};

public:
    explicit Pipeline(Vulkan *vulkan)
    {
        std::ifstream file{"../shaders/slang.spv", std::ios::ate | std::ios::binary};
        if (!file.is_open())
            throw std::runtime_error("Could not open shader file");
        std::vector<char> shaderCode(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(shaderCode.data(), static_cast<std::streamsize>(shaderCode.size()));
        file.close();

        vk::ShaderModuleCreateInfo shaderModuleInfo{
            .codeSize = shaderCode.size(),
            .pCode = reinterpret_cast<const uint32_t *>(shaderCode.data())};
        vk::raii::ShaderModule shaderModule{vulkan->device, shaderModuleInfo};

        vk::PipelineShaderStageCreateInfo vertexShaderInfo{
            .stage = vk::ShaderStageFlagBits::eVertex,
            .module = shaderModule,
            .pName = "vertMain"};
        vk::PipelineShaderStageCreateInfo fragmentShaderInfo{
            .stage = vk::ShaderStageFlagBits::eFragment,
            .module = shaderModule,
            .pName = "fragMain"};
        std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages{vertexShaderInfo,
                                                                      fragmentShaderInfo};
        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &bindingDescription,
            .vertexAttributeDescriptionCount = attributeDescriptions.size(),
            .pVertexAttributeDescriptions = attributeDescriptions.data()};

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{
            .topology = vk::PrimitiveTopology::eTriangleList,
        };
        vk::PipelineViewportStateCreateInfo viewportInfo{.viewportCount = 1,
                                                         .scissorCount = 1};
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo{
            .depthClampEnable = vk::False,
            .rasterizerDiscardEnable = vk::False,
            .polygonMode = vk::PolygonMode::eFill,
            .cullMode = vk::CullModeFlagBits::eBack,
            .frontFace = vk::FrontFace::eClockwise,
            .depthBiasEnable = vk::False,
            .lineWidth = 1.0f};

        vk::PipelineMultisampleStateCreateInfo multisampleInfo{
            .rasterizationSamples = vk::SampleCountFlagBits::e1,
            .sampleShadingEnable = vk::False};

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{
            .blendEnable = vk::False,
            .colorWriteMask = {
                vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA}};
        vk::PipelineColorBlendStateCreateInfo colorBlendInfo{.logicOpEnable = vk::False,
                                                             .logicOp = vk::LogicOp::eCopy,
                                                             .attachmentCount = 1,
                                                             .pAttachments =
                                                                 &colorBlendAttachment};
        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data(),
        };
        vk::PipelineLayoutCreateInfo layoutInfo{.setLayoutCount = 0,
                                                .pushConstantRangeCount = 0};
        vulkan->pipelineLayout = vk::raii::PipelineLayout{vulkan->device, layoutInfo};
        vk::PipelineRenderingCreateInfo renderingInfo{.colorAttachmentCount = 1,
                                                      .pColorAttachmentFormats =
                                                          &vulkan->swapChainFormat.format};
        vk::GraphicsPipelineCreateInfo graphicsPipelineInfo{
            .pNext = &renderingInfo,
            .stageCount = shaderStages.size(),
            .pStages = shaderStages.data(),
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssemblyInfo,
            .pViewportState = &viewportInfo,
            .pRasterizationState = &rasterizationInfo,
            .pMultisampleState = &multisampleInfo,
            .pColorBlendState = &colorBlendInfo,
            .pDynamicState = &dynamicStateInfo,
            .layout = vulkan->pipelineLayout,
            .renderPass = nullptr};
        vulkan->graphicsPipeline =
            vk::raii::Pipeline{vulkan->device, nullptr, graphicsPipelineInfo};
    }
    ~Pipeline() = default;
    Pipeline(const Pipeline &) = delete;
    Pipeline &operator=(const Pipeline &) = delete;
};

export class CommandPool
{
private:
public:
    explicit CommandPool(Vulkan *vulkan)
    {
        vk::CommandPoolCreateInfo commandPoolInfo{
            .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            .queueFamilyIndex = vulkan->graphicsQueueIndex};
        vulkan->commandPool = vk::raii::CommandPool{vulkan->device, commandPoolInfo};
    }
    ~CommandPool() = default;
    CommandPool(const CommandPool &) = delete;
    CommandPool &operator=(const CommandPool &) = delete;
};

export class CommandBuffers
{
private:
    vk::CommandBufferAllocateInfo allocateInfo;

public:
    explicit CommandBuffers(Vulkan *vulkan)
        : allocateInfo{.commandPool = vulkan->commandPool,
                       .level = vk::CommandBufferLevel::ePrimary,
                       .commandBufferCount = MAX_FRAMES_IN_FLIGHT}
    {
        vulkan->commandBuffers = vk::raii::CommandBuffers{vulkan->device, allocateInfo};
    }
    ~CommandBuffers() = default;
    CommandBuffers(const CommandBuffers &) = delete;
    CommandBuffers &operator=(const CommandBuffers &) = delete;
};

export class SyncObjects
{
private:
public:
    explicit SyncObjects(Vulkan *vulkan)
    {
        for (size_t i{}; i < vulkan->swapChainImages.size(); i++) {
            vulkan->renderFinishedSemaphores.emplace_back(vulkan->device,
                                                          vk::SemaphoreCreateInfo());
        }
        for (size_t i{}; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vulkan->presentCompleteSemaphores.emplace_back(vulkan->device,
                                                           vk::SemaphoreCreateInfo());
            vulkan->inFlightFences.emplace_back(
                vulkan->device,
                vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});
        }
    }
    ~SyncObjects() = default;
    SyncObjects(const SyncObjects &) = delete;
    SyncObjects &operator=(const SyncObjects &) = delete;
};
export class BaseBuffer
{
    friend class VertexBuffer;
private:
    vk::DeviceSize size;
    vk::PhysicalDeviceMemoryProperties memoryProperties;
    Vulkan *vulkan;
    
public:
    BaseBuffer(Vulkan *vulkan, vk::DeviceSize size)
        : vulkan{vulkan}, size{size},
          memoryProperties{vulkan->physicalDevice.getMemoryProperties()}
    {
    }
    void createBuffer(vk::raii::Buffer &buffer, vk::raii::DeviceMemory &bufferMemory,
                     vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
    {
        vk::BufferCreateInfo bufferInfo{
            .size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive};
        buffer = vk::raii::Buffer{vulkan->device, bufferInfo};
        vk::MemoryRequirements memoryRequirements{buffer.getMemoryRequirements()};
        vk::MemoryAllocateInfo memoryAllocateInfo{
            .allocationSize = memoryRequirements.size,
            .memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties)};
        bufferMemory = vk::raii::DeviceMemory{vulkan->device, memoryAllocateInfo};
        buffer.bindMemory(bufferMemory, 0);
    }
    uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        for (uint32_t i{}; i < memoryProperties.memoryTypeCount; i++) {
            if (!!(typeFilter & (i << i)) &&
                (memoryProperties.memoryTypes[i].propertyFlags & properties))
                return i;
        }
        throw std::runtime_error("Could not find suitabel memory type");
    }
    void copyBuffer(vk::raii::Buffer &srcBuffer, vk::raii::Buffer &dstBuffer)
    {
        vk::CommandBufferAllocateInfo allocateInfo{.commandPool = vulkan->commandPool,
                                                   .level = vk::CommandBufferLevel::ePrimary,
                                                   .commandBufferCount = 1};
        vk::raii::CommandBuffer _commandBuffer{
            std::move(vk::raii::CommandBuffers{vulkan->device, allocateInfo}.front())};
        _commandBuffer.begin(vk::CommandBufferBeginInfo{
            .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
        _commandBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
        _commandBuffer.end();
        vk::raii::Fence fence{vulkan->device, vk::FenceCreateInfo{}};
        vulkan->graphicsQueue.submit(
            vk::SubmitInfo{.commandBufferCount = 1, .pCommandBuffers = &*_commandBuffer},
            *fence);
        auto fenceResult = vulkan->device.waitForFences(*fence, vk::True, UINT64_MAX);
    }
    ~BaseBuffer() = default;
    BaseBuffer(const BaseBuffer &) = delete;
    BaseBuffer &operator=(const BaseBuffer &) = delete;
};

export class VertexBuffer : public BaseBuffer
{
private:
    vk::raii::Buffer stageBuffer{nullptr};
    vk::raii::DeviceMemory stageBufferMemory{nullptr};
    
public:
    explicit VertexBuffer(Vulkan *vulkan, vk::DeviceSize size)
        : BaseBuffer{vulkan, size}
    {
        createBuffer(stageBuffer, stageBufferMemory,
                     vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible |
                         vk::MemoryPropertyFlagBits::eHostCoherent);
        
        void *stageData = stageBufferMemory.mapMemory(0, size);
        memcpy(stageData, vertices.data(), size);
        stageBufferMemory.unmapMemory();

        createBuffer(vulkan->vertexBuffer, vulkan->vertexBufferMemory,
                     vk::BufferUsageFlagBits::eTransferDst |
                         vk::BufferUsageFlagBits::eVertexBuffer,
                     vk::MemoryPropertyFlagBits::eDeviceLocal);
        copyBuffer(stageBuffer, vulkan->vertexBuffer);
    }
    ~VertexBuffer() = default;
    VertexBuffer(const VertexBuffer &) = delete;
    VertexBuffer &operator=(const VertexBuffer &) = delete;
};

export class Draw
{
private:
    static void recreateSwapChain(Vulkan *vulkan, SDL_Window *window)
    {
        int width{}, height{};
        SDL_GetWindowSizeInPixels(window, &width, &height);
        while (width == 0 || height == 0) {
            SDL_GetWindowSizeInPixels(window, &width, &height);
            SDL_WaitEvent(nullptr);
        }
        vulkan->device.waitIdle();
        vulkan->swapChainImageViews.clear();
        vulkan->swapChain = nullptr;
        std::make_unique<SwapChain>(vulkan, window);
    }
    static void recordCommandBuffer(Vulkan *vulkan, uint32_t imageIndex)
    {
        auto &commandBuffer = vulkan->commandBuffers[vulkan->frameIndex];
        commandBuffer.begin({});
        transitionImageLayout(vulkan, imageIndex, vk::ImageLayout::eUndefined,
                              vk::ImageLayout::eColorAttachmentOptimal, {},
                              vk::AccessFlagBits2::eColorAttachmentWrite,
                              vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                              vk::PipelineStageFlagBits2::eColorAttachmentOutput);
        vk::ClearValue clearColor{vk::ClearColorValue{0.0f, 0.0f, 0.0f, 1.0f}};
        vk::RenderingAttachmentInfo attachmentInfo{
            .imageView = vulkan->swapChainImageViews[imageIndex],
            .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
            .loadOp = vk::AttachmentLoadOp::eClear,
            .storeOp = vk::AttachmentStoreOp::eStore,
            .clearValue = clearColor,
        };
        vk::RenderingInfo renderingInfo{
            .renderArea = {vk::Rect2D(vk::Offset2D(0, 0), vulkan->swapChainExtent)},
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &attachmentInfo};
        commandBuffer.beginRendering(renderingInfo);
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, vulkan->graphicsPipeline);
        commandBuffer.setViewport(
            0, vk::Viewport(0.0f, 0.0f, static_cast<float>(vulkan->swapChainExtent.width),
                            static_cast<float>(vulkan->swapChainExtent.height), 0.0f, 1.0f));
        commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), vulkan->swapChainExtent));
        commandBuffer.bindVertexBuffers(0, *vulkan->vertexBuffer, {0});
        commandBuffer.draw(vertices.size(), 1, 0, 0);
        commandBuffer.endRendering();
        transitionImageLayout(vulkan, imageIndex, vk::ImageLayout::eColorAttachmentOptimal,
                              vk::ImageLayout::ePresentSrcKHR,
                              vk::AccessFlagBits2::eColorAttachmentWrite, {},
                              vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                              vk::PipelineStageFlagBits2::eBottomOfPipe);
        commandBuffer.end();
    }
    static void transitionImageLayout(Vulkan *vulkan, uint32_t imageIndex,
                                      vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                                      vk::AccessFlags2 srcAccessMask,
                                      vk::AccessFlags2 dstAccessMask,
                                      vk::PipelineStageFlags2 srcStageMask,
                                      vk::PipelineStageFlags2 dstStageMask)
    {
        vk::ImageMemoryBarrier2 barrier{
            .srcStageMask = srcStageMask,
            .srcAccessMask = srcAccessMask,
            .dstStageMask = dstStageMask,
            .dstAccessMask = dstAccessMask,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = vulkan->swapChainImages[imageIndex],
            .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};
        vk::DependencyInfo dependencyInfo{.imageMemoryBarrierCount = 1,
                                          .pImageMemoryBarriers = &barrier};
        vulkan->commandBuffers[vulkan->frameIndex].pipelineBarrier2(dependencyInfo);
    }

public:
    static void drawFrame(Vulkan *vulkan, SDL_Window *window)
    {
        auto fenceResult = vulkan->device.waitForFences(
            *vulkan->inFlightFences[vulkan->frameIndex], vk::True, UINT64_MAX);
        auto [result, imageIndex] = vulkan->swapChain.acquireNextImage(
            UINT64_MAX, vulkan->presentCompleteSemaphores[vulkan->frameIndex], nullptr);
        if (result == vk::Result::eErrorOutOfDateKHR) {
            recreateSwapChain(vulkan, window);
            return;
        }
        vulkan->device.resetFences(*vulkan->inFlightFences[vulkan->frameIndex]);
        vulkan->commandBuffers[vulkan->frameIndex].reset();
        recordCommandBuffer(vulkan, imageIndex);
        vk::PipelineStageFlags waitDestinationStageMask{
            vk::PipelineStageFlagBits::eColorAttachmentOutput};
        const vk::SubmitInfo submitInfo{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*vulkan->presentCompleteSemaphores[vulkan->frameIndex],
            .pWaitDstStageMask = &waitDestinationStageMask,
            .commandBufferCount = 1,
            .pCommandBuffers = &*vulkan->commandBuffers[vulkan->frameIndex],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &*vulkan->renderFinishedSemaphores[imageIndex]};
        vulkan->graphicsQueue.submit(submitInfo, *vulkan->inFlightFences[vulkan->frameIndex]);
        const vk::PresentInfoKHR presentInfo{
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*vulkan->renderFinishedSemaphores[imageIndex],
            .swapchainCount = 1,
            .pSwapchains = &*vulkan->swapChain,
            .pImageIndices = &imageIndex};
        result = vulkan->graphicsQueue.presentKHR(presentInfo);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR ||
            vulkan->framebufferResized) {
            vulkan->framebufferResized = false;
            recreateSwapChain(vulkan, window);
        }
        vulkan->frameIndex = (vulkan->frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    Draw() = default;
    ~Draw() = default;
};
