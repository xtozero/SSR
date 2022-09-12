#pragma once

namespace rendercore
{
	class VertexShader;

	class FullScreenQuadVS
	{
	public:
		FullScreenQuadVS();
		VertexShader* Shader() { return m_shader; }

	private:
		VertexShader* m_shader = nullptr;
	};
}
