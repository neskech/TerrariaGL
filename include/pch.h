#pragma once

#include <memory>


template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T>
using WeakRef = std::weak_ptr<T>;

template<typename T>
using Scoped = std::unique_ptr<T>;