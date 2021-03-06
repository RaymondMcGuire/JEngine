#include "JE_PCH.h"
#include "Renderer/VertexArray.h"

#include "Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace JEngine
{
	
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetRenderPlatform())
		{
			case RendererAPI::RenderPlatform::None: JE_CORE_ASSERT(false, "RendererAPI::None is not suppported!"); return nullptr;
			case RendererAPI::RenderPlatform::OpenGL: return std::make_shared<OpenGLVertexArray>();
		}

		JE_CORE_ASSERT(false, "Unknow RendererAPI!");
		return nullptr;
	}

}