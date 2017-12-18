#ifndef CONFIG_H
#define CONFIG_H

/**
*	Config
*
*	Defines various configurations.
*/

/**
*	@def V_BUILD_NUMBER
*	If enabled, counts build number
*/
#ifndef V_BUILD_NUMBER
#define V_BUILD_NUMBER 1
#endif

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
*	@def V_DEBUG_EDITOR
*	If enabled, can use editor. Only for development. Editor is not incldued in the main game.
*/
#ifndef V_DEBUG_EDITOR
#define V_DEBUG_EDITOR 1
#endif


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
#define V_DEBUG_UI_TEST 0
#endif

/**
*	@def V_DEBUG_DRAW_UI_BOUNDING_BOX
*	If enabled, draws UI bounding box. Need to enable each ui bounding boxe configs.
*/
#ifndef V_DEBUG_DRAW_UI_BOUNDING_BOX
#define V_DEBUG_DRAW_UI_BOUNDING_BOX 1
#endif

#if V_DEBUG_DRAW_UI_BOUNDING_BOX

	/**
	*	@def V_DEBUG_DRAW_IMAGE_BOUNDING_BOX
	*	If enabled, draws Image UI bounding box.
	*/
	#ifndef V_DEBUG_DRAW_IMAGE_BOUNDING_BOX
	#define V_DEBUG_DRAW_IMAGE_BOUNDING_BOX 0
	#endif

	/**
	*	@def V_DEBUG_DRAW_ANIMATED_IMAGE_BOUNDING_BOX
	*	If enabled, draws animated image UI bounding box.
	*/
	#ifndef V_DEBUG_DRAW_ANIMATED_IMAGE_BOUNDING_BOX
	#define V_DEBUG_DRAW_ANIMATED_IMAGE_BOUNDING_BOX 0
	#endif

	/**
	*	@def V_DEBUG_DRAW_NINE_PATH_IMAGE_BOUNDING_BOX
	*	If enabled, draws nine patch UI bounding box.
	*/
	#ifndef V_DEBUG_DRAW_NINE_PATH_IMAGE_BOUNDING_BOX
	#define V_DEBUG_DRAW_NINE_PATH_IMAGE_BOUNDING_BOX 0
	#endif

	/**
	*	@def V_DEBUG_DRAW_TEXT_BOUNDING_BOX
	*	If enabled, draws Text UI bounding box.
	*/
	#ifndef V_DEBUG_DRAW_TEXT_BOUNDING_BOX
	#define V_DEBUG_DRAW_TEXT_BOUNDING_BOX 0
	#endif

	#if V_DEBUG_DRAW_TEXT_BOUNDING_BOX

		/**
		*	@def V_DEBUG_DRAW_TEXT_LINE_DIVIDER
		*	If enabled, draws debug line between texts lines.
		*/
		#ifndef V_DEBUG_DRAW_TEXT_LINE_DIVIDER
		#define V_DEBUG_DRAW_TEXT_LINE_DIVIDER 0
		#endif

		/**
		*	@def V_DEBUG_DRAW_TEXT_CHARACTER_DIVIDER
		*	If enabled, draws debug line between each character in text.
		*/
		#ifndef V_DEBUG_DRAW_TEXT_CHARACTER_DIVIDER
		#define V_DEBUG_DRAW_TEXT_CHARACTER_DIVIDER 0
		#endif

	#endif //V_DEBUG_DRAW_TEXT_BOUNDING_BOX

	/**
	*	@def V_DEBUG_DRAW_BUTTON_BOUNDING_BOX
	*	If enabled, draws Button UI bounding box.
	*/
	#ifndef V_DEBUG_DRAW_BUTTON_BOUNDING_BOX
	#define V_DEBUG_DRAW_BUTTON_BOUNDING_BOX 0
	#endif

	/**
	*	@def V_DEBUG_DRAW_CHECKBOX_BOUNDING_BOX
	*	If enabled, draws Checkbox UI bounding box.
	*/
	#ifndef V_DEBUG_DRAW_CHECKBOX_BOUNDING_BOX
	#define V_DEBUG_DRAW_CHECKBOX_BOUNDING_BOX 0
	#endif

	/**
	*	@def V_DEBUG_DRAW_PROGRESS_TIMER_BOUNDING_BOX
	*	If enabled, draws ProgressTimer UI bounding box.
	*/
	#ifndef V_DEBUG_DRAW_PROGRESS_TIMER_BOUNDING_BOX
	#define V_DEBUG_DRAW_PROGRESS_TIMER_BOUNDING_BOX 0
	#endif

	/**
	*	@def V_DEBUG_DRAW_SLIDER_BOUNDING_BOX
	*	If enabled, draws slider UI bounding box.
	*/
	#ifndef V_DEBUG_DRAW_SLIDER_BOUNDING_BOX
	#define V_DEBUG_DRAW_SLIDER_BOUNDING_BOX 0
	#endif

#endif // V_DEBUG_DRAW_UI_BOUNDING_BOX


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
#define V_DEBUG_FRUSTUM_LINE 0
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
*	@def V_DEBUG_LOG_CONSOLE
*	If enabled, output log in console from logger
*/
#ifndef V_DEBUG_LOG_CONSOLE
#define V_DEBUG_LOG_CONSOLE 1
#endif


#endif
// Sub debug defiens ends. V_DEBUG

#endif	// CONFIG_H