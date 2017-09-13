#include "Voronoi.h"
#include <iostream>
#include <Utility.h>
#include <ProgramManager.h>
#include <Program.h>
#include <Color.h>
#include <Region.h>
#include <limits>

using namespace Voxel;
using namespace Voxel::Voronoi;

Voxel::Voronoi::Edge::Edge(const glm::vec2& start, const glm::vec2& end)
	: start(start)
	, end(end)
	, owner(nullptr)
	, coOwner(nullptr)
{}

glm::vec2 Voxel::Voronoi::Edge::getStart()
{
	return start;
}

glm::vec2 Voxel::Voronoi::Edge::getEnd()
{
	return end;
}

bool Voxel::Voronoi::Edge::equal(const Edge * edge)
{
	if (this->start == edge->start && this->end == edge->end)
	{
		return true;
	}
	else if (this->start == edge->end && this->end == edge->start)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Voxel::Voronoi::Edge::setOwner(Cell * cell)
{
	this->owner = cell;
}

void Voxel::Voronoi::Edge::setCoOwner(Cell * cell)
{
	this->coOwner = cell;
}

Cell * Voxel::Voronoi::Edge::getCoOwner()
{
	return coOwner;
}

Voxel::Voronoi::Cell::Cell(const unsigned int ID)
	: ID(ID)
	, site(nullptr)
	, valid(false)
	, region(nullptr)
{}

Voxel::Voronoi::Cell::~Cell()
{
	clearEdges();

	if (site)
	{
		delete site;
	}
}

void Voxel::Voronoi::Cell::addEdge(Edge * edge)
{
	edges.push_back(edge);
}

glm::vec2 Voxel::Voronoi::Cell::getSitePosition()
{
	return site->getPosition();
}

Site::Type Voxel::Voronoi::Cell::getSiteType()
{
	return site->getType();
}

void Voxel::Voronoi::Cell::setSite(const glm::vec2& position, const Site::Type type)
{
	site = new Site(position, type);
}

void Voxel::Voronoi::Cell::setSite(Site * site)
{
	this->site = site;
}

void Voxel::Voronoi::Cell::setValidation(const bool valid)
{
	this->valid = valid;
}

bool Voxel::Voronoi::Cell::isValid()
{
	return valid;
}

std::vector<Edge*>& Voxel::Voronoi::Cell::getEdges()
{
	return edges;
}

void Voxel::Voronoi::Cell::addNeighbor(Cell * cell)
{
	neighbors.push_back(cell);
}

std::list<Cell*>& Voxel::Voronoi::Cell::getNeighbors()
{
	return neighbors;
}

unsigned int Voxel::Voronoi::Cell::getNeighborSize()
{
	return this->neighbors.size();
}

void Voxel::Voronoi::Cell::clearEdges()
{
	for (auto edge : edges)
	{
		if (edge)
		{
			delete edge;
		}
	}

	edges.clear();
}

void Voxel::Voronoi::Cell::clearNeighbors()
{
	this->neighbors.clear();
}

unsigned int Voxel::Voronoi::Cell::getID()
{
	return ID;
}

void Voxel::Voronoi::Cell::updateSiteType(Site::Type type)
{
	this->site->updateType(type);
}

void Voxel::Voronoi::Cell::setRegion(Region * region)
{
	this->region = region;
}

Region * Voxel::Voronoi::Cell::getRegion()
{
	return region;
}




Voxel::Voronoi::Diagram::Diagram()
	: vao(0)
	, size(0)
	, lineVao(0)
	, lineSize(0)
	, minBound(0)
	, maxBound(0)
	, totalValidCells(0)
	, scale(1.0f)
	, debugScale(1.0f)
{}

Voxel::Voronoi::Diagram::~Diagram()
{
	for (auto cell : cells)
	{
		if (cell.second)
		{
			delete cell.second;
		}
	}

	cells.clear();

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
	}

	if (fillVao)
	{
		glDeleteVertexArrays(1, &fillVao);
	}

	if (lineVao)
	{
		glDeleteVertexArrays(1, &lineVao);
	}

	if (graphLineVao)
	{
		glDeleteVertexArrays(1, &graphLineVao);
	}
}

