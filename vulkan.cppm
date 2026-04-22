module;
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <iostream>
#include <vulkan/vulkan_raii.hpp>
export module initVulkan;
const std::vector<const char *> validationLayers{"VK_LAYER_KHRONOS_validation"};
const std::vector<const char *> requiredDeviceExtensions{
    vk::KHRSwapchainExtensionName};
constexpr bool enableValidationLayers{
#ifdef NDEBUG
    false
#else
    true
#endif
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
    void mainLoop()
    {
        SDL_ShowWindow(window);
        while (running) {
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                default:
                    break;
                }
            }
        }
    }

public:
    SDL_Window *getWindow() { return window; };
    void run() { mainLoop(); }
    SDLGuard()
    {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow(title, width, height,
                                  SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN);
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

export class Vulkan
{
    friend class Instance;
    friend class DebugMessenger;
    friend class Surface;
    friend class PhysicalDevice;
    friend class Device;

private:
    vk::raii::Context context;
    vk::raii::Instance instance{nullptr};
    vk::raii::DebugUtilsMessengerEXT debugMessenger{nullptr};
    vk::raii::SurfaceKHR surface{nullptr};
    vk::raii::PhysicalDevice physicalDevice{nullptr};
    vk::raii::Device device{nullptr};
    vk::raii::Queue graphicsQueue{nullptr};
    
public:
    Vulkan() = default;
    ~Vulkan() = default;
    Vulkan(const Vulkan &) = delete;
    Vulkan &operator=(const Vulkan &) = delete;
};

export class Instance
{
private:
    uint32_t sdlExtensionCount{};
    char const *const *sdlExtensions{
        SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount)};
    std::vector<const char *> extensions{sdlExtensions,
                                         sdlExtensions + sdlExtensionCount};
    std::vector<vk::ExtensionProperties> extensionProperties;
    std::vector<const char *> layers;
    std::vector<vk::LayerProperties> layerProperties;

public:
    explicit Instance(Vulkan *vulkan)
        : extensionProperties{
              vulkan->context.enumerateInstanceExtensionProperties()},
          layerProperties{vulkan->context.enumerateInstanceLayerProperties()}
    {
        vk::ApplicationInfo appInfo{.pApplicationName = "Vulkan",
                                    .applicationVersion =
                                        VK_MAKE_VERSION(1, 0, 0),
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
                    this->extensionProperties,
                    [extension](const auto &extensionProperty) {
                        return std::strcmp(extensionProperty.extensionName,
                                           extension) == 0;
                    });
            });
        if (unsupportedExtension != extensions.end()) {
            throw std::runtime_error(std::format(
                "Required extension not supported: {}", *unsupportedExtension));
        }
        auto unsupportedLayer =
            std::ranges::find_if(layers, [this](const auto &layer) {
                return std::ranges::none_of(
                    this->layerProperties, [layer](const auto &layerProperty) {
                        return std::strcmp(layerProperty.layerName, layer) == 0;
                    });
            });
        if (unsupportedLayer != layers.end()) {
            throw std::runtime_error(std::format(
                "Required layer not supported: {}", *unsupportedLayer));
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
                  const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
                  void *pUserData)
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
        if (enableValidationLayers)
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
        vulkan->debugMessenger =
            vk::raii::DebugUtilsMessengerEXT{vulkan->instance, debugInfo};
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
        if (!SDL_Vulkan_CreateSurface(window, *vulkan->instance, nullptr,
                                      &_surface)) {
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
        auto physicalDevice = std::ranges::find_if(
            physicalDevices, [&](const auto &physicalDevice) {
                supportVulkan13 = physicalDevice.getProperties().apiVersion >=
                                  vk::ApiVersion13;
                auto queueFamilyProperties =
                    physicalDevice.getQueueFamilyProperties();
                supportGraphics = std::ranges::any_of(
                    queueFamilyProperties, [](const auto &qfp) {
                        return !!(qfp.queueFlags &
                                  vk::QueueFlagBits::eGraphics);
                    });
                auto availableDeviceExtensions =
                    physicalDevice.enumerateDeviceExtensionProperties();
                supportDeviceExtensions = std::ranges::all_of(
                    requiredDeviceExtensions,
                    [&availableDeviceExtensions](
                        const auto &requiredDeviceExtension) {
                        return std::ranges::any_of(
                            availableDeviceExtensions,
                            [requiredDeviceExtension](
                                const auto &availableDeviceExtension) {
                                return std::strcmp(availableDeviceExtension
                                                       .extensionName,
                                                   requiredDeviceExtension) ==
                                       0;
                            });
                    });
                auto features = physicalDevice.template getFeatures2<
                    vk::PhysicalDeviceFeatures2,
                    vk::PhysicalDeviceVulkan11Features,
                    vk::PhysicalDeviceVulkan13Features,
                    vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
                supportFeatures =
                    features.template get<vk::PhysicalDeviceVulkan11Features>()
                        .shaderDrawParameters &&
                    features.template get<vk::PhysicalDeviceVulkan13Features>()
                        .dynamicRendering &&
                    features
                        .template get<
                            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
                        .extendedDynamicState;
                return supportVulkan13 && supportGraphics &&
                       supportDeviceExtensions && supportFeatures;
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
    float queuePriority{0.5f};
    explicit Device(Vulkan *vulkan)
    {
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties{
            vulkan->physicalDevice.getQueueFamilyProperties()};
        auto graphicsQueueFamilyProperty =
            std::ranges::find_if(queueFamilyProperties, [](const auto &qfp) {
                return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
            });
        uint32_t graphicsIndex{static_cast<uint32_t>(std::distance(
            queueFamilyProperties.begin(), graphicsQueueFamilyProperty))};
        vk::StructureChain<vk::PhysicalDeviceFeatures2,
                           vk::PhysicalDeviceVulkan11Features,
                           vk::PhysicalDeviceVulkan13Features,
                           vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
            featureChain{{},
                         {.shaderDrawParameters = true},
                         {.dynamicRendering = true},
                         {.extendedDynamicState = true}};
        vk::DeviceQueueCreateInfo queueInfo{.queueFamilyIndex = graphicsIndex,
                                            .queueCount = 1,
                                            .pQueuePriorities = &queuePriority};
        vk::DeviceCreateInfo deviceInfo{
            .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queueInfo,
            .enabledExtensionCount =
                static_cast<uint32_t>(requiredDeviceExtensions.size()),
        };
        
        vulkan->device = vk::raii::Device{vulkan->physicalDevice, deviceInfo};
        vulkan->graphicsQueue =
            vk::raii::Queue{vulkan->device, graphicsIndex, 0};
    }
    ~Device() = default;
    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;
};
