#include <iostream>
#include <cstdlib>
#include <chrono>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
import initVulkan;

template <typename T, typename... Types>
void initStep(Types &&...args)
{
    std::make_unique<T>(std::forward<Types>(args)...);
}

int main()
{
    try {    
        auto start = std::chrono::steady_clock::now();
        auto sdl = std::make_unique<SDLGuard>();
        auto vulkan = std::make_unique<VulkanGuard>();
        {
            initStep<Instance>(vulkan.get());
            initStep<DebugMessenger>(vulkan.get());
            initStep<Surface>(vulkan.get(), sdl->getWindow());
            initStep<PhysicalDevice>(vulkan.get());
        }
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration{end - start};
        std::cout << std::format("Runtime: {}\n", duration.count());
        sdl->run();
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}


 