void Voxel::Voronoi::Diagram::construct(const std::vector<Site>& randomSites)
{
	auto start = Utility::Time::now();

	// Convert random sites to float and store.
	// Convert glm::vec2 to boost polygon points
	std::vector<boost::polygon::point_data<int>> points;

	this->scale = 0.01f;
	this->debugScale = 1.0f;

	for (auto site : randomSites)
	{
		auto pos = site.getPosition() * scale;

		sitePositions.push_back(pos);
		siteTypes.push_back(site.getType());

		int x = static_cast<int>(pos.x);
		int z = static_cast<int>(pos.y);
		points.push_back(boost::polygon::point_data<int>(x, z));
	}

	// Construct voronoi diagram
	boost::polygon::construct_voronoi(points.begin(), points.end(), &vd);

	auto end = Utility::Time::now();

	std::cout << "Voronoi construction took: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

void Voxel::Voronoi::Diagram::buildCells(const float minBound, const float maxBound)
{
	auto start = Utility::Time::now();

	// save bound
	this->minBound = minBound * this->scale;
	this->maxBound = maxBound * this->scale;

	// Iterate cells. Ignore all the cells that contains infinite edge
	for (auto it = vd.cells().begin(); it != vd.cells().end(); ++it)
	{
		const auto& cell = *it;

		// Get cell ID
		auto cellID = cell.source_index();

		// Get first edge
		auto edge = cell.incident_edge();

		// True if this cell is valid (All edges in boundary)
		bool valid = true;

		// save edges
		std::vector<glm::vec2> edges;

		// Iterate all edges
		do
		{
			// Edge must be primary
			if (edge->is_primary())
			{
				// Check if edge is finite or infinite
				if (edge->is_finite())
				{
					// Edge if finite
					glm::vec2 e0 = glm::vec2(edge->vertex0()->x(), edge->vertex0()->y());
					glm::vec2 e1 = glm::vec2(edge->vertex1()->x(), edge->vertex1()->y());

					// Check boundary. 
					if (e0.x > maxBound || e0.x < minBound)
						valid = false;
					if (e0.y > maxBound || e0.y < minBound)
						valid = false;

					if (e1.x > maxBound || e1.x < minBound)
						valid = false;
					if (e1.y > maxBound || e1.y < minBound)
						valid = false;

					if (!valid)
					{
						// Stop if this cell isn't valid
						break;
					}

					// Add edge
					edges.push_back(e0);
					edges.push_back(e1);
				}
				else
				{
					valid = false;
					break;
				}
			}

			//edge = edge->rot_next();
			edge = edge->next();
		} while (edge != cell.incident_edge());

		// This cell is valid
		Cell* newCell = new Cell(cellID);

		auto pos = sitePositions.at(cellID);

		// Save site position
		newCell->setSite(sitePositions.at(cellID), siteTypes.at(cellID));

		if (valid)
		{
			// Save edges
			auto len = edges.size();

			for (unsigned int i = 0; i < len; i += 2)
			{
				Edge* newEdge = new Edge(edges.at(i), edges.at(i + 1));
				newEdge->setOwner(newCell);
				newCell->addEdge(newEdge);
			}

			newCell->setValidation(true);

			totalValidCells++;
		}
		else
		{
			newCell->setValidation(false);
		}

		cells.emplace(cellID, newCell);
	}

	std::cout << "Cell size = " << cells.size() << std::endl;
	std::cout << "Valid cell size = " << totalValidCells << std::endl;

	auto end = Utility::Time::now();

	std::cout << "Voronoi build took: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

void Voxel::Voronoi::Diagram::randomizeCells(const int w, const int l)
{
	// randomly omit outer cells in grid. 
	int count = Utility::Random::randomInt(w, w + (w/4));
	// Get candidates
	std::vector<unsigned int> candidates;

	int xLen = w - 2;
	int zLen = l - 2;
	for (int x = 1; x <= xLen; ++x)
	{
		if (x == 1 || x == xLen)
		{
			for (int z = 1; z <= zLen; ++z)
			{
				candidates.push_back(xzToIndex(x, z, w));
			}
		}
		else
		{
			candidates.push_back(xzToIndex(x, 1, w));
			candidates.push_back(xzToIndex(x, zLen, w));
		}
	}

	std::random_shuffle(candidates.begin(), candidates.end());

	unsigned int index = 0;
	auto len = candidates.size();

	int totalRemoved = 0;

	while (count > 0 && index < len)
	{
		auto find_it = cells.find(candidates.at(index));
		if (find_it == cells.end())
		{
			index++;
			continue;
		}
		else
		{
			auto cell = find_it->second;
			if (cell->isValid())
			{
				//std::cout << "Attempt to remove cell: " << index << std::endl;
				// Check if this cell can be removed. 
				// Iterate through neighbor and check if they will be isolated if this cell gets removed
				auto& neighborCells = cell->getNeighbors();
				bool skip = false;

				//std::cout << "Total neighbors: " << neighborCells.size() << std::endl;
				// If neighbor cell has only 1 cell connected, skip
				for(auto nc : neighborCells)
				{
					auto size = nc->getNeighborSize();
					if (size == 1)
					{
						skip = true;
						break;
					}
				}

				if (skip)
				{
					// Skip. Next index
					//std::cout << "This cell can't be removed" << std::endl;
					index++;
					continue;
				}
				else
				{
					// This cell can be removed. Iterate neighbor cells again and remove form their neighbor list
					auto cellID = cell->getID();
					for (auto nc : neighborCells)
					{
						//std::cout << "Checking on nieghbor cell: " << nc->getID() << std::endl;
						auto& nnc = nc->getNeighbors();
						//std::cout << "Total neighbors in neighbors: " << nnc.size() << std::endl;

						auto it = nnc.begin();
						for (; it != nnc.end();)
						{
							if (cellID == (*it)->getID())
							{
								// Found the matching cell. remove from neighbor
								//std::cout << "Found" << std::endl;
								// Make sure to reset coowner for sharing edges
								auto& edges = (*it)->getEdges();

								for (auto e : edges)
								{
									auto coOwner = e->getCoOwner();
									if (coOwner)
									{
										if (coOwner->getID() == cellID)
										{
											//std::cout << "Removing from cowoner" << std::endl;
											e->setCoOwner(nullptr);
											break;
										}
									}
								}

								nnc.erase(it);
								break;
							}
							else
							{
								it++;
							}
						}
					}

					cell->setValidation(false);
					cell->clearEdges();
					cell->clearNeighbors();
					cell->updateSiteType(Site::Type::OMITTED);
					totalRemoved++;
					totalValidCells--;
					count--;
					index++;
					//std::cout << "Success!" << std::endl;
				}
			}
		}
	}

	std::cout << "Removed " << totalRemoved << " cells" << std::endl;
	std::cout << "Total valid cell count: " << totalValidCells << std::endl;
}

void Voxel::Voronoi::Diagram::initDebugDiagram()
{
	//float scale = 1.0f;
	std::vector<float> buffer;
	std::vector<float> posBuffer;
	std::vector<float> graphBuffer;
	std::vector<float> fillBuffer;
	std::vector<float> fillColor;
	std::vector<unsigned int> fillIndices;

	auto randColor = Color::getRandomColor();
	auto graphColor = Color::getRandomColor();

	const float y = 200.0f;

	unsigned int index = 0;
	GLuint PRIMITIVE_RESTART = 99999;
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(PRIMITIVE_RESTART);

	// Build edges and graph (delaunay triangulation)
	for (auto&c : cells)
	{
		bool addPolygon = false;

		auto region = (c.second)->getRegion();
		auto difficulty = region->getDifficulty();

		auto difficultyColor = glm::vec3(0);

		switch (difficulty)
		{
		case 0:
			difficultyColor = glm::vec3(0, 0, 1);
			break;
		case 1:
			difficultyColor = glm::vec3(0, 1, 0);
			break;
		case 2:
			difficultyColor = glm::vec3(1, 1, 0);
			break;
		case 3:
			difficultyColor = glm::vec3(1, 180.0f / 255.0f, 0);
			break;
		case 4:
			difficultyColor = glm::vec3(1, 100.0f / 255.0f, 0);
			break;
		case 5:
			difficultyColor = glm::vec3(1, 0, 0);
			break;
		case 6:
			difficultyColor = glm::vec3(0.6f, 0, 0);
			break;
		default:
			difficultyColor = glm::vec3(0.4f, 0.4f, 0.4f);
			break;
		}

		auto& edges = (c.second)->getEdges();
		for (auto e : edges)
		{
			glm::vec2 e0 = e->getStart();
			glm::vec2 e1 = e->getEnd();

			auto type = (c.second)->getSiteType();

			if (type == Site::Type::MARKED)
			{
				buffer.push_back(e0.x * debugScale);
				buffer.push_back(y);
				buffer.push_back(e0.y * debugScale);
				buffer.push_back(randColor.r);
				buffer.push_back(randColor.g);
				buffer.push_back(randColor.b);
				buffer.push_back(1.0f);

				buffer.push_back(e1.x * debugScale);
				buffer.push_back(y);
				buffer.push_back(e1.y * debugScale);
				buffer.push_back(randColor.r);
				buffer.push_back(randColor.g);
				buffer.push_back(randColor.b);
				buffer.push_back(1.0f);

				fillBuffer.push_back(e0.x * debugScale);
				fillBuffer.push_back(y);
				fillBuffer.push_back(e0.y * debugScale);

				fillColor.push_back(difficultyColor.r);
				fillColor.push_back(difficultyColor.g);
				fillColor.push_back(difficultyColor.b);
				fillColor.push_back(0.8f);

				fillBuffer.push_back(e1.x * debugScale);
				fillBuffer.push_back(y);
				fillBuffer.push_back(e1.y * debugScale);

				fillColor.push_back(difficultyColor.r);
				fillColor.push_back(difficultyColor.g);
				fillColor.push_back(difficultyColor.b);
				fillColor.push_back(0.8f);

				addPolygon = true;

				for (int i = 0; i < 2; i++)
				{
					fillIndices.push_back((index * 2) + i);
				}
				index++;
			}
		}

		if(addPolygon)
		{
			fillIndices.push_back(PRIMITIVE_RESTART);
		}

		auto& neighbors = (c.second)->getNeighbors();
		auto pos = (c.second)->getSitePosition();

		for (auto nc : neighbors)
		{
			auto nPos = nc->getSitePosition();
			graphBuffer.push_back(pos.x * debugScale);
			graphBuffer.push_back(y + 0.5f);
			graphBuffer.push_back(pos.y * debugScale);
			graphBuffer.push_back(graphColor.r);
			graphBuffer.push_back(graphColor.g);
			graphBuffer.push_back(graphColor.b);
			graphBuffer.push_back(1.0f);

			graphBuffer.push_back(nPos.x * debugScale);
			graphBuffer.push_back(y + 0.5f);
			graphBuffer.push_back(nPos.y * debugScale);
			graphBuffer.push_back(graphColor.r);
			graphBuffer.push_back(graphColor.g);
			graphBuffer.push_back(graphColor.b);
			graphBuffer.push_back(1.0f);
		}
	}

	fillIndices.pop_back();

	// Draw infinite edges for debug .
	auto vdEdges = vd.edges();
	for (auto& e : vdEdges)
	{
		if (e.is_secondary()) continue;

		if (e.is_linear())
		{
			if (e.is_infinite())
			{
				glm::vec2 e0, e1;
				clipInfiniteEdge(e, e0, e1, maxBound * debugScale);

				buffer.push_back(e0.x * debugScale);
				buffer.push_back(y + 0.5f);
				buffer.push_back(e0.y * debugScale);
				buffer.push_back(0.0f);
				buffer.push_back(0.0f);
				buffer.push_back(1.0f);
				buffer.push_back(1.0f);

				buffer.push_back(e1.x * debugScale);
				buffer.push_back(y + 0.5f);
				buffer.push_back(e1.y * debugScale);
				buffer.push_back(0.0f);
				buffer.push_back(0.0f);
				buffer.push_back(1.0f);
				buffer.push_back(1.0f);
			}
		}
	}

	for (auto& c : cells)
	{
		auto pos = (c.second)->getSitePosition();
		if ((c.second)->isValid())
		{
			auto type = (c.second)->getSiteType();

			glm::vec3 color;
			if (type == Site::Type::MARKED)
			{
				color = glm::vec3(0.0f, 1.0f, 0.0f);
			}
			else if (type == Site::Type::OMITTED)
			{
				color = glm::vec3(1.0f, 0.5f, 0.0f);
			}
			else
			{
				color = glm::vec3(1.0f);
			}

			auto difficulty = (c.second)->getRegion()->getDifficulty();

			float yPad = 1.0f;
			if (difficulty == 0)
			{
				yPad = 3.0f;
				color = glm::vec3(0.5f, 1.0f, 0.0f);
			}

			posBuffer.push_back(pos.x * debugScale);
			posBuffer.push_back(y + yPad);
			posBuffer.push_back(pos.y * debugScale);
			posBuffer.push_back(color.r);
			posBuffer.push_back(color.g);
			posBuffer.push_back(color.b);
			posBuffer.push_back(1.0f);

			posBuffer.push_back(pos.x * debugScale);
			posBuffer.push_back(y - yPad);
			posBuffer.push_back(pos.y * debugScale);
			posBuffer.push_back(color.r);
			posBuffer.push_back(color.g);
			posBuffer.push_back(color.b);
			posBuffer.push_back(1.0f);
		}
		else
		{
			posBuffer.push_back(pos.x * debugScale);
			posBuffer.push_back(y + 1.0f);
			posBuffer.push_back(pos.y * debugScale);
			posBuffer.push_back(0.3f);
			posBuffer.push_back(0.3f);
			posBuffer.push_back(0.3f);
			posBuffer.push_back(1.0f);

			posBuffer.push_back(pos.x * debugScale);
			posBuffer.push_back(y - 1.0f);
			posBuffer.push_back(pos.y * debugScale);
			posBuffer.push_back(0.3f);
			posBuffer.push_back(0.3f);
			posBuffer.push_back(0.3f);
			posBuffer.push_back(1.0f);

		}
	}

	// draw 
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffer.size(), &buffer.front(), GL_STATIC_DRAW);
	// Enable vertices attrib
	auto defaultProgram = ProgramManager::getInstance().getDefaultProgram(ProgramManager::PROGRAM_NAME::SHADER_COLOR);
	GLint vertLoc = defaultProgram->getAttribLocation("vert");
	GLint colorLoc = defaultProgram->getAttribLocation("color");

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);

	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);

	size = buffer.size() / 7;



	glGenVertexArrays(1, &fillVao);
	glBindVertexArray(fillVao);

	GLuint fillVbo;
	glGenBuffers(1, &fillVbo);
	glBindBuffer(GL_ARRAY_BUFFER, fillVbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * fillBuffer.size(), &fillBuffer.front(), GL_STATIC_DRAW);

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint fillColorVbo;
	glGenBuffers(1, &fillColorVbo);
	glBindBuffer(GL_ARRAY_BUFFER, fillColorVbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * fillColor.size(), &fillColor.front(), GL_STATIC_DRAW);

	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	GLuint fillIbo;
	glGenBuffers(1, &fillIbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fillIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * fillIndices.size(), &fillIndices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &fillVbo);
	glDeleteBuffers(1, &fillColorVbo);
	glDeleteBuffers(1, &fillIbo);

	fillSize = fillIndices.size();

	{
		posBuffer.push_back(maxBound * debugScale);
		posBuffer.push_back(y);
		posBuffer.push_back(maxBound * debugScale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(maxBound * debugScale);
		posBuffer.push_back(y);
		posBuffer.push_back(minBound * debugScale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(maxBound * debugScale);
		posBuffer.push_back(y);
		posBuffer.push_back(minBound * debugScale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(minBound * debugScale);
		posBuffer.push_back(y);
		posBuffer.push_back(minBound * debugScale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(minBound * debugScale);
		posBuffer.push_back(y);
		posBuffer.push_back(minBound * debugScale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(minBound * debugScale);
		posBuffer.push_back(y);
		posBuffer.push_back(maxBound * debugScale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(minBound * debugScale);
		posBuffer.push_back(y);
		posBuffer.push_back(maxBound * debugScale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(maxBound * debugScale);
		posBuffer.push_back(y);
		posBuffer.push_back(maxBound * debugScale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);
	}

	glGenVertexArrays(1, &lineVao);
	glBindVertexArray(lineVao);

	GLuint lineVbo;
	glGenBuffers(1, &lineVbo);
	glBindBuffer(GL_ARRAY_BUFFER, lineVbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * posBuffer.size(), &posBuffer.front(), GL_STATIC_DRAW);

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);

	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &lineVbo);

	lineSize = (posBuffer.size() / 7) + 8;

	// graph
	glGenVertexArrays(1, &graphLineVao);
	glBindVertexArray(graphLineVao);

	GLuint graphVbo;
	glGenBuffers(1, &graphVbo);
	glBindBuffer(GL_ARRAY_BUFFER, graphVbo);

	// Load cube vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * graphBuffer.size(), &graphBuffer.front(), GL_STATIC_DRAW);

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);

	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &graphVbo);

	graphLineSize = graphBuffer.size() / 7;
}

std::unordered_map<unsigned int, Cell*>& Voxel::Voronoi::Diagram::getCells()
{
	return cells;
}

void Voxel::Voronoi::Diagram::findShortestPathFromSrc(const unsigned int src, std::vector<float>& dist, std::vector<unsigned int>& prevPath)
{
	// get size
	auto cellSize = cells.size();
	float MAX_FLOAT = std::numeric_limits<float>::max();

	// Initialize dist to some large number that can be considered as infinite number.
	dist.clear();
	dist = std::vector<float>(cellSize, MAX_FLOAT);
	// previous cell id.
	prevPath.clear();
	prevPath = std::vector<unsigned int>(cellSize, -1);

	// Queue
	std::list<Cell*> queue;

	// Add src cell to queue
	queue.push_back(cells.find(src)->second);
	
	// Dist from src to src is 0
	dist.at(src) = 0;
	// previous cell of src is src
	prevPath.at(src) = src;

	// find 
	while (!queue.empty())
	{
		auto curCell = queue.front();
		queue.pop_front();

		auto curPos = curCell->getSitePosition();

		auto curCellID = curCell->getID();
		auto d = dist[curCellID];

		//std::cout << "CurCell #" << curCellID << " dist: " << d << std::endl;

		auto& neighborCells = curCell->getNeighbors();
		for (auto nCell : neighborCells)
		{
			if (nCell->isValid())
			{
				glm::vec2 nPos = nCell->getSitePosition();
				auto distFromCur = glm::abs(glm::distance(curPos, nPos));
				auto distFromSrc = distFromCur + d;
				auto nID = nCell->getID();

				if (distFromSrc < dist[nID])
				{
					//std::cout << "nCell is shorter #" << nID << " with dist: " << distFromCur << ", distFromSrc: " << distFromSrc << std::endl;
					dist[nID] = distFromSrc;
					prevPath[nID] = curCellID;
					queue.push_back(nCell);
				}
			}
		}
	}

	std::vector<unsigned int> path;

	// Iterate over
}

float Voxel::Voronoi::Diagram::getMinBound()
{
	return minBound;
}

float Voxel::Voronoi::Diagram::getMaxBound()
{
	return maxBound;
}

void Voxel::Voronoi::Diagram::buildGraph(const int w, const int l)
{
	auto start = Utility::Time::now();
	// start form 0, 0
	auto cellSize = cells.size();

	for (int x = 0; x < w; ++x)
	{
		for (int z = 0; z < l; ++z)
		{
			auto index = xzToIndex(x, z, w);

			//assert(index >= 0 && index < cellSize);

			auto find_it = cells.find(index);
			if (find_it == cells.end())
			{
				assert(false);
			}
			else
			{
				// Check if cell is valid
				auto& cell = find_it->second;
				auto& edges = cell->getEdges();

				if (cell->isValid())
				{
					// Check all 8 adjacent cells. 

					// top
					auto topIndex = xzToIndex(x - 1, z, w);
					checkNeighborCell(edges, cell, topIndex);

					// bottom
					auto botIndex = xzToIndex(x + 1, z, w);
					checkNeighborCell(edges, cell, botIndex);

					// left top
					auto leftTopIndex = xzToIndex(x - 1, z - 1, w);
					checkNeighborCell(edges, cell, leftTopIndex);

					// left
					auto leftIndex = xzToIndex(x, z - 1, w);
					checkNeighborCell(edges, cell, leftIndex);

					// left bottom
					auto leftBotIndex = xzToIndex(x + 1, z - 1, w);
					checkNeighborCell(edges, cell, leftBotIndex);

					// right top
					auto rightTopIndex = xzToIndex(x - 1, z + 1, w);
					checkNeighborCell(edges, cell, rightTopIndex);

					// right
					auto rightIndex = xzToIndex(x, z + 1, w);
					checkNeighborCell(edges, cell, rightIndex);

					// right bottom
					auto rightBotIndex = xzToIndex(x + 1, z + 1, w);
					checkNeighborCell(edges, cell, rightBotIndex);

				}
				else
				{
					continue;
				}
			}
		}
	}
	auto end = Utility::Time::now();

	std::cout << "graph construction took: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

unsigned int Voxel::Voronoi::Diagram::xzToIndex(const int x, const int z, const int w)
{
	return static_cast<unsigned int>((x * w) + z);
}

bool Voxel::Voronoi::Diagram::inRange(const unsigned int index)
{
	return (index >= 0) && (index < cells.size());
}

void Voxel::Voronoi::Diagram::checkNeighborCell(std::vector<Edge*>& edges, Cell* curCell, const unsigned int index)
{
	if (inRange(index))
	{
		auto nCell = cells.find(index)->second;
		if (isConnected(edges, nCell))
		{
			curCell->addNeighbor(nCell);
		}
	}
}

bool Voxel::Voronoi::Diagram::isConnected(std::vector<Edge*>& edges, Cell* neighborCell)
{
	if (neighborCell->isValid())
	{
		auto& neighborEdges = neighborCell->getEdges();
		for (auto e : edges)
		{
			for (auto ne : neighborEdges)
			{
				if (e->equal(ne))
				{
					e->setCoOwner(neighborCell);
					return true;
				}
			}
		}
	}

	return false;
}

void Voxel::Voronoi::Diagram::mergeDuplicatedEdges()
{
}

void Voxel::Voronoi::Diagram::clipInfiniteEdge(const EdgeType& edge, glm::vec2& e0, glm::vec2& e1, const float bound)
{
	const CellType& cell1 = *edge.cell();
	const CellType& cell2 = *edge.twin()->cell();

	glm::vec2 origin;
	glm::vec2 direction;

	// Infinite edges could not be created by two segment sites.
	if (cell1.contains_point() && cell2.contains_point()) 
	{
		glm::vec2 p1 = retrivePoint(cell1);
		glm::vec2 p2 = retrivePoint(cell2);

		origin.x = ((p1.x + p2.x) * 0.5f);
		origin.y = ((p1.y + p2.y) * 0.5f);

		direction.x = (p1.y - p2.y);
		direction.y = (p2.x - p1.x);
	}
	else 
	{
		/*
		origin = cell1.contains_segment() ?
			retrivePoint(cell2) :
			retrivePoint(cell1);

		SegmentType segment = cell1.contains_segment() ?
			retrieveSegment(cell1) :
			retrieveSegment(cell2);

		CoordinateType dx = high(segment).x() - low(segment).x();
		CoordinateType dy = high(segment).y() - low(segment).y();

		if ((low(segment) == origin) ^ cell1.contains_point()) 
		{
			direction.x(dy);
			direction.y(-dx);
		}
		else 
		{
			direction.x(-dy);
			direction.y(dx);
		}
		*/
		assert(false);
	}

	float coef = bound / glm::max(glm::abs(direction.x), glm::abs(direction.y));

	if (edge.vertex0() == nullptr)
	{
		e0 = origin - direction * coef;
	}
	else
	{
		e0 = glm::vec2(edge.vertex0()->x(), edge.vertex0()->y());
	}

	if (edge.vertex1() == nullptr)
	{
		e1 = origin + direction * coef;
	}
	else
	{
		e1 = glm::vec2(edge.vertex1()->x(), edge.vertex1()->y());
	}

	//e0 = glm::clamp(e0, -6000.0f, 6000.0f);
	//e1 = glm::clamp(e1, -6000.0f, 6000.0f);
}

glm::vec2 Voxel::Voronoi::Diagram::retrivePoint(const CellType & cell)
{
	SourceIndexType index = cell.source_index();
	SourceCategoryType category = cell.source_category();

	if (category == SOURCE_CATEGORY_SINGLE_POINT) 
	{
		return sitePositions[index];
	}
	else
	{
		assert(false);
	}
	return sitePositions[index];
}

void Voxel::Voronoi::Diagram::render(const bool edges, const bool fill, const bool pin, const bool graph)
{
	if (vao && edges)
	{
		glBindVertexArray(vao);

		glDrawArrays(GL_LINES, 0, size);
	}

	if (fillVao && fill)
	{
		glBindVertexArray(fillVao);
		glDrawElements(GL_TRIANGLE_FAN, fillSize, GL_UNSIGNED_INT, 0);
	}

	if (lineVao && pin)
	{
		glBindVertexArray(lineVao);

		glDrawArrays(GL_LINES, 0, lineSize);
	}

	if (graphLineVao && graph)
	{
		glBindVertexArray(graphLineVao);

		glDrawArrays(GL_LINES, 0, graphLineSize);
	}
}



Voxel::Voronoi::Site::Site(const glm::vec2 & position, const Type type)
	: position(position)
	, type(type)
{}

glm::vec2 Voxel::Voronoi::Site::getPosition()
{
	return position;
}

Site::Type Voxel::Voronoi::Site::getType()
{
	return type;
}

void Voxel::Voronoi::Site::updateType(const Type type)
{
	this->type = type;
}
