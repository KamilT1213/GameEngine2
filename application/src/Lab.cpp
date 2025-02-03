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

	//VAOs ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Screen Quad VAOs
	std::shared_ptr<VAO> screenQuadVAO;
	screenQuadVAO = std::make_shared<VAO>(screenIndices);
	screenQuadVAO->addVertexBuffer(screenVertices, {
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
	//ComputePass normalTextureComputePass;
	//normalTextureComputePass.material = normalComputeMat;
	//normalTextureComputePass.workgroups = { 128,128,1 };
	//normalTextureComputePass.barrier = MemoryBarrier::ShaderImageAccess;

	//Image CDMnormals;
	//CDMnormals.mipLevel = 0;
	//CDMnormals.layered = false;
	//CDMnormals.texture = terrainNormalTexure;
	//CDMnormals.imageUnit = normalTextureComputePass.material->m_shader->m_imageBindingPoints["terrainNormalImg"];
	//CDMnormals.access = TextureAccess::ReadWrite;

	Actor quad;
	quad.geometry = screenQuadVAO;

	quad.material = screenQuadMaterial;
	m_screenScene->m_actors.push_back(quad);


/*************************
*  Final Screen Render Pass
**************************/

	RenderPass finalScreenPass;
	finalScreenPass.scene = m_screenScene;
	finalScreenPass.parseScene();
	finalScreenPass.target = std::make_shared<FBO>();
	finalScreenPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };

	finalScreenPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	finalScreenPass.UBOmanager.setCachedValue("b_camera2D", "u_view2D", finalScreenPass.camera.view);
	finalScreenPass.UBOmanager.setCachedValue("b_camera2D", "u_projection2D", finalScreenPass.camera.projection);




	m_mainRenderer.addRenderPass(finalScreenPass);
}
void Scene0::onRender() const
{
	m_mainRenderer.render();
	
}
void Scene0::onUpdate(float timestep)
{
	auto& screenPass = m_mainRenderer.getRenderPass(0);
	auto QuadMat = screenPass.scene->m_actors.at(0).material;

	if (m_winRef.doIsKeyPressed(GLFW_KEY_TAB) && !modeToggle) {
		modeToggle = true;
		focusMode = !focusMode;
		m_winRef.doSwitchInput();
	}
	if (!m_winRef.doIsKeyPressed(GLFW_KEY_TAB) && modeToggle) {
		modeToggle = false;
	}

	if (focusMode) {
		m_PointerPos += (m_winRef.doGetMouseVector() * glm::vec2(1, -1)) / m_ScreenSize;
		m_PointerPos = glm::clamp((m_PointerPos), glm::vec2(0), glm::vec2(1));
	}

	QuadMat->setValue("MousePos",(m_PointerPos) );

	//colPass.target->use();
	//float UVData[3];
	//glNamedFramebufferReadBuffer(colPass.target->getID(), colPass.target->m_colAttachments[1]);
	//glReadPixels(colW / 2, colH / 2, 1, 1, GL_RGB, GL_FLOAT, &UVData);

	
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