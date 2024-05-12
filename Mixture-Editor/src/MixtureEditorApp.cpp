#include <Mixture.h>
#include <Mixture/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Mixture {
	class MixtureEditor : public Application {
	public:
		MixtureEditor() : Application("Mixture Editor") {
			pushLayer(new EditorLayer());
		}

		~MixtureEditor() {}
	};

	Application* CreateApplication() {
		return new MixtureEditor();
	}
}