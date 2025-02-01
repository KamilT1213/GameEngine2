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
	m_compositeScreenScene.reset(new Scene);
	m_screenScene.reset(new Scene);
	m_screenCubeMapScene.reset(new Scene);
	m_antialiasingScreenScene.reset(new Scene);
	m_collisionScene.reset(new Scene);
	m_terrainParticlesScene.reset(new Scene);

	

	//Shaders and Materials
	//ShaderDescription phongShaderDesc;
	//phongShaderDesc.type = ShaderType::rasterization;
	//phongShaderDesc.vertexSrcPath = "./assets/shaders/PhongVert.glsl";
	//phongShaderDesc.fragmentSrcPath = "./assets/shaders/PhongFrag.glsl";
	//std::shared_ptr<Shader> phongShader = std::make_shared<Shader>(phongShaderDesc);
	//std::shared_ptr<Material> cubeMaterial = std::make_shared<Material>(phongShader);

	//Textures -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Floor Textures
	std::shared_ptr<Texture> floorTexture = std::make_shared<Texture>("./assets/textures/Floor/floor.png");
	std::shared_ptr<Texture> floorTexture2 = std::make_shared<Texture>("./assets/textures/Floor/floor2.png");
	std::shared_ptr<Texture> floorNormalMap = std::make_shared<Texture>("./assets/textures/Floor/floorNormalMap.png");

	//Water Texture
	std::shared_ptr<Texture> waterAlbedo = std::make_shared<Texture>("./assets/textures/Water/water.png");

	//Characater Textures (Witch)
	std::shared_ptr<Texture> characterDiffuseTexture, characterNormalTexture, characterSpecularTexture, characterGlowTexture;
	characterDiffuseTexture = std::make_shared<Texture>("./assets/models/witch/textures/diffuse.png");
	characterNormalTexture = std::make_shared<Texture>("./assets/models/witch/textures/normal.png");
	characterSpecularTexture = std::make_shared<Texture>("./assets/models/witch/textures/specular.png");
	characterGlowTexture = std::make_shared<Texture>("./assets/models/witch/textures/glow.png");

	//Player Textures (Orb, chicken, eye thing)
	std::shared_ptr<Texture> thingDiffuseTexture, thingNormalTexture, thingSpecularTexture;
	thingDiffuseTexture = std::make_shared<Texture>("./assets/models/thing/textures/diffuse.png");
	thingNormalTexture = std::make_shared<Texture>("./assets/models/thing/textures/normal.png");
	thingSpecularTexture = std::make_shared<Texture>("./assets/models/thing/textures/spec.png");

	//Grass Texture
	std::shared_ptr<Texture> billboardTexture = std::make_shared<Texture>("./assets/textures/Billboard/front2.png");

	//Particle Texture
	std::shared_ptr<Texture> particleTexture = std::make_shared<Texture>("./assets/textures/Particles/front.png");


	//Textures for compute 
	TextureDescription textDesc;
	textDesc.width = 4096;
	textDesc.height = 4096;
	textDesc.channels = 4;
	textDesc.isHDR = true;

	//Terrain Displacement and Generated Normals
	std::shared_ptr<Texture> terrainTexture = std::make_shared<Texture>(textDesc);
	std::shared_ptr<Texture> terrainNormalTexure = std::make_shared<Texture>(textDesc);

	//Water Displacement and Generated Normals
	std::shared_ptr<Texture> waterTexture = std::make_shared<Texture>(textDesc);
	std::shared_ptr<Texture> waterNormalTexure = std::make_shared<Texture>(textDesc);

	//Cube maps -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Skybox Cubemap
	std::array<const char*, 6> skyboxCubeMapPaths = {
	"./assets/textures/Skybox/right.png",
	"./assets/textures/Skybox/left.png",
	"./assets/textures/Skybox/top.png",
	"./assets/textures/Skybox/bottom.png",
	"./assets/textures/Skybox/front.png",
	"./assets/textures/Skybox/back.png"
	};
	std::shared_ptr<CubeMap> cubeMap;
	cubeMap = std::make_shared<CubeMap>(skyboxCubeMapPaths, false, false);

	//Paper Screen overaly Cubemap
	std::array<const char*, 6> screenCubeMapPaths = {
	"./assets/textures/Paper/Paperright.png",
	"./assets/textures/Paper/Paperleft.png",
	"./assets/textures/Paper/Papertop.png",
	"./assets/textures/Paper/Paperbottom.png",
	"./assets/textures/Paper/Paperfront.png",
	"./assets/textures/Paper/Paperback.png"
	};
	std::shared_ptr<CubeMap> screenCubeMap = std::make_shared<CubeMap>(screenCubeMapPaths, false, false);

	//Materials -------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Collision material for collision pass (Player colliding with models)
	ShaderDescription colDesc;
	colDesc.type = ShaderType::rasterization;
	colDesc.vertexSrcPath = "./assets/shaders/CollisionVert.glsl";
	colDesc.fragmentSrcPath = "./assets/shaders/CollisionFrag.glsl";
	std::shared_ptr<Shader> colShader = std::make_shared<Shader>(colDesc);
	std::shared_ptr<Material> colMaterial = std::make_shared<Material>(colShader);

	//Terrain colour material
	ShaderDescription gridPhongShaderDesc;
	gridPhongShaderDesc.type = ShaderType::tessellationAndGeometry;
	gridPhongShaderDesc.vertexSrcPath = "./assets/shaders/GridPhongVert.glsl";
	gridPhongShaderDesc.controlSrcPath = "./assets/shaders/floorTCS.glsl";
	gridPhongShaderDesc.evaluationSrcPath = "./assets/shaders/floorTES.glsl";
	gridPhongShaderDesc.geometrySrcPath = "./assets/shaders/Geometry/GridGeo.glsl";
	gridPhongShaderDesc.fragmentSrcPath = "./assets/shaders/GridPhongFrag.glsl";
	std::shared_ptr<Shader> gridPhongShader = std::make_shared<Shader>(gridPhongShaderDesc);
	std::shared_ptr<Material> gridMaterial = std::make_shared<Material>(gridPhongShader);
	gridMaterial->setPrimitive(GL_PATCHES);

	gridMaterial->setValue("u_albedoMap", floorTexture);
	gridMaterial->setValue("u_altAlbedoMap", floorTexture2);
	//gridMaterial->setValue("u_normalMap", floorNormalMap);
	gridMaterial->setValue("u_heightMap", terrainTexture);
	gridMaterial->setValue("CalculatedNormalMap", terrainNormalTexure);

	//Water colour material
	ShaderDescription gridWaterShaderDesc;
	gridWaterShaderDesc.type = ShaderType::tessellationAndGeometry;
	gridWaterShaderDesc.vertexSrcPath = "./assets/shaders/GridPhongVert.glsl";
	gridWaterShaderDesc.controlSrcPath = "./assets/shaders/floorTCS.glsl";
	gridWaterShaderDesc.evaluationSrcPath = "./assets/shaders/floorTES.glsl";
	gridWaterShaderDesc.geometrySrcPath = "./assets/shaders/Geometry/GridGeo.glsl";
	gridWaterShaderDesc.fragmentSrcPath = "./assets/shaders/GridWaterFrag.glsl";
	std::shared_ptr<Shader> gridWaterShader = std::make_shared<Shader>(gridWaterShaderDesc);
	std::shared_ptr<Material> waterMaterial = std::make_shared<Material>(gridWaterShader);
	waterMaterial->setPrimitive(GL_PATCHES);

	waterMaterial->setValue("u_altAlbedoMap", terrainTexture);
	waterMaterial->setValue("u_albedoMap", waterAlbedo);
	waterMaterial->setValue("CalculatedNormalMap", waterNormalTexure);
	waterMaterial->setValue("u_heightMap", waterTexture);

	//Terrain material from which particle starting points are derived
	ShaderDescription gridParticleShaderDesc;
	gridParticleShaderDesc.type = ShaderType::tessellationAndGeometry;
	gridParticleShaderDesc.vertexSrcPath = "./assets/shaders/GridPhongVert.glsl";
	gridParticleShaderDesc.controlSrcPath = "./assets/shaders/floorParticleTCS.glsl";
	gridParticleShaderDesc.evaluationSrcPath = "./assets/shaders/floorParticleTES.glsl";
	gridParticleShaderDesc.geometrySrcPath = "./assets/shaders/Geometry/GridGeoAdvanced.glsl";
	gridParticleShaderDesc.fragmentSrcPath = "./assets/shaders/GridPhongFrag.glsl";
	std::shared_ptr<Shader> gridParticleShader = std::make_shared<Shader>(gridParticleShaderDesc);
	std::shared_ptr<Material> gridParticleMaterial = std::make_shared<Material>(gridParticleShader);
	gridParticleMaterial->setPrimitive(GL_PATCHES);

	gridParticleMaterial->setValue("u_heightMap", terrainTexture);

	//Terrain material for depth pass (shadow pass)
	ShaderDescription gridDepthShaderDesc;
	gridDepthShaderDesc.type = ShaderType::tessellationAndGeometry;
	gridDepthShaderDesc.vertexSrcPath = "./assets/shaders/GridDepthVert.glsl";
	gridDepthShaderDesc.controlSrcPath = "./assets/shaders/floorDepthTCS.glsl";
	gridDepthShaderDesc.evaluationSrcPath = "./assets/shaders/floorDepthTES.glsl";
	gridDepthShaderDesc.geometrySrcPath = "./assets/shaders/Geometry/GridGeo.glsl";
	gridDepthShaderDesc.fragmentSrcPath = "./assets/shaders/GridDepthFrag.glsl";
	std::shared_ptr<Shader> gridDepthShader = std::make_shared<Shader>(gridDepthShaderDesc);
	std::shared_ptr<Material> gridDepthMaterial = std::make_shared<Material>(gridDepthShader);
	std::shared_ptr<Material> waterDepthMaterial = std::make_shared<Material>(gridDepthShader);
	gridDepthMaterial->setPrimitive(GL_PATCHES);
	waterDepthMaterial->setPrimitive(GL_PATCHES);

	gridDepthMaterial->setValue("u_heightMap", terrainTexture);
	waterDepthMaterial->setValue("u_heightMap", waterTexture);

	//Terrain and Water material for player collision 
	ShaderDescription gridColDesc;
	gridColDesc.type = ShaderType::tessellationAndGeometry;
	gridColDesc.vertexSrcPath = "./assets/shaders/GridColVert.glsl";
	gridColDesc.controlSrcPath = "./assets/shaders/floorColTCS.glsl";
	gridColDesc.evaluationSrcPath = "./assets/shaders/floorColTES.glsl";
	gridColDesc.geometrySrcPath = "./assets/shaders/Geometry/GridGeo.glsl";
	gridColDesc.fragmentSrcPath = "./assets/shaders/GridCollisionFrag.glsl";
	std::shared_ptr<Shader> gridColShader = std::make_shared<Shader>(gridColDesc);
	std::shared_ptr<Material> gridColMaterial = std::make_shared<Material>(gridColShader);	
	std::shared_ptr<Material> waterColMaterial = std::make_shared<Material>(gridColShader);
	gridColMaterial->setPrimitive(GL_PATCHES);	
	waterColMaterial->setPrimitive(GL_PATCHES);

	//gridColMaterial->setValue("u_normalMap", thingNormalTexture);
	gridColMaterial->setValue("u_heightMap", terrainTexture);

	//waterColMaterial->setValue("u_normalMap", thingNormalTexture);
	waterColMaterial->setValue("u_heightMap", waterTexture);

	//Character colour material (witch model) and thing (player model)
	ShaderDescription characterPhongShaderDesc;
	characterPhongShaderDesc.type = ShaderType::rasterization;
	characterPhongShaderDesc.vertexSrcPath = "./assets/shaders/CharacterPhongVert.glsl";
	characterPhongShaderDesc.fragmentSrcPath = "./assets/shaders/CharacterPhongFrag.glsl";
	std::shared_ptr<Shader> characterPhongShader = std::make_shared<Shader>(characterPhongShaderDesc);
	std::shared_ptr<Material> characterMaterial = std::make_shared<Material>(characterPhongShader);
	std::shared_ptr<Material> thingMaterial = std::make_shared<Material>(characterPhongShader);

	characterMaterial->setValue("u_albedoMap", characterDiffuseTexture);
	characterMaterial->setValue("u_specularMap", characterSpecularTexture);
	characterMaterial->setValue("u_normalMap", characterNormalTexture);
	characterMaterial->setValue("u_emissionMap", characterGlowTexture);
	characterMaterial->setValue("u_emission", m_emission);

	thingMaterial->setValue("u_albedoMap", thingDiffuseTexture);
	thingMaterial->setValue("u_specularMap", thingSpecularTexture);
	thingMaterial->setValue("u_normalMap", thingNormalTexture);
	thingMaterial->setValue("u_emissionMap", thingDiffuseTexture);

	//skybox material and screen overlay material
	ShaderDescription skyboxShaderDesc;
	skyboxShaderDesc.type = ShaderType::rasterization;
	skyboxShaderDesc.vertexSrcPath = "./assets/shaders/SkyboxVert.glsl";
	skyboxShaderDesc.fragmentSrcPath = "./assets/shaders/SkyboxFrag.glsl";
	std::shared_ptr<Shader> skyboxShader = std::make_shared<Shader>(skyboxShaderDesc);
	std::shared_ptr<Material> skyboxMaterial = std::make_shared<Material>(skyboxShader);
	std::shared_ptr<Material> cubeMapMaterial = std::make_shared<Material>(skyboxShader);

	skyboxMaterial->setValue("u_skybox", cubeMap);
	cubeMapMaterial->setValue("u_skybox", screenCubeMap);

	//Final screen post processing pass (screen effects) material
	ShaderDescription screenQuadShaderDesc;
	screenQuadShaderDesc.type = ShaderType::rasterization;
	screenQuadShaderDesc.vertexSrcPath = "./assets/shaders/QuadVert.glsl";
	screenQuadShaderDesc.fragmentSrcPath = "./assets/shaders/QuadFrag.glsl";
	std::shared_ptr<Shader> screenQuadShader = std::make_shared<Shader>(screenQuadShaderDesc);
	std::shared_ptr<Material> screenQuadMaterial = std::make_shared<Material>(screenQuadShader);

	//Defered rendering composition pass for light and shadow calculations
	ShaderDescription compositeScreenQuadShaderDesc;
	compositeScreenQuadShaderDesc.type = ShaderType::rasterization;
	compositeScreenQuadShaderDesc.vertexSrcPath = "./assets/shaders/QuadVert.glsl";
	compositeScreenQuadShaderDesc.fragmentSrcPath = "./assets/shaders/CompositeQuadFrag.glsl";
	std::shared_ptr<Shader> compositeScreenQuadShader = std::make_shared<Shader>(compositeScreenQuadShaderDesc);
	std::shared_ptr<Material> compositeScreenQuadMaterial = std::make_shared<Material>(compositeScreenQuadShader);

	//antialiassing screen shader
	ShaderDescription antialiasingScreenQuadShaderDesc;
	antialiasingScreenQuadShaderDesc.type = ShaderType::rasterization;
	antialiasingScreenQuadShaderDesc.vertexSrcPath = "./assets/shaders/QuadVert.glsl";
	antialiasingScreenQuadShaderDesc.fragmentSrcPath = "./assets/shaders/AntialiasingQuadFrag.glsl";
	std::shared_ptr<Shader> antialiasingScreenQuadShader = std::make_shared<Shader>(antialiasingScreenQuadShaderDesc);
	std::shared_ptr<Material> antialiasingScreenQuadMaterial = std::make_shared<Material>(antialiasingScreenQuadShader);

	antialiasingScreenQuadMaterial->setValue("u_ScreenSize", m_ScreenSize);

	//Depth pass shader for models (used for shadow pass)
	ShaderDescription depthShaderDesc;
	depthShaderDesc.type = ShaderType::rasterization;
	depthShaderDesc.vertexSrcPath = "./assets/shaders/DepthVert.glsl";
	depthShaderDesc.fragmentSrcPath = "./assets/shaders/DepthFrag.glsl";
	std::shared_ptr<Shader> depthShader = std::make_shared<Shader>(depthShaderDesc);
	std::shared_ptr<Material> depthMaterial = std::make_shared<Material>(depthShader);

	//Not in use// testing for grass billboard depth to use shadows
	ShaderDescription grassDepthShaderDesc;
	grassDepthShaderDesc.type = ShaderType::geometry;
	grassDepthShaderDesc.vertexSrcPath = "./assets/shaders/Billboard/BillboardDepthVert.glsl";
	grassDepthShaderDesc.geometrySrcPath = "./assets/shaders/Billboard/BillboardDepthGeo.glsl";
	grassDepthShaderDesc.fragmentSrcPath = "./assets/shaders/DepthFrag.glsl";
	std::shared_ptr<Shader> grassDepthShader = std::make_shared<Shader>(grassDepthShaderDesc);
	std::shared_ptr<Material> grassDepthMaterial = std::make_shared<Material>(grassDepthShader);

	//
	//ShaderDescription lightDepthShaderDesc;
	//lightDepthShaderDesc.type = ShaderType::rasterization;
	//lightDepthShaderDesc.vertexSrcPath = "./assets/shaders/Lab2/LightDepthVert.glsl";
	//lightDepthShaderDesc.fragmentSrcPath = "./assets/shaders/Lab2/DepthFrag.glsl";
	//std::shared_ptr<Shader> lightDepthShader = std::make_shared<Shader>(lightDepthShaderDesc);
	//std::shared_ptr<Material> lightDepthMaterial = std::make_shared<Material>(lightDepthShader);
	
	//Particle rendering material
	ShaderDescription ParticleShaderDesc;
	ParticleShaderDesc.type = ShaderType::geometry;
	ParticleShaderDesc.vertexSrcPath = "./assets/shaders/Particles/ParticleVert.glsl";
	ParticleShaderDesc.geometrySrcPath = "./assets/shaders/Particles/ParticleGeo.glsl";
	ParticleShaderDesc.fragmentSrcPath = "./assets/shaders/Particles/ParticleFrag.glsl";
	std::shared_ptr<Shader> ParticleShader = std::make_shared<Shader>(ParticleShaderDesc);
	std::shared_ptr<Material> ParticleMaterial = std::make_shared<Material>(ParticleShader);
	ParticleMaterial->setPrimitive(GL_POINTS);

	ParticleMaterial->setValue("u_billboardImage", particleTexture);
	ParticleMaterial->setValue("u_heightMap", terrainTexture);

	//Grass rendering material
	ShaderDescription billboardShaderDesc;
	billboardShaderDesc.type = ShaderType::geometry;
	billboardShaderDesc.vertexSrcPath = "./assets/shaders/Billboard/BillboardVert.glsl";
	billboardShaderDesc.geometrySrcPath = "./assets/shaders/Billboard/BillboardGeo.glsl";
	billboardShaderDesc.fragmentSrcPath = "./assets/shaders/Billboard/BillboardFrag.glsl";
	std::shared_ptr<Shader> billboardShader = std::make_shared<Shader>(billboardShaderDesc);
	std::shared_ptr<Material> billboardMaterial = std::make_shared<Material>(billboardShader);
	billboardMaterial->setPrimitive(GL_POINTS);

	billboardMaterial->setValue("u_billboardImage", billboardTexture);
	billboardMaterial->setValue("u_heightMap", terrainTexture);

	//Normal compute pass material for terrrain
	ShaderDescription normalTextureComputeDesc;
	normalTextureComputeDesc.type = ShaderType::compute;
	normalTextureComputeDesc.computeSrcPath = "./assets/shaders/compute_CDMnormals.glsl";
	std::shared_ptr<Shader> normalComputeShader = std::make_shared<Shader>(normalTextureComputeDesc);
	std::shared_ptr<Material> normalComputeMat = std::make_shared<Material>(normalComputeShader);

	//Normal compute pass material for water
	ShaderDescription waterNormalTextureComputeDesc;
	waterNormalTextureComputeDesc.type = ShaderType::compute;
	waterNormalTextureComputeDesc.computeSrcPath = "./assets/shaders/compute_CDMwaterNormals.glsl";
	std::shared_ptr<Shader> waterNormalComputeShader = std::make_shared<Shader>(waterNormalTextureComputeDesc);
	std::shared_ptr<Material> waterNormalComputeMat = std::make_shared<Material>(waterNormalComputeShader);

	//Displacement painting compute pass material for terrain
	ShaderDescription textureComputeDesc;
	textureComputeDesc.type = ShaderType::compute;
	textureComputeDesc.computeSrcPath = "./assets/shaders/compute_textureTest.glsl";
	std::shared_ptr<Shader> computeShader = std::make_shared<Shader>(textureComputeDesc);
	std::shared_ptr<Material> computeMat = std::make_shared<Material>(computeShader);

	//Water Noise based displacement compute pass
	textureComputeDesc.computeSrcPath = "./assets/shaders/compute_Water.glsl";
	std::shared_ptr<Shader> waterComputeShader = std::make_shared<Shader>(textureComputeDesc);
	std::shared_ptr<Material> waterComputeMat = std::make_shared<Material>(waterComputeShader);

	//Particle simulation compute pass material
	ShaderDescription particleComputeDesc;
	particleComputeDesc.type = ShaderType::compute;
	particleComputeDesc.computeSrcPath = "./assets/shaders/compute_Particles.glsl";
	std::shared_ptr<Shader> particleComputeShader = std::make_shared<Shader>(particleComputeDesc);
	std::shared_ptr<Material> particleComputeMat = std::make_shared<Material>(particleComputeShader);

	//Bezier quantifying compute pass to create bezier lookup table
	ShaderDescription bezierComputeDesc;
	bezierComputeDesc.type = ShaderType::compute;
	bezierComputeDesc.computeSrcPath = "./assets/shaders/compute_BezierLookUp.glsl";
	std::shared_ptr<Shader> bezierComputeShader = std::make_shared<Shader>(bezierComputeDesc);
	std::shared_ptr<Material> bezierComputeMat = std::make_shared<Material>(bezierComputeShader);

	//Draw to displacement map using a bezier
	ShaderDescription terrainSpellsComputeDesc;
	terrainSpellsComputeDesc.type = ShaderType::compute;
	terrainSpellsComputeDesc.computeSrcPath = "./assets/shaders/compute_terrainSpells.glsl";
	std::shared_ptr<Shader> computeTerrainSpellsShader = std::make_shared<Shader>(terrainSpellsComputeDesc);
	std::shared_ptr<Material> computeTerrainSpellsMat = std::make_shared<Material>(computeTerrainSpellsShader);

	//VAOs ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Floor and water VAOs
	std::shared_ptr<Grid> grid = std::make_shared<Grid>(100, 100, 1);
	std::vector<float> floor_vertices = grid->getVertices();
	std::vector<unsigned int> floor_indices = grid->getIndices();

	VBOLayout gridLayout = {
	{GL_FLOAT, 3}, // position
	{GL_FLOAT, 2}, // UV
	};

	std::shared_ptr<VAO> gridVAO;
	gridVAO = std::make_shared<VAO>(floor_indices);
	gridVAO->addVertexBuffer(floor_vertices, gridLayout);

	std::shared_ptr<VAO> gridDepthVAO;
	gridDepthVAO = std::make_shared<VAO>(floor_indices);
	gridDepthVAO->addVertexBuffer(grid->getVertexPositions(), depthLayout);

	//Character (witch) VAOs
	VBOLayout characterLayout = {
	{GL_FLOAT, 3},  // position
	{GL_FLOAT, 3},  // normal
	{GL_FLOAT, 2},  // UV
	{GL_FLOAT, 3}   // TAN
	};

	uint32_t attributeTypes = Model::VertexFlags::positions |
		Model::VertexFlags::normals |
		Model::VertexFlags::uvs |
		Model::VertexFlags::tangents;

	Model characterModel("./assets/models/witch/witch.obj", attributeTypes);
	std::shared_ptr<VAO> characterVAO;
	characterVAO = std::make_shared<VAO>(characterModel.m_meshes[0].indices);
	characterVAO->addVertexBuffer(characterModel.m_meshes[0].vertices, characterLayout);

	std::shared_ptr<VAO> characterDepthVAO;
	characterDepthVAO = std::make_shared<VAO>(characterModel.m_meshes[0].indices);
	characterDepthVAO->addVertexBuffer(characterModel.m_meshes[0].positions, depthLayout);

	//Thing (player model) VAOs
	Model thingModel("./assets/models/thing/thing.obj", attributeTypes);
	std::shared_ptr<VAO> thingVAO;
	thingVAO = std::make_shared<VAO>(thingModel.m_meshes[0].indices);
	thingVAO->addVertexBuffer(thingModel.m_meshes[0].vertices, characterLayout);

	std::shared_ptr<VAO> thingDepthVAO;
	thingDepthVAO = std::make_shared<VAO>(thingModel.m_meshes[0].indices);
	thingDepthVAO->addVertexBuffer(thingModel.m_meshes[0].positions, depthLayout);


	//Skybox VAOs
	std::vector<uint32_t> skyboxIndices(skyboxVertices.size() / 3);
	std::iota(skyboxIndices.begin(), skyboxIndices.end(), 0);

	std::shared_ptr<VAO> skyboxVAO;
	skyboxVAO = std::make_shared<VAO>(skyboxIndices);
	skyboxVAO->addVertexBuffer(skyboxVertices, {
		{GL_FLOAT, 3}  // position
	});

	//Grass VAOs
	std::vector<uint32_t> billboardIndices(billbPositions.size() / 3);
	std::iota(billboardIndices.begin(), billboardIndices.end(), 0);

	std::shared_ptr<VAO> billbVAO;
	billbVAO = std::make_shared<VAO>(billboardIndices);
	billbVAO->addVertexBuffer(billbPositions, { {GL_FLOAT,3} , {GL_FLOAT,1} });

	//Screen Quad VAOs
	std::shared_ptr<VAO> screenQuadVAO;
	screenQuadVAO = std::make_shared<VAO>(screenIndices);
	screenQuadVAO->addVertexBuffer(screenVertices, {
		{GL_FLOAT, 3},  // position
		{GL_FLOAT, 2}   // UV
	});

	//Actors ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Terrain Actor
	Actor floor;
	floor.geometry = gridVAO;
	floor.depthGeometry = gridVAO;
	floor.material = gridMaterial;
	floor.depthMaterial = gridDepthMaterial;
	floor.translation = glm::vec3(-50.f, -5.f, -50.f);
	floor.recalc();
	floorIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(floor);	

	//Water Actor
	floor.material = waterMaterial;
	floor.depthMaterial = waterDepthMaterial;
	floor.translation = glm::vec3(-50.f, -6.f, -50.f);
	floor.recalc();
	m_mainScene->m_actors.push_back(floor);

	floor.material = gridParticleMaterial;
	particleFloorIdx = m_terrainParticlesScene->m_actors.size();
	m_terrainParticlesScene->m_actors.push_back(floor);
	///

	//Character (Witch) Actor

	Actor Character;

	for (int i = 0; i < 10; i++) {
		Character.geometry = characterVAO;
		Character.depthGeometry = characterDepthVAO;
		Character.material = characterMaterial;
		Character.depthMaterial = depthMaterial;
		Character.translation = glm::vec3(Randomiser::uniformFloatBetween(-30.0, 30.0), -5, Randomiser::uniformFloatBetween(-30.0, 30.0));
		Character.scale = glm::vec3(5.0f);
		Character.recalc();
		characterIdx = m_mainScene->m_actors.size();
		m_mainScene->m_actors.push_back(Character);
	}

	//Player Actor

	Actor thing;

	for (int i = 0; i < 1; i++) {
		thing.geometry = thingVAO;
		thing.depthGeometry = thingDepthVAO;
		thing.material = thingMaterial;
		thing.depthMaterial = depthMaterial;
		thing.translation = glm::vec3(Randomiser::uniformFloatBetween(-30.0, 30.0), Randomiser::uniformFloatBetween(-5.0, -3.0), Randomiser::uniformFloatBetween(-30.0, 30.0));
		thing.scale = glm::vec3(0.5f);
		thing.recalc();
		characterIdx = m_mainScene->m_actors.size();
		m_mainScene->m_actors.push_back(thing);
	}

	///
	// Lights
	DirectionalLight dl;
	dl.direction = glm::normalize(glm::vec3(-0.4, -0.5, -0.9));
	dl.colour = glm::vec3(1, 0.5, 0.25);
	m_mainScene->m_directionalLights.push_back(dl);

	PointLight pointLight;
	uint32_t numPointLights = 10;

	for (int i = 0; i < numPointLights; i++) {
		pointLight.colour = glm::vec3(Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0), Randomiser::uniformFloatBetween(0.0, 1.0));
		pointLight.position = glm::vec3(Randomiser::uniformFloatBetween(-30.0, 30.0), -4, Randomiser::uniformFloatBetween(-30.0, 30.0));
		pointLight.constants = glm::vec3(Randomiser::uniformFloatBetween(5.0, 30.0),0,0);
		m_mainScene->m_pointLights.push_back(pointLight);
	}

	///
	// Skybox

	Actor cubeMapBox;
	cubeMapBox.geometry = skyboxVAO;
	cubeMapBox.material = skyboxMaterial;
	//skyboxMaterial->setValue("u_skyboxView", glm::inverse(cubeMapBox.transform));

	cubeMapBox.scale = glm::vec3(3, 3, 3);
	cubeMapBox.recalc();
	skyboxIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(cubeMapBox);

	///
	// Grass

	int numBillboards = 3000;

	for (int i = 0; i < numBillboards; i++) {
		billbPositions.push_back(Randomiser::uniformFloatBetween(0, 100.0));
		billbPositions.push_back(Randomiser::uniformFloatBetween(0, 0));
		billbPositions.push_back(Randomiser::uniformFloatBetween(0, 100.0));
		billbPositions.push_back(Randomiser::uniformFloatBetween(0.1,0.9 ));
	}

	Actor billBoard;
	
	billBoard.geometry = billbVAO;
	billBoard.material = billboardMaterial;
	billBoard.translation = glm::vec3(-50.f, -4.f, -50.f);
	billBoard.recalc();
	//billBoard.depthGeometry = billbVAO;
	//billBoard.depthMaterial = billboardMaterial;
	//billBoard.depthMaterial = depthMaterial;
	grassIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(billBoard);


	//Particles
	Actor particles;

	uint32_t numberOfParticles = 4096 * 4;

	terrainParticlesStartPoints = std::make_shared<SSBO>(sizeof(Particle) * numberOfParticles, numberOfParticles);
	terrainParticlesStartPoints->bind(0);
	terrainParticles = std::make_shared<SSBO>(sizeof(Particle) * numberOfParticles, numberOfParticles);
	terrainParticles->bind(1);
	//ParticleMaterial->setValue("particles", terrainParticles->getID());

	particles.SSBOgeometry = terrainParticlesStartPoints;
	particles.material = ParticleMaterial;
	//particles.SSBOdepthGeometry = terrainParticlesStartPoints;
	//particles.depthMaterial = ParticleMaterial;
	//particles.translation = glm::vec3(-5, -3, -5);
	particles.recalc();
	particlesIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(particles);

	//Main Camera

	Actor camera;
	cameraIdx = m_mainScene->m_actors.size();
	m_mainScene->m_actors.push_back(camera);

	/*************************
	*  Terrain Pass
	**************************/

	RenderPass terrain;
	terrain.scene = m_terrainParticlesScene;
	terrain.target = std::make_shared<FBO>(glm::ivec2(1,1), typicalLayout);

	m_terrainUpdateRenderer.addRenderPass(terrain);


	/*************************
	*  Compute Pass
	**************************/

	//Terrain:

	//Terrain Normals Compute Pass
	ComputePass normalTextureComputePass;
	normalTextureComputePass.material = normalComputeMat;
	normalTextureComputePass.workgroups = { 128,128,1 };
	normalTextureComputePass.barrier = MemoryBarrier::ShaderImageAccess;

	Image CDMnormals;
	CDMnormals.mipLevel = 0;
	CDMnormals.layered = false;
	CDMnormals.texture = terrainNormalTexure;
	CDMnormals.imageUnit = normalTextureComputePass.material->m_shader->m_imageBindingPoints["terrainNormalImg"];
	CDMnormals.access = TextureAccess::ReadWrite;

	//Water Normals Compute Pass
	ComputePass waterNormalTextureComputePass;
	waterNormalTextureComputePass.material = waterNormalComputeMat;
	waterNormalTextureComputePass.workgroups = { 128,128,1 };
	waterNormalTextureComputePass.barrier = MemoryBarrier::ShaderImageAccess;

	Image CDMwaterNormals;
	CDMwaterNormals.mipLevel = 0;
	CDMwaterNormals.layered = false;
	CDMwaterNormals.texture = waterNormalTexure;
	CDMwaterNormals.imageUnit = waterNormalTextureComputePass.material->m_shader->m_imageBindingPoints["waterNormalImg"];
	CDMwaterNormals.access = TextureAccess::ReadWrite;


	//Terrain Draw Compute Pass

	ComputePass textureComputePass;
	textureComputePass.material = computeMat;
	textureComputePass.workgroups = { 128,128,1 };
	textureComputePass.barrier = MemoryBarrier::ShaderImageAccess;

	Image img;
	img.mipLevel = 0;
	img.layered = false;
	img.texture = terrainTexture;
	img.imageUnit = textureComputePass.material->m_shader->m_imageBindingPoints["displacementImg"];
	img.access = TextureAccess::ReadWrite;

	textureComputePass.images.push_back(img);

	//img.imageUnit = normalTextureComputePass.material->m_shader->m_imageBindingPoints["displacementImg"];

	//Adding values of displacement for Normal calculations
	normalTextureComputePass.images.push_back(img);
	normalTextureComputePass.images.push_back(CDMnormals);

	terrainNormalsComputePassIdx = m_terrainUpdateRenderer.getSumPassCount();
	m_terrainUpdateRenderer.addComputePass(normalTextureComputePass);

	terrainComputePassIdx = m_terrainUpdateRenderer.getSumPassCount();
	m_terrainUpdateRenderer.addComputePass(textureComputePass);

	//Inisitalizing terrain with reset values
	m_terrainUpdateRenderer.getComputePass(terrainComputePassIdx).material->setValue("reset", (float)(int)resetTexture);

	//m_terrainUpdateRenderer.render();

	textureID = m_terrainUpdateRenderer.getComputePass(0).images[1].texture->getID();

	//Water compute noise pass
	ComputePass waterComputePass;
	waterComputePass.material = waterComputeMat;
	waterComputePass.workgroups = { 128,128,1 };
	waterComputePass.barrier = MemoryBarrier::ShaderImageAccess;
	Image water;
	water.mipLevel = 0;
	water.layered = false;
	water.texture = waterTexture;
	water.imageUnit = waterComputePass.material->m_shader->m_imageBindingPoints["waterImg"];
	water.access = TextureAccess::ReadWrite;

	waterComputePass.images.push_back(water);

	//Adding values of displacement for Normal calculations
	waterNormalTextureComputePass.images.push_back(water);
	waterNormalTextureComputePass.images.push_back(CDMwaterNormals);

	waterNormalsComputePassIdx = m_computeRenderer.getSumPassCount();
	m_computeRenderer.addComputePass(waterNormalTextureComputePass);

	waterComputePassIdx = m_computeRenderer.getSumPassCount();
	m_computeRenderer.addComputePass(waterComputePass);
	waterTextureID = m_terrainUpdateRenderer.getComputePass(1).images[0].texture->getID();

	
	
	//Particles Update Compute Pass
	ComputePass particleComputePass;
	particleComputePass.material = particleComputeMat;
	particleComputePass.workgroups = { 64,1,1 };
	particleComputePass.barrier = MemoryBarrier::ShaderStorageAccess;
	particleComputePass.ssbos.push_back(terrainParticlesStartPoints);
	particleComputePass.ssbos.push_back(terrainParticles);
	particleComputePass.images.push_back(img);

	m_computeRenderer.addComputePass(particleComputePass);


	uint32_t NumberOfSegments = 128;

	bezierLookUpTable = std::make_shared<SSBO>(sizeof(BezierTable) * 128, 128);
	bezierLookUpTable->bind(2);


	//Bezier Quantifying Compute Pass
	ComputePass bezierComputePass;
	bezierComputePass.material = bezierComputeMat;
	bezierComputePass.workgroups = { NumberOfSegments,1,1 };
	bezierComputePass.barrier = MemoryBarrier::ShaderStorageAccess;
	bezierComputePass.ssbos.push_back(bezierLookUpTable);

	m_bezierApproximation.addComputePass(bezierComputePass);

	m_terrainUpdateRenderer.render();


	//Bezier Application to displacment map Compute Pass
	ComputePass terrainSpellsComputePass;
	terrainSpellsComputePass.material = computeTerrainSpellsMat;
	terrainSpellsComputePass.workgroups = { 128,128,1 };
	terrainSpellsComputePass.barrier = MemoryBarrier::ShaderStorageAccess;
	terrainSpellsComputePass.images.push_back(img);
	terrainSpellsComputePass.ssbos.push_back(bezierLookUpTable);


	m_terrainSpells.addComputePass(terrainSpellsComputePass);





	/*************************
	*  Depth Shadow Pass
	**************************/


	DepthPass shadowPass;
	shadowPass.scene = m_mainScene;
	shadowPass.parseScene();
	shadowPass.target = std::make_shared<FBO>(glm::ivec2((1024.0 * 8)), shadowLayout);
	shadowPass.viewPort = { 0, 0, (1024 * 8 ), (1024  * 8) };
	
	ShadowMapVars shadowMapsVars;
	

	glm::vec3 lightPosition = shadowMapsVars.centre - (dl.direction * shadowMapsVars.distanceAlongLightVector);
	glm::mat4 shadowView = glm::lookAt(lightPosition, shadowMapsVars.centre, shadowMapsVars.UP);

	shadowPass.camera.view = shadowView;
	
	shadowPass.camera.projection = glm::ortho(-shadowMapsVars.orthoSize,
											   shadowMapsVars.orthoSize,
											  -shadowMapsVars.orthoSize,
											   shadowMapsVars.orthoSize,
											  -shadowMapsVars.orthoSize / 2,
											   shadowMapsVars.orthoSize * 2.5f);

	shadowPass.UBOmanager.setCachedValue("b_shadowCamera", "u_view", shadowPass.camera.view);
	shadowPass.UBOmanager.setCachedValue("b_shadowCamera", "u_projection", shadowPass.camera.projection);
	//shadowPass.UBOmanager.setCachedValue("b_shadowCamera", "u_viewPos", lightPosition);
	shadowPassIdx = m_mainRenderer.getSumPassCount();
	m_mainRenderer.addDepthPass(shadowPass);

	/*************************
	*  Screen Cube Map
	**************************/

	cubeMapBox.material = cubeMapMaterial;
	screencubeIdx = m_screenCubeMapScene->m_actors.size();
	m_screenCubeMapScene->m_actors.push_back(cubeMapBox);

	RenderPass cubeMapPass;
	cubeMapPass.scene = m_screenCubeMapScene;
	cubeMapPass.parseScene();
	cubeMapPass.target = std::make_shared<FBO>(m_winRef.getSize(), screenCubeMapLayout);
	cubeMapPass.camera.projection =  glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.f);
	cubeMapPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	cubeMapPass.camera.updateView(m_mainScene->m_actors.at(cameraIdx).transform);
	cubeMapPass.UBOmanager.setCachedValue("b_camera", "u_view", cubeMapPass.camera.view);
	cubeMapPass.UBOmanager.setCachedValue("b_camera", "u_projection", cubeMapPass.camera.projection);
	cubeMapPass.UBOmanager.setCachedValue("b_camera", "u_viewPos", m_mainScene->m_actors.at(cameraIdx).translation);

	m_mainRenderer.addRenderPass(cubeMapPass);

	/*************************
	*  Main Render Pass
	**************************/
	RenderPass mainPass;
	mainPass.scene = m_mainScene;
	mainPass.parseScene();
	mainPass.target = std::make_shared<FBO>(m_winRef.getSize(), typicalLayout);
	mainPass.camera.projection = glm::perspective(45.f, m_winRef.getWidthf() / m_winRef.getHeightf(), 0.1f, 1000.f);
	mainPass.viewPort = { 0, 0, m_winRef.getWidth(), m_winRef.getHeight() };
	mainPass.camera.updateView(m_mainScene->m_actors.at(cameraIdx).transform);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_view", mainPass.camera.view);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_projection", mainPass.camera.projection);
	mainPass.UBOmanager.setCachedValue("b_camera", "u_viewPos", m_mainScene->m_actors.at(cameraIdx).translation);
	
	mainPass.UBOmanager.setCachedValue("b_lights", "dLight.colour", m_mainScene->m_directionalLights.at(0).colour);
	mainPass.UBOmanager.setCachedValue("b_lights", "dLight.direction", m_mainScene->m_directionalLights.at(0).direction);
	//mainPass.UBOmanager.setCachedValue("")

	for (int i = 0; i < numPointLights; i++) {
		mainPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].colour", m_mainScene->m_pointLights.at(i).colour);
		mainPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].position", m_mainScene->m_pointLights.at(i).position);
		mainPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_mainScene->m_pointLights.at(i).constants);
	}
	//**************


	m_mainScene->m_actors.at(cameraIdx).attachScript<CameraScript>(mainPass.scene->m_actors.at(cameraIdx), m_winRef, &m_mainScene->m_actors.at(characterIdx).translation, glm::vec3(0, 2, 0), 10.0f, glm::vec3(2.6f, 1.6f, 5.f), 0.002f);
	m_mainScene->m_actors.at(characterIdx).attachScript<ControllerScript>(mainPass.scene->m_actors.at(characterIdx), mainPass.scene->m_actors.at(cameraIdx), m_winRef, glm::vec3(2.0f, 2.0f, 5.f), 1.0f);



	//**************
	
	//m_mainScene->m_actors.at(characterIdx).attachScript<RotationScript>(mainPass.scene->m_actors.at(characterIdx), glm::vec3(0.0f, 0.6f, 0.0f), GLFW_KEY_1);
	mainPassIdx = m_mainRenderer.getSumPassCount();
	m_mainRenderer.addRenderPass(mainPass);
	
	
	/*************************
	*  Player Collision Render Pass
	**************************/

	for (int i = 0; i < m_mainScene->m_actors.size(); i++) {
			

		if (i != cameraIdx && i != floorIdx && i != floorIdx + 1 && i != grassIdx && i != characterIdx) {
			m_collisionScene->m_actors.push_back(m_mainScene->m_actors.at(i));
			m_collisionScene->m_actors.at(i).material = colMaterial;
			m_collisionScene->m_actors.at(i).material->setValue("ID", glm::vec3((float)i / m_mainScene->m_actors.size()));
		}
		else if (i == floorIdx) {
			floor.material = gridColMaterial;
			floor.translation = glm::vec3(-50.f, -5.f, -50.f);
			floor.recalc();
			m_collisionScene->m_actors.push_back(floor);
			m_collisionScene->m_actors.at(i).material = gridColMaterial;
			m_collisionScene->m_actors.at(i).material->setValue("ID", glm::vec3((float)i / m_mainScene->m_actors.size()));
		}
		else if (i == floorIdx + 1) {
			floor.material = waterColMaterial;
			floor.translation = glm::vec3(-50.f, -6.f, -50.f);
			floor.recalc();
			m_collisionScene->m_actors.push_back(floor);
			m_collisionScene->m_actors.at(i).material = waterColMaterial;
			m_collisionScene->m_actors.at(i).material->setValue("ID", glm::vec3((float)i / m_mainScene->m_actors.size()));
		}
		else if (i == characterIdx) {
			Actor a;
			m_collisionScene->m_actors.push_back(a);
		}
		else {
			Actor camera;
			m_collisionScene->m_actors.push_back(camera);
		}

	}

	RenderPass colPass;
	colPass.scene = m_collisionScene;
	colPass.parseScene();
	colPass.target = std::make_shared<FBO>(glm::ivec2(colW,colH), colLayout);
	colPass.camera.projection = glm::perspective(179.99f, 1.0f, 0.1f, 1000.f);
	colPass.viewPort = { 0, 0, colW, colH };
	colPass.camera.updateView(glm::lookAt(camera.translation, camera.translation - glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)));// m_collisionScene->m_actors.at(cameraIdx).transform);
	colPass.UBOmanager.setCachedValue("b_cameraCol", "u_view", glm::lookAt(camera.translation,camera.translation - glm::vec3(0,1,0), glm::vec3(0,1,0)));
	colPass.UBOmanager.setCachedValue("b_cameraCol", "u_projection", colPass.camera.projection);
	colPass.UBOmanager.setCachedValue("b_cameraCol", "u_viewPos", m_collisionScene->m_actors.at(cameraIdx).translation);

	//mainPass.UBOmanager.setCachedValue("")


	
	//m_collisionScene->m_actors.at(cameraIdx).attachScript<CameraScript>(colPass.scene->m_actors.at(cameraIdx), m_winRef, glm::vec3(2.6f, 1.6f, 5.f), 2.5f);
	//m_mainScene->m_actors.at(characterIdx).attachScript<RotationScript>(mainPass.scene->m_actors.at(characterIdx), glm::vec3(0.0f, 0.6f, 0.0f), GLFW_KEY_1);
	playerColPassIdx = m_mainRenderer.getSumPassCount();
	m_mainRenderer.addRenderPass(colPass);
	

	/*******************************************************
	* Screen Passes
	*******************************************************/
	// QUAD




	Actor quad;
	quad.geometry = screenQuadVAO;



	

	///



	/*************************
	*  Composite Screen Render Pass
	**************************/
		

	compositeScreenQuadMaterial->setValue("u_gColour", mainPass.target->getTarget(0));
	compositeScreenQuadMaterial->setValue("u_gFragPosition", mainPass.target->getTarget(1));
	compositeScreenQuadMaterial->setValue("u_gNormal", mainPass.target->getTarget(2));
	compositeScreenQuadMaterial->setValue("u_gSpecMetallicRoughness", mainPass.target->getTarget(3));	

	compositeScreenQuadMaterial->setValue("u_gDepth", mainPass.target->getTarget(5));

	compositeScreenQuadMaterial->setValue("u_lightSpaceMatrix", shadowPass.camera.projection * shadowPass.camera.view);

	compositeScreenQuadMaterial->setValue("u_lightDepthMap", shadowPass.target->getTarget(0));

	quad.material = compositeScreenQuadMaterial;
	m_compositeScreenScene->m_actors.push_back(quad);


	RenderPass compositeScreenPass;
	compositeScreenPass.scene = m_compositeScreenScene;
	compositeScreenPass.parseScene();
	compositeScreenPass.target = std::make_shared<FBO>(m_winRef.getSize(), compositeLayout);
	compositeScreenPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };

	compositeScreenPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	compositeScreenPass.UBOmanager.setCachedValue("b_camera2D", "u_view2D", compositeScreenPass.camera.view);
	compositeScreenPass.UBOmanager.setCachedValue("b_camera2D", "u_projection2D", compositeScreenPass.camera.projection);

	//compositeScreenPass.camera.updateView(m_mainScene->m_actors.at(cameraIdx).transform);
	
	compositeScreenPass.UBOmanager.setCachedValue("b_camera", "u_view", mainPass.camera.view);
	compositeScreenPass.UBOmanager.setCachedValue("b_camera", "u_projection", mainPass.camera.projection);
	compositeScreenPass.UBOmanager.setCachedValue("b_camera", "u_viewPos", m_mainScene->m_actors.at(cameraIdx).translation);


	compositeScreenPass.UBOmanager.setCachedValue("b_lights", "dLight.colour", m_mainScene->m_directionalLights.at(0).colour);
	compositeScreenPass.UBOmanager.setCachedValue("b_lights", "dLight.direction", m_mainScene->m_directionalLights.at(0).direction);

	for (int i = 0; i < numPointLights; i++) {
		compositeScreenPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].colour", m_mainScene->m_pointLights.at(i).colour);
		compositeScreenPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].position", m_mainScene->m_pointLights.at(i).position);
		compositeScreenPass.UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_mainScene->m_pointLights.at(i).constants);
	}

	

	m_mainRenderer.addRenderPass(compositeScreenPass);

