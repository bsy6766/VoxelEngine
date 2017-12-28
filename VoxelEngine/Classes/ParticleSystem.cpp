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
	, totalParticles(0)
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
	, textureSize(0.0f)
	, startSize(0.0f)
	, startSizeVar(0.0f)
	, endSize(0.0f)
	, endSizeVar(0.0f)
	, startAngle(0.0f)
	, startAngleVar(0.0f)
	, endAngle(0.0f)
	, endAngleVar(0.0f)
	, blendSrc(0)
	, blendDest(0)
	, state(State::UNINITIALIZED)
	, spawnPoint(0.0f)
	, rand(new Random(name))
	, uvbo(0)
	, posbo(0)
	, srbo(0)
	, cbo(0)
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
	auto logger = &Voxel::Logger::getInstance();

	auto ssm = &Voxel::SpriteSheetManager::getInstance();

	if (ssm == nullptr)
	{
		logger->error("[Error.ParticleSystem] Sprite sheet manager is null.");
		return false;
	}

	program = Voxel::ProgramManager::getInstance().getProgram(Voxel::ProgramManager::PROGRAM_NAME::UI_PARTICLE_SYSTEM_SHADER);

	if (program == nullptr)
	{
		logger->error("[Error.ParticleSystem] Particle system shader is null.");
		return false;
	}

	const std::string spriteSheetName = particleSystemDataTree->getString("spriteSheetName");

	if (spriteSheetName.empty())
	{
		logger->error("[Error.ParticleSystem] Empty sprite sheet name");
		return false;
	}

	auto ss = ssm->getSpriteSheetByKey(spriteSheetName);

	if (ss == nullptr)
	{
		logger->error("[Error.ParticleSystem] Sprite sheet \"" + spriteSheetName + "\" doesn't exist");
		return false;
	}

	this->texture = ss->getTexture();

	if (this->texture == nullptr)
	{
		logger->error("[Error.ParticleSystem] \"" + spriteSheetName + "\" sprite sheet's texture is null.");
		return false;
	}

	const std::string particleTextureName = particleSystemDataTree->getString("textureName");

	if (particleTextureName.empty())
	{
		logger->error("[Error.ParticleSystem] Particle texture name is empty.");
		return false;
	}

	auto ie = ss->getImageEntry(particleTextureName);

	if (ie == nullptr)
	{
		logger->error("[Error.ParticleSystem] \"" + spriteSheetName + "\" sprite sheet does not have image: " + particleTextureName);
		return false;
	}

	textureSize = ie->width;

	setAttributesWithFile(particleSystemDataTree);

	// init particles
	for (unsigned int i = 0; i < totalParticles + ExtraParticlesInPool; ++i)
	{
		// all new particle data are default.
		particles.push_back(new Particle());
	}

	// activate
	state = State::RUNNING;
	
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
	uvbo;
	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * uvs.size(), &uvs.front(), GL_DYNAMIC_DRAW);

	GLint uvVertLoc = program->getAttribLocation("uvVert");

	glEnableVertexAttribArray(uvVertLoc);
	glVertexAttribPointer(uvVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribDivisor(uvVertLoc, 0);	//0. Always use same indices

	const unsigned int count = totalParticles + ExtraParticlesInPool;

	// gen posbo
	glGenBuffers(1, &posbo);
	glBindBuffer(GL_ARRAY_BUFFER, posbo);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 2, nullptr, GL_DYNAMIC_DRAW);	// vec2

	GLint posVertLoc = program->getAttribLocation("posVert");

	glEnableVertexAttribArray(posVertLoc);
	glVertexAttribPointer(posVertLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribDivisor(posVertLoc, 1);	//1, Use 1 pos(vec2) value for each quad


	// gen scale rot buffer
	glGenBuffers(1, &srbo);
	glBindBuffer(GL_ARRAY_BUFFER, srbo);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 4, nullptr, GL_DYNAMIC_DRAW); // vec4

	GLint scaleRotLoc = program->getAttribLocation("scaleRotVert");

	glEnableVertexAttribArray(scaleRotLoc);
	glVertexAttribPointer(scaleRotLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribDivisor(scaleRotLoc, 1);


	// gen color buffer object
	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 4, nullptr, GL_DYNAMIC_DRAW); // vec4

	GLint colorVertLoc = program->getAttribLocation("colorVert");

	glEnableVertexAttribArray(colorVertLoc);
	glVertexAttribPointer(colorVertLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribDivisor(colorVertLoc, 1);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);


