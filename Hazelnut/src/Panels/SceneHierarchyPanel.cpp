#include "hzpch.h"
#include "SceneHierarchyPanel.h"
#include <imgui/imgui.h>
#include "Hazel/Scene/Components.h"
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_internal.h>




namespace Hazel {

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		auto view = m_Context->m_Registry.view<TagComponent>();
		for (auto entityID : view) {
			Entity entity{ entityID, m_Context.get() };

			DrawEntityNode(entity);
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
			m_SelectionContext = {};
		}

		// Right-click on blank space
		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems)) {
			if (ImGui::MenuItem("Create Empty Entity")) {
				m_Context->CreateEntity("Empty Entity");
			}
			ImGui::EndPopup();
		}


		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext) {
			DrawComponents(m_SelectionContext);

			char* popupMenuId = "Add Component";
			if (ImGui::Button(popupMenuId)) {
				ImGui::OpenPopup(popupMenuId);
			}

			if (ImGui::BeginPopup(popupMenuId)) {
				if (ImGui::MenuItem("Camera")) {
					m_SelectionContext.AddComponent<CameraComponent>(); 
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Sprite Renderer")) {
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_SelectionContext = entity;
		}

		// Right-click on entity
		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem(0, 1 | ImGuiPopupFlags_NoOpenOverItems)) {
			if (ImGui::MenuItem("Delete Entity")) {
				//m_Context->DestroyEntity(entity);
				entityDeleted = true; // flag it as deleted and deal with it at end of method
			}
			ImGui::EndPopup();
		}


		if (opened) {
			ImGui::TreePop();
		}

		if (entityDeleted) {
			HZ_INFO("Deleting entity: {0}", entity.GetComponent<TagComponent>().Tag.c_str());
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity) {
				m_SelectionContext = {};
			}
		}
		//ImGui::Text("%s", tagComponent.Tag.c_str());
	}

	ImVec4 operator+(const ImVec4& a, const ImVec4& b) {
		return ImVec4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
	}
	
	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100) {
		ImGui::PushID(label.c_str());


		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
		int32_t min = 0.0f;
		int32_t max = 0.0f;

		ImVec4 red = ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f };
		ImVec4 green = ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f };
		ImVec4 blue = ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f };
		ImVec4 lighter = ImVec4{ 0.1f, 0.1f, 0.1f, 0.0f };

		char* decimalFormat = "%.2f";
		float changeDelta = 0.1f;

		ImGui::PushStyleColor(ImGuiCol_Button, red);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, red + lighter);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, red);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, changeDelta, min, max, decimalFormat);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, green);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, green + lighter);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, green);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, changeDelta, min, max, decimalFormat);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, blue);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, blue + lighter);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, blue);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, changeDelta, min, max, decimalFormat);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

	}
	

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>()) {
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

		if (entity.HasComponent<TransformComponent>()) {
			bool open = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), treeNodeFlags, "Transform");
			if (open) {

				auto& transformComp = entity.GetComponent<TransformComponent>();
				//ImGui::DragFloat3("Position", glm::value_ptr(transformComp.Translation), 0.1f);
				DrawVec3Control("Translation", transformComp.Translation);

				glm::vec3 rotation = glm::degrees(transformComp.Rotation);
				DrawVec3Control("Rotation", rotation);
				transformComp.Rotation = glm::radians(rotation);

				DrawVec3Control("Scale", transformComp.Scale, 1.0f);
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<CameraComponent>()) {

			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), treeNodeFlags, "Camera")) {

				auto& cameraComponent = entity.GetComponent<CameraComponent>();
				auto& camera = cameraComponent.Camera;

				ImGui::Checkbox("Primary", &cameraComponent.Primary);

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)cameraComponent.Camera.GetProjectionType()];
				if( ImGui::BeginCombo("Projection", currentProjectionTypeString)){
					for (int i = 0; i < 2; i++) {
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
					float verticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat("Vertical FOV", &verticalFov)) {
						camera.SetPerspectiveVerticalFOV(glm::radians(verticalFov));
					}

					float perspNear = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &perspNear)) {
						camera.SetPerspectiveNearClip(perspNear);
					}

					float perspFar = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &perspFar)) {
						camera.SetPerspectiveFarClip(perspFar);
					}
				}
				else if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize)) {
						camera.SetOrthographicSize(orthoSize);
					}
					
					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear)) {
						camera.SetOrthographicNearClip(orthoNear);
					}
					
					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar)) {
						camera.SetOrthographicFarClip(orthoFar);
					}

					ImGui::Checkbox("FixedAspectRatio", &cameraComponent.FixedAspectRatio);
				}

				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<SpriteRendererComponent>()) {
			bool open = ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), treeNodeFlags, "Sprite Renderer");
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
			ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
			char* compSettingsPopupId = "ComponentSettings";
			if (ImGui::Button("+", ImVec2{ 20,20 })) {
				ImGui::OpenPopup(compSettingsPopupId);
			}
			ImGui::PopStyleVar();

			bool removeComponent = false;
			if (ImGui::BeginPopup(compSettingsPopupId)) {
				if (ImGui::MenuItem("Remove component")) {
					removeComponent = true;
				}
				ImGui::EndPopup();
			}

			if (open) {
				auto& srComp = entity.GetComponent<SpriteRendererComponent>();
				
				ImGui::ColorEdit4("Color", glm::value_ptr(srComp.Color));
				ImGui::TreePop();
			}

			if (removeComponent) {
				entity.RemoveComponent<SpriteRendererComponent>();
			}
		}

	}

}
