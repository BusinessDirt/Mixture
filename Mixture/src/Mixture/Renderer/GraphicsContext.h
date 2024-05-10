#pragma once

namespace Mixture {
	class GraphicsContext {
	public:
		virtual void init() = 0;
		virtual void swapBuffers() = 0;

		static Scope<GraphicsContext> create(void* window);
	};
}