#if V_DEBUG && V_DEBUG_LOG_CONSOLE
	logger->consoleInfo("[ParticleSystem] Creating particle system");
	logger->consoleInfo("[ParticleSystem] Name = " + name);
	logger->consoleInfo("[ParticleSystem] Duration = " + std::to_string(duration));
	logger->consoleInfo("[ParticleSystem] Total particles = " + std::to_string(totalParticles));
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
	logger->consoleInfo("[ParticleSystem] Angle start = " + std::to_string(startAngle) + " +/- " + std::to_string(startAngleVar));
	logger->consoleInfo("[ParticleSystem] Angle end = " + std::to_string(endAngle) + " +/- " + std::to_string(endAngleVar));
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

	totalParticles = psDataTree->getInt("totalParticles");

	if (totalParticles < 0)
	{
		totalParticles = 0;
	}

	particleLifeSpan = psDataTree->getFloat("particleLifeSpan");

	if (particleLifeSpan <= 0.0f)
	{
		particleLifeSpan = 0.0f;
		emissionRate = 0.0f;
	}
	else
	{
		emissionRate = static_cast<float>(totalParticles) / particleLifeSpan;
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
	startColor.a = glm::clamp(psDataTree->getFloat("color.start.a"), 0.0f, 1.0f);

	// [0.0f, 1.0f]
	startColorVar.r = glm::clamp(psDataTree->getFloat("color.startVar.r"), 0.0f, 1.0f);
	startColorVar.g = glm::clamp(psDataTree->getFloat("color.startVar.g"), 0.0f, 1.0f);
	startColorVar.b = glm::clamp(psDataTree->getFloat("color.startVar.b"), 0.0f, 1.0f);
	startColorVar.a = glm::clamp(psDataTree->getFloat("color.startVar.a"), 0.0f, 1.0f);

	// [0.0f, 1.0f]
	endColor.r = glm::clamp(psDataTree->getFloat("color.end.r"), 0.0f, 1.0f);
	endColor.g = glm::clamp(psDataTree->getFloat("color.end.g"), 0.0f, 1.0f);
	endColor.b = glm::clamp(psDataTree->getFloat("color.end.b"), 0.0f, 1.0f);
	endColor.a = glm::clamp(psDataTree->getFloat("color.end.a"), 0.0f, 1.0f);

	// [0.0f, 1.0f]
	endColorVar.r = glm::clamp(psDataTree->getFloat("color.endVar.r"), 0.0f, 1.0f);
	endColorVar.g = glm::clamp(psDataTree->getFloat("color.endVar.g"), 0.0f, 1.0f);
	endColorVar.b = glm::clamp(psDataTree->getFloat("color.endVar.b"), 0.0f, 1.0f);
	endColorVar.a = glm::clamp(psDataTree->getFloat("color.endVar.a"), 0.0f, 1.0f);

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

	startAngle = psDataTree->getFloat("angle.start");
	startAngleVar = psDataTree->getFloat("angle.startVar");

	endAngle = psDataTree->getFloat("angle.end");
	endAngleVar = psDataTree->getFloat("angle.endVar");

	return true;
}

void Voxel::UI::ParticleSystem::pause()
{
	if (state == State::RUNNING)
	{
		state = State::PAUSED;
	}
}

void Voxel::UI::ParticleSystem::resume()
{
	if (state == State::PAUSED)
	{
		state = State::RUNNING;
	}
}

void Voxel::UI::ParticleSystem::stop()
{
	if (state == State::RUNNING || state == State::PAUSED)
	{
		state = State::STOPPED;
		reset();
	}
}

