#include "ProgramManager.h"
#include "Program.h"
#include <ShaderManager.h>

using namespace Voxel;

ProgramManager::~ProgramManager()
{
	releaseAll();
}

void Voxel::ProgramManager::initPrograms()
{
	auto& shaderManager = ShaderManager::getInstance();

	auto voxelShaderLineVert = shaderManager.createShader("voxelShaderLine", "shaders/voxelShaderLine.vert", GL_VERTEX_SHADER);
	auto voxelShaderLineFrag = shaderManager.createShader("voxelShaderLine", "shaders/voxelShaderLine.frag", GL_FRAGMENT_SHADER);
	auto voxelShaderLineProgram = Program::create(voxelShaderLineVert, voxelShaderLineFrag);
	programs.emplace(PROGRAM_NAME::LINE_SHADER, voxelShaderLineProgram);

	auto voxelShaderBlockVert = shaderManager.createShader("voxelShaderBlock", "shaders/voxelShaderBlock.vert", GL_VERTEX_SHADER);
	auto voxelShaderBlockFrag = shaderManager.createShader("voxelShaderBlock", "shaders/voxelShaderBlock.frag", GL_FRAGMENT_SHADER);
	auto voxelShaderBlockProgram = Program::create(voxelShaderBlockVert, voxelShaderBlockFrag);
	programs.emplace(PROGRAM_NAME::BLOCK_SHADER, voxelShaderBlockProgram);

	auto voxelShaderTextureColorVert = shaderManager.createShader("voxelShaderTextureColor", "shaders/voxelShaderTextureColor.vert", GL_VERTEX_SHADER);
	auto voxelShaderTextureColorFrag = shaderManager.createShader("voxelShaderTextureColor", "shaders/voxelShaderTextureColor.frag", GL_FRAGMENT_SHADER);
	auto voxelShaderTextureColorProgram = Program::create(voxelShaderTextureColorVert, voxelShaderTextureColorFrag);
	programs.emplace(PROGRAM_NAME::TEXTURE_SHADER, voxelShaderTextureColorProgram);

	auto voxelShaderTextVert = shaderManager.createShader("voxelShaderText", "shaders/voxelShaderText.vert", GL_VERTEX_SHADER);
	auto voxelShaderTextFrag = shaderManager.createShader("voxelShaderText", "shaders/voxelShaderText.frag", GL_FRAGMENT_SHADER);
	auto voxelShaderTextProgram = Program::create(voxelShaderTextVert, voxelShaderTextFrag);
	programs.emplace(PROGRAM_NAME::TEXT_SHADER, voxelShaderTextProgram);
	
	auto voxelShaderSkyboxVert = shaderManager.createShader("voxelShaderSkybox", "shaders/voxelShaderSkybox.vert", GL_VERTEX_SHADER);
	auto voxelShaderSkyboxFrag = shaderManager.createShader("voxelShaderSkybox", "shaders/voxelShaderSkybox.frag", GL_FRAGMENT_SHADER);
	auto voxelShaderSkyboxProgram = Program::create(voxelShaderSkyboxVert, voxelShaderSkyboxFrag);
	programs.emplace(PROGRAM_NAME::SKYBOX_SHADER, voxelShaderSkyboxProgram);

	auto voxelShaderPolygonVert = shaderManager.createShader("voxelShaderPolygon", "shaders/voxelShaderPolygon.vert", GL_VERTEX_SHADER);
	auto voxelShaderPolygonFrag = shaderManager.createShader("voxelShaderPolygon", "shaders/voxelShaderPolygon.frag", GL_FRAGMENT_SHADER);
	auto voxelShaderPolygonProgram = Program::create(voxelShaderPolygonVert, voxelShaderPolygonFrag);
	programs.emplace(PROGRAM_NAME::POLYGON_SHADER, voxelShaderPolygonProgram);

	auto voxelShaderPolygonSideVert = shaderManager.createShader("voxelShaderPolygonSide", "shaders/voxelShaderPolygonSide.vert", GL_VERTEX_SHADER);
	auto voxelShaderPolygonSideFrag = shaderManager.createShader("voxelShaderPolygonSide", "shaders/voxelShaderPolygonSide.frag", GL_FRAGMENT_SHADER);
	auto voxelShaderPolygonSideProgram = Program::create(voxelShaderPolygonSideVert, voxelShaderPolygonSideFrag);
	programs.emplace(PROGRAM_NAME::POLYGON_SIDE_SHADER, voxelShaderPolygonSideProgram);

	shaderManager.releaseAll();
}

Program * Voxel::ProgramManager::getProgram(const PROGRAM_NAME programID)
{
	if (programID >= 0 && programID < SHADER_MAX_COUNT)
	{
		return programs[programID];
	}
	else
	{
		return nullptr;
	}
}

void Voxel::ProgramManager::useDefaultProgram(const PROGRAM_NAME programID)
{
	auto find_it = programs.find(programID);
	if (find_it != programs.end())
	{
		programs[programID]->use(true);
	}
	else
	{
		throw std::runtime_error("Bad program use");
	}
}

void Voxel::ProgramManager::updateProjMat(const glm::mat4 & projMat)
{
	for (auto& e : programs)
	{
		if ((e.first) == PROGRAM_NAME::SKYBOX_SHADER)
		{
			continue;
		}
		(e.second)->setUniformMat4("projMat", projMat);
	}
}

void ProgramManager::releaseAll()
{
	for (auto it : programs)
	{
		if (it.second)
		{
			//second.release();
			delete it.second;
		}
	}

	programs.clear();
}