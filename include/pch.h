#pragma once

#if 1
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

#include <include/entt/entt.hpp>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <stdlib.h>
#include <cassert>
#include <unordered_map>
#include <fastNoiseLite.hpp>
#include <algorithm>
#include <time.h>  
#include <array>
#include <optional>
#include <string>
#include <functional>
#include <algorithm>


template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
using WeakRef = std::weak_ptr<T>;

template<typename T>
using Scoped = std::unique_ptr<T>;

#define MAX(x, y) x > y ? x : y
#define MIN(x, y) x < y ? x : y
#define CLAMP(x, min, max) MIN(max, MAX(x, min))

