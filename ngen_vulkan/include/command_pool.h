
#ifndef NGEN_VULKAN_COMMAND_POOL
#define NGEN_VULKAN_COMMAND_POOL

////////////////////////////////////////////////////////////////////////////

#include <vector>
#include "platform.h"


////////////////////////////////////////////////////////////////////////////

namespace ngen::vulkan {
    class Device;
    class PhysicalDevice;

    class CommandPool {
    public:
        CommandPool();
        ~CommandPool();

        CommandPool(const CommandPool &other) = delete;
        CommandPool& operator=(const CommandPool &other) = delete;

        void dispose();
        [[nodiscard]] bool create(const Device &device, const PhysicalDevice &physicalDevice, VkQueueFlags queueFlags);
        [[nodiscard]] bool create(const Device &device, const PhysicalDevice &physicalDevice, VkQueueFlags queueFlags, int flags);

        [[nodiscard]] bool allocateCommandBuffers(size_t count);

        [[nodiscard]] VkCommandBuffer begin(size_t index) const;

        [[nodiscard]] size_t size() const;

        operator VkCommandPool() const;  // NOLINT

    private:
        std::vector<VkCommandBuffer> m_commandBuffers;

        VkDevice m_device;
        VkCommandPool m_handle;
    };

    //! \brief Retrieves the number of command buffer contained within the pool.
    //! \returns The number of command buffers contained within the pool.
    inline size_t CommandPool::size() const {
        return m_commandBuffers.size();
    }

    inline CommandPool::operator VkCommandPool() const {
        return m_handle;
    }
}

////////////////////////////////////////////////////////////////////////////

#endif //NGEN_VULKAN_COMMAND_POOL
