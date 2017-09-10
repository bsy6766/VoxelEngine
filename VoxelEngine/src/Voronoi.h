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

			glm::vec2 getStart();
			glm::vec2 getEnd();

			bool operator==(const Edge* edge)
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
		};

		/**
		*	@class Site
		*	@brief A vec2 point in voronoi diagram
		*/
		class Site
		{
		public:
			enum class Type
			{
				NONE = 0,
				BORDER,
				MARKED,
				OMITTED
			};
		private:
			glm::vec2 position;
			Type type;
		public:
			Site() = delete;
			Site(const glm::vec2& position, const Type type);
			~Site() = default;

			glm::vec2 getPosition();
			Type getType();
		};

		/**
		*	@class Cell
		*	@brief A cell in voronoi diagram. Cell must be surrounded by edges. 
		*
		*	Cell works as an node in undirected graph structure of cells.
		*/
		class Cell
		{
		private:
			// Position of cell in x and z axis
			Site* site;
			// List if edges that forms the cell
			std::vector<Edge*> edges;
			// Number id
			unsigned int ID;
			// True if cell is valid. For now, its only for debugging
			bool valid;
			// Neighbors
			std::vector<Cell*> neightbors;
		public:
			Cell() = delete;
			Cell(const unsigned int ID);
			~Cell();

			void addEdge(Edge* edge);

			glm::vec2 getSitePosition();
			Site::Type getSiteType();
			void setSite(const glm::vec2& position, const Site::Type type);
			void setSite(Site* site);
			void setValidation(const bool valid);
			bool isValid();
			const std::vector<Edge*>& getEdges();
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

		/**
		*	@class Diagram
		*	@brief A voronoi diagram. Contains cell and site datas with boost's voronoi diagram
		*/
		class Diagram
		{
		private:
			boost::polygon::voronoi_diagram<double> vd;

			// List of cells. key equals to cell's ID that made from boost
			std::unordered_map<unsigned int, Cell*> cells;

			// site points
			std::vector<glm::vec2> sitePositions;
			std::vector<Site::Type> siteTypes;

			// Boundary
			float minBound;
			float maxBound;

			// Build undirected graph between cells
			unsigned int xzToIndex(const int x, const int z, const int w);

			// For debug rendering
			GLuint vao;
			unsigned int size;
			GLuint lineVao;
			unsigned int lineSize;

			// For inifinite edges
			void clipInfiniteEdge(const EdgeType& edge, glm::vec2& e0, glm::vec2& e1, const float bound);
			glm::vec2 retrivePoint(const CellType& cell);
		public:
			Diagram();
			~Diagram();

			// Construct voronoi diagram based on random sites
			//void construct(const std::vector<glm::ivec2>& randomSites);
			void construct(const std::vector<Site>& randomSites);
			// Build cells with edges. Any cells that has edges out of boundary will be omitted.
			void buildCells(const float minBound, const float maxBound);
			// Build graph based on cells.
			void buildGraph(const int w, const int l);
			// Intialize debug lines of diagram
			void initDebugDiagram();
			void render();
		};
	}
}

#endif