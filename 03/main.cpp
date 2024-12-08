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
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>
#include <compare>

struct Block
{
    uint64_t x, y, h;

    friend auto operator<=>(const Block &, const Block &) = default;
};

bool check_tower(const std::vector<Block> &tower)
{
    for (size_t i = 1; i < tower.size(); i++)
        if (tower[i - 1].x < tower[i].x || tower[i - 1].y < tower[i].y)
            return false;

    return true;
}

uint64_t tower_height(const std::vector<Block> &tower)
{
    uint64_t h = 0;
    for (const auto &b : tower)
        h += b.h;
    return h;
}

#endif

uint64_t NRP(const std::vector<Block> &sortedBlocks, std::vector<uint64_t> &T, std::vector<int> &N)
{
    size_t n = sortedBlocks.size() - 1;
    for (int i = n; i >= 0; i--)
    {
        T[i] = sortedBlocks[i].h;
        uint64_t currHeight = T[i];
        N[i] = -1;
        for (size_t j = i + 1; j <= n; j++)
        {
            if (sortedBlocks[i].y <= sortedBlocks[j].y && T[i] < currHeight + T[j])
            {
                T[i] = currHeight + T[j];
                N[i] = j;
            }
        }
    }
    return T[0];
}

std::vector<Block> highest_tower(const std::vector<Block> &available_blocks)
{
    std::vector<Block> sorted_blocks = available_blocks;

    std::sort(sorted_blocks.begin(), sorted_blocks.end(), [](const Block &a, const Block &b)
              {
                  return (a.x < b.x) || (a.x == b.x && a.y < b.y);
              });
    std::vector<int> N(available_blocks.size() + 1, -1);
    std::vector<uint64_t> T(available_blocks.size() + 1, 0);
    sorted_blocks.insert(sorted_blocks.begin(), {0, 0, 0});
    std::cout << "==============result: " << std::endl;
    std::cout << "len: " << NRP(sorted_blocks, T, N) << std::endl;
    int currIndex = N[0];
    std::vector<Block> result;
    while (currIndex > -1)
    {
        result.push_back(sorted_blocks[currIndex]);
        currIndex = N[currIndex];
    }
    std::reverse(result.begin(), result.end());
    for (const auto &block : result)
    {
        std::cout << "{ " << block.x << ", " << block.y << ", " << block.h << " }" << std::endl;
    }
    std::cout << "====================" << std::endl;
    return result;
}

#ifndef __PROGTEST__

////////////////// Dark magic, ignore ////////////////////////

template <typename T>
auto quote(const T &t) { return t; }

std::string quote(const std::string &s)
{
    std::string ret = "\"";
    for (char c : s)
        if (c != '\n')
            ret += c;
        else
            ret += "\\n";
    return ret + "\"";
}

#define STR_(a) #a
#define STR(a) STR_(a)

#define CHECK_(a, b, a_str, b_str)                                   \
    do                                                               \
    {                                                                \
        auto _a = (a);                                               \
        decltype(a) _b = (b);                                        \
        if (_a != _b)                                                \
        {                                                            \
            std::cout << "Line " << __LINE__ << ": Assertion "       \
                      << a_str << " == " << b_str << " failed!"      \
                      << " (lhs: " << quote(_a) << ")" << std::endl; \
            fail++;                                                  \
        }                                                            \
        else                                                         \
            ok++;                                                    \
    } while (0)

#define CHECK(a, b) CHECK_(a, b, #a, #b)

////////////////// End of dark magic ////////////////////////

