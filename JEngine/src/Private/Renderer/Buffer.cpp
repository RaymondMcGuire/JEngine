#include "JE_PCH.h"
#include "Renderer/Buffer.h"

#include "Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace JEngine
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetRenderPlatform())
		{
		case RendererAPI::RenderPlatform::None:
			JE_CORE_ASSERT(false, "Renderer API is not Supported");
			return nullptr;
		case RendererAPI::RenderPlatform::OpenGL:
			return new OpenGLVertexBuffer(vertices, size);
		}

		JE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* vertices, uint32_t count)
	{
		switch (Renderer::GetRenderPlatform())
		{
		case RendererAPI::RenderPlatform::None:
			JE_CORE_ASSERT(false, "Renderer API is not Supported");
			return nullptr;
		case RendererAPI::RenderPlatform::OpenGL:
			return new OpenGLIndexBuffer(vertices, count);
		}

		JE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}