void Voxel::UI::ParticleSystem::start()
{
	if (state == State::FINISHED || state == State::STOPPED)
	{
		reset();

		state = State::RUNNING;
	}
}

bool Voxel::UI::ParticleSystem::isPaused() const
{
	return state == State::PAUSED;
}

bool Voxel::UI::ParticleSystem::isRunning() const
{
	return state == State::RUNNING;
}

bool Voxel::UI::ParticleSystem::isFinished() const
{
	return state == State::FINISHED;
}

void Voxel::UI::ParticleSystem::setDuration(const float duration)
{
	if (duration < 0.0f)
	{
		// duration is negtiave. Treat this as infinite duration
		this->duration = -1.0f;
	}
	else
	{
		// duration is positive
		this->duration = duration;

		if (this->duration == 0.0f)
		{
			// if 0, finish it
			state = State::FINISHED;
		}

		// reset particles and restart if duration is not 0 seconds
		reset();
	}

	// reset elapsed time
	elapsedTime = 0.0f;
}

bool Voxel::UI::ParticleSystem::isDurationInfinite() const
{
	return duration == -1.0f;
}

float Voxel::UI::ParticleSystem::getEmissionRate() const
{
	return emissionRate;
}

int Voxel::UI::ParticleSystem::getLivingParticleNumber() const
{
	return livingParticleNumber;
}

void Voxel::UI::ParticleSystem::setTotalParticles(const unsigned int totalParticles)
{
	bool reallocate = false;

	if (this->totalParticles == totalParticles)
	{
		return;
	}
	else if (this->totalParticles < totalParticles)
	{
		auto amount = totalParticles - this->totalParticles;

		for (unsigned int i = 0; i < amount; i++)
		{
			particles.push_back(new Particle());
		}

		reallocate = true;
	}
	else
	{
		auto amount = this->totalParticles - totalParticles;

		for (unsigned int i = 0; i < amount; i++)
		{
			delete particles.back();
			particles.pop_back();
		}
	}

	// particles shoudl always have 10 more in pool
	assert((particles.size() - totalParticles) == ExtraParticlesInPool);

	this->totalParticles = totalParticles;

	if(reallocate)
	{
		const unsigned int count = totalParticles + ExtraParticlesInPool;

		glBindBuffer(GL_ARRAY_BUFFER, posbo);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 2, nullptr, GL_DYNAMIC_DRAW);	// vec2

		glBindBuffer(GL_ARRAY_BUFFER, srbo);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 4, nullptr, GL_DYNAMIC_DRAW);	// vec4

		glBindBuffer(GL_ARRAY_BUFFER, cbo);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(float) * 4, nullptr, GL_DYNAMIC_DRAW);	// vec4
	}
	
	// recalculate emission rate
	if (particleLifeSpan <= 0.0f)
	{
		// particle's life span is 0 = no particles spawning
		emissionRate = 0.0f;
	}
	else if(this->totalParticles <= 0)
	{
		// total particles is 0
		emissionRate = 0.0f;
	}
	else
	{
		// get emission rate
		emissionRate = this->totalParticles / particleLifeSpan;
	}

	if (duration > 0.0f)
	{
		reset();
	}
}

void Voxel::UI::ParticleSystem::setParticleLifeSpan(const float lifeSpan)
{
	particleLifeSpan = lifeSpan;

	if (particleLifeSpan <= 0.0f)
	{
		particleLifeSpan = 0.0f;
		emissionRate = 0.0f;
	}
	else
	{
		emissionRate = static_cast<float>(this->totalParticles) / particleLifeSpan;
	}

	if (duration > 0.0f)
	{
		reset();
	}
}

void Voxel::UI::ParticleSystem::setParticleLifeSpanVar(const float lifeSpanVar)
{
	particleLifeSpanVar = lifeSpanVar;

	if (duration > 0.0f)
	{
		reset();
	}
}

void Voxel::UI::ParticleSystem::setSpeed(const float speed)
{
	this->speed = speed;
}

