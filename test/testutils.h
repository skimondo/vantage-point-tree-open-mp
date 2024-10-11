#pragma once

#include <vpserial.h>

#include <catch2/catch_all.hpp>

namespace Catch {

// Permet d'afficher le résultat de la comparaison des vecteurs dans un test
template <>
struct StringMaker<SearchItem> {
  static std::string convert(const SearchItem& obj) {
    return "(" + std::to_string(obj.dist) + ", " + std::to_string(obj.idx) + ")";
  }
};

}  // namespace Catch
