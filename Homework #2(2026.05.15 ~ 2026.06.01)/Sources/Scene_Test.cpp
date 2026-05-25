#include "Precompiled.h"
#include "Scene_Test.h"

#include "Application.h"
#include "Button.h"
#include "Camera.h"
#include "CameraController.h"
#include "Font.h"
#include "ImageView.h"
#include "Light.h"
#include "MeshRenderer.h"
#include "PlayerController.h"
#include "RectTransform.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "TextView.h"
#include "Transform.h"
#include "Vector2D.h"

namespace
{
	ColorRGBA GetFallbackColor(std::uint32_t index_)
	{
		switch (index_ % 8u)
		{
		case 0: return ColorRGBA(ColorRGB::GetRed(), 1.0f);
		case 1: return ColorRGBA(ColorRGB::GetGreen(), 1.0f);
		case 2: return ColorRGBA(ColorRGB::GetBlue(), 1.0f);
		case 3: return ColorRGBA(ColorRGB::GetYellow(), 1.0f);
		case 4: return ColorRGBA(ColorRGB::GetCyan(), 1.0f);
		case 5: return ColorRGBA(ColorRGB::GetMagenta(), 1.0f);
		case 6: return ColorRGBA(ColorRGB(1.0f, 0.5f, 0.2f), 1.0f);
		default: return ColorRGBA(ColorRGB(0.8f, 0.8f, 0.8f), 1.0f);
		}
	}

	bool ReadSectionName(const std::vector<std::byte>& bytes_, std::size_t& offset_, std::string& name_)
	{
		if (offset_ >= bytes_.size())
		{
			return false;
		}

		const std::size_t length{ std::to_integer<std::size_t>(bytes_[offset_]) };
		++offset_;

		if (offset_ + length > bytes_.size())
		{
			return false;
		}

		name_.assign(reinterpret_cast<const char*>(bytes_.data() + offset_), length);
		offset_ += length;
		return true;
	}

	template <typename T>
	bool ReadValue(const std::vector<std::byte>& bytes_, std::size_t& offset_, T& value_)
	{
		if (offset_ + sizeof(T) > bytes_.size())
		{
			return false;
		}

		std::memcpy(&value_, bytes_.data() + offset_, sizeof(T));
		offset_ += sizeof(T);
		return true;
	}

	bool ReadString(const std::vector<std::byte>& bytes_, std::size_t& offset_, std::string& text_)
	{
		std::uint8_t length{ 0 };
		if (!ReadValue(bytes_, offset_, length))
		{
			return false;
		}

		if (offset_ + length > bytes_.size())
		{
			return false;
		}

		text_.assign(reinterpret_cast<const char*>(bytes_.data() + offset_), length);
		offset_ += length;
		return true;
	}
}

