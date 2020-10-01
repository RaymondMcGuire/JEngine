#include "JEngine.h"

#include "imgui/imgui.h"

#include "../../src/Public/Platform/OpenGL/OpenGLShader.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "../../src/Public/Renderer/Texture.h"

class ExampleLayer : public JEngine::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		m_VertexArray.reset(JEngine::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.5f, 0.4f, 0.1f, 1.0f,
			0.5, -0.5f, 0.0f, 0.1f, 0.4f, 0.5f, 1.0f,
			0.0f, 0.5f, 0.0f, 0.2f, 0.1f, 0.4f, 1.0f
		};

		JEngine::Ref<JEngine::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(JEngine::VertexBuffer::Create(vertices, sizeof(vertices)));

		JEngine::BufferLayout layout = {
			{JEngine::ShaderDataType::Float3, "a_Position"},
			{JEngine::ShaderDataType::Float4, "a_Color"}
		};

		vertexBuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		JEngine::Ref<JEngine::IndexBuffer> indexBuffer;
		indexBuffer.reset(JEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;	
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0f);
			}
		)";

		m_Shader.reset(JEngine::Shader::Create(vertexSrc, fragmentSrc));

		//-----------------SQUARE PLANE--------------------
		m_SquareVertexArray.reset(JEngine::VertexArray::Create());

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
			-0.5, 0.5f, 0.0f, 0.0f, 1.0f
		};

		JEngine::Ref<JEngine::VertexBuffer> squareVertexBuffer;
		squareVertexBuffer.reset(JEngine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

		JEngine::BufferLayout squareLayout = {
			{JEngine::ShaderDataType::Float3, "a_Position"},
			{JEngine::ShaderDataType::Float2, "a_TexCoord"}
		};
		
		squareVertexBuffer->SetLayout(squareLayout);

		m_SquareVertexArray->AddVertexBuffer(squareVertexBuffer);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		JEngine::Ref<JEngine::IndexBuffer> squareIndexBuffer;
		squareIndexBuffer.reset(JEngine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVertexArray->SetIndexBuffer(squareIndexBuffer);

		std::string textureShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;
			
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string textureShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec2 v_TexCoord;
			uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture, v_TexCoord);
			}
		)";

		m_TextureShader.reset(JEngine::Shader::Create(textureShaderVertexSrc, textureShaderFragmentSrc));

		m_Texture = JEngine::Texture2D::Create("assets/textures/jengine_logo.png");
		m_TransparentEngineTexture = JEngine::Texture2D::Create("assets/textures/Engine_logo.png");

		std::dynamic_pointer_cast<JEngine::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<JEngine::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(const JEngine::Timestep& DeltaTime) override 
	{
		//JE_TRACE("Delta Time {0}, ms: {1}", DeltaTime.GetSeconds(), DeltaTime.GetMilliSeconds());

		if (JEngine::Input::IsKeyDown(JE_KEY_DOWN))
			m_CameraPosition.y -= m_CameraSpeed * DeltaTime;
		else if (JEngine::Input::IsKeyDown(JE_KEY_UP))
			m_CameraPosition.y += m_CameraSpeed * DeltaTime;
		
		if (JEngine::Input::IsKeyDown(JE_KEY_RIGHT))
			m_CameraPosition.x += m_CameraSpeed * DeltaTime;
		else if (JEngine::Input::IsKeyDown(JE_KEY_LEFT))
			m_CameraPosition.x -= m_CameraSpeed * DeltaTime;

		if (JEngine::Input::IsKeyDown(JE_KEY_D))
			m_CameraRotation += m_CameraRotateSpeed * DeltaTime;
		else if (JEngine::Input::IsKeyDown(JE_KEY_A))
			m_CameraRotation -= m_CameraRotateSpeed * DeltaTime;

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		// JE_INFO("Example Layer Updated!");
		// Clear Buffer
		JEngine::RenderCommand::SetClearColor({ 0.1, 0.1, 0.1, 1.0 });
		JEngine::RenderCommand::Clear();

		JEngine::Renderer::BeginScene(m_Camera);

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<JEngine::OpenGLShader>(m_Shader)->Bind();
		std::dynamic_pointer_cast<JEngine::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", m_ShaderColor);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 trans = glm::translate(glm::mat4(1.0f), pos) * scale;
				JEngine::Renderer::Submit(m_Shader, m_VertexArray, trans);
			}
		}

		m_Texture->Bind();
		JEngine::Renderer::Submit(m_TextureShader, m_SquareVertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_TransparentEngineTexture->Bind();
		JEngine::Renderer::Submit(m_TextureShader, m_SquareVertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		JEngine::Renderer::EndScene();
	}

	virtual void OnImguiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Shader Color", glm::value_ptr(m_ShaderColor));
		ImGui::End();
	}

	void OnEvent(JEngine::Event& e) override
	{
		JE_TRACE("{0}", e);
	}

private:
	JEngine::OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
	float m_CameraSpeed = 0.1f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotateSpeed = 0.2f;

	JEngine::Ref<JEngine::Shader> m_Shader, m_TextureShader;

	JEngine::Ref<JEngine::VertexArray> m_VertexArray;
	JEngine::Ref<JEngine::VertexArray> m_SquareVertexArray;

	JEngine::Ref<JEngine::Texture> m_Texture, m_TransparentEngineTexture;
	glm::vec3 m_ShaderColor = { 0.4f, 0.4f, 0.8f };
};

class Sandbox : public JEngine::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {

	}
};

JEngine::Application* JEngine::CreateApplication() {
	return new Sandbox();
}