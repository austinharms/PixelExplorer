#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_MAX_DIMENSIONS 2048
#ifdef PXE_DEBUG
#define STBI_FAILURE_USERMSG
#endif // PXE_DEBUG

#include "stb_image.h"