bool Scene_Test::LoadResources(ID3D12Device* device_)
{
	if (nullptr == device_)
	{
		return false;
	}

	testMesh = std::make_shared<Mesh>();
	if (!testMesh->LoadFromBinary(device_, "Resources/Models/Cube.bin"))
	{
		return false;
	}
	testMesh->SetId(1);

	stairsMesh = std::make_shared<Mesh>();
	if (!stairsMesh->LoadFromBinary(device_, "Resources/Models/Stairs.bin"))
	{
		return false;
	}
	stairsMesh->SetId(2);

	playerMesh = std::make_shared<Mesh>();
	if (!playerMesh->LoadFromBinary(device_, "Resources/Models/Player.bin"))
	{
		return false;
	}
	playerMesh->SetId(3);

	const Mesh::Vertex uiVertices[]
	{
		{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
		{ { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
	};
	const std::uint32_t uiIndices[]{ 0u, 1u, 2u, 0u, 2u, 3u };

	uiQuadMesh = std::make_shared<Mesh>();
	if (!uiQuadMesh->BuildFromRaw(device_, uiVertices, uiIndices))
	{
		return false;
	}
	uiQuadMesh->SetId(4);

	testShader = std::make_shared<Shader>();
	if (!testShader->LoadFromFile(device_, L"Resources/Shaders/GameObject.hlsl"))
	{
		return false;
	}
	testShader->SetPipelineId(1);

	uiShader = std::make_shared<Shader>();
	if (!uiShader->LoadFromFile(device_, L"Resources/Shaders/UIObject.hlsl"))
	{
		return false;
	}
	uiShader->SetPipelineId(2);

	cubeMaterial = std::make_shared<Material>();
	cubeMaterial->SetId(1);
	cubeMaterial->SetColor(ColorRGBA(ColorRGB(0.25f, 0.65f, 1.0f), 1.0f));
	cubeMaterial->SetShader(testShader.get());

	stairsMaterial = std::make_shared<Material>();
	stairsMaterial->SetId(2);
	stairsMaterial->SetColor(ColorRGBA(ColorRGB(1.0f, 0.55f, 0.2f), 1.0f));
	stairsMaterial->SetShader(testShader.get());

	playerMaterial = std::make_shared<Material>();
	playerMaterial->SetId(3);
	playerMaterial->SetColor(ColorRGBA(ColorRGB(0.2f, 1.0f, 0.35f), 1.0f));
	playerMaterial->SetShader(testShader.get());

	uiMaterial = std::make_shared<Material>();
	uiMaterial->SetId(4);
	uiMaterial->SetColor(ColorRGBA(ColorRGB(0.2f, 0.2f, 0.2f), 1.0f));
	uiMaterial->SetShader(uiShader.get());

	uiFont = std::make_shared<Font>();
	uiFont->Load({});
	uiFont->SetFamilyName(L"Segoe UI");
	uiFont->SetSize(28);
	uiFont->SetWeight(FW_BOLD);

	return true;
}

void Scene_Test::OnLoad()
{
	LoadResources(Application::GetRenderer().GetDevice());

	GameObject& cameraObject{ CreateGameObject("Main Camera") };
	Transform* cameraTransform{ cameraObject.AddComponent<Transform>() };
	Camera* camera{ cameraObject.AddComponent<Camera>() };
	CameraController* cameraController{ cameraObject.AddComponent<CameraController>() };

	cameraTransform->SetLocalPosition(Vector3D(0.0f, 0.0f, -10.0f));
	camera->SetProjection(Camera::Projection::Persprective);

	GameObject& lightObject{ CreateGameObject("Main Light") };
	lightObject.AddComponent<Transform>();
	Light* light{ lightObject.AddComponent<Light>() };
	light->SetType(Light::Type::Directional);
	light->SetColor(ColorRGB::GetWhite());
	light->SetIntensity(1.0f);
	light->SetDirection(Vector3D(0.2f, -1.0f, 0.3f));
	AddLight(light);

	std::ifstream file{ "Resources/Scenes/Scene.bin", std::ios::binary };
	if (!file)
	{
		GameObject& meshObject{ CreateGameObject("Test Mesh") };
		meshObject.AddComponent<Transform>();

		MeshRenderer* meshRenderer{ meshObject.AddComponent<MeshRenderer>() };
		meshRenderer->SetMesh(testMesh.get());
		meshRenderer->SetMaterial(cubeMaterial.get());

		PlayerController* playerController{ meshObject.AddComponent<PlayerController>() };
		playerController->SetRotationSpeed(90.0f);
		return;
	}

	std::vector<char> rawBytes{
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	};
	std::vector<std::byte> bytes(rawBytes.size());
	std::memcpy(bytes.data(), rawBytes.data(), rawBytes.size());

	std::size_t offset{ 0 };
	std::string sectionName;
	if (!ReadSectionName(bytes, offset, sectionName) || "<GameObjects>:" != sectionName)
	{
		return;
	}

	std::uint32_t objectCount{ 0 };
	if (!ReadValue(bytes, offset, objectCount))
	{
		return;
	}

	for (std::uint32_t objectIndex{ 0 }; objectIndex < objectCount; ++objectIndex)
	{
		if (!ReadSectionName(bytes, offset, sectionName) || "<GameObject>:" != sectionName)
		{
			break;
		}

		std::string objectName;
		if (!ReadString(bytes, offset, objectName))
		{
			break;
		}

		Matrix4x4 worldMatrix{};
		if (!ReadValue(bytes, offset, worldMatrix))
		{
			break;
		}

		std::string meshMetadataName;

		// Optional metadata sections.
		if (offset < bytes.size())
		{
			while (offset < bytes.size())
			{
				const std::size_t probeOffset{ offset };
				std::string nextSection;
				if (!ReadSectionName(bytes, offset, nextSection))
				{
					offset = probeOffset;
					break;
				}

				if ("<GameObject>:" == nextSection)
				{
					offset = probeOffset;
					break;
				}

				if ("<Mesh>:" == nextSection)
				{
					if (!ReadString(bytes, offset, meshMetadataName))
					{
						break;
					}
					continue;
				}

				if ("<Color>:" == nextSection || "<AlbedoColor>:" == nextSection)
				{
					ColorRGBA parsedColor{};
					if (!ReadValue(bytes, offset, parsedColor))
					{
						break;
					}
					continue;
				}

				offset = probeOffset;
				break;
			}
		}

		GameObject& gameObject{ CreateGameObject(objectName.empty() ? "GameObject" : objectName) };
		Transform* transform{ gameObject.AddComponent<Transform>() };
		MeshRenderer* meshRenderer{ gameObject.AddComponent<MeshRenderer>() };

		Mesh* selectedMesh{ testMesh.get() };
		Material* selectedMaterial{ cubeMaterial.get() };
		if (objectName == "Stairs" || meshMetadataName.find("Stairs") != std::string::npos)
		{
			selectedMesh = stairsMesh.get();
			selectedMaterial = stairsMaterial.get();
		}
		meshRenderer->SetMesh(selectedMesh);
		meshRenderer->SetMaterial(selectedMaterial);

		transform->SetLocalPosition(worldMatrix.GetWorldPosition());
		transform->SetLocalRotation(worldMatrix.GetRotation());
		transform->SetLocalScale(worldMatrix.GetScale());

	}

	GameObject& playerObject{ CreateGameObject("Player") };
	Transform* playerTransform{ playerObject.AddComponent<Transform>() };
	MeshRenderer* playerMeshRenderer{ playerObject.AddComponent<MeshRenderer>() };

	playerMeshRenderer->SetMesh(playerMesh.get());
	playerMeshRenderer->SetMaterial(playerMaterial.get());

	playerTransform->SetLocalPosition(Vector3D(0.0f, 0.5f, 0.0f));

	PlayerController* playerController{ playerObject.AddComponent<PlayerController>() };
	playerController->SetRotationSpeed(90.0f);
	playerController->SetMoveSpeed(8.0f);

	if (nullptr != cameraController)
	{
		if (Transform* targetTransform{ playerObject.GetComponent<Transform>() })
		{
			cameraController->SetTarget(targetTransform);
		}
	}

	GameObject& buttonObject{ CreateGameObject("UI_Button") };
	RectTransform* buttonRect{ buttonObject.AddComponent<RectTransform>() };
	buttonRect->SetAnchoredPosition(Vector2D(0.0f, -260.0f));
	buttonRect->SetSize(Vector2D(260.0f, 72.0f));

	ImageView* buttonImage{ buttonObject.AddComponent<ImageView>() };
	buttonImage->SetMesh(uiQuadMesh.get());
	buttonImage->SetMaterial(uiMaterial.get());
	buttonImage->SetColor(ColorRGBA(ColorRGB(0.22f, 0.22f, 0.22f), 1.0f));

	Button* button{ buttonObject.AddComponent<Button>() };
	button->SetOnNormal(
		[buttonImage](Button&)
		{
			buttonImage->SetColor(ColorRGBA(ColorRGB(0.22f, 0.22f, 0.22f), 1.0f));
		});
	button->SetOnHighlighted(
		[buttonImage](Button&)
		{
			buttonImage->SetColor(ColorRGBA(ColorRGB(0.35f, 0.35f, 0.35f), 1.0f));
		});
	button->SetOnPressed(
		[buttonImage](Button&)
		{
			buttonImage->SetColor(ColorRGBA(ColorRGB(0.12f, 0.12f, 0.12f), 1.0f));
			SceneManager::LoadScene(L"Title");
		});
	button->SetOnSelected(
		[buttonImage](Button&)
		{
			buttonImage->SetColor(ColorRGBA(ColorRGB(0.25f, 0.32f, 0.55f), 1.0f));
		});
	button->SetOnDisabled(
		[buttonImage](Button&)
		{
			buttonImage->SetColor(ColorRGBA(ColorRGB(0.08f, 0.08f, 0.08f), 0.65f));
		});

	GameObject& textObject{ CreateGameObject("UI_ButtonText") };
	RectTransform* textRect{ textObject.AddComponent<RectTransform>() };
	textRect->SetAnchoredPosition(Vector2D(0.0f, -260.0f));
	textRect->SetSize(Vector2D(260.0f, 72.0f));

	TextView* textView{ textObject.AddComponent<TextView>() };
	textView->SetText(L"Start");
	textView->SetFont(uiFont.get());
	textView->SetMesh(uiQuadMesh.get());
	textView->SetMaterial(uiMaterial.get());
	textView->SetColor(ColorRGBA::GetWhite());
}
