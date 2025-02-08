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
	std::shared_ptr<Scene> m_RelicScene;
	std::shared_ptr<Scene> m_screenScene;

	SoundManager m_soundManager;
	
	float width = m_winRef.getWidthf();
	float height = m_winRef.getHeightf();

	glm::vec2 m_ScreenSize = glm::vec2(m_winRef.getSizef());
	glm::vec2 m_PointerPos = glm::vec2(0.5);
	glm::vec2 m_DigPos = glm::vec2(0.5);

	bool b{true};
	bool focusMode{ false };
	bool modeToggle{ false };
	bool Reseting{ false };
	bool Pressed{ false };
	bool finished{ false };
	bool extrBegan{ false };
	float ProgressSegmentTarget{ 0.0f };

	ImVec2 imageSize = ImVec2(width / 3, height / 3);
	ImVec2 uv0 = ImVec2(0.0f, 1.0f);
	ImVec2 uv1 = ImVec2(1.0f, 0.0f);

	glm::vec2 gameMouseLocation{ 0,0 };
	
	float ProgressBar{0.0f};
	float allTime{ 0.0f };
	float factor{ 1.0f };
	float ResetWave{ 1.0f };
	float Subby{ 0.3f };

	size_t GroundComputePassIDx;
	size_t GroundNormalComputePassIDx;
	size_t ScreenGroundPassIDx;
	size_t ScreenRelicPassIDx;
	size_t AAPassIDx;
	
	//std::shared_ptr<SSBO> terrainParticlesStartPoints;
	//std::shared_ptr<SSBO> terrainParticles;


	/*struct Particle {
		glm::vec4 Position;
		glm::vec2 UV;
	};*/

	Renderer m_mainRenderer;
	Renderer m_computeRenderer;



	// Actor positions for ease of use and to avoid additonal magic numbers

	//size_t mainPassIdx;

	

	//Gui
	int Buffer{ 0 };

	FBOLayout mainScreenPassLayout = {
		{AttachmentType::ColourHDR,true},
		{AttachmentType::ColourHDR,true}

	};

	

	//FBOLayout typicalLayout = {
	//	{AttachmentType::ColourHDR, true},
	//	{AttachmentType::ColourHDR, true},
	//	{AttachmentType::ColourHDR, true},
	//	{AttachmentType::ColourHDR, true},
	//	{AttachmentType::ColourHDR, true},
	//	{AttachmentType::Depth, true}
	//};


};