void Voxel::UI::ParticleSystem::setSpeedVar(const float speedVar)
{
	this->speedVar = speedVar;
}

void Voxel::UI::ParticleSystem::setEmitPositionX(const float x)
{
	emitPos.x = x;
}

void Voxel::UI::ParticleSystem::setEmitPositionY(const float y)
{
	emitPos.y = y;
}

void Voxel::UI::ParticleSystem::setEmitPosition(const glm::vec2 & emitPos)
{
	this->emitPos = emitPos;
}

glm::vec2 Voxel::UI::ParticleSystem::getEmissionPosition() const
{
	return emitPos;
}

void Voxel::UI::ParticleSystem::setEmitPosXVar(const float xVar)
{
	emitPosVar.x = xVar;
}

void Voxel::UI::ParticleSystem::setEmitPosYVar(const float yVar)
{
	emitPosVar.y = yVar;
}

glm::vec2 Voxel::UI::ParticleSystem::getEmissionPositionVar() const
{
	return emitPosVar;
}

void Voxel::UI::ParticleSystem::setGravityX(const float gravityX)
{
	gravity.x = gravityX;
}

void Voxel::UI::ParticleSystem::setGravityY(const float gravityY)
{
	gravity.y = gravityY;
}

void Voxel::UI::ParticleSystem::setEmitAngle(const float angle)
{
	emitAngle = angle;
}

void Voxel::UI::ParticleSystem::setEmitAngleVar(const float angleVar)
{
	emitAngleVar = angleVar;
}

void Voxel::UI::ParticleSystem::setAccelRad(const float accelRad)
{
	this->accelRad = accelRad;
}

void Voxel::UI::ParticleSystem::setAccelRadVar(const float accelRadVar)
{
	this->accelRadVar = accelRadVar;
}

void Voxel::UI::ParticleSystem::setAccelTan(const float accelTan)
{
	this->accelTan = accelTan;
}

void Voxel::UI::ParticleSystem::setAccelTanVar(const float accelTanVar)
{
	this->accelTanVar = accelTanVar;
}

void Voxel::UI::ParticleSystem::setStartSize(const float size)
{
	startSize = size;

	if (startSize < 0.0f) startSize = 0.0f;
}

void Voxel::UI::ParticleSystem::setStartSizeVar(const float size)
{
	startSizeVar = size;

	if (startSizeVar < 0.0f) startSizeVar = 0.0f;
}

void Voxel::UI::ParticleSystem::setEndSize(const float size)
{
	endSize = size;

	if (endSize < 0.0f) endSize = 0.0f;
}

void Voxel::UI::ParticleSystem::setEndSizeVar(const float size)
{
	endSizeVar = size;

	if (endSizeVar < 0.0f) endSizeVar = 0.0f;
}

void Voxel::UI::ParticleSystem::setStartAngle(const float angle)
{
	startAngle = angle;
}

void Voxel::UI::ParticleSystem::setStartAngleVar(const float angle)
{
	startAngleVar = angle;
}

void Voxel::UI::ParticleSystem::setEndAngle(const float angle)
{
	endAngle = angle;
}

void Voxel::UI::ParticleSystem::setEndAngleVar(const float angle)
{
	endAngleVar = angle;
}

void Voxel::UI::ParticleSystem::setStartColor(const glm::vec4 & color)
{
	startColor = glm::clamp(color, 0.0f, 1.0f);
}

void Voxel::UI::ParticleSystem::setStartColorVar(const glm::vec4 & color)
{
	startColorVar = glm::clamp(color, 0.0f, 1.0f);
}

void Voxel::UI::ParticleSystem::setEndColor(const glm::vec4 & color)
{
	endColor = glm::clamp(color, 0.0f, 1.0f);
}

void Voxel::UI::ParticleSystem::setEndColorVar(const glm::vec4 & color)
{
	endColorVar = glm::clamp(color, 0.0f, 1.0f);
}

