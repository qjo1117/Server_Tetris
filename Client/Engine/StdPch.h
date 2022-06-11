#pragma once

#define _HAS_STD_BYTE 0

#include <Windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <memory>
#include <atomic>
#include <mutex>
#include <queue>
#include <stack>
#include <unordered_map>
#include <set>
#include <array>
#include <functional>
#include <typeinfo>
#include <cassert>
#include <filesystem>
#include <fstream>


using namespace std;
namespace fs = std::filesystem;

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;


template<typename T>
using Ref = shared_ptr<T>;

template<typename T>
using WRef = weak_ptr<T>;