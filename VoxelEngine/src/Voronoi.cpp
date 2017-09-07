#include "Voronoi.h"

using namespace Voxel;
using namespace Voxel::Voronoi;

Voxel::Voronoi::Edge::Edge(const glm::vec2& start, const glm::vec2& end)
	: start(start)
	, end(end)
{}

Voxel::Voronoi::Cell::Cell(const unsigned int ID)
	: ID(ID)
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

void Voxel::Voronoi::Diagram::init(const std::vector<glm::ivec2>& sites)
{
	// Convert glm::vec2 to boost polygon points
	std::vector<boost::polygon::point_data<int>> points;

	for (auto& site : sites)
	{
		points.push_back(boost::polygon::point_data<int>(site.x, site.y));
	}

	// Construct voronoi diagram
	boost::polygon::construct_voronoi(points.begin(), points.end(), &vd);

	// Iterate cells. Ignore all the cells that contains infinite edge
	for (auto it = vd.cells().begin(); it != vd.cells().end(); ++it)
	{
		const auto& cell = *it;

		auto edge = cell.incident_edge();

		bool valid = true;

		std::vector<glm::vec2> edges;

		do
		{
			if (edge->is_primary())
			{
				if (edge->is_finite())
				{
					edges.push_back(glm::vec2(edge->vertex0()->x(), edge->vertex0()->y()));
					edges.push_back(glm::vec2(edge->vertex1()->x(), edge->vertex1()->y()));
				}
				else
				{
					valid = false;
					break;
				}
			}

			edge = edge->rot_next();
		} while (edge != cell.incident_edge());

		if (valid)
		{
			// Add cell
			auto cellID = cell.source_index();

			Cell* newCell = new Cell(cellID);
			auto len = edges.size();
			for (unsigned int i = 0; i < len; i += 2)
			{
				Edge* newEdge = new Edge(edges.at(i), edges.at(i + 1));
				newCell->addEdge(newEdge);
			}
		}
		else
		{
			continue;
		}
	}
}
