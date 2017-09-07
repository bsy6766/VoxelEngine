#ifndef VORONOI_H
#define VORONOI_H

#include <glm\glm.hpp>
#include <vector>
#include <unordered_map>
#include <boost\polygon\voronoi.hpp>

namespace Voxel
{
	namespace Voronoi
	{
		/**
		*	@class Edge
		*	@brief A edge in voronoi diagram. 
		*/
		class Edge
		{
		private:
			// Start point of edge
			glm::vec2 start;
			// End point of edge
			glm::vec2 end;
		public:
			Edge() = delete;
			Edge(const glm::vec2& start, const glm::vec2& end);
			~Edge() = default;
		};

		/**
		*	@class Cell
		*	@brief A cell in voronoi diagram. Cell must be surrounded by edges. 
		*/
		class Cell
		{
		private:
			// Position of cell in x and z axis
			glm::vec2 position;
			// List if edges that forms the cell
			std::vector<Edge*> edges;
			// Number id
			unsigned int ID;
		public:
			Cell() = delete;
			Cell(const unsigned int ID);
			~Cell();

			void addEdge(Edge* edge);

			glm::vec2 getPosition();
		};

		class Diagram
		{
		private:
			boost::polygon::voronoi_diagram<double> vd;

			// List of cells. key equals to cell's ID that made from boost
			std::unordered_map<unsigned int, Cell*> cells;
		public:
			Diagram() = default;
			~Diagram();

			void init(const std::vector<glm::ivec2>& sites);
		};
	}
}

#endif