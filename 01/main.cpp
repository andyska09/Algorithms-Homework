#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>
#include <compare>

enum struct Direction {
  UP, DOWN, LEFT, RIGHT
};

struct Position {
  size_t row = -1, col = -1;

  constexpr Position() = default;
  constexpr Position(size_t r, size_t c) : row(r), col(c) {}

  Position move(Direction d) const {
    // using enum Direction;
    switch (d) {
      case Direction::UP:    return { row - 1, col };
      case Direction::DOWN:  return { row + 1, col };
      case Direction::LEFT:  return { row, col - 1 };
      case Direction::RIGHT: return { row, col + 1 };
    }
    assert(0);
  }

  friend constexpr auto operator <=> (Position, Position) = default;
};

enum struct Tile : uint8_t {
  WALL, EMPTY, TRAP
};

struct Map {
  static inline constexpr Position INVALID_POS = { -size_t(1), -size_t(1) };

  std::vector<std::vector<Tile>> tiles;
  Position hero = INVALID_POS, beast = INVALID_POS;
  Position exit = INVALID_POS;


  Map() = default;
  Map(std::string_view s) {
    tiles.emplace_back();

    auto set_pos = [&](Position& p, const char *err_msg) {
      if (p != INVALID_POS) throw std::invalid_argument(err_msg);
      p = { tiles.size() - 1, tiles.back().size() };
      tiles.back().push_back(Tile::EMPTY);
    };

    for (char c : s) switch (c) {
      case 'H': set_pos(hero, "Multiple heroes");
        break;
      case 'B': set_pos(beast, "Multiple beasts");
        break;
      case 'E': set_pos(exit, "Multiple exits");
        break;
      case ' ': tiles.back().push_back(Tile::EMPTY);
        break;
      case 'W': tiles.back().push_back(Tile::WALL);
        break;
      case 'T': tiles.back().push_back(Tile::TRAP);
        break;
      case '\n': tiles.emplace_back();
        break;
      default:
        throw std::invalid_argument("Unknown tile");
    }

    if (hero == INVALID_POS) throw std::invalid_argument("No hero");
    if (beast == INVALID_POS) throw std::invalid_argument("No beast");
    if (exit == INVALID_POS) throw std::invalid_argument("No exit");

    for (const auto& row : tiles)
      if (row.size() != tiles[0].size())
        throw std::invalid_argument("Non-rectangular maze");
  }

  // Get tile at given position, WALL if the position is out of map
  Tile operator [] (Position p) const {
    if (p.row >= tiles.size()) return Tile::WALL;
    const auto& row = tiles[p.row];
    return p.col < row.size() ? row[p.col] : Tile::WALL;
  }
};

using Path = std::vector<Position>;

#endif

// Beast is always a struct with a method `move`
// with the same signature as in SampleBeast
constexpr std::size_t PRIME_NUM = 37;
struct HashPair {
    std::size_t operator()(const std::pair<Position, Position>& pair) const {
        std::size_t hashHeroRow = std::hash<size_t>{}(pair.first.row);
        std::size_t hashHeroCol = std::hash<size_t>{}(pair.first.col);
        std::size_t hashHero = (hashHeroRow + (hashHeroCol << 1)) * PRIME_NUM;

        std::size_t hashBeastRow = std::hash<size_t>{}(pair.second.row);
        std::size_t hashBeastCol = std::hash<size_t>{}(pair.second.col);
        std::size_t hashBeast = (hashBeastRow + (hashBeastCol << 1)) * PRIME_NUM;

        return (hashHero ^ (hashBeast << 1)) * PRIME_NUM;
    }
};

void reconstructPath(Path& result, const std::unordered_map<std::pair<Position, Position>, std::pair<Position, Position>, HashPair>& previous, const std::pair<Position, Position>& endGameState, const std::pair<Position, Position>& startGameState) {
    std::pair<Position, Position> currentGameState = endGameState;
    while(startGameState != currentGameState) {
        result.push_back(currentGameState.first);
        currentGameState = previous.at(currentGameState);
    }
    result.push_back(currentGameState.first);
    std::reverse(result.begin(), result.end());
}

