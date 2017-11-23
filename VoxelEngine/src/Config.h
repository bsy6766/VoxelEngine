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
*	If disabled, all sub debug defines will be ignored.
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
*	If disabled, can't toggle to camera mode.
*/
#ifndef V_DEBUG_CAMERA_MODE
#define V_DEBUG_CAMERA_MODE 1
#endif

/**
*	@def V_DEBUG_CONSOLE
*	If enabled, allow debug console in the game
*	If disabled, disallow debug console in the game.
*/
#ifndef V_DEBUG_CONSOLE
#define V_DEBUG_CONSOLE 1
#endif

/**
*	@def V_DEBUG_CHUNK_BORDER_LINE
*	If enabled, can render chunk border debug line in ChunkMap.
*	If disabled, ignored.
*/
#ifndef V_DEBUG_CHUNK_BORDER_LINE
#define V_DEBUG_CHUNK_BORDER_LINE 1
#endif

/**
*	@def
*/

#endif
// Sub debug defiens ends

#endif	// CONFIG_H