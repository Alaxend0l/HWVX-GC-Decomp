#include "common.h"

/* the movie list to play back, and how far into it the next name starts */
u32 g_curMovieIndex;
s32 g_moviePlaybackName;
u8 g_buttonUpCnt;
s32 g_returnState = 4;

static char *StateMovie_GetNextMovieName(void) {
	/* the name is copied out of the list one character at a time */
	extern u32 strlen(char *);
	static char buffer[32];
	char *src;
	u32 n;
	u32 len;

	n = 0;
	if (g_moviePlaybackName == 0) {
		return NULL;
	}
	len = strlen((char *)g_moviePlaybackName);
	if (g_curMovieIndex >= len) {
		return NULL;
	}
	src = (char *)g_moviePlaybackName + g_curMovieIndex;
	while (g_curMovieIndex < len) {
		if (*src != ',') {
			buffer[n++] = *src;
		} else {
			g_curMovieIndex += 1;
			return buffer;
		}
		src += 1;
		g_curMovieIndex += 1;
	}
	buffer[n] = 0;
	return buffer;
}


void StateMovie_SetMovieToPlay(s32 arg0) {
    extern s32 g_moviePlaybackName;
    g_moviePlaybackName = arg0;
}

s32 StateMovie_Shutdown(void) {
    M2C_UNK StateGameManager_RefreshMCPTextures();
    StateGameManager_RefreshMCPTextures();
    return 1;
}
