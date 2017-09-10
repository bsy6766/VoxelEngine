#include "Voronoi.h"
#include <iostream>
#include <Utility.h>
#include <ProgramManager.h>
#include <Program.h>
#include <Color.h>

using namespace Voxel;
using namespace Voxel::Voronoi;

Voxel::Voronoi::Edge::Edge(const glm::vec2& start, const glm::vec2& end)
	: start(start)
	, end(end)
{}

Voxel::Voronoi::Cell::Cell(const unsigned int ID)
	: ID(ID)
	, position(0)
	, valid(false)
{}

Voxel::Voronoi::Cell::~Cell()
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

void Voxel::Voronoi::Cell::addEdge(Edge * edge)
{
	edges.push_back(edge);
}

glm::vec2 Voxel::Voronoi::Cell::getPosition()
{
	return position;
}

void Voxel::Voronoi::Cell::setPosition(const glm::vec2 & position)
{
	this->position = position;
}

void Voxel::Voronoi::Cell::setValidation(const bool valid)
{
	this->valid = valid;
}

bool Voxel::Voronoi::Cell::isValid()
{
	return valid;
}




Voxel::Voronoi::Diagram::Diagram()
	: vao(0)
	, size(0)
	, lineVao(0)
	, lineSize(0)
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
}

void Voxel::Voronoi::Diagram::construct(const std::vector<glm::ivec2>& randomSites)
{
	auto start = Utility::Time::now();

	// Convert random sites to float and store.
	for (auto site : randomSites)
	{
		this->sites.push_back(glm::vec2(site));
	}

	// Convert glm::vec2 to boost polygon points
	std::vector<boost::polygon::point_data<int>> points;

	for (auto& site : randomSites)
	{
		points.push_back(boost::polygon::point_data<int>(site.x, site.y));
	}

	// Construct voronoi diagram
	boost::polygon::construct_voronoi(points.begin(), points.end(), &vd);

	auto end = Utility::Time::now();

	std::cout << "Voronoi construction took: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

void Voxel::Voronoi::Diagram::build()
{
	auto start = Utility::Time::now();
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
					if (e0.x > 5000.0f || e0.x < -5000.0f)
						valid = false;
					if (e0.y > 5000.0f || e0.y < -5000.0f)
						valid = false;

					if (e1.x > 5000.0f || e1.x < -5000.0f)
						valid = false;
					if (e1.y > 5000.0f || e1.y < -5000.0f)
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

		auto pos = this->sites.at(cellID);
		// Save site position
		newCell->setPosition(this->sites.at(cellID));

		if (valid)
		{
			// Save edges
			auto len = edges.size();

			for (unsigned int i = 0; i < len; i += 2)
			{
				Edge* newEdge = new Edge(edges.at(i), edges.at(i + 1));
				newCell->addEdge(newEdge);
			}

			newCell->setValidation(true);
		}
		else
		{
			newCell->setValidation(false);
		}

		cells.emplace(cellID, newCell);
	}

	std::cout << "Cell size = " << cells.size() << std::endl;

	auto end = Utility::Time::now();

	std::cout << "Voronoi build took: " << Utility::Time::toMilliSecondString(start, end) << std::endl;
}

