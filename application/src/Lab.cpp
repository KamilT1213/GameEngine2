#include "Lab.hpp"
#include "scripts/include/camera.hpp"
#include "scripts/include/controller.hpp"
#include "scripts/include/rotation.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <numeric> // For std::iota
#include <string>
//#include <windows.h>
//#include <mmsystem.h>

Scene0::Scene0(GLFWWindowImpl& win) : Layer(win)
{

	m_winRef.setVSync(false);
	m_RelicScene.reset(new Scene);
	m_screenScene.reset(new Scene);

	//Textures -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	std::shared_ptr<Texture> testRelicTexture = std::make_shared<Texture>("./assets/textures/Relic.png");

	TextureDescription groundTextureDesc;
	groundTextureDesc.height = 4096.0f;
	groundTextureDesc.width = 4096.0f;
	groundTextureDesc.channels = 4;
	groundTextureDesc.isHDR = true;
	
	std::shared_ptr<Texture> groundTexture = std::make_shared<Texture>(groundTextureDesc);
	std::shared_ptr<Texture> groundTextureTemp = std::make_shared<Texture>(groundTextureDesc);
	std::shared_ptr<Texture> groundNormalTexture = std::make_shared<Texture>(groundTextureDesc);

	//Cube maps -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Materials -------------------------------------------------------------------------------------------------------------------------------------------------------------


	//Final screen post processing pass (screen effects) material
	ShaderDescription screenQuadShaderDesc;
	screenQuadShaderDesc.type = ShaderType::rasterization;
	screenQuadShaderDesc.vertexSrcPath = "./assets/shaders/QuadVert.glsl";
	screenQuadShaderDesc.fragmentSrcPath = "./assets/shaders/QuadFrag.glsl";
	std::shared_ptr<Shader> screenQuadShader = std::make_shared<Shader>(screenQuadShaderDesc);
	std::shared_ptr<Material> screenQuadMaterial = std::make_shared<Material>(screenQuadShader);

	screenQuadMaterial->setValue("u_ScreenSize", m_ScreenSize);

	//AA screen pass mat
	ShaderDescription screenAAQuadShaderDesc;
	screenAAQuadShaderDesc.type = ShaderType::rasterization;
	screenAAQuadShaderDesc.vertexSrcPath = "./assets/shaders/QuadVert.glsl";
	screenAAQuadShaderDesc.fragmentSrcPath = "./assets/shaders/AAFrag.glsl";
	std::shared_ptr<Shader> screenAAQuadShader = std::make_shared<Shader>(screenAAQuadShaderDesc);
	std::shared_ptr<Material> screenAAQuadMaterial = std::make_shared<Material>(screenAAQuadShader);

	screenAAQuadMaterial->setValue("u_ScreenSize", m_ScreenSize);

	//Relic mat
	ShaderDescription RelicShaderDesc;
	RelicShaderDesc.type = ShaderType::rasterization;
	RelicShaderDesc.vertexSrcPath = "./assets/shaders/RelicVert.glsl";
	RelicShaderDesc.fragmentSrcPath = "./assets/shaders/RelicFrag.glsl";
	std::shared_ptr<Shader> RelicShader = std::make_shared<Shader>(RelicShaderDesc);
	//std::shared_ptr<Material> RelicMaterial = std::make_shared<Material>(RelicShader);

	ShaderDescription compute_GroundShaderDesc;
	compute_GroundShaderDesc.type = ShaderType::compute;
	compute_GroundShaderDesc.computeSrcPath = "./assets/shaders/compute_Ground.glsl";
	std::shared_ptr<Shader> compute_GroundShader = std::make_shared<Shader>(compute_GroundShaderDesc);
	std::shared_ptr<Material> compute_GroundMaterial = std::make_shared<Material>(compute_GroundShader);

	ShaderDescription compute_GroundNormalShaderDesc;
	compute_GroundNormalShaderDesc.type = ShaderType::compute;
	compute_GroundNormalShaderDesc.computeSrcPath = "./assets/shaders/compute_CDMnormals.glsl";
	std::shared_ptr<Shader> compute_GroundNormalShader = std::make_shared<Shader>(compute_GroundNormalShaderDesc);
	std::shared_ptr<Material> compute_GroundNormalMaterial = std::make_shared<Material>(compute_GroundNormalShader);

	//VAOs ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Screen Quad VAOs
	std::vector<float> screenVertices;
	std::vector<float> screenAAVertices;

	screenAAVertices = std::vector<float>{
		//Position               UV
	0.0f,  0.0f,   0.0f,     0.0f, 1.0f, // Bottom Left
	width, 0.0f,   0.0f,     1.0f, 1.0f, // Bottom Right
	width, height, 0.0f,     1.0f, 0.0f, // Top Right
	0.0f,  height, 0.0f,     0.0f, 0.0f  // Top Left
	};

	float a = glm::max(width, height);
	float b = glm::min(width, height);
	float c = (a - b)/2;

	float widthOffset, heightOffset;
	if (width > height) {
		screenVertices = std::vector<float>{
			//Position               UV
		c,  0.0f,   0.0f,     0.0f, 1.0f, // Bottom Left
		width - c, 0.0f,   0.0f,     1.0f, 1.0f, // Bottom Right
		width - c, height, 0.0f,     1.0f, 0.0f, // Top Right
		c,  height, 0.0f,     0.0f, 0.0f  // Top Left
		};
	}
	else {
		screenVertices = std::vector<float>{
			//Position               UV
		0.0f,  c,   0.0f,     0.0f, 1.0f, // Bottom Left
		width, c,   0.0f,     1.0f, 1.0f, // Bottom Right
		width, height - c, 0.0f,     1.0f, 0.0f, // Top Right
		0.0f,  height - c, 0.0f,     0.0f, 0.0f  // Top Left
		};
	}



	const std::vector<uint32_t> screenIndices = { 0,1,2,2,3,0 };

	std::shared_ptr<VAO> screenQuadVAO;
	screenQuadVAO = std::make_shared<VAO>(screenIndices);
	screenQuadVAO->addVertexBuffer(screenVertices, {
		{GL_FLOAT, 3},  // position
		{GL_FLOAT, 2}   // UV
	});

	std::shared_ptr<VAO> screenAAQuadVAO;
	screenAAQuadVAO = std::make_shared<VAO>(screenIndices);
	screenAAQuadVAO->addVertexBuffer(screenAAVertices, {
		{GL_FLOAT, 3},  // position
		{GL_FLOAT, 2}   // UV
		});

	std::vector<float> RelicVertices;

	RelicVertices = std::vector<float>{
		//Position               UV
	-1.0f,  -1.0f,   0.0f,     0.0f, 1.0f, // Bottom Left
	 1.0f,  -1.0f,   0.0f,     1.0f, 1.0f, // Bottom Right
	 1.0f,   1.0f,   0.0f,     1.0f, 0.0f, // Top Right
	-1.0f,   1.0f,   0.0f,     0.0f, 0.0f  // Top Left
	};

	std::shared_ptr<VAO> RelicVAO;
	RelicVAO = std::make_shared<VAO>(screenIndices);
	RelicVAO->addVertexBuffer(RelicVertices, {
		{GL_FLOAT,3},  // position
		{GL_FLOAT,2}   // UV
		});

	//Actors ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	Actor Relic;
	Relic.geometry = RelicVAO;

	for (int i = 0; i < Relics; i++) {
		ActiveRelics[i] = true;
		float rarity = Randomiser::uniformFloatBetween(0.01f, 6.0f);
		std::shared_ptr<Material> RelicMaterial = std::make_shared<Material>(RelicShader);
		RelicMaterial->setValue("u_RelicTexture", testRelicTexture);
		RelicMaterial->setValue("u_Rarity", rarity);
		RelicMaterial->setValue("u_Id", (float)(i + 1)/ (Relics + 1.0f));
		RelicMaterial->setValue("u_active", (float)(int)true);

		Relic.scale = glm::vec3(Randomiser::uniformFloatBetween(100.0f, 150.0f) * (( 0.5f *( (7.0f - (1 + rarity)) / 7.0f)) + 0.5f) );

		Relic.translation =  glm::vec3(Randomiser::uniformFloatBetween(Relic.scale.x, 4096.0f - Relic.scale.x), Randomiser::uniformFloatBetween(Relic.scale.y, 4096.0f - Relic.scale.y), -1.0f);
		

		Relic.material = RelicMaterial;

		Relic.recalc();
		m_RelicScene->m_actors.push_back(Relic);
	}

	//Particles
	//Actor particles;

	//uint32_t numberOfParticles = 4096 * 4;

	//terrainParticlesStartPoints = std::make_shared<SSBO>(sizeof(Particle) * numberOfParticles, numberOfParticles);
	//terrainParticlesStartPoints->bind(0);
	//terrainParticles = std::make_shared<SSBO>(sizeof(Particle) * numberOfParticles, numberOfParticles);
	//terrainParticles->bind(1);
	////ParticleMaterial->setValue("particles", terrainParticles->getID());

	//particles.SSBOgeometry = terrainParticlesStartPoints;
	//particles.material = ParticleMaterial;
	////particles.SSBOdepthGeometry = terrainParticlesStartPoints;
	////particles.depthMaterial = ParticleMaterial;
	////particles.translation = glm::vec3(-5, -3, -5);
	//particles.recalc();
	//particlesIdx = m_mainScene->m_actors.size();
	//m_mainScene->m_actors.push_back(particles);

	//Main Camera

	//Actor camera;
	//cameraIdx = m_mainScene->m_actors.size();
	//m_mainScene->m_actors.push_back(camera);

	/*************************
	*  Compute Pass
	**************************/

	//Terrain:

	//Terrain Normals Compute Pass
	ComputePass GroundComputePass;
	GroundComputePass.material = compute_GroundMaterial;
	GroundComputePass.workgroups = { 128,128,1 };
	GroundComputePass.barrier = MemoryBarrier::ShaderImageAccess;

	Image GroundImg;
	GroundImg.mipLevel = 0;
	GroundImg.layered = false;
	GroundImg.texture = groundTexture;
	GroundImg.imageUnit = GroundComputePass.material->m_shader->m_imageBindingPoints["GroundImg"];
	GroundImg.access = TextureAccess::ReadWrite;	
	
	Image GroundImgTemp;
	GroundImgTemp.mipLevel = 0;
	GroundImgTemp.layered = false;
	GroundImgTemp.texture = groundTextureTemp;
	GroundImgTemp.imageUnit = GroundComputePass.material->m_shader->m_imageBindingPoints["GroundImgHold"];
	GroundImgTemp.access = TextureAccess::ReadWrite;

	GroundComputePass.images.push_back(GroundImg);
	GroundComputePass.images.push_back(GroundImgTemp);

	GroundComputePassIDx = m_mainRenderer.getSumPassCount();
	m_mainRenderer.addComputePass(GroundComputePass);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ComputePass GroundNormalComputePass;
	GroundNormalComputePass.material = compute_GroundNormalMaterial;
	GroundNormalComputePass.workgroups = { 128,128,1 };
	GroundNormalComputePass.barrier = MemoryBarrier::ShaderImageAccess;

	Image GroundNormalImg;
	GroundNormalImg.mipLevel = 0;
	GroundNormalImg.layered = false;
	GroundNormalImg.texture = groundNormalTexture;
	GroundNormalImg.imageUnit = GroundNormalComputePass.material->m_shader->m_imageBindingPoints["groundNormalImg"];
	GroundNormalImg.access = TextureAccess::ReadWrite;

	GroundNormalComputePass.images.push_back(GroundImg);
	GroundNormalComputePass.images.push_back(GroundNormalImg);

	GroundNormalComputePassIDx = m_computeRenderer.getSumPassCount();
	m_computeRenderer.addComputePass(GroundNormalComputePass);
	m_computeRenderer.render();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	Actor quad;
	quad.geometry = screenQuadVAO;
	
	screenQuadMaterial->setValue("u_GroundDepthTexture", groundTextureTemp);
	
	//screenQuadMaterial->setValue("u_GroundNormalTexture", groundNormalTexture);
	quad.material = screenQuadMaterial;

	m_screenScene->m_actors.push_back(quad);

	Actor AAquad;
	AAquad.geometry = screenAAQuadVAO;
	AAquad.material = screenAAQuadMaterial;
	//AAquad.translation = glm::vec3(500.9f,0,0);
	//AAquad.scale = glm::vec3(2);
	//AAquad.recalc();

/*************************
*  Screen Relic Render Pass
**************************/

	RenderPass ScreenRelicPass;
	ScreenRelicPass.scene = m_RelicScene;
	ScreenRelicPass.parseScene();
	ScreenRelicPass.target = std::make_shared<FBO>(glm::ivec2(4096,4096), mainScreenPassLayout);
	ScreenRelicPass.viewPort = { 0,0,4096, 4096 };

	ScreenRelicPass.camera.projection = glm::ortho(0.f, 4096.0f, 4096.0f, 0.f);

	ScreenRelicPass.UBOmanager.setCachedValue("b_relicCamera2D", "u_relicView2D", ScreenRelicPass.camera.view);
	ScreenRelicPass.UBOmanager.setCachedValue("b_relicCcamera2D", "u_relicProjection2D", ScreenRelicPass.camera.projection);

	for (int i = 0; i < m_RelicScene->m_actors.size(); i++) {
		m_RelicScene->m_actors.at(i).material->setValue("u_relicView2D", ScreenRelicPass.camera.view);
		m_RelicScene->m_actors.at(i).material->setValue("u_relicProjection2D", ScreenRelicPass.camera.projection);
	}

	ScreenRelicPassIDx = m_mainRenderer.getSumPassCount();
	m_mainRenderer.addRenderPass(ScreenRelicPass);

	

	screenQuadMaterial->setValue("u_RelicTexture", ScreenRelicPass.target->getTarget(0));
	screenQuadMaterial->setValue("u_RelicDataTexture", ScreenRelicPass.target->getTarget(1));

/*************************
*  Screen Ground Render Pass
**************************/
	RenderPass ScreenGroundPass;
	ScreenGroundPass.scene = m_screenScene;
	ScreenGroundPass.parseScene();
	ScreenGroundPass.target = std::make_shared<FBO>(m_winRef.doGetSize(), mainScreenPassLayout);
	ScreenGroundPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };

	ScreenGroundPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	ScreenGroundPass.UBOmanager.setCachedValue("b_camera2D", "u_view2D", ScreenGroundPass.camera.view);
	ScreenGroundPass.UBOmanager.setCachedValue("b_camera2D", "u_projection2D", ScreenGroundPass.camera.projection);

	Buffer = ScreenGroundPass.target->getTarget(1)->getID();

	ScreenGroundPassIDx = m_mainRenderer.getSumPassCount();
	m_mainRenderer.addRenderPass(ScreenGroundPass);

	/*************************
	*  AA Render Pass
	**************************/

	screenAAQuadMaterial->setValue("u_inputTexture", ScreenGroundPass.target->getTarget(0));

	m_screenScene.reset(new Scene);
	m_screenScene->m_actors.push_back(AAquad);

	RenderPass ScreenAAPass;
	ScreenAAPass.scene = m_screenScene;
	ScreenAAPass.parseScene();
	ScreenAAPass.target = std::make_shared<FBO>();
	ScreenAAPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };

	ScreenAAPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	ScreenAAPass.UBOmanager.setCachedValue("b_camera2D", "u_view2D", ScreenAAPass.camera.view);
	ScreenAAPass.UBOmanager.setCachedValue("b_camera2D", "u_projection2D", ScreenAAPass.camera.projection);

	AAPassIDx = m_mainRenderer.getSumPassCount();
	m_mainRenderer.addRenderPass(ScreenAAPass);

}
void Scene0::onRender() const
{
	m_mainRenderer.render();
	
}
void Scene0::onUpdate(float timestep)
{
	allTime += timestep / 10.0f;

	auto& computeGroundPass = m_mainRenderer.getComputePass(GroundComputePassIDx);
	auto& relicPass = m_mainRenderer.getRenderPass(ScreenRelicPassIDx);
	auto& screenGroundPass = m_mainRenderer.getRenderPass(ScreenGroundPassIDx);
	auto& screenAAPass = m_mainRenderer.getRenderPass(AAPassIDx);

	//for (int i = 0; i < m_mainRenderer.getRenderPassCount(); i++) {
	//	auto& targetPass = m_mainRenderer.getRenderPass(i);
	//	targetPass.UBOmanager.setCachedValue("b_camera2D", "u_view2D", screenGroundPass.camera.view);
	//	targetPass.UBOmanager.setCachedValue("b_camera2D", "u_projection2D", screenGroundPass.camera.projection);


	//	targetPass.UBOmanager.setCachedValue("b_relicCamera2D", "u_relicView2D", relicPass.camera.view);
	//	targetPass.UBOmanager.setCachedValue("b_relicCcamera2D", "u_relicProjection2D", relicPass.camera.projection);
	//}
	auto QuadMat = screenGroundPass.scene->m_actors.at(0).material;
	auto AAQuadMat = screenAAPass.scene->m_actors.at(0).material;

	if (m_winRef.doIsKeyPressed(GLFW_KEY_TAB) && !modeToggle) {
		modeToggle = true;
		focusMode = !focusMode;
		m_winRef.doSwitchInput();
	}
	if (!m_winRef.doIsKeyPressed(GLFW_KEY_TAB) && modeToggle) {
		modeToggle = false;
	}

	if (focusMode) {
		m_PointerPos += (m_winRef.doGetMouseVector() * glm::vec2(1, -1)) / glm::min(width,height);
		m_PointerPos = glm::clamp((m_PointerPos), glm::vec2(0), glm::vec2(1));
	}
	if (!Pressed) {
		m_DigPos = m_PointerPos;
	}

	float timeToDig = 3.0f;

	float Segments = 7.0f;
	float timePerSegment = 0.6f;

	float x = ProgressBar;

	glm::vec2 temp = m_DigPos * glm::min(width, height);
	float a = glm::max(width, height) - glm::min(width, height);
	a /= 2;
	if (width > height) {
		temp.x += a;
	}
	else {
		temp.y += a;
	}
	//temp = m_DigPos;
	temp -= glm::vec2(0.0001f, 0.0001f);

	//temp = glm::clamp(temp, glm::vec2(0), glm::vec2(width, height));

	screenGroundPass.target->use();
	float UVData[4];
	glNamedFramebufferReadBuffer(screenGroundPass.target->getID(), screenGroundPass.target->m_colAttachments[1]);
	glReadPixels(temp.x, temp.y, 1, 1, GL_RGBA, GL_FLOAT, &UVData);

	//for (int i = 0; i < 4; i++) {
	//	UVData[i] = glm::clamp(UVData[i], 0.0f, 1.0f);
	//}

	gameMouseLocation = glm::vec2(UVData[0], UVData[1]);
	//spdlog::info("mouse x: {:03.2f}, mouse y: {:03.2f}", gameMouseLocation.x, gameMouseLocation.y);
	//spdlog::info("Relic id: {:03.5f}", glm::round(UVData[2] * (Relics + 1)));

	int RelId = (int)glm::round(UVData[2] * (Relics + 1));



	Segments = glm::ceil(UVData[3] * 6.0f) + 1.0f;
	//spdlog::info("Relic Segments: {:03.5f}", Segments);

	bool isExtracting = (RelId != 0 && (ProgressBar == 0 || extrBegan));// m_winRef.doIsKeyPressed(GLFW_KEY_E);
	bool LevelComplete = false;
	if (RelicSetWave >= 1 && focusMode) {
		if (isExtracting) {
			if (!extrBegan) {
				extrBegan = true;
				ProgressBar = 0;
			}

			if (m_winRef.doIsMouseButtonPressed(GLFW_MOUSE_BUTTON_1) && !finished) {
				Pressed = true;
				ProgressBar += timestep * ((1 / timePerSegment) / Segments);
				if (ProgressBar > 1) {
					finished = true;
					ProgressBar = 1;
					m_RelicScene->m_actors.at(RelId - 1).material->setValue("u_active", 0.0f);
					ActiveRelics[RelId - 1] = false;
					for (int i = 0; i < Relics; i++) {
						if (ActiveRelics[i] == true) {
							LevelComplete = false;
							break;
						}
						else {
							LevelComplete = true;
						}
					}
				}
			}
			else {
				finished = true;
				ProgressBar -= timestep * 5;
				if (ProgressBar < 0) {
					extrBegan = false;
					ProgressBar = 0;
					ProgressSegmentTarget = 1;
					Pressed = false;
					finished = false;
				}
			}

			if (ProgressBar * Segments >= ProgressSegmentTarget) {

				int r = rand() % 4;

				std::string s = "./assets/sounds/Extraction_soft_var";
				s += char('0' + r);
				s.append(".wav");

				m_soundManager.playSound(s.c_str());
				//spdlog::info("PlaySound");
				ProgressSegmentTarget++;
			}

			x = floor(ProgressBar * Segments) / Segments;
		}
		else {
			if (m_winRef.doIsMouseButtonPressed(GLFW_MOUSE_BUTTON_1) && !finished) {
				Pressed = true;
				ProgressBar += timestep / timeToDig;
				if (ProgressBar > 1) {
					finished = true;
					ProgressBar = 1;
				}
				if ((RelId != 0)) {
					finished = true;
				}
			}
			else {
				finished = true;
				//m_computeRenderer.render();
				ProgressBar -= timestep * 5;
				if (ProgressBar < 0) {
					ProgressBar = 0;
					extrBegan = false;
					ProgressSegmentTarget = 0.5;
					Pressed = false;
					finished = false;
				}
			}

			if (ProgressBar * timeToDig >= ProgressSegmentTarget) {

				int r = rand() % 2;
				//int r2 = rand() % 4;

				//std::string s2 = "./assets/sounds/Extraction_soft_var";
				//s2 += char('0' + r2);
				//s2.append(".wav");

				//m_soundManager.playSound(s2.c_str());

				std::string s = "./assets/sounds/Digging_soft_var";
				s += char('0' + r);
				s.append(".wav");

				m_soundManager.playSound(s.c_str());
				//spdlog::info("PlaySound");
				ProgressSegmentTarget++;
			}
		}
	}
	

	if (m_winRef.doIsKeyPressed(GLFW_KEY_R) || LevelComplete) {
		Reseting = true;
		ResetWave = 1.0f;
	}
	else {
		if (Reseting) {
			ResetWave -= timestep / 3;
			if (ResetWave <= 0) {
				RelicSetWave = -0.1f;
			}
			if (ResetWave <= -0.1f) {
				Reseting = false;
				
				for (int i = 0; i < Relics; i++) {
					ActiveRelics[i] = true;
					float rarity = Randomiser::uniformFloatBetween(0.01f, 6.0f);

					auto RelicMaterial = m_RelicScene->m_actors.at(i).material;
					auto& Relic = m_RelicScene->m_actors.at(i);

					RelicMaterial->setValue("u_Rarity", rarity);
					//RelicMaterial->setValue("u_Id", (float)(i + 1) / (Relics + 1.0f));
					RelicMaterial->setValue("u_active", (float)(int)true);

					Relic.scale = glm::vec3(Randomiser::uniformFloatBetween(100.0f, 150.0f) * ((0.5f * ((7.0f - (1 + rarity)) / 7.0f)) + 0.5f));
					Relic.translation = glm::vec3(Randomiser::uniformFloatBetween(Relic.scale.x, 4096.0f - Relic.scale.x), Randomiser::uniformFloatBetween(Relic.scale.y, 4096.0f - Relic.scale.y), -1.0f);
					Relic.recalc();

				}
			}
		}
	}

	if (RelicSetWave < 1) {
		RelicSetWave += timestep / 3;
		if (RelicSetWave >= 1) {
			RelicSetWave = 1;
		}
	}
	

	QuadMat->setValue("MousePos",(m_PointerPos) );
	QuadMat->setValue("DigPos",(m_DigPos) );
	QuadMat->setValue("Progress", x);
	QuadMat->setValue("RelicFill", RelicSetWave);

	AAQuadMat->setValue("allTime", allTime);
	computeGroundPass.material->setValue("Reset", (float)(int)Reseting);
	computeGroundPass.material->setValue("ResetWave", glm::clamp(( - glm::pow(ResetWave - 1, 2.0f)) + 1,0.0f,1.0f));
	//if (ProgressBar >= 1) {
	//	Pressed = true;
	//}
	//else {
	//	Pressed = false;
	//}
	float action = ProgressBar;
	float x2 = glm::mod(action, 1 / timeToDig);
	float x3 = glm::pow(x2 * timeToDig, 30)/ timeToDig;
	float x4 = glm::floor(action * timeToDig) / timeToDig;
	action = x3 + x4;
	computeGroundPass.material->setValue("action", action);
	computeGroundPass.material->setValue("digging", (float)(int)(!finished && !isExtracting));
	computeGroundPass.material->setValue("MousePos", (m_DigPos));
	computeGroundPass.material->setValue("subBy", Subby);

	//spdlog::info("Reset Wave: {:03.2f}", ResetWave);
	//spdlog::info("Reseting: {:03.2f}", (float)(int)Reseting);


}


