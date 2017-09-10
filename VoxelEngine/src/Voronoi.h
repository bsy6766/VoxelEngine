#ifndef VORONOI_H
#define VORONOI_H

#include <glm\glm.hpp>
#include <vector>
#include <unordered_map>
#include <boost\polygon\voronoi.hpp>
#include <boost\polygon\segment_data.hpp>
#include <GL\glew.h>

using namespace boost::polygon;

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
			// True if cell has infinite edge
			bool infinite;
		public:
			Cell() = delete;
			Cell(const unsigned int ID);
			~Cell();

			void addEdge(Edge* edge);

			glm::vec2 getPosition();
			void setPosition(const glm::vec2& position);
			void setInfinite(const bool infinite);
		};

		class Program;

		// For clipping infinite edges to boundary in voronoi diagram
		typedef double CoordinateType;
		typedef point_data< CoordinateType > PointType;
		typedef segment_data< CoordinateType > SegmentType;
		typedef voronoi_diagram< CoordinateType > VoronoiDiagram;
		typedef VoronoiDiagram::cell_type CellType;
		typedef VoronoiDiagram::cell_type::source_index_type SourceIndexType;
		typedef VoronoiDiagram::cell_type::source_category_type SourceCategoryType;
		typedef VoronoiDiagram::edge_type EdgeType;

		class Diagram
		{
		private:
			boost::polygon::voronoi_diagram<double> vd;

			// List of cells. key equals to cell's ID that made from boost
			std::unordered_map<unsigned int, Cell*> cells;

			// site points
			std::vector<glm::vec2> sites;

			// For debug rendering
			GLuint vao;
			GLuint vbo;
			unsigned int size;

			GLuint lineVao;
			GLuint lineVbo;
			unsigned int lineSize;

			void clipInfiniteEdge(const EdgeType& edge, glm::vec2& e0, glm::vec2& e1, const glm::vec2& boundary);
			glm::vec2 retrivePoint(const CellType& cell);
		public:
			Diagram() = default;
			~Diagram();

			void init(const std::vector<glm::ivec2>& randomSites);
			void render();
		};
	}
}

#endif