void Voxel::Voronoi::Diagram::initDebugDiagram()
{
	float scale = 0.005f;
	std::vector<float> buffer;
	std::vector<float> posBuffer;

	auto vdEdges = vd.edges();
	buffer.clear();

	auto randColor = Color::getRandomColor();

	// Build edges
	for (auto& e : vdEdges)
	{
		if (e.is_secondary()) continue;

		if (e.is_linear())
		{
			glm::vec2 e0, e1;
			if (e.is_infinite())
			{
				clipInfiniteEdge(e, e0, e1, glm::vec2(7000, 7000));

				buffer.push_back(e0.x * scale);
				buffer.push_back(100.0f);
				buffer.push_back(e0.y * scale);
				buffer.push_back(0.0f);
				buffer.push_back(0.0f);
				buffer.push_back(1.0f);
				buffer.push_back(1.0f);

				buffer.push_back(e1.x * scale);
				buffer.push_back(100.0f);
				buffer.push_back(e1.y * scale);
				buffer.push_back(0.0f);
				buffer.push_back(0.0f);
				buffer.push_back(1.0f);
				buffer.push_back(1.0f);
			}
			else
			{
				e0 = glm::vec2(e.vertex0()->x(), e.vertex0()->y());
				e1 = glm::vec2(e.vertex1()->x(), e.vertex1()->y());

				//e0 = glm::clamp(e0, -5000.0f, 5000.0f);
				//e1 = glm::clamp(e1, -5000.0f, 5000.0f);

				buffer.push_back(e0.x * scale);
				buffer.push_back(100.0f);
				buffer.push_back(e0.y * scale);
				buffer.push_back(randColor.r);
				buffer.push_back(randColor.g);
				buffer.push_back(randColor.b);
				buffer.push_back(1.0f);

				buffer.push_back(e1.x * scale);
				buffer.push_back(100.0f);
				buffer.push_back(e1.y * scale);
				buffer.push_back(randColor.r);
				buffer.push_back(randColor.g);
				buffer.push_back(randColor.b);
				buffer.push_back(1.0f);
			}
		}
	}

	for (auto& c : cells)
	{
		auto pos = (c.second)->getPosition();
		if ((c.second)->isValid())
		{
			posBuffer.push_back(pos.x * scale);
			posBuffer.push_back(103.0f);
			posBuffer.push_back(pos.y * scale);
			posBuffer.push_back(1.0f);
			posBuffer.push_back(1.0f);
			posBuffer.push_back(1.0f);
			posBuffer.push_back(1.0f);

			posBuffer.push_back(pos.x * scale);
			posBuffer.push_back(97.0f);
			posBuffer.push_back(pos.y * scale);
			posBuffer.push_back(1.0f);
			posBuffer.push_back(1.0f);
			posBuffer.push_back(1.0f);
			posBuffer.push_back(1.0f);
		}
		else
		{
			posBuffer.push_back(pos.x * scale);
			posBuffer.push_back(103.0f);
			posBuffer.push_back(pos.y * scale);
			posBuffer.push_back(0.3f);
			posBuffer.push_back(0.3f);
			posBuffer.push_back(0.3f);
			posBuffer.push_back(1.0f);

			posBuffer.push_back(pos.x * scale);
			posBuffer.push_back(97.0f);
			posBuffer.push_back(pos.y * scale);
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

	{
		posBuffer.push_back(5000.0f * scale);
		posBuffer.push_back(100.0f);
		posBuffer.push_back(5000.0f * scale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(5000.0f * scale);
		posBuffer.push_back(100.0f);
		posBuffer.push_back(-5000.0f * scale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(5000.0f * scale);
		posBuffer.push_back(100.0f);
		posBuffer.push_back(-5000.0f * scale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(-5000.0f * scale);
		posBuffer.push_back(100.0f);
		posBuffer.push_back(-5000.0f * scale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(-5000.0f * scale);
		posBuffer.push_back(100.0f);
		posBuffer.push_back(-5000.0f * scale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(-5000.0f * scale);
		posBuffer.push_back(100.0f);
		posBuffer.push_back(5000.0f * scale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(-5000.0f * scale);
		posBuffer.push_back(100.0f);
		posBuffer.push_back(5000.0f * scale);
		posBuffer.push_back(1.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(0.0f);
		posBuffer.push_back(1.0f);

		posBuffer.push_back(5000.0f * scale);
		posBuffer.push_back(100.0f);
		posBuffer.push_back(5000.0f * scale);
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
	// Enable vertices attrib

	// vert
	glEnableVertexAttribArray(vertLoc);
	glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), nullptr);

	// color
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	glDeleteBuffers(1, &lineVbo);

	lineSize = (posBuffer.size() / 7) + 8;

}

void Voxel::Voronoi::Diagram::clipInfiniteEdge(const EdgeType& edge, glm::vec2& e0, glm::vec2& e1, const glm::vec2& boundary)
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

	float coefWidth = boundary.x / glm::max(glm::abs(direction.x), glm::abs(direction.y));
	float coefHeight = boundary.y / glm::max(glm::abs(direction.x), glm::abs(direction.y));

	if (edge.vertex0() == nullptr)
	{
		e0 = origin - direction * coefWidth;
	}
	else
	{
		e0 = glm::vec2(edge.vertex0()->x(), edge.vertex0()->y());
	}

	if (edge.vertex1() == nullptr)
	{
		e1 = origin + direction * coefWidth;
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
		return sites[index];
	}
	else
	{
		assert(false);
	}
	return sites[index];
}

void Voxel::Voronoi::Diagram::render()
{
	if (vao)
	{
		glBindVertexArray(vao);

		glDrawArrays(GL_LINES, 0, size);
	}

	if (lineVao)
	{
		glBindVertexArray(lineVao);

		glDrawArrays(GL_LINES, 0, lineSize);
	}
}