void Scene0::onImGUIRender()
{
	float ms = 1000.0f / ImGui::GetIO().Framerate; ;  //milisecconds
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("GAMR3521");
	ImGui::Text("FPS %.3f ms/frame (%.1f FPS)", ms, ImGui::GetIO().Framerate);  // display FPS and ms

	if(ImGui::BeginTabBar("Tabs")){
		if (ImGui::BeginTabItem("Description of Project")) {
			ImGui::TextWrapped("Welcome to my Project \n For marking please search for keywords in the cpp and find various shaders and functions\n Particles only spawn on left side of map so please draw an overhand there to see them \n .. \n (for beziers to work you must switch from bezier mode and back once at the beginning) \n Controls:\n Holding [F1] switches between play and flying modes\n Pressing [Insert] switiched between bezier mode and landscape painting mode\n [WASD] controls movement, [Shift] flies down, [Space] flies up or jumps depending on control mode\n .. \n When in edit mode, [R] Resets displacment Map, [-] & [+] changes brush size, [LMB] is Add terrain and [RMB] is subtract terrain\n .. \n In Bezier mode [LMB] is Draw Bezier at location in front of player and [RMB] is select Start point on terrain \n..\n Finally Pressing [TAB] will focus mouse in and out of the game window (use this to play and stop playing the game)");
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Debugging Tools")) {

			
			ImGui::Checkbox("Toggle Screen Shaders ", &b);

			ImGui::NewLine();
			ImGui::DragFloat("Factor", &factor, 1.0f, 1.0f, 10000.0f);
			ImGui::NewLine();
			ImGui::NewLine();
			//ImGui::Checkbox("Wireframe ", &m_wireFrame);

			//if (ImGui::Button("Point Lights Overlay")) { m_Toggle = (int)!(bool)m_Toggle; }

			ImGui::Text("Tool For looking through all buffers (some not shown as they are being sampled to the cpu in update)");
			if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { Buffer--; }
			ImGui::SameLine();
			ImGui::DragInt(".", &Buffer, 1, 0, 16, "%d", ImGuiSliderFlags_NoInput + ImGuiSliderFlags_AlwaysClamp);
			ImGui::SameLine();
			if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { Buffer++; }


			ImGui::Image((void*)(intptr_t)Buffer, imageSize, uv0, uv1);


			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Texture Inspector")) {

			ImGui::Image((void*)(intptr_t)Buffer, ImVec2(4096.0f, 4096.0f), ImVec2(0, 0), ImVec2(1, 1));
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Colour Wheel of fun")) {

			//ImGui::ColorPicker3("HSV Colour", (float*)&m_HSVcolours, ImGuiColorEditFlags_DisplayHSV + ImGuiColorEditFlags_PickerHueWheel);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	
	ImGui::End();
	//ImGui::ShowDemoWindow();
	ImGui::Render();
}

void Scene0::onKeyPressed(KeyPressedEvent& e)
{
	for (auto it = m_RelicScene->m_actors.begin(); it != m_RelicScene->m_actors.end(); ++it)
	{
		it->onKeyPress(e);
	}
}