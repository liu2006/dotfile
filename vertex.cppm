module;
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>
export module vertexInput;

export class Vertex
{
private:
    glm::vec2 position;
    glm::vec3 color;

public:
    static vk::VertexInputBindingDescription getBindingDescription()
    {
        return {0, sizeof(Vertex), vk::VertexInputRate::eVertex};
    }
    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions()
    {
        return {vk::VertexInputAttributeDescription{.location = 0,
                                                    .binding = 0,
                                                    .format = vk::Format::eR32G32Sfloat,
                                                    .offset = offsetof(Vertex, position)},
                vk::VertexInputAttributeDescription{.location = 1,
                                                    .binding = 0,
                                                    .format = vk::Format::eR32G32B32Sfloat,
                                                    .offset = offsetof(Vertex, color)}};
    }
    Vertex(glm::vec2 position, glm::vec3 color) : position{position}, color{color} {}
    ~Vertex() = default;
};

export const std::vector<Vertex> vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                             {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                             {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                             {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

export const std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