bool Voxel::UI::ParticleSystem::setTexture(const std::string & spriteSheetName, const std::string & textureName)
{
	if (spriteSheetName.empty()) return false;
	if (textureName.empty()) return false;

	auto sm = &Voxel::SpriteSheetManager::getInstance();

	if (sm == nullptr) return false;

	auto ss = sm->getSpriteSheetByKey(spriteSheetName);

	if (ss == nullptr) return false;

	Voxel::Texture2D* newTexture = ss->getTexture();

	if (newTexture == nullptr) return false;

	if (newTexture->getName() != this->texture->getName())
	{
		this->texture = newTexture;
	}

	auto ie = ss->getImageEntry(textureName);

	if (ie == nullptr) return false;

	auto& uvOrigin = ie->uvOrigin;
	auto& uvEnd = ie->uvEnd;

	std::array<float, 8> uvs = { uvOrigin.x, uvOrigin.y, uvOrigin.x, uvEnd.y, uvEnd.x, uvOrigin.y, uvEnd.x, uvEnd.y };

	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, uvs.size() * sizeof(float), &uvs.front());

	return true;
}

void Voxel::UI::ParticleSystem::reset()
{
	for (auto particle : particles)
	{
		if (particle->alive)
		{
			particle->alive = false;
		}
	}

	livingParticleNumber = 0;

	elapsedTime = 0.0f;
}

