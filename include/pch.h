#pragma once

#if 1
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>



#include <memory>


template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
using WeakRef = std::weak_ptr<T>;

template<typename T>
using Scoped = std::unique_ptr<T>;