/*************************
*  Antialiasing Screen Render Pass
**************************/

	RenderPass antialiasingPass;
	antialiasingPass.scene = m_antialiasingScreenScene;
	antialiasingPass.parseScene();
	antialiasingPass.target = std::make_shared<FBO>(m_winRef.getSize(), antialiasingLayout);
	antialiasingPass.viewPort = { 0,0,m_winRef.getWidth(), m_winRef.getHeight() };

	antialiasingPass.camera.projection = glm::ortho(0.f, width, height, 0.f);
	antialiasingPass.UBOmanager.setCachedValue("b_camera2D", "u_view2D", antialiasingPass.camera.view);
	antialiasingPass.UBOmanager.setCachedValue("b_camera2D", "u_projection2D", antialiasingPass.camera.projection);

	antialiasingScreenQuadMaterial->setValue("u_inputTexture", compositeScreenPass.target->getTarget(0));
	

	quad.material = antialiasingScreenQuadMaterial;
	aaQuadIdx = m_screenScene->m_actors.size();
	m_antialiasingScreenScene->m_actors.push_back(quad);


	m_mainRenderer.addRenderPass(antialiasingPass);

	

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

	screenQuadMaterial->setValue("u_inputTexture", antialiasingPass.target->getTarget(0));
	screenQuadMaterial->setValue("u_UVmap", mainPass.target->getTarget(4));
	screenQuadMaterial->setValue("u_inputDepthTexture", mainPass.target->getTarget(5));
	screenQuadMaterial->setValue("u_gCubeMap", cubeMapPass.target->getTarget(0));
	screenQuadMaterial->setValue("u_terrainDisplacmentMap", terrainTexture);
	screenQuadMaterial->setValue("u_ScreenSize", m_ScreenSize);


	quad.material = screenQuadMaterial;
	quadIdx = m_screenScene->m_actors.size();
	m_screenScene->m_actors.push_back(quad);


	m_mainRenderer.addRenderPass(finalScreenPass);

	Buffer = terrainNormalTexure->getID();// shadowPass.target->getTarget(0)->getID();// colPass.target->getTarget(0)->getID();
	//cameraIdx = m_mainScene->m_actors.size() - cameraIdx;
}
void Scene0::onRender() const
{
	m_mainRenderer.render();
	m_computeRenderer.render();
	
}
void Scene0::onUpdate(float timestep)
{
	allTime += timestep;

	auto& camera = m_mainScene->m_actors.at(cameraIdx);
	auto& player = m_mainScene->m_actors.at(characterIdx);
	auto& pass = m_mainRenderer.getRenderPass(mainPassIdx);
	auto& colPass = m_mainRenderer.getRenderPass(playerColPassIdx);
	auto& ShadowPass = m_mainRenderer.getDepthPass(shadowPassIdx);

	auto floorMat = m_mainScene->m_actors.at(floorIdx).material;
	auto particleFloorMat = m_terrainParticlesScene->m_actors.at(particleFloorIdx).material;
	auto SkyboxMat = m_mainScene->m_actors.at(skyboxIdx).material;
	auto CubemapMat = m_screenCubeMapScene->m_actors.at(screencubeIdx).material;
	auto Quad = m_screenScene->m_actors.at(quadIdx).material;
	auto QuadComp = m_compositeScreenScene->m_actors.at(0).material;

	glm::mat4 b_cameraTrans = camera.transform;
	pass.camera.updateView(b_cameraTrans);
	glm::mat4 b_cameraView = pass.camera.view;
	glm::vec3 b_cameraViewPos = camera.translation;


	SkyboxMat->setValue("u_skyboxView", glm::mat4(glm::mat3(b_cameraView)));
	CubemapMat->setValue("u_skyboxView", glm::mat4(glm::mat3(b_cameraView)));

	Quad->setValue("u_Factor", m_Factor);
	Quad->setValue("u_Toggle", (float)m_Toggle);
	Quad->setValue("u_ScreenSize", m_ScreenSize);
	Quad->setValue("u_HSVColour", m_HSVcolours);

	// Player Collision sampling from gpu//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	colPass.target->use();
	glNamedFramebufferReadBuffer(colPass.target->getID(), GL_COLOR_ATTACHMENT0);

	float Data[4];
	float UVData[3];
	bool onFloor = false;
	glReadPixels(colW / 2, colH / 2, 1, 1, GL_RGBA, GL_FLOAT, &Data);
	std::dynamic_pointer_cast<ControllerScript>(player.m_script)->FloorCast = glm::vec3(Data[0], Data[1], Data[2]);
	if (-Data[1] <= 0.7f) {
		onFloor = true;
	}

	glNamedFramebufferReadBuffer(colPass.target->getID(), colPass.target->m_colAttachments[1]);
	glReadPixels(colW / 2, colH / 2, 1, 1, GL_RGB, GL_FLOAT, &UVData);
	//spdlog::info(" ObjectID: {}",glm::ceil(UVData[0] * m_collisionScene->m_actors.size()));
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Data of raycast from centre of screen///////////////////////////////////////////////////////////////////////////////////////////////////////////
	pass.target->use();
	//glNamedFramebufferReadBuffer(pass.target->getID(), pass.target->m_colAttachments[2]);

	float worldPosData[3];
	glNamedFramebufferReadBuffer(pass.target->getID(), pass.target->m_colAttachments[1]);
	glReadPixels(width / 2, height / 2, 1, 1, GL_RGB, GL_FLOAT, &worldPosData);
	worldPosRayTarget = glm::vec3(worldPosData[0], worldPosData[1], worldPosData[2]) + b_cameraViewPos;
	
	//2 = norm , 5 = uv
	glm::vec3 normal;

	float Data2[2];

	glm::vec2 UV;
	float blur = 0;
	glNamedFramebufferReadBuffer(pass.target->getID(), pass.target->m_colAttachments[2]);
	glReadPixels(width / 2, height / 2, 1, 1, GL_RGB, GL_FLOAT, &Data);
	normal = glm::vec3(Data[0], Data[1], Data[2]);
	glNamedFramebufferReadBuffer(pass.target->getID(), pass.target->m_colAttachments[4]);

	glReadPixels(width / 2, height / 2, 1, 1, GL_RG, GL_FLOAT, &Data2);
	UV = glm::vec2(Data2[0], Data2[1]) * 4096.0f;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Enable and disable editing mode////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (m_winRef.doIsKeyPressed(GLFW_KEY_INSERT) && !editLandScapeToggle) {
		editLandScape = !editLandScape;
		editLandScapeToggle = true;
	}
	else if (!m_winRef.doIsKeyPressed(GLFW_KEY_INSERT) && editLandScapeToggle) {
		editLandScapeToggle = false;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Landscape editing keybinds and logic
	if (editLandScape) {
		relMousePos = ImVec2(UV.x, UV.y);
		
		if (m_winRef.doIsKeyPressed(GLFW_KEY_P) && blur != 1) {
			blur = 1;
		}
		resetTexture = m_winRef.doIsKeyPressed(GLFW_KEY_R);

		if (m_winRef.doIsKeyPressed(GLFW_KEY_MINUS)) {
			m_size -= timestep * 200;
		}
		if (m_winRef.doIsKeyPressed(GLFW_KEY_EQUAL)) {
			m_size += timestep * 200;
		}
		if (m_winRef.doIsMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) {
			penColor = normal;
			m_brushMode = 1;
			mouseDown = true;

		}
		else if (m_winRef.doIsMouseButtonPressed(GLFW_MOUSE_BUTTON_2)) {

			penColor = normal;
			m_brushMode = -1;
			mouseDown = true;

		}
		else {
			mouseDown = false;
		}

		//Update landscape editing values
		auto& computePassMat = m_terrainUpdateRenderer.getComputePass(terrainComputePassIdx).material;
		computePassMat->setValue("mousepos", glm::vec2(relMousePos.x, relMousePos.y));
		computePassMat->setValue("mousedown", (float)(int)mouseDown);
		computePassMat->setValue("dt", timestep / 10);
		computePassMat->setValue("penColor", penColor);
		computePassMat->setValue("brushMode", m_brushMode);
		computePassMat->setValue("reset", (float)(int)resetTexture);
		computePassMat->setValue("size", m_size);

		if (mouseDown || resetTexture) {
			//Edit landscape
			m_terrainUpdateRenderer.render();
		}

	}
	else {
		//Draw beziers instead if not landscape editing
		auto& bezierMat = m_bezierApproximation.getComputePass(0).material;
		bezierMat->setValue("p0", worldPosRayStart);
		bezierMat->setValue("p1", worldPosRayStart + glm::max(2.0f, worldPosRayTarget.y - worldPosRayStart.y));
		bezierMat->setValue("p2", worldPosRayTarget + glm::max(2.0f,worldPosRayTarget.y - worldPosRayStart.y));
		bezierMat->setValue("p3", worldPosRayTarget);

		m_bezierApproximation.render();


		BezierProgress += timestep * (((float)(int)m_winRef.doIsMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) - 0.5f) * 5;

		if (BezierProgress < 0) {
			BezierProgress = 0;
		}
		else if (BezierProgress > 1) {
			BezierProgress = 1;
		}

		auto& terrainSpellsMat = m_terrainSpells.getComputePass(0).material;
		terrainSpellsMat->setValue("Progress", BezierProgress);
		m_terrainSpells.render();


		//Set new bezier information
		if (m_winRef.doIsMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) {
			worldPosRayTarget = glm::vec3(worldPosData[0], worldPosData[1], worldPosData[2]) + b_cameraViewPos;

			
			terrainSpellsMat->setValue("mousepos", glm::vec2(relMousePos.x, relMousePos.y));
			terrainSpellsMat->setValue("penColor", ((worldPosRayTarget - m_mainScene->m_actors.at(floorIdx).translation) / 25.0f) + glm::vec3(0.5f));
			terrainSpellsMat->setValue("size", m_size);


			
			mouseDown = true;
		}
		else if (m_winRef.doIsMouseButtonPressed(GLFW_MOUSE_BUTTON_2)) {
			relMousePos = ImVec2(UV.x, UV.y);
			worldPosRayStart = glm::vec3(worldPosData[0], worldPosData[1], worldPosData[2]) + b_cameraViewPos;

		}
		else {
			mouseDown = false;
		}
	
	}
	if (mouseDown) {
		
	}
	m_terrainUpdateRenderer.render();
	particleFloorMat->setValue("editing", (float)(int)editLandScape);

	//Update Lights in UBO with flickering effects
	for (int i = 0; i < 10; i++) {
		m_mainScene->m_pointLights.at(i).constants = LightConsts;
		glm::vec3 temp = m_mainScene->m_pointLights.at(i).position;

		glm::vec3 offset = glm::vec3((glm::sin((temp.x + (temp.z * 0.53423f)) + allTime * 1) + 1.0f)/2.0f, 0, 0);
		//m_mainScene->m_pointLights.at(i).position += glm::vec3(Randomiser::uniformFloatBetween(-25.0, 25.0) * timestep, Randomiser::uniformFloatBetween(-25.0, 25.0) * timestep, Randomiser::uniformFloatBetween(-25.0, 25.0) * timestep);
		m_mainRenderer.getRenderPass(mainPassIdx).UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].position", m_mainScene->m_pointLights.at(i).position);
		m_mainRenderer.getRenderPass(mainPassIdx).UBOmanager.setCachedValue("b_lights", "pLights[" + std::to_string(i) + "].constants", m_mainScene->m_pointLights.at(i).constants * offset * 10.0f);

	}

	//Set wire frame mode
	m_mainRenderer.getRenderPass(mainPassIdx).drawInWireFrame = m_wireFrame;

	// Update scripts
	for (auto it = m_mainScene->m_actors.begin(); it != m_mainScene->m_actors.end(); ++it)
	{
		it->onUpdate(timestep);
	}

	//Update Shadow camera dynamically ///////////////////////////////////////////////////////////////////////////////////////////////
	float ShadowBoxScale = 60;
	float cameraForwardDist = -ShadowBoxScale * 0.5f;
	float pixelSize = 1.0f / (1024.0f * 8.0f);

	glm::vec3 lightDir = m_mainScene->m_directionalLights.at(0).direction;// glm::normalize(glm::vec3(-0.2, -1.0, -0.5));
	glm::vec3 cameraForward = { -b_cameraTrans[2][0], -b_cameraTrans[2][1], -b_cameraTrans[2][2] };
	cameraForward = -glm::normalize(cameraForward * glm::vec3(1, 0, 1));
	glm::vec3 CameraOffsetView = (b_cameraViewPos) + (cameraForward * cameraForwardDist);

	glm::vec3 lightPosition = (CameraOffsetView - (lightDir * 60.0f));

	glm::mat4 shadowView = glm::lookAt(lightPosition, lightPosition + lightDir, glm::vec3(0, 1, 0));

	ShadowPass.camera.view = shadowView;
	ShadowPass.UBOmanager.setCachedValue("b_shadowCamera", "u_view", ShadowPass.camera.view);
	QuadComp->setValue("u_lightSpaceMatrix", ShadowPass.camera.projection * ShadowPass.camera.view);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Double down update camera values in all passes
	for (int i = 0; i < m_mainRenderer.getRenderPassCount(); i++) {
		auto& target = m_mainRenderer.getRenderPass(i);

		target.UBOmanager.setCachedValue("b_camera", "u_view", b_cameraView);
		target.UBOmanager.setCachedValue("b_camera", "u_viewPos", b_cameraViewPos);
	}

	//Collision Camera Update UBOS/////////////////////////////////////////////////////////////////////////////////////////////////
	auto& Colubo = m_mainRenderer.getRenderPass((playerColPassIdx));

	glm::mat4 downView = glm::lookAt(player.translation, player.translation - glm::vec3(0.01f, 1, 0.01f), glm::vec3(0, 1, 0));
	
	Colubo.camera.view = downView;
	Colubo.UBOmanager.setCachedValue("b_cameraCol", "u_view", Colubo.camera.view);
	Colubo.UBOmanager.setCachedValue("b_cameraCol", "u_viewPos", player.translation);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Water Collision and ripple Logic/////////////////////////////////////////////////////////////////////////////////////////////

	float waterUV[2];
	colPass.target->use();
	glNamedFramebufferReadBuffer(colPass.target->getID(), colPass.target->m_colAttachments[3]);



	rippleTime += timestep * 3;
	if (rippleTime > 2) {
		rippleTime -= 2;
	}

	if (glm::ceil(UVData[0] * m_collisionScene->m_actors.size()) == 2 && onFloor) {
		waterEaseIn += timestep/100;
		if (waterEaseIn > 0.009f) {
			waterEaseIn = 0.009f;
		}
		if (waterEaseOut > 0.0f) {
			waterEaseOut -= timestep / 100;
			if (waterEaseOut < 0.0f) {
				waterEaseOut = 0.0f;
			}
		}

		glReadPixels(colW / 2, colH / 2, 1, 1, GL_RG, GL_FLOAT, &waterUV);
		lastWaterPos = glm::vec2(waterUV[0], waterUV[1]);
	}
	else if (waterEaseIn > 0.0f) {
		waterEaseOut += timestep / 100;
		if (waterEaseOut > 0.03f) {
			waterEaseOut = 0.0f;
			waterEaseIn = 0.0f;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Update UI for Terrain manipulation
	Quad->setValue("u_posOnMap", glm::vec2(relMousePos.x, relMousePos.y));
	Quad->setValue("u_BrushSize", m_size);
	Quad->setValue("u_SurfaceNormal", penColor);
	Quad->setValue("u_ToggleShader", (float)(int)b);

	//Update Water Values for ripple (not functional atm)
	auto& WaterComputePassMat = m_computeRenderer.getComputePass(waterComputePassIdx).material;
	WaterComputePassMat->setValue("time", allTime);
	WaterComputePassMat->setValue("rippleTime", rippleTime);
	WaterComputePassMat->setValue("waterEaseIn", waterEaseIn);
	WaterComputePassMat->setValue("waterEaseOut", waterEaseOut);
	WaterComputePassMat->setValue("position", lastWaterPos);

	//Update dt value for Particles
	auto& computePassMat2 = m_computeRenderer.getComputePass(2).material;
	computePassMat2->setValue("dt", timestep);
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
			ImGui::Checkbox("Wireframe ", &m_wireFrame);

			if (ImGui::Button("Point Lights Overlay")) { m_Toggle = (int)!(bool)m_Toggle; }

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

			ImGui::ColorPicker3("HSV Colour", (float*)&m_HSVcolours, ImGuiColorEditFlags_DisplayHSV + ImGuiColorEditFlags_PickerHueWheel);
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