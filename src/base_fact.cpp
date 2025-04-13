// Copyright 2025 Justin Greisiger Frost
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include "myrmidon/base_fact.hpp"

// Include standard library headers for the types used in explicit instantiations
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
// #include <vector> // Duplicate include removed

// --- Forward Declaration or Include for Custom Types ---
namespace Myrmidon {
    // Add forward declarations for custom types used in RefFact if needed
    // class MyObject;
} // namespace Myrmidon


namespace Myrmidon {

// --- Explicit Template Instantiations ---
// Pre-compile common BaseFact specializations to potentially improve
// overall build times. Other cpp files linking against the library
// will use these pre-compiled versions instead of generating their own.

// --- Instantiations for ListFact<T> / ValueFact<T> ---
template class BaseFact<std::vector<int>>;
template class BaseFact<std::vector<double>>;
template class BaseFact<std::vector<std::string>>;
template class BaseFact<std::vector<bool>>;

// --- Instantiations for MapFact<K, V> ---
template class BaseFact<std::map<std::string, int>>;
template class BaseFact<std::map<std::string, double>>;
template class BaseFact<std::map<std::string, std::string>>;
template class BaseFact<std::map<int, std::string>>;
template class BaseFact<std::map<std::string, bool>>;

// --- Instantiations for SetFact<T> ---
template class BaseFact<std::unordered_set<int>>;
template class BaseFact<std::unordered_set<std::string>>;
template class BaseFact<std::unordered_set<bool>>;

// --- Instantiations for RefFact<T> ---
// template class BaseFact<MyObject*>;
// Add other common pointer types used with RefFact if needed

} // namespace Myrmidon
