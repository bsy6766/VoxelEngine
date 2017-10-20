#ifndef VORONOI_H
#define VORONOI_H

#include <glm\glm.hpp>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <polygon\voronoi.hpp>
#include <polygon\segment_data.hpp>
#include <GL\glew.h>
#include <memory>

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

			// bool 
		public:
			Edge() = delete;
			Edge(const glm::vec2& start, const glm::vec2& end);
			~Edge() = default;

			glm::vec2 getStart();
			glm::vec2 getEnd();

			bool equal(const Edge* edge) const;
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
			std::list<std::shared_ptr<Edge>> edges;
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
			std::list<std::shared_ptr<Edge>>& getEdges();
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
			std::map<unsigned int, Cell*> cells;
			// Set of all edges
			std::unordered_set<Edge*> edges;

			// site points
			std::vector<glm::vec2> sitePositions;
			std::vector<Site::Type> siteTypes;

			// Boundary
			float minBound;
			float maxBound;

			// Scale of diagram. For debug
			float scale;
			float debugScale;
			
			// total valid cells
			int totalValidCells;

			// Build undirected graph between cells
			unsigned int xzToIndex(const int x, const int z, const int w);
			bool inRange(const unsigned int index);
			void checkNeighborCell(const std::list<std::shared_ptr<Edge>>& edges, Cell* curCell, const unsigned int index);
			bool isConnected(const std::list<std::shared_ptr<Edge>>& edges, Cell* neighborCell);

			// For debug rendering
			GLuint vao;
			unsigned int size;
			GLuint fillVao;
			unsigned int fillSize;
			GLuint borderVao;
			unsigned int borderSize;
			GLuint posPinVao;
			unsigned int posPinSize;
			GLuint graphLineVao;
			unsigned int graphLineSize;

			/**
			*	helper recursion function for noisy edge
			*	@param [in] e0 Start vertex of edge
			*	@param [in] e1 End vertex of edge
			*	@param [in] c0 Owner cell position of edge
			*	@param [in] c1 CoOwner cell position of edge
			*	@param level A level of recursion.
			*/
			void buildNoisyEdge(const glm::vec2& e0, const glm::vec2& e1, const glm::vec2& c0, const glm::vec2& c1, std::vector<glm::vec2>& points, int level, const int startLevel);
			
			// For inifinite edges
			void clipInfiniteEdge(const EdgeType& edge, glm::vec2& e0, glm::vec2& e1, const float bound);
			glm::vec2 retrivePoint(const CellType& cell);
		public:
			Diagram();
			~Diagram();

			// Construct voronoi diagram based on random sites
			//void construct(const std::vector<glm::ivec2>& randomSites);
			void construct(const std::vector<Site>& randomSites, const float minBound, const float maxBound);
			// Relax voronoi diagram by using Lloyd's relaxation algorithm
			std::vector<Site> relax();
			// Build cells with edges. Any cells that has edges out of boundary will be omitted.
			void buildCells(boost::polygon::voronoi_diagram<double>& vd);
			// Randomize cells by removing cells
			void randomizeCells(const int w, const int l);
			// Build graph based on cells.
			void buildGraph(const int w, const int l);
			// Remove duplicated edges
			void removeDuplicatedEdges();
			// Make edges noisy. Ref: https://www.redblobgames.com/maps/noisy-edges
			void makeSharedEdgesNoisy();
			// Intialize debug lines of diagram
			void initDebugDiagram(const bool sharedEdges, const bool omittedCells, const bool posPin, const bool graph, const bool fill, const bool infiniteEdges, const bool border);

			// get cells
			std::map<unsigned int, Cell*>& getCells();

			// Find shortest path from src to all cell using dijkstra
			void findShortestPathFromSrc(const unsigned int src, std::vector<float>& dist, std::vector<unsigned int>& prevPath);
			
			// Check if point is in boundary
			bool isPointInBoundary(const glm::vec2& point);

			// Get boundary
			float getMinBound();
			float getMaxBound();

			// render the diagram
			void render();
		};
	}
}

#endif