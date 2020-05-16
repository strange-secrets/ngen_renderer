
#include "render_pass.h"
#include "vulkan_context.h"
#include "device.h"

namespace ngen::vulkan {
    const VkRenderPass RenderPass::kInvalidHandle = 0l;

    RenderPass::RenderPass()
    : m_handle(kInvalidHandle)
    , m_device(nullptr) {
    }

    RenderPass::~RenderPass() {
        dispose();
    }

    //! Destroys any resources allocated by this object.
    void RenderPass::dispose() {
        if (m_handle != kInvalidHandle) {
            vkDestroyRenderPass(m_device, m_handle, nullptr);

            m_handle = kInvalidHandle;
            m_device = nullptr;
        }
    }

    //! \brief  Prepares the object for use by the application.
    //! \param  context [in] - The vulkan context we will be interacting with.
    //! \returns <em>True</em> if the render pass was created successfully otherwise <em>false</em>.
    bool RenderPass::create(const VulkanContext &context) {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = context.getSwapChain().getImageFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        // TODO: Need to be able to specify sub-passes and color attachments
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(context.getDevice(), &renderPassInfo, nullptr, &m_handle) != VK_SUCCESS) {
            printf("Failed to create render pass");
            return false;
        }

        m_device = context.getDevice();
        return true;
    }

    //! \brief Begins the command recording using a command buffer contained within the supplied command pool.
    //! \param commandPool [in] - The command pool that contains the command buffer to be used.
    //! \param commandBuffer [in] - Index of the buffer to be used within the command pool.
    //! \param frameBuffer [in] - The frame buffer we will be rendering to.
    //! \returns <em>True</em> if the render pass began successfully otherwise <em>false</em>.
    bool RenderPass::begin(const VulkanContext &context, VkCommandBuffer commandBuffer, VkFramebuffer frameBuffer) {
        if (m_handle == kInvalidHandle || commandBuffer == VK_NULL_HANDLE || frameBuffer == VK_NULL_HANDLE) {
            return false;
        }

        // TODO: Clears, swap-chains, graphics pipelines etc. need to be configurable
        VkClearValue clearColor = {1.0f, 1.0f, 0.0f, 1.0f};

        // https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Command_buffers
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_handle;
        renderPassInfo.framebuffer = frameBuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = context.getSwapChain().getExtent();
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        return true;
    }

    //! \brief Informs the render pass that the recording of render commands has completed.
    void RenderPass::end(VkCommandBuffer commandBuffer) {
        vkCmdEndRenderPass(commandBuffer);
    }

    void RenderPass::draw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
        vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }
}
