#include "Lab.hpp"
#include "scripts/include/camera.hpp"
#include "scripts/include/controller.hpp"
#include "scripts/include/rotation.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <numeric> // For std::iota
#include <string>


Scene0::Scene0(GLFWWindowImpl& win) : Layer(win)
{

	m_winRef.setVSync(false);
	m_mainScene.reset(new Scene);
	m_screenScene.reset(new Scene);

	//Textures -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	TextureDescription groundTextureDesc;
	groundTextureDesc.height = 4096.0f;
	groundTextureDesc.width = 4096.0f;
	groundTextureDesc.channels = 4;
	groundTextureDesc.isHDR = true;
	
	std::shared_ptr<Texture> groundTexture = std::make_shared<Texture>(groundTextureDesc);
	

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

	ShaderDescription compute_GroundShaderDesc;
	compute_GroundShaderDesc.type = ShaderType::compute;
	compute_GroundShaderDesc.computeSrcPath = "./assets/shaders/compute_Ground.glsl";
	std::shared_ptr<Shader> compute_GroundShader = std::make_shared<Shader>(compute_GroundShaderDesc);
	std::shared_ptr<Material> compute_GroundMaterial = std::make_shared<Material>(compute_GroundShader);

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

	//Actors ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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

	GroundComputePass.workgroups = { 128,128,1 };
	GroundComputePass.barrier = MemoryBarrier::ShaderImageAccess;
	GroundComputePass.images.push_back(GroundImg);

	m_mainRenderer.addComputePass(GroundComputePass);

	Actor quad;
	quad.geometry = screenQuadVAO;

	quad.material = screenQuadMaterial;
	screenQuadMaterial->setValue("u_GroundTexture", groundTexture);
	m_screenScene->m_actors.push_back(quad);

	Actor AAquad;
	AAquad.geometry = screenAAQuadVAO;
	AAquad.material = screenAAQuadMaterial;


/*************************
*  Screen Render Pass
**************************/

	RenderPass ScreenPass;
	ScreenPass.scene = m_screenScene;
	ScreenPass.parseScene();
	ScreenPass.target = std::make_shared<FBO>(m_winRef.doGetSize(), mainScreenPassLayout);
	ScreenPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };

	ScreenPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	ScreenPass.UBOmanager.setCachedValue("b_camera2D", "u_view2D", ScreenPass.camera.view);
	ScreenPass.UBOmanager.setCachedValue("b_camera2D", "u_projection2D", ScreenPass.camera.projection);

	m_mainRenderer.addRenderPass(ScreenPass);

	/*************************
	*  AA Render Pass
	**************************/

	screenAAQuadMaterial->setValue("u_inputTexture", ScreenPass.target->getTarget(0));

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

	m_mainRenderer.addRenderPass(ScreenAAPass);

}
void Scene0::onRender() const
{
	m_mainRenderer.render();
	
}
void Scene0::onUpdate(float timestep)
{
	allTime += timestep / 10.0f;

	auto& computeGroundPass = m_mainRenderer.getComputePass(0);
	auto& screenPass = m_mainRenderer.getRenderPass(1);
	auto& screenAAPass = m_mainRenderer.getRenderPass(2);

	auto QuadMat = screenPass.scene->m_actors.at(0).material;
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

	float timePerSegment = 0.5f;
	float Segments = 7.0f;

	if (m_winRef.doIsMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) {
		ProgressBar += timestep;// *((1 / timePerSegment) / Segments);
		if (ProgressBar > 1) {
			ProgressBar = 1;
		}
	}
	else {
		ProgressBar -= timestep * 5;
		if (ProgressBar < 0) {
			ProgressBar = 0;
		}
	}

	if (m_winRef.doIsKeyPressed(GLFW_KEY_R)) {
		Reseting = true;
		ResetWave = 1.0f;
	}
	else {
		if (Reseting) {
			ResetWave -= timestep;
			if (ResetWave <= -0.1f) {
				Reseting = false;
			}
		}
	}

	float x = floor(ProgressBar * Segments) / Segments;

	QuadMat->setValue("MousePos",(m_PointerPos) );
	QuadMat->setValue("Progress", x);
	AAQuadMat->setValue("allTime", allTime);
	computeGroundPass.material->setValue("Reset", (float)(int)Reseting);
	computeGroundPass.material->setValue("ResetWave", ResetWave);
	if (ProgressBar >= 1) {
		Pressed = true;
	}
	else {
		Pressed = false;
	}
	computeGroundPass.material->setValue("action", (float)(int)Pressed);
	computeGroundPass.material->setValue("MousePos", (m_PointerPos));
	computeGroundPass.material->setValue("dt", timestep);

	//spdlog::info("Reset Wave: {:03.2f}", ResetWave);
	//spdlog::info("Reseting: {:03.2f}", (float)(int)Reseting);
	screenPass.target->use();
	float UVData[4];
	glNamedFramebufferReadBuffer(screenPass.target->getID(), screenPass.target->m_colAttachments[1]);
	glm::vec2 temp = m_PointerPos * glm::min(width, height);
	float a = glm::max(width, height) - glm::min(width, height);
	a /= 2;
	if (width > height) {
		temp.x += a;
	}
	else {
		temp.y += a;
	}
	
	temp -= glm::vec2(0.0001f, 0.0001f);

	//temp = glm::clamp(temp, glm::vec2(0), glm::vec2(width, height));
	glReadPixels(temp.x, temp.y, 1, 1, GL_RGB, GL_FLOAT, &UVData);

	for (int i = 0; i < 4; i++) {
		UVData[i] = glm::clamp(UVData[i], 0.0f, 1.0f);
	}

	gameMouseLocation = glm::vec2(UVData[0], UVData[1]);
	//spdlog::info("mouse x: {:03.2f}, mouse y: {:03.2f}", gameMouseLocation.x , gameMouseLocation.y);
	
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
	for (auto it = m_mainScene->m_actors.begin(); it != m_mainScene->m_actors.end(); ++it)
	{
		it->onKeyPress(e);
	}
}