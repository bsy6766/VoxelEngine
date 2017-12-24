// pch
#include "PreCompiled.h"

#include "ParticleSystem.h"

// voxel
#include "Particle.h"
#include "DataTree.h"
#include "FileSystem.h"
#include "SpriteSheet.h"
#include "Program.h"
#include "ProgramManager.h"
#include "Logger.h"
#include "Config.h"
#include "Quad.h"
#include "Utility.h"
#include "Random.h"

Voxel::UI::ParticleSystem::ParticleSystem(const std::string & name)
	: RenderNode(name)
	, mode(Mode::GRAVITY_MODE)
	, positionType(PositionType::PT_RELATIVE)
	, livingParticleNumber(0)
	, emitPos(0.0f)
	, emitPosVar(0.0f)
	, gravity(0.0f)
	, speed(0.0f)
	, speedVar(0.0f)
	, accelRad(0.0f)
	, accelRadVar(0.0f)
	, accelTan(0.0f)
	, accelTanVar(0.0f)
	, particleSize(0)
	, emitAngle(0.0f)
	, emitAngleVar(0.0f)
	, particleLifeSpan(0.0f)
	, particleLifeSpanVar(0.0f)
	, duration(0.0f)
	, elapsedTime(0.0f)
	, startColor(0.0f)
	, startColorVar(0.0f)
	, endColor(0.0f)
	, endColorVar(0.0f)
	, blendSrc(0)
	, blendDest(0)
	, spawnPoint(0.0f)
	, rand(new Random(name))
{}

Voxel::UI::ParticleSystem::~ParticleSystem()
{
	for(auto particle : particles)
	{
		if (particle)
		{
			delete particle;
		}
	}

	particles.clear();

	if (rand)
	{
		delete rand;
		rand = nullptr;
	}
}

bool Voxel::UI::ParticleSystem::init(Voxel::DataTree* particleSystemDataTree)
{	
	auto ssm = &Voxel::SpriteSheetManager::getInstance();

	if (ssm == nullptr) return false;

	program = Voxel::ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::UI_PARTICLE_SYSTEM_SHADER);

	if (program == nullptr) return false;

	const std::string spriteSheetName = particleSystemDataTree->getString("spriteSheetName");

	if (spriteSheetName.empty()) return false;

	auto ss = ssm->getSpriteSheetByKey(spriteSheetName);

	if (ss == nullptr) return false;

	this->texture = ss->getTexture();

	if (this->texture == nullptr) return false;

	const std::string particleTextureName = particleSystemDataTree->getString("textureName");

	if (particleTextureName.empty()) return false;

	auto ie = ss->getImageEntry(particleTextureName);

	if (ie == nullptr) return false;

	bool result = setAttributesWithFile(particleSystemDataTree);

	if (result == false) return false;

	// init particles
	for (unsigned int i = 0; i < particleSize; ++i)
	{
		// all new particle data are default.
		particles.push_back(new Particle());
	}
	
	auto size = glm::vec2(ie->width, ie->height);
	float widthHalf = size.x * 0.5f;
	float heightHalf = size.y * 0.5f;

	std::array<float, 12> vertices = { -widthHalf, -heightHalf, 0.0f, -widthHalf, heightHalf, 0.0f, widthHalf, -heightHalf, 0.0f, widthHalf, heightHalf, 0.0f };
	
	auto& uvOrigin = ie->uvOrigin;
	auto& uvEnd = ie->uvEnd;

	std::array<float, 8> uvs = { uvOrigin.x, uvOrigin.y, uvOrigin.x, uvEnd.y, uvEnd.x, uvOrigin.y, uvEnd.x, uvEnd.y };

	auto indices = Quad::indices;

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	// gen vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// gen vbo
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	GLint vertLoc = program->getAttribLocation("vert");

	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribDivisor(vertLoc, 0);		//0. Always use same quad vertex

	// gen uvbo
	GLuint uvbo;
	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), &uvs.front(), GL_STATIC_DRAW);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribDivisor(uvVertLoc, 0);	//0. Always use same indices

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &uvbo);