inline const std::vector<std::pair<uint64_t, std::vector<Block>>> TESTS = {
    {16, {
             {3, 9, 6},
             {9, 13, 3},
             {8, 11, 4},
             {4, 13, 4},
             {1, 7, 3},
         }}, /* 20 */
    {24, {
             {2, 12, 7},
             {3, 13, 7},
             {4, 20, 3},
             {9, 18, 10},
             {4, 9, 6},
         }}, /* 33 */
    {17, {
             {7, 11, 5},
             {8, 20, 7},
             {5, 18, 10},
             {13, 10, 9},
             {9, 18, 7},
             {19, 2, 10},
         }}, /* 48 */
    {34, {
             {14, 18, 12},
             {17, 11, 12},
             {11, 2, 7},
             {5, 2, 12},
             {14, 7, 2},
             {11, 9, 3},
         }}, /* 48 */
    {24, {
             {8, 1, 3},
             {8, 18, 11},
             {17, 18, 6},
             {4, 6, 2},
             {16, 1, 12},
             {1, 4, 5},
             {15, 11, 11},
         }}, /* 50 */
    {29, {
             {18, 17, 7},
             {12, 8, 3},
             {15, 3, 12},
             {3, 1, 5},
             {11, 17, 7},
             {12, 19, 9},
             {16, 17, 5},
         }}, /* 48 */
    {26, {
             {18, 3, 9},
             {11, 19, 1},
             {2, 1, 7},
             {3, 10, 8},
             {12, 10, 3},
             {14, 16, 8},
             {8, 14, 2},
             {5, 3, 7},
         }}, /* 45 */
    {19, {
             {12, 6, 1},
             {14, 1, 6},
             {16, 7, 1},
             {9, 8, 4},
             {19, 19, 4},
             {15, 19, 2},
             {19, 1, 9},
             {12, 6, 6},
         }}, /* 33 */
    {25, {
             {12, 16, 1},
             {6, 10, 9},
             {7, 13, 5},
             {11, 9, 6},
             {10, 6, 8},
             {11, 5, 2},
             {10, 4, 2},
             {1, 8, 10},
             {3, 20, 6},
         }}, /* 49 */
    {40, {
             {13, 11, 10},
             {7, 17, 6},
             {19, 8, 7},
             {7, 9, 10},
             {19, 1, 10},
             {4, 1, 12},
             {7, 3, 8},
             {20, 10, 2},
             {2, 17, 11},
         }}, /* 76 */
    {47, {
             {3, 10, 11},
             {7, 11, 7},
             {5, 11, 11},
             {14, 1, 8},
             {18, 13, 6},
             {14, 13, 12},
             {9, 8, 8},
             {12, 8, 6},
             {19, 6, 11},
             {11, 17, 7},
         }}, /* 87 */
    {40, {
             {14, 3, 9},
             {13, 4, 2},
             {4, 4, 9},
             {13, 3, 12},
             {4, 5, 11},
             {8, 13, 4},
             {13, 5, 10},
             {5, 14, 4},
             {13, 6, 10},
             {6, 11, 4},
         }}, /* 75 */
    {24, {
             {15, 16, 1},
             {19, 10, 2},
             {14, 7, 10},
             {19, 18, 2},
             {9, 17, 2},
             {11, 5, 3},
             {17, 19, 10},
             {18, 15, 9},
             {10, 12, 11},
             {6, 13, 6},
             {7, 11, 1},
         }}, /* 57 */
    {35, {
             {15, 19, 9},
             {5, 18, 4},
             {4, 5, 5},
             {7, 7, 8},
             {5, 11, 9},
             {10, 14, 1},
             {3, 18, 1},
             {19, 1, 5},
             {11, 11, 4},
             {6, 6, 9},
             {2, 15, 2},
         }}, /* 57 */
    {38, {
             {4, 1, 5},
             {4, 6, 5},
             {18, 1, 4},
             {5, 8, 7},
             {6, 3, 11},
             {19, 5, 7},
             {19, 16, 10},
             {2, 12, 7},
             {19, 11, 5},
             {10, 12, 9},
             {1, 14, 4},
             {14, 17, 1},
         }}, /* 75 */
    {33, {
             {1, 9, 7},
             {8, 16, 6},
             {13, 14, 5},
             {20, 4, 12},
             {7, 19, 11},
             {7, 9, 10},
             {16, 4, 4},
             {5, 16, 10},
             {10, 14, 11},
             {2, 1, 5},
             {15, 2, 5},
             {8, 19, 5},
         }}, /* 91 */
    {35, {
             {12, 6, 8},
             {7, 11, 8},
             {3, 15, 3},
             {5, 6, 4},
             {20, 13, 11},
             {12, 10, 4},
             {12, 16, 9},
             {18, 6, 2},
             {16, 6, 6},
             {5, 2, 3},
             {4, 10, 10},
             {17, 3, 10},
             {7, 14, 8},
         }}, /* 86 */
    {57, {
             {11, 15, 9},
             {18, 14, 4},
             {10, 8, 4},
             {16, 18, 8},
             {18, 18, 6},
             {4, 7, 6},
             {15, 11, 1},
             {19, 18, 12},
             {19, 5, 2},
             {2, 15, 1},
             {16, 8, 10},
             {14, 16, 12},
             {18, 13, 9},
         }}, /* 84 */
    {62, {
             {10, 3, 9},
             {6, 3, 7},
             {12, 3, 12},
             {7, 15, 1},
             {2, 13, 12},
             {14, 12, 11},
             {16, 18, 2},
             {14, 20, 6},
             {20, 15, 5},
             {9, 3, 8},
             {7, 9, 6},
             {4, 11, 6},
             {2, 3, 9},
             {12, 20, 8},
         }}, /* 102 */
    {37, {
             {5, 1, 6},
             {2, 1, 7},
             {5, 8, 11},
             {19, 5, 3},
             {11, 17, 5},
             {14, 10, 2},
             {6, 8, 4},
             {2, 8, 4},
             {2, 20, 6},
             {15, 20, 2},
             {13, 16, 3},
             {15, 14, 5},
             {3, 14, 9},
             {11, 1, 3},
         }}, /* 70 */
    {16'000'000'000, {
                         {3, 9, 6'000'000'000},
                         {9, 13, 3'000'000'000},
                         {8, 11, 4'000'000'000},
                         {4, 13, 4'000'000'000},
                         {1, 7, 3'000'000'000},
                     }}, /* 20'000'000'000 */
};

int main()
{
    int ok = 0, fail = 0;
    for (auto &&T : TESTS)
    {
        auto sol = highest_tower(T.second);
        // some checks are intentionally missing
        if (check_tower(sol) && tower_height(sol) == T.first)
        {
            ok++;
            std::cout << "OK" << std::endl;
        }
        else
        {
            fail++;
            std::cout << "FAIL" << std::endl;
        }
    }

    if (!fail)
        std::cout << "Passed all " << ok << " tests!" << std::endl;
    else
        std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;
}

#endif
