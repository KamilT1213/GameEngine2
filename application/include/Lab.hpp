#pragma once
#include <DemonRenderer.hpp>
#include "Grid.hpp"

//bool operator<(glm::vec3 a, glm::vec3 b) {
//	return (glm::distance(glm::vec3(0, 0, 0), a) < glm::distance(glm::vec3(0, 0, 0), b));
//}

class Scene0 : public Layer
{
public:
	Scene0(GLFWWindowImpl& win);

private:
	void onRender() const override;
	void onUpdate(float timestep) override;
	void onKeyPressed(KeyPressedEvent& e) override;
	void onImGUIRender() override;
private:
	std::shared_ptr<Scene> m_mainScene;
	std::shared_ptr<Scene> m_screenScene;
	std::shared_ptr<Scene> m_compositeScreenScene;
	std::shared_ptr<Scene> m_screenCubeMapScene;
	std::shared_ptr<Scene> m_antialiasingScreenScene;
	std::shared_ptr<Scene> m_collisionScene;
	std::shared_ptr<Scene> m_terrainParticlesScene;
	//std::shared_ptr<Scene> m_lightScene;

	float width = m_winRef.getWidthf();
	float height = m_winRef.getHeightf();

	float allTime{ 0 };
	float rippleTime{ 0 };
	float waterEaseIn{ 0.0f };
	float waterEaseOut{ 0.0f };
	glm::vec2 lastWaterPos{ 0,0 };
	bool b{true};

	unsigned int colW = 3;
	unsigned int colH = colW;

	ImVec2 imageSize = ImVec2(width / 3, height / 3);
	ImVec2 uv0 = ImVec2(0.0f, 1.0f);
	ImVec2 uv1 = ImVec2(1.0f, 0.0f);
	ImVec2 relMousePos;
	bool mouseDown{ false };
	bool resetTexture{ true };
	float m_brushMode{ 1 };
	glm::vec3 penColor{ 1,0,0 };
	float m_size{ 200.0f };

	glm::vec3 Velocity{ 0,0,0 };
	bool onFloor{ false };
	bool editLandScape{ false };
	bool editLandScapeToggle{ false };

	float BezierProgress{ 0 };
	
	std::shared_ptr<SSBO> terrainParticlesStartPoints;
	std::shared_ptr<SSBO> terrainParticles;
	std::shared_ptr<SSBO> bezierLookUpTable;

	struct Particle {
		glm::vec4 Position;
		glm::vec2 UV;
	};

	struct BezierTable {
		float LocalDistance;
		float Tvalue;
	};

	glm::vec3 worldPosRayTarget{ 0.0f };
	glm::vec3 worldPosRayStart{ 0.0f };


	glm::vec3 cameraForward{ 0,0,1 };

	Renderer m_mainRenderer;
	Renderer m_computeRenderer;
	Renderer m_terrainUpdateRenderer;
	Renderer m_terrainSpells;
	Renderer m_bezierApproximation;
	// Actor positions for ease of use and to avoid additonal magic numbers
	size_t modelIdx;
	size_t AcubeIdx;
	size_t BcubeIdx;
	size_t floorIdx;
	size_t particleFloorIdx;
	size_t grassIdx;
	size_t cameraIdx;
	size_t characterIdx;
	size_t skyboxIdx;
	size_t screencubeIdx;
	size_t quadIdx;
	size_t aaQuadIdx;
	size_t particlesIdx;

	GLuint textureID;
	GLuint waterTextureID;

	size_t mainPassIdx;
	size_t playerColPassIdx;
	size_t shadowPassIdx;
	size_t terrainComputePassIdx;
	size_t terrainNormalsComputePassIdx;	
	size_t waterComputePassIdx;
	size_t waterNormalsComputePassIdx;

	bool m_wireFrame{ false };

	//Gui
	glm::vec3 m_colour{ 1.0 };
	glm::vec3 m_floorColour{ 1.0 };
	float m_Factor{ 10.0f };
	float m_Factor2{ 0.0f };
	int m_Toggle{ 0 };
	glm::vec2 m_ScreenSize{ width , height };
	float m_emission = 1.0f;
	int Buffer{ 0 };
	glm::vec3 LightConsts{ 1.0f, 0.7f, 0.02f };
	glm::vec3 m_HSVcolours{ 0.5f };

