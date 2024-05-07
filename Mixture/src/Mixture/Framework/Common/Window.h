#pragma once

#include <string>

using uint32_t = unsigned int;

namespace Mixture::Window {
	class Window {
	public:
		struct Extent {
			uint32_t width;
			uint32_t height;
		};

		enum class Mode {
			Headless,
			Fullscreen,
			FullscreenBorderless,
			FullsreenStretch,
			Default
		};

		enum class Vsync {
			OFF, ON, Default
		};

		struct Properties {
			std::string title = "";
			Mode mode = Mode::Default;
			bool resizable = true;
			Vsync vsync = Vsync::Default;
			Extent extent = { 1280, 720 };
		};

		Window(const Properties& properties);

		virtual ~Window() = default;

		virtual bool shouldClose() = 0;
		virtual void processEvents();
		virtual void close() = 0;
		virtual float getDpiFactor() const = 0;
		virtual float getContentScaleFactor() const;

		Extent resize(const Extent& extent);

		const Extent& getExtent() const;
		Mode getWindowMode() const;
		inline const Properties& getProperties() const {
			return m_Properties;
		}

	protected:
		Properties m_Properties;
	};
}