#pragma once
#include "Hazel/Scene/Scene.h"
#include "Hazel/Core/Base.h"
#include "Hazel/Scene/Entity.h";


namespace Hazel {
	class SceneHierarchyPanel {
		

		public:

			SceneHierarchyPanel() = default;
			SceneHierarchyPanel(const Ref<Scene>& scene);
			void SetContext(const Ref<Scene>& context);

			void OnImGuiRender();

		private:
			void DrawEntityNode(Entity entity);
			void DrawComponents(Entity entity);


		private:
			Ref<Scene> m_Context;
			Entity m_SelectionContext; // the selected entity
	};
}
