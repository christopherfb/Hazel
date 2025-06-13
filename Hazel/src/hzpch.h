#pragma once

// Precompiled header 

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Hazel/Log.h"

// -- Renderer related ----------------------
#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/RenderCommand.h"

#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/VertexArray.h"

#include "Hazel/Renderer/OrthographicCamera.h"
// -----------------------------------------


#ifdef HZ_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

