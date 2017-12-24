// pch
#include "PreCompiled.h"

#include "ProgramManager.h"
#include "Program.h"
#include "ShaderManager.h"

using namespace Voxel;

ProgramManager::~ProgramManager()
{
	releaseAll();
}

void Voxel::ProgramManager::initPrograms()
{
	auto& shaderManager = ShaderManager::getInstance();

	// Doesn't have to check if programs are valid. They throw exception if fails.

	auto voxelShaderLineVert = shaderManager.createShader("voxelShaderLine", "shaders/voxelShaderLine.vert", Voxel::Shader::Type::VERTEX);
	auto voxelShaderLineFrag = shaderManager.createShader("voxelShaderLine", "shaders/voxelShaderLine.frag", Voxel::Shader::Type::FRAGMENT);
	auto voxelShaderLineProgram = Program::create(voxelShaderLineVert, voxelShaderLineFrag);
	programs.emplace(PROGRAM_NAME::LINE_SHADER, voxelShaderLineProgram);

	auto voxelShaderBlockVert = shaderManager.createShader("voxelShaderBlock", "shaders/voxelShaderBlock.vert", Voxel::Shader::Type::VERTEX);
	auto voxelShaderBlockFrag = shaderManager.createShader("voxelShaderBlock", "shaders/voxelShaderBlock.frag", Voxel::Shader::Type::FRAGMENT);
	auto voxelShaderBlockProgram = Program::create(voxelShaderBlockVert, voxelShaderBlockFrag);
	programs.emplace(PROGRAM_NAME::BLOCK_SHADER, voxelShaderBlockProgram);

	auto voxelShaderTextureVert = shaderManager.createShader("voxelShaderTextureColor", "shaders/voxelShaderUITexture.vert", Voxel::Shader::Type::VERTEX);
	auto voxelShaderTextureFrag = shaderManager.createShader("voxelShaderTextureColor", "shaders/voxelShaderUITexture.frag", Voxel::Shader::Type::FRAGMENT);
	auto voxelShaderTextureProgram = Program::create(voxelShaderTextureVert, voxelShaderTextureFrag);
	programs.emplace(PROGRAM_NAME::UI_TEXTURE_SHADER, voxelShaderTextureProgram);

	auto voxelShaderTextVert = shaderManager.createShader("voxelShaderText", "shaders/voxelShaderUIText.vert", Voxel::Shader::Type::VERTEX);
	auto voxelShaderTextFrag = shaderManager.createShader("voxelShaderText", "shaders/voxelShaderUIText.frag", Voxel::Shader::Type::FRAGMENT);
	auto voxelShaderTextProgram = Program::create(voxelShaderTextVert, voxelShaderTextFrag);
	programs.emplace(PROGRAM_NAME::UI_TEXT_SHADER, voxelShaderTextProgram);
	
	auto voxelShaderSkyboxVert = shaderManager.createShader("voxelShaderSkybox", "shaders/voxelShaderSkybox.vert", Voxel::Shader::Type::VERTEX);
	auto voxelShaderSkyboxFrag = shaderManager.createShader("voxelShaderSkybox", "shaders/voxelShaderSkybox.frag", Voxel::Shader::Type::FRAGMENT);
	auto voxelShaderSkyboxProgram = Program::create(voxelShaderSkyboxVert, voxelShaderSkyboxFrag);
	programs.emplace(PROGRAM_NAME::SKYBOX_SHADER, voxelShaderSkyboxProgram);

	auto voxelShaderPolygonVert = shaderManager.createShader("voxelShaderPolygon", "shaders/voxelShaderPolygon.vert", Voxel::Shader::Type::VERTEX);
	auto voxelShaderPolygonFrag = shaderManager.createShader("voxelShaderPolygon", "shaders/voxelShaderPolygon.frag", Voxel::Shader::Type::FRAGMENT);
	auto voxelShaderPolygonProgram = Program::create(voxelShaderPolygonVert, voxelShaderPolygonFrag);
	programs.emplace(PROGRAM_NAME::POLYGON_SHADER, voxelShaderPolygonProgram);

	auto voxelShaderPolygonSideVert = shaderManager.createShader("voxelShaderPolygonSide", "shaders/voxelShaderPolygonSide.vert", Voxel::Shader::Type::VERTEX);
	auto voxelShaderPolygonSideFrag = shaderManager.createShader("voxelShaderPolygonSide", "shaders/voxelShaderPolygonSide.frag", Voxel::Shader::Type::FRAGMENT);
	auto voxelShaderPolygonSideProgram = Program::create(voxelShaderPolygonSideVert, voxelShaderPolygonSideFrag);
	programs.emplace(PROGRAM_NAME::POLYGON_SIDE_SHADER, voxelShaderPolygonSideProgram);

	auto voxelShaderColorPickerVert = shaderManager.createShader("voxelShaderColorPicker", "shaders/voxelShaderColorPicker.vert", Voxel::Shader::Type::VERTEX);
	auto voxelShaderColorPickerFrag = shaderManager.createShader("voxelShaderColorPicker", "shaders/voxelShaderColorPicker.frag", Voxel::Shader::Type::FRAGMENT);
	auto voxelShaderColorPickerProgram = Program::create(voxelShaderColorPickerVert, voxelShaderColorPickerFrag);
	programs.emplace(PROGRAM_NAME::UI_COLOR_PICKER_SHADER, voxelShaderColorPickerProgram);

	auto voxelShaderParticleSystemVert = shaderManager.createShader("voxelShaderParticleSystem", "shaders/voxelShaderParticleSystem.vert", Voxel::Shader::Type::VERTEX);
	auto voxelShaderParticleSystemFrag = shaderManager.createShader("voxelShaderParticleSystem", "shaders/voxelShaderParticleSystem.frag", Voxel::Shader::Type::FRAGMENT);
	auto voxelShaderParticleSystemProgram = Program::create(voxelShaderParticleSystemVert, voxelShaderParticleSystemFrag);
	programs.emplace(PROGRAM_NAME::UI_PARTICLE_SYSTEM_SHADER, voxelShaderParticleSystemProgram);
	
	// Don't need shader anymore if it's attached to program
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

void Voxel::ProgramManager::updateProjMat(const glm::mat4 & projMat)
{
	programs.at(PROGRAM_NAME::LINE_SHADER)->use(true);
	programs.at(PROGRAM_NAME::LINE_SHADER)->setUniformMat4("projMat", projMat);

	programs.at(PROGRAM_NAME::BLOCK_SHADER)->use(true);
	programs.at(PROGRAM_NAME::BLOCK_SHADER)->setUniformMat4("projMat", projMat);

	programs.at(PROGRAM_NAME::POLYGON_SHADER)->use(true);
	programs.at(PROGRAM_NAME::POLYGON_SHADER)->setUniformMat4("projMat", projMat);

	programs.at(PROGRAM_NAME::POLYGON_SIDE_SHADER)->use(true);
	programs.at(PROGRAM_NAME::POLYGON_SIDE_SHADER)->setUniformMat4("projMat", projMat);
}

void Voxel::ProgramManager::updateUIProjMat(const glm::mat4 & uiProjMat)
{
	programs.at(PROGRAM_NAME::UI_TEXTURE_SHADER)->use(true);
	programs.at(PROGRAM_NAME::UI_TEXTURE_SHADER)->setUniformMat4("projMat", uiProjMat);

	programs.at(PROGRAM_NAME::UI_TEXT_SHADER)->use(true);
	programs.at(PROGRAM_NAME::UI_TEXT_SHADER)->setUniformMat4("projMat", uiProjMat);

	programs.at(PROGRAM_NAME::UI_COLOR_PICKER_SHADER)->use(true);
	programs.at(PROGRAM_NAME::UI_COLOR_PICKER_SHADER)->setUniformMat4("projMat", uiProjMat);

	programs.at(PROGRAM_NAME::UI_PARTICLE_SYSTEM_SHADER)->use(true);
	programs.at(PROGRAM_NAME::UI_PARTICLE_SYSTEM_SHADER)->setUniformMat4("projMat", uiProjMat);
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