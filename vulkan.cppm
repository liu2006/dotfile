module;
#include <iostream>
#include <SDL3/SDL_vulkan.h>
#include <memory>
#include <vulkan/vulkan_raii.hpp>
#include <SDL3/SDL.h>
export module initVulkan;

constexpr bool enableValidationLayers
{
#ifdef NDEBUG
    false
#else
    true
#endif
};
constexpr uint32_t WIDTH{1920};
constexpr uint32_t HEIGHT{1080};
const std::vector<const char *> validationLayers{"VK_LAYER_KHRONOS_validation"};

export class SDLGuard
{
private:
    SDL_Window *window;
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
    SDL_Window *getWindow() { return window; }
    SDLGuard()
    {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            throw std::runtime_error(std::format("Could not initialize SDL: {}\n", SDL_GetError()));
        }
        window = SDL_CreateWindow("vulkan", WIDTH, HEIGHT,
                                  SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE |
                                      SDL_WINDOW_HIDDEN);
        if (!window) {
            throw std::runtime_error(
                std::format("Could not create window: {}\n", SDL_GetError()));
        }
    }
    void run() { mainLoop(); }
    SDLGuard(const SDLGuard &) = delete;
    SDLGuard &operator=(const SDLGuard &) = delete;
    SDLGuard(SDLGuard &&) = delete;
    SDLGuard &operator=(SDLGuard &&) = delete;
    ~SDLGuard()
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

export class VulkanGuard
{
    friend class Instance;
    friend class DebugMessenger;
    friend class Surface;
private:
    vk::raii::Context context;
    vk::raii::Instance instance{nullptr};
    vk::raii::DebugUtilsMessengerEXT debugMessenger{nullptr};
    vk::raii::SurfaceKHR surface{nullptr};
public:    
    VulkanGuard() = default;
    VulkanGuard(const VulkanGuard &) = delete;
    VulkanGuard &operator=(const VulkanGuard &) = delete;
    ~VulkanGuard() = default; 
};

export class Instance 
{
private:
    std::vector<const char *> layers{};
    std::vector<vk::LayerProperties> layerProperties{};
    uint32_t sdlExtensionCount{};
    char const *const *sdlExtensions{
        SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount)};
    std::vector<const char *> extensions{sdlExtensions,
                                         sdlExtensions + sdlExtensionCount};
    std::vector<vk::ExtensionProperties> extensionProperties{};
    
public:
    explicit Instance(VulkanGuard *vulkan)
        : layerProperties{vulkan->context.enumerateInstanceLayerProperties()},
          extensionProperties(
              vulkan->context.enumerateInstanceExtensionProperties())
    {
        vk::ApplicationInfo appInfo{.pApplicationName = "Vulkan",
                                    .applicationVersion =
                                        VK_MAKE_VERSION(1, 0, 0),
                                    .pEngineName = "No engine",
                                    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                                    .apiVersion = vk::ApiVersion14};
        if (enableValidationLayers) {
            layers.assign(validationLayers.begin(), validationLayers.end());
            extensions.push_back(vk::EXTDebugUtilsExtensionName);
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
                "required layer not supported: {}", *unsupportedLayer));
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
                "required extension not supported: {}", *unsupportedExtension));
        }
    }
    Instance(const Instance &) = delete;
    Instance &operator=(const Instance &) = delete;
    ~Instance() = default;
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
        if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
            std::cout << std::format("validation layer: {}\n",
                                     pCallbackData->pMessage);
        return vk::False;
    }
    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags{
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose};
    vk::DebugUtilsMessageTypeFlagsEXT typeFlags{
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral};

public:
    DebugMessenger(VulkanGuard *vulkan)
    {
        if (!enableValidationLayers)
            return;
        vk::DebugUtilsMessengerCreateInfoEXT debugInfo{
            .messageSeverity = severityFlags,
            .messageType = typeFlags,
            .pfnUserCallback = &debugCallback};
        vulkan->debugMessenger = vk::raii::DebugUtilsMessengerEXT{vulkan->instance, debugInfo};
    }
    DebugMessenger(const DebugMessenger &) = delete;
    DebugMessenger &operator=(const DebugMessenger &) = delete;
    ~DebugMessenger() = default;
};

export class Surface
{
private:
    VkSurfaceKHR _surface;
public:
    Surface(VulkanGuard *vulkan, SDL_Window *window)
    {
        if (!SDL_Vulkan_CreateSurface(window, *vulkan->instance, nullptr,
                                     &_surface)) {
            throw std::runtime_error("Could not create surface");
        }
        vulkan->surface = vk::raii::SurfaceKHR{vulkan->instance, _surface};
    }
    
    Surface(const Surface &) = delete;
    Surface &operator=(const Surface &) = delete;
    ~Surface() = default;
};
