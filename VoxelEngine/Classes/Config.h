#ifndef CONFIG_H
#define CONFIG_H

/**
*	Config
*
*	Defines various configurations.
*/

/**
*	@def V_DEBUG
*	If enabled, all sub debug defines will be applied. 
*/
#ifndef V_DEBUG
#define V_DEBUG 1
#endif

/**
*	Sub debug defines
*/
#if V_DEBUG

/**
*	@def V_DEBUG_CAMERA_MODE
*	If enabled, can toggle between normal view and camera view
*/
#ifndef V_DEBUG_CAMERA_MODE
#define V_DEBUG_CAMERA_MODE 1
#endif

/**
*	@def V_DEBUG_CONSOLE
*	If enabled, allow debug console in the game
*/
#ifndef V_DEBUG_CONSOLE
#define V_DEBUG_CONSOLE 1
#endif

/**
*	@def V_DEBUG_UI_TEST
*	If enabled, render test uis.
*/
#ifndef V_DEBUG_UI_TEST
#define V_DEBUG_UI_TEST 1
#endif

/**
*	@def V_DEBUG_CHUNK_BORDER_LINE
*	If enabled, can render chunk border debug line in ChunkMap.
*/
#ifndef V_DEBUG_CHUNK_BORDER_LINE
#define V_DEBUG_CHUNK_BORDER_LINE 1
#endif

/**
*	@def V_DEBUG_VORONOI_LINE
*	If enabled, can render voronoi diagram
*/
#ifndef V_DEBUG_VORONOI_LINE
#define V_DEBUG_VORONOI_LINE 0
#endif

/**
*	@def V_DEBUG_PLAYER_BOUNDING_BOX
*	If enabled, renders player's bounding box
*/
#ifndef V_DEBUG_PLAYER_BOUNDING_BOX
#define V_DEBUG_PLAYER_BOUNDING_BOX 1
#endif

/**
*	@def V_DEBUG_PLAYER_DIR_LINE
*	If enabled, renders player's direction vector
*/
#ifndef V_DEBUG_PLAYER_DIR_LINE
#define V_DEBUG_PLAYER_DIR_LINE 0
#endif

/**
*	@def V_DEBUG_FRUSTUM_LINE
*	if enabled, renders frustum line
*/
#ifndef V_DEBUG_FRUSTUM_LINE
#define V_DEBUG_FRUSTUM_LINE 1
#endif

/**
*	@def V_DEBUG_COUNT_VISIBLE_VERTICES
*	If enabled, counts total visbile vertices currently loaded in the game. This excludes debug console.
*/
#ifndef V_DEBUG_COUNT_VISIBLE_VERTICES
#define V_DEBUG_COUNT_VISIBLE_VERTICES 1
#endif

/**
*	@def V_DEBUG_COUNT_TOTAL_VERTICES
*	If enabled, counts total visbile vertices currently loaded in the game. This excludes debug console.
*/
#ifndef V_DEBUG_COUNT_TOTAL_VERTICES
#define V_DEBUG_COUNT_TOTAL_VERTICES 1
#endif

/**
*	@def V_DEBUG_COUNT_DRAW_CALLS
*	If enabled, counts number of draw calls taken each frame
*/
#ifndef V_DEBUG_COUNT_DRAW_CALLS
#define V_DEBUG_COUNT_DRAW_CALLS 1
#endif

/**
*	@def V_DEBUG_DRAW_UI_BOUNDING_BOX
*	If enabled, draws UI bounding box
*/
#ifndef V_DEBUG_DRAW_UI_BOUNDING_BOX
#define V_DEBUG_DRAW_UI_BOUNDING_BOX 0
#endif

#endif
// Sub debug defiens ends

#endif	// CONFIG_H