template < typename Beast >
Path find_escape_route(const Map& map, const Beast& beast) {
    std::unordered_map<std::pair<Position, Position>, std::pair<Position, Position>, HashPair> previous;
    std::queue<std::pair<Position, Position>> q;
    std::vector<Direction> directions = { Direction::UP,  Direction::DOWN,  Direction::RIGHT,  Direction::LEFT};
    
    std::pair<Position, Position> startGameState(map.hero, map.beast);
    std::pair<Position, Position> endGameState(map.hero, map.beast);

    previous.insert({startGameState, startGameState});
    q.push(startGameState);
    Path result;
    while(q.size() > 0) {
        std::pair<Position, Position> curr = q.front();
        q.pop();
        for(const auto &dir : directions) {
            Position newHeroPos = curr.first.move(dir);
            if(map[newHeroPos] == Tile::EMPTY) {
                Position newBeastPos = beast.move(map, newHeroPos, curr.second);
                if(newBeastPos != newHeroPos) {
                    std::pair<Position, Position> nextGameState(newHeroPos, newBeastPos);
                    if(!previous.contains(nextGameState)) {
                        previous.insert({nextGameState, curr});
                        q.push(nextGameState);
                        if(nextGameState.first == map.exit) {
                            endGameState = nextGameState;
                            reconstructPath(result, previous, endGameState, startGameState);
                            return result;
                        }
                    }
                }
            }
        }
    }
    return result;
}

#ifndef __PROGTEST__


// Sample beast which does two moves per one hero move and it might be
// allowed to step on traps
struct SampleBeast {
  SampleBeast(bool can_step_on_trap) : can_step_on_trap(can_step_on_trap) {}

  Position move(const Map& map, Position hero, Position beast) const {
    return one_move(map, hero, one_move(map, hero, beast));
  }

  private:
  Position one_move(const Map& map, Position hero, Position beast) const {
    // using enum Direction;

    if (beast.row != hero.row) {
      Position target = beast.move(beast.row > hero.row ? Direction::UP : Direction::DOWN);
      if (can_move_to(map, target)) return target;
    }

    if (beast.col != hero.col) {
      Position target = beast.move(beast.col > hero.col ? Direction::LEFT : Direction::RIGHT);
      if (can_move_to(map, target)) return target;
    }

    return beast;
  }

  bool can_move_to(const Map& map, Position p) const {
    switch (map[p]) {
      case Tile::EMPTY: return true;
      case Tile::TRAP: return can_step_on_trap;
      default: return false;
    }
  }

  bool can_step_on_trap;
};

// solutions for SampleBeast<true> and SampleBeast<false>
const std::tuple<size_t, size_t, Map> TESTS[] = {
  {  7,  7, Map{"E     H              B"}},
  {  0,  0, Map{"E            H       B"}},
  { 14, 14, Map{"E            H   W   B"}},
  {  0,  0, Map{"E  W  H              B"}},
  {  0,  0, Map{"            H       BE"}},
  {  0,  0, Map{"E  T  H              B"}},
  {  7,  7, Map{"E     H        T     B"}},

  { 10, 10, Map{"E        H  W   W   WB\n"
                "              W   W   "}},

  { 10, 10, Map{"E   W  H    W   W   WB\n"
                "              W   W   "}},


  {  0, 26, Map{"W   W   T   T   T   WB\n"
                "E W   W   W   W  H    "}},

  { 16, 16, Map{"W   W   W H W   W   WB\n"
                "E W   W   W   W   W   "}},

  {  0, 27, Map{"E                     \n"
                "   WWWWWWWWWTWWWWWW   \n"
                "   W  B         H W   \n"
                "                      "}},

  { 36, 36, Map{"E                     \n"
                "  WWWWWWWWWWWWWWW W   \n"
                "            W   WHW B \n"
                "            W W W W   \n"
                "            W W W W   \n"
                "            W W W W   \n"
                "              W   W   "}},

  {  0, 33, Map{"E                                 B\n"
                "                                   \n"
                "                                   \n"
                "                                   \n"
                "                         T         \n"
                "                           H       \n"
                "                                   "}},

  { 35, 35, Map{"E                                 B\n"
                "                                   \n"
                "                                   \n"
                "                                   \n"
                "                         W         \n"
                "                         W H       \n"
                "                                   "}},
};

template < typename Beast >
bool test_map(size_t ref, const Map& map, const Beast& beast) {
  auto sol = find_escape_route(map, beast);

  // only basic check, feel free to improve
  return sol.size() == ref;
}

int main() {
  int ok = 0, fail = 0;

  for (auto&& [ ref_t, ref_f, map ] : TESTS) {
    (test_map(ref_t, map, SampleBeast{true}) ? ok : fail)++;
    (test_map(ref_f, map, SampleBeast{false}) ? ok : fail)++;
  }
 
  if (fail == 0) std::cout << "Passed all " << ok << " tests!" << std::endl;
  else std::cout << fail << " of " << ok + fail << " tests failed" << std::endl;
  return 0;
}

#endif


