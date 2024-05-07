#include "Window.h"

namespace Mixture::Window {
	Window::Window(const Properties& properties) : m_Properties{ properties } {}

	void Window::processEvents() {

	}

	Window::Extent Window::resize(const Extent& newExtent) {
		if (m_Properties.resizable) {
			m_Properties.extent.width = newExtent.width;
			m_Properties.extent.height = newExtent.height;
		}

		return m_Properties.extent;
	}

	const Window::Extent& Window::getExtent() const {
		return m_Properties.extent;
	}

	float Window::getContentScaleFactor() const {
		return 1.0f;
	}

	Window::Mode Window::getWindowMode() const {
		return m_Properties.mode;
	}
}