#if V_DEBUG && V_DEBUG_LOG_CONSOLE
	auto logger = &Voxel::Logger::getInstance();

	logger->consoleInfo("[ParticleSystem] Creating particle system");
	logger->consoleInfo("[ParticleSystem] Name = " + name);
	logger->consoleInfo("[ParticleSystem] Duration = " + std::to_string(duration));
	logger->consoleInfo("[ParticleSystem] Total particles = " + std::to_string(particleSize));
	logger->consoleInfo("[ParticleSystem] Particle life span = " + std::to_string(particleLifeSpan) + " +/- " + std::to_string(particleLifeSpanVar));
	logger->consoleInfo("[ParticleSystem] Emiision rate = " + std::to_string(emissionRate));
	logger->consoleInfo("[ParticleSystem] Emission angle = " + std::to_string(emitAngle) + " +/- " + std::to_string(emitAngleVar));
	logger->consoleInfo("[ParticleSystem] Emission position = (" + std::to_string(emitPos.x) + ", " + std::to_string(emitPos.y) + ") +/- (" + std::to_string(emitPosVar.x) + ", " + std::to_string(emitPosVar.y) + ")");
	logger->consoleInfo("[ParticleSystem] Speed = " + std::to_string(speed) + " +/- " + std::to_string(speedVar));
	logger->consoleInfo("[ParticleSystem] Color start = (" + std::to_string(startColor.r) + ", " + std::to_string(startColor.g) + ", " + std::to_string(startColor.b) + ") +/- " + std::to_string(startColorVar.r) + ", " + std::to_string(startColorVar.g) + ", " + std::to_string(startColorVar.b) + ")");
	logger->consoleInfo("[ParticleSystem] Color end = (" + std::to_string(endColor.r) + ", " + std::to_string(endColor.g) + ", " + std::to_string(endColor.b) + ") +/- " + std::to_string(endColorVar.r) + ", " + std::to_string(endColorVar.g) + ", " + std::to_string(endColorVar.b) + ")");
	logger->consoleInfo("[ParticleSystem] Radial acceleration = " + std::to_string(accelRad) + " +/- " + std::to_string(accelRadVar));
	logger->consoleInfo("[ParticleSystem] Tangent acceleration = " + std::to_string(accelTan) + " +/- " + std::to_string(accelTan));
	logger->consoleInfo("[ParticleSystem] Gravity X = " + std::to_string(gravity.x));
	logger->consoleInfo("[ParticleSystem] Gravity Y = " + std::to_string(gravity.y));
	logger->consoleInfo("[ParticleSystem] Size start = " + std::to_string(startSize) + " +/- " + std::to_string(startSizeVar));
	logger->consoleInfo("[ParticleSystem] Size end = " + std::to_string(endSize) + " +/- " + std::to_string(endSizeVar));
	logger->consoleInfo("[ParticleSystem] BlendFunc src = " + std::to_string(blendSrc) + ", dest = " + std::to_string(blendDest));

	if (blendSrc == 770 && blendDest == 771)
	{
		logger->consoleInfo("[ParticleSystem] BlendFunc is Normal");
	}
	else if (blendSrc == 770 && blendDest == 1)
	{
		logger->consoleInfo("[ParticleSystem] BlendFunc is Additive");
	}
#endif

	return true;
}

