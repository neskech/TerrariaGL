#pragma once

#if 1
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

#include "include/entt/entt.hpp"
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <stdlib.h>
#include <cassert>
#include <unordered_map>


template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
using WeakRef = std::weak_ptr<T>;

template<typename T>
using Scoped = std::unique_ptr<T>;