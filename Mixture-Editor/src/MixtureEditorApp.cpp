#include <Mixture.h>
#include <Mixture/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Mixture {
	class MixtureEditor : public Application {
	public:
		MixtureEditor(ApplicationCommandLineArgs args) : Application("Mixture Editor", args) {
			pushLayer(new EditorLayer());
		}

		~MixtureEditor() {}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args) {
		return new MixtureEditor(args);
	}
}
