#include "mxpch.h"
#include "ImGuiLayer.h"

#include <imgui.h>
#include "Mixture/Platform/OpenGL/ImGuiOpenGLRenderer.h"

#include "Mixture/Application.h"

// TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Mixture {

#define IMGUI_GETIO() ImGuiIO& io = ::ImGui::GetIO()

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}
	ImGuiLayer::~ImGuiLayer() {}

	void ImGuiLayer::onAttach() {
		::ImGui::CreateContext();
		::ImGui::StyleColorsDark();

		ImGuiIO& io = ::ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		// TEMPORARY
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::onDetach() {

	}

	void ImGuiLayer::onUpdate() {
		ImGuiIO& io = ::ImGui::GetIO();
		Application& app = Application::get();
		io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
		m_Time = time;

		ImGui_ImplOpenGL3_NewFrame();
		::ImGui::NewFrame();

		static bool show = true;
		::ImGui::ShowDemoWindow(&show);

		::ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
	}

#define IMGUI_LAYER_DISPATCH_EVENT(e) dispatcher.dispatch<Events::##e>(MX_BIND_EVENT_FN(ImGuiLayer::on##e))
	void ImGuiLayer::onEvent(Events::Event& event) {
		Events::EventDispatcher dispatcher(event);
		IMGUI_LAYER_DISPATCH_EVENT(MouseButtonPressedEvent);
		IMGUI_LAYER_DISPATCH_EVENT(MouseButtonReleasedEvent);
		IMGUI_LAYER_DISPATCH_EVENT(MouseMovedEvent);
		IMGUI_LAYER_DISPATCH_EVENT(MouseScrolledEvent);
		IMGUI_LAYER_DISPATCH_EVENT(KeyPressedEvent);
		IMGUI_LAYER_DISPATCH_EVENT(KeyReleasedEvent);
		IMGUI_LAYER_DISPATCH_EVENT(KeyTypedEvent);
		IMGUI_LAYER_DISPATCH_EVENT(WindowResizeEvent);
	}
	
	bool ImGuiLayer::onMouseButtonPressedEvent(Events::MouseButtonPressedEvent& e) {
		IMGUI_GETIO();
		io.MouseDown[e.getMouseButton()] = true;

		return false;
	}

	bool ImGuiLayer::onMouseButtonReleasedEvent(Events::MouseButtonReleasedEvent& e) {
		IMGUI_GETIO();
		io.MouseDown[e.getMouseButton()] = false;

		return false;
	}

	bool ImGuiLayer::onMouseMovedEvent(Events::MouseMovedEvent& e) {
		IMGUI_GETIO();
		io.MousePos = ImVec2(e.getX(), e.getY());

		return false;
	}

	bool ImGuiLayer::onMouseScrolledEvent(Events::MouseScrolledEvent& e) {
		IMGUI_GETIO();
		io.MouseWheelH += e.getXOffset();
		io.MouseWheel += e.getYOffset();

		return false;
	}

	bool ImGuiLayer::onKeyPressedEvent(Events::KeyPressedEvent& e) {
		IMGUI_GETIO();
		io.KeysDown[e.getKeyCode()] = true;

		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

		return false;
	}

	bool ImGuiLayer::onKeyReleasedEvent(Events::KeyReleasedEvent& e) {
		IMGUI_GETIO();
		io.KeysDown[e.getKeyCode()] = false;

		return false;
	}

	bool ImGuiLayer::onKeyTypedEvent(Events::KeyTypedEvent& e) {
		IMGUI_GETIO();
		int keycode = e.getKeyCode();
		if (keycode > 0 && keycode < 0x10000)
			io.AddInputCharacter((unsigned short)keycode);

		return false;
	}

	bool ImGuiLayer::onWindowResizeEvent(Events::WindowResizeEvent& e) {
		IMGUI_GETIO();
		io.DisplaySize = ImVec2(e.getWidth(), e.getHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		glViewport(0, 0, e.getWidth(), e.getHeight());
		
		return false;
	}
}