Voxel::UI::ParticleSystem * Voxel::UI::ParticleSystem::create(const std::string & name, const std::string & dataFileName)
{
	if (dataFileName.empty()) return false;

	auto psDataTree = Voxel::DataTree::create(Voxel::FileSystem::getInstance().getWorkingDirectory() + "/" + dataFileName);

	if (psDataTree)
	{
		auto ps = new ParticleSystem(name);

		if (ps->init(psDataTree))
		{
			return ps;
		}
		else
		{
			delete ps;
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

bool Voxel::UI::ParticleSystem::setAttributesWithFile(Voxel::DataTree * psDataTree)
{
	duration = psDataTree->getFloat("duration");

	if (duration < -1)
	{
		duration = -1;
	}

	particleSize = psDataTree->getInt("totalParticles");

	if (particleSize < 0)
	{
		particleSize = 0;
		emissionRate = 0.0f;
	}
	else
	{
		emissionRate = static_cast<float>(particleSize) / duration;
	}

	particleLifeSpan = psDataTree->getFloat("particleLifeSpan");

	if (particleLifeSpan < 0.0f)
	{
		particleLifeSpan = 0.0f;
	}

	particleLifeSpanVar = psDataTree->getFloat("particleLifeSpanVar");

	if (particleLifeSpanVar < 0.0f)
	{
		particleLifeSpanVar = 0.0f;
	}

	blendSrc = psDataTree->getInt("blendSrc");
	blendDest = psDataTree->getInt("blendDest");

	emitPos.x = psDataTree->getFloat("position.x");
	emitPos.y = psDataTree->getFloat("position.y");

	emitPosVar.x = psDataTree->getFloat("positionVar.x");
	emitPosVar.y = psDataTree->getFloat("positionVar.y");

	// [0.0f, 1.0f]
	startColor.r = glm::clamp(psDataTree->getFloat("color.start.r"), 0.0f, 1.0f);
	startColor.g = glm::clamp(psDataTree->getFloat("color.start.g"), 0.0f, 1.0f);
	startColor.b = glm::clamp(psDataTree->getFloat("color.start.b"), 0.0f, 1.0f);

	// [0.0f, 1.0f]
	startColorVar.r = glm::clamp(psDataTree->getFloat("color.startVar.r"), 0.0f, 1.0f);
	startColorVar.g = glm::clamp(psDataTree->getFloat("color.startVar.g"), 0.0f, 1.0f);
	startColorVar.b = glm::clamp(psDataTree->getFloat("color.startVar.b"), 0.0f, 1.0f);

	// [0.0f, 1.0f]
	endColor.r = glm::clamp(psDataTree->getFloat("color.end.r"), 0.0f, 1.0f);
	endColor.g = glm::clamp(psDataTree->getFloat("color.end.g"), 0.0f, 1.0f);
	endColor.b = glm::clamp(psDataTree->getFloat("color.end.b"), 0.0f, 1.0f);

	// [0.0f, 1.0f]
	endColorVar.r = glm::clamp(psDataTree->getFloat("color.endVar.r"), 0.0f, 1.0f);
	endColorVar.g = glm::clamp(psDataTree->getFloat("color.endVar.g"), 0.0f, 1.0f);
	endColorVar.b = glm::clamp(psDataTree->getFloat("color.endVar.b"), 0.0f, 1.0f);

	speed = psDataTree->getFloat("speed");
	speedVar = psDataTree->getFloat("speedVar");

	gravity.x = psDataTree->getFloat("gravity.x");
	gravity.y = psDataTree->getFloat("gravity.y");

	accelTan = psDataTree->getFloat("accel.tan");
	accelTanVar = psDataTree->getFloat("accel.tanVar");

	accelRad = psDataTree->getFloat("accel.rad");
	accelRadVar = psDataTree->getFloat("accel.radVar");
	
	// [-180.0f, 180.0f]. Final angle range [-360.0f, 360.0f]
	emitAngle = glm::clamp(psDataTree->getFloat("emitAngle"), -180.0f, 180.0f);
	emitAngleVar = glm::clamp(psDataTree->getFloat("emitAngleVar"), -180.0f, 180.0f);

	startSize = psDataTree->getFloat("size.start");
	startSizeVar = psDataTree->getFloat("size.startVar");

	endSize = psDataTree->getFloat("size.end");
	endSizeVar = psDataTree->getFloat("size.endVar");

	return true;
}

void Voxel::UI::ParticleSystem::update(const float delta)
{
	float actualTime = 0.0f;

	if (duration == -1)
	{
		// infinite
		actualTime = delta;
	}
	else
	{
		elapsedTime += delta;
		
		if (elapsedTime > duration)
		{
			float exceededTime = elapsedTime - duration;
			
			actualTime = delta - exceededTime;
		}
	}

	spawnPoint += (emissionRate * actualTime);

	int newParticleNumber = static_cast<int>(glm::floor(spawnPoint));

	if (newParticleNumber > 0)
	{
		spawnPoint -= static_cast<float>(newParticleNumber);

		int counter = newParticleNumber;
		while (counter > 0)
		{
			Particle* lastParticle = particles.back();

			

			counter--;
		}
	}
	// Else, nothing to spawn

	Voxel::UI::TransformNode::update(delta);
}

void Voxel::UI::ParticleSystem::renderSelf()
{
	if (texture == nullptr) return;
	if (program == nullptr) return;

	program->use(true);
	program->setUniformMat4("modelMat", glm::scale(modelMat, glm::vec3(scale, 1)));
	program->setUniformFloat("opacity", opacity);
	program->setUniformVec3("color", color);

	texture->activate(GL_TEXTURE0);
	texture->bind();

	if (vao)
	{
		glBindVertexArray(vao);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, livingParticleNumber);
	}
}