void Voxel::UI::ParticleSystem::update(const float delta)
{
	if (state != State::RUNNING) return;
	if (totalParticles == 0) return;

	float actualTime = 0.0f;

	if (duration == -1)
	{
		// infinite
		actualTime = delta;
	}
	else if(elapsedTime < duration)
	{
		elapsedTime += delta;
		
		if (elapsedTime > duration)
		{
			float exceededTime = elapsedTime - duration;
			
			actualTime = delta - exceededTime;
		}
		else
		{
			actualTime = delta;
		}
	}
	
	if (actualTime != 0.0f)
	{
		spawnPoint += (emissionRate * actualTime);

		int newParticleNumber = static_cast<int>(glm::floor(spawnPoint));

		if (newParticleNumber > 0)
		{
			spawnNewParticles(newParticleNumber);
		}
		// Else, nothing to spawn
	}

	// buffer
	std::vector<float> posBuffer;
	std::vector<float> scaleRotBuffer;
	std::vector<float> colorBuffer;
	
	auto p_it = particles.begin();
	auto p_last = std::prev(particles.end());

	int curLivingParticleNum = 0;
		
	for (; p_it != particles.end();)
	{
		Particle* curP = (*p_it);

		if (curP->lifeSpan == 0.0f)
		{
			p_it++;
			continue;
		}

		if (curP->alive == false)
		{
			break;
		}

		if (curP->alive && curP->livedTime >= curP->lifeSpan)
		{
			Particle* deadParticle = curP;
			
			deadParticle->alive = false;

			p_it = particles.erase(p_it);

			deadParticles.push_back(deadParticle);
		}
		else
		{
			// add time
			curP->livedTime += delta;

			// check if it's dead
			if (curP->livedTime >= curP->lifeSpan)
			{
				// dead. match time with life span
				curP->livedTime = curP->lifeSpan;
				// yet, treat this particle and living particle
			}

			// count living particle
			curLivingParticleNum++;

			// calculate particle position

			// temp vars
			glm::vec2 tmp(0.0f);
			glm::vec2 radial(0.0f);
			glm::vec2 tangential(0.0f);

			// radial acceleration
			curP->normalizePoint(radial);

			// copy
			tangential = radial;

			// apply radial acceleration
			radial *= curP->accelRad;

			// tengential acceleration
			std::swap(tangential.x, tangential.y);

			// apply tengential acceleration
			tangential.x *= -(curP->accelTan);
			tangential.y *= curP->accelTan;

			// sum up gravity + radial + tengential. Apply time
			tmp = (radial + tangential + gravity) * delta;

			//tmp.x = radial.x + tangential.x + gravity.x;
			//tmp.y = radial.y + tangential.y + gravity.y;
			//tmp *= delta;

			// sum new direction to current direction vector
			curP->dirVec += tmp;

			// Update position
			curP->pos += (curP->dirVec * delta);

			if (positionType == PositionType::PT_GROUPED)
			{
				// Grouped position type particles move together when emitter.
				curP->emitPos = emitPos;
			}
			// else, Position type  is relative. Doens't move together with emitter.

			// Add to position buffer.
			posBuffer.push_back(curP->emitPos.x + curP->pos.x);
			posBuffer.push_back(curP->emitPos.y + curP->pos.y);

			//std::cout << "pos (" << curP->pos.x << ", " << curP->pos.y << ")\n";

			const float lifeTimeRatio = curP->livedTime / curP->lifeSpan;

			// size
			float curSize = glm::lerp(curP->startSize, curP->endSize, lifeTimeRatio);

			// to scale
			float scale = curSize / textureSize;

			// angle
			float curAngle = glm::lerp(curP->startAngle, curP->endAngle, lifeTimeRatio);

			// to matrix
			auto scaleRotMat = glm::rotate(glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0f)), glm::radians(curAngle), glm::vec3(0.0f, 0.0f, 1.0f));

			// Particle transformation in vec4 buffer
			// we only need first two row and column for scale and rotation matrix
			scaleRotBuffer.push_back(scaleRotMat[0][0]);
			scaleRotBuffer.push_back(scaleRotMat[0][1]);
			scaleRotBuffer.push_back(scaleRotMat[1][0]);
			scaleRotBuffer.push_back(scaleRotMat[1][1]);

			// Color buffer (rgba)
			glm::vec4 curColor = glm::lerp(curP->startColor, curP->endColor, lifeTimeRatio);

			// to buffer
			colorBuffer.push_back(curColor.r);
			colorBuffer.push_back(curColor.g);
			colorBuffer.push_back(curColor.b);
			colorBuffer.push_back(curColor.a);

			p_it++;
		}
	}

	livingParticleNumber = curLivingParticleNum;

	particles.insert(particles.end(), deadParticles.begin(), deadParticles.end());

	deadParticles.clear();

	assert((particles.size() - totalParticles) == ExtraParticlesInPool);

	if ((duration != -1) && (elapsedTime >= duration) && (livingParticleNumber == 0))
	{
		// time's up and all particles are dead
		state = State::FINISHED;
	}

	if (livingParticleNumber > 0)
	{
		// Update buffers
		
		// pos buffer
		glBindBuffer(GL_ARRAY_BUFFER, posbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, livingParticleNumber * 2 * sizeof(float), &posBuffer.front());

		glBindBuffer(GL_ARRAY_BUFFER, srbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, livingParticleNumber * 4 * sizeof(float), &scaleRotBuffer.front());

		glBindBuffer(GL_ARRAY_BUFFER, cbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, livingParticleNumber * 4 * sizeof(float), &colorBuffer.front());
	}

	Voxel::UI::TransformNode::update(delta);
}