	FBOLayout typicalLayout = {
		{AttachmentType::ColourHDR, true},
		{AttachmentType::ColourHDR, true},
		{AttachmentType::ColourHDR, true},
		{AttachmentType::ColourHDR, true},
		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, true}
	};

	FBOLayout colLayout = {
		{AttachmentType::ColourHDR, true},
		{AttachmentType::ColourHDR, true},
		{AttachmentType::ColourHDR, true},
		{AttachmentType::ColourHDR, true},
		{AttachmentType::Depth, false}
	};

	FBOLayout compositeLayout = {
		{AttachmentType::ColourHDR, true}
	};

	FBOLayout antialiasingLayout = {
		{AttachmentType::ColourHDR,true}
	};

	FBOLayout screenCubeMapLayout = {
		{AttachmentType::ColourHDR, true}
	};

	FBOLayout prePassLayout = {
		{AttachmentType::Depth, true}
	};

	FBOLayout shadowLayout = {
		{AttachmentType::Depth, true}
	};

	VBOLayout depthLayout = { {GL_FLOAT, 3} };

	struct ShadowMapVars {
		glm::vec3 centre;
		float distanceAlongLightVector;
		float orthoSize;
		const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);

		ShadowMapVars(glm::vec3 c = glm::vec3(0.0), float dist = 60.0f) : centre(c), distanceAlongLightVector(dist) {
			orthoSize = distanceAlongLightVector * 0.75f;
		};
	};

	std::vector<float> skyboxVertices = {
		// positions          
		-100.f,  100.f, -100.f,
		-100.f, -100.f, -100.f,
		 100.f, -100.f, -100.f,
		 100.f, -100.f, -100.f,
		 100.f,  100.f, -100.f,
		-100.f,  100.f, -100.f,

		-100.f, -100.f,  100.f,
		-100.f, -100.f, -100.f,
		-100.f,  100.f, -100.f,
		-100.f,  100.f, -100.f,
		-100.f,  100.f,  100.f,
		-100.f, -100.f,  100.f,

		 100.f, -100.f, -100.f,
		 100.f, -100.f,  100.f,
		 100.f,  100.f,  100.f,
		 100.f,  100.f,  100.f,
		 100.f,  100.f, -100.f,
		 100.f, -100.f, -100.f,

		-100.f, -100.f,  100.f,
		-100.f,  100.f,  100.f,
		 100.f,  100.f,  100.f,
		 100.f,  100.f,  100.f,
		 100.f, -100.f,  100.f,
		-100.f, -100.f,  100.f,

		-100.f,  100.f, -100.f,
		 100.f,  100.f, -100.f,
		 100.f,  100.f,  100.f,
		 100.f,  100.f,  100.f,
		-100.f,  100.f,  100.f,
		-100.f,  100.f, -100.f,

		-100.f, -100.f, -100.f,
		-100.f, -100.f,  100.f,
		 100.f, -100.f, -100.f,
		 100.f, -100.f, -100.f,
		-100.f, -100.f,  100.f,
		 100.f, -100.f,  100.f
	};

	std::vector<float> billbPositions;

	const std::vector<float> screenVertices = {
		//Position               UV
	0.0f,  0.0f,   0.0f,     0.0f, 1.0f, // Bottom Left
	width, 0.0f,   0.0f,     1.0f, 1.0f, // Bottom Right
	width, height, 0.0f,     1.0f, 0.0f, // Top Right
	0.0f,  height, 0.0f,     0.0f, 0.0f  // Top Left
	};

	const std::vector<uint32_t> screenIndices = { 0,1,2,2,3,0 };

	glm::vec3 modulateVectors(glm::vec3 a, glm::vec3 b) {
		return glm::vec3(modulateFloats(a.x, b.x), modulateFloats(a.y, b.y), modulateFloats(a.z, b.z));
	}

	float modulateFloats(float a, float b) {
		return a - b * floorf(a / b);
	}

	/*glm::vec3 operator%(glm::vec3 a, glm::vec3 b) {
		return glm::mod(a, b);
	}*/
};

