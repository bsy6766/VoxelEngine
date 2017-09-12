#ifndef VORONOI_H
#define VORONOI_H

#include <glm\glm.hpp>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <boost\polygon\voronoi.hpp>
#include <boost\polygon\segment_data.hpp>
#include <GL\glew.h>

using namespace boost::polygon;

namespace Voxel
{
	class Region;

	namespace Voronoi
	{
		class Cell;

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

			// Cell that owns this edge
			Cell* owner;
			// Cell that shares this edge
			Cell* coOwner;
		public:
			Edge() = delete;
			Edge(const glm::vec2& start, const glm::vec2& end);
			~Edge() = default;

			glm::vec2 getStart();
			glm::vec2 getEnd();

			bool equal(const Edge* edge);
			void setOwner(Cell* cell);
			Cell* getOwner();
			void setCoOwner(Cell* cell);
			Cell* getCoOwner();
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
			void updateType(const Type type);
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
			std::list<Cell*> neighbors;
			// Pointer to region that claims this cell
			Region* region;
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
			std::vector<Edge*>& getEdges();
			void addNeighbor(Cell* cell);
			std::list<Cell*>& getNeighbors();
			unsigned int getNeighborSize();
			void clearEdges();
			void clearNeighbors();
			unsigned int getID();
			void updateSiteType(Site::Type type);
			void setRegion(Region* region);
			Region* getRegion();
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
			// Set of all edges
			std::unordered_set<Edge*> edges;

			// site points
			std::vector<glm::vec2> sitePositions;
			std::vector<Site::Type> siteTypes;

			// Boundary
			float minBound;
			float maxBound;

			// total valid cells
			int totalValidCells;

			// Build undirected graph between cells
			unsigned int xzToIndex(const int x, const int z, const int w);
			bool inRange(const unsigned int index);
			void checkNeighborCell(std::vector<Edge*>& edges, Cell* curCell, const unsigned int index);
			bool isConnected(std::vector<Edge*>& edges, Cell* neighborCell);

			// For debug rendering
			GLuint vao;
			unsigned int size;
			GLuint fillVao;
			unsigned int fillSize;
			GLuint lineVao;
			unsigned int lineSize;
			GLuint graphLineVao;
			unsigned int graphLineSize;

			// void merge duplicate edges
			void mergeDuplicatedEdges();

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
			// Randomize cells by removing cells
			void randomizeCells(const int w, const int l);
			// Build graph based on cells.
			void buildGraph(const int w, const int l);
			// Intialize debug lines of diagram
			void initDebugDiagram();

			// get cells
			std::unordered_map<unsigned int, Cell*>& getCells();

			// Find shortest path from src to all cell using dijkstra
			void findShortestPathFromSrc(const unsigned int src, std::vector<float>& dist, std::vector<unsigned int>& prevPath);
			
			// Get boundary
			float getMinBound();
			float getMaxBound();

			// render the diagram
			void render();
		};
	}
}

#endif