void Voxel::UI::ParticleSystem::spawnNewParticles(const int count)
{
	if (count > 0)
	{
		spawnPoint -= static_cast<float>(count);

		int counter = count;

		while (counter > 0)
		{
			// spawn new particle

			// if life span is 0, there is no point of spawning
			if (particleLifeSpan <= 0.0f)
			{
				counter--;
				continue;
			}

			float randLifeSpan = 0.0f;

			if (particleLifeSpanVar == 0.0f)
			{
				randLifeSpan = particleLifeSpan;
			}
			else
			{
				randLifeSpan = rand->randRangeFloat(glm::clamp(particleLifeSpan - particleLifeSpanVar, 0.0f, particleLifeSpan), particleLifeSpan + particleLifeSpanVar);
			}

			if (randLifeSpan <= 0.0f)
			{
				counter--;
				continue;
			}

			Particle* lastParticle = particles.back();

			if (lastParticle->alive)
			{
				counter--;
				continue;
				//std::cout << "Reusing alive particle\n";
			}

			lastParticle->alive = true;

			lastParticle->id = Voxel::UI::Particle::idCounter++;

			const float randEmitAngle = rand->randRangeFloat(emitAngle - emitAngleVar, emitAngle + emitAngleVar);

			lastParticle->dirVec.x = glm::cos(glm::radians(randEmitAngle));
			lastParticle->dirVec.y = glm::sin(glm::radians(randEmitAngle));

			lastParticle->speed = rand->randRangeFloat(speed - speedVar, speed + speedVar);

			lastParticle->dirVec *= lastParticle->speed;

			//lastParticle->pos.x = rand->randRangeFloat(emitPos.x - emitPosVar.x, emitPos.x + emitPosVar.x);
			//lastParticle->pos.y = rand->randRangeFloat(emitPos.y - emitPosVar.y, emitPos.y + emitPosVar.y);
			lastParticle->pos.x = rand->randRangeFloat(-emitPosVar.x, emitPosVar.x);
			lastParticle->pos.y = rand->randRangeFloat(-emitPosVar.y, emitPosVar.y);

			lastParticle->emitPos = emitPos;

			glm::vec4 startMinColor = glm::clamp(startColor - startColorVar, 0.0f, 1.0f);
			glm::vec4 startMaxColor = glm::clamp(startColor + startColorVar, 0.0f, 1.0f);

			lastParticle->startColor.r = rand->randRangeFloat(startMinColor.r, startMaxColor.r);
			lastParticle->startColor.g = rand->randRangeFloat(startMinColor.g, startMaxColor.g);
			lastParticle->startColor.b = rand->randRangeFloat(startMinColor.b, startMaxColor.b);
			lastParticle->startColor.a = rand->randRangeFloat(startMinColor.a, startMaxColor.a);

			glm::vec4 endMinColor = glm::clamp(endColor - endColorVar, 0.0f, 1.0f);
			glm::vec4 endMaxColor = glm::clamp(endColor + endColorVar, 0.0f, 1.0f);

			lastParticle->endColor.r = rand->randRangeFloat(endMinColor.r, endMaxColor.r);
			lastParticle->endColor.g = rand->randRangeFloat(endMinColor.g, endMaxColor.g);
			lastParticle->endColor.b = rand->randRangeFloat(endMinColor.b, endMaxColor.b);
			lastParticle->endColor.a = rand->randRangeFloat(endMinColor.a, endMaxColor.a);

			lastParticle->accelRad = rand->randRangeFloat(accelRad - accelRadVar, accelRad + accelRadVar);
			lastParticle->accelTan = rand->randRangeFloat(accelTan - accelTanVar, accelTan + accelTanVar);

			lastParticle->lifeSpan = randLifeSpan;
			lastParticle->livedTime = 0.0f;

			lastParticle->startSize = rand->randRangeFloat(startSize - startSizeVar, startSize + startSizeVar);
			lastParticle->endSize = rand->randRangeFloat(endSize - endSizeVar, endSize + endSizeVar);

			lastParticle->startAngle = rand->randRangeFloat(startAngle - startAngleVar, startAngle + startAngleVar);
			lastParticle->endAngle = rand->randRangeFloat(endAngle - endAngleVar, endAngle + endAngleVar);

			particles.pop_back();

			particles.push_front(lastParticle);

			//std::cout << "Particle spawned #" + std::to_string(lastParticle->id) + "\n";

			counter--;
		}
	}
	// Else, nothing to spawn
}

void Voxel::UI::ParticleSystem::renderSelf()
{
	if (totalParticles == 0) return;
	if (livingParticleNumber <= 0) return;
	if (texture == nullptr) return;
	if (program == nullptr) return;

	program->use(true);
	program->setUniformMat4("modelMat", glm::scale(modelMat, glm::vec3(scale, 1)));
	program->setUniformFloat("opacity", opacity);
	program->setUniformVec3("color", color);

	texture->activate(GL_TEXTURE0);
	texture->bind();
	texture->enableTexLoc();

	if (vao)
	{
		glBindVertexArray(vao);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, livingParticleNumber);
	}
}
