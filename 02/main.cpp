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
#include <stack>
#include <queue>
#include <random>
#include <type_traits>
#include <utility>

#endif

template <typename T>
struct Queue
{
    Queue() : root(nullptr), m_size(0) {};
    ~Queue()
    {
        deleteTree(root);
    }

    struct Node
    {
        Node *left;
        Node *right;
        size_t key;
        T value;
        Node(size_t key, T value) : left(nullptr), right(nullptr), key(key), value(value) {};
    };

    struct Ref
    {
        Node *ptrNode;
        Ref(Node *newNode) : ptrNode(newNode) {};
    };

    // AVL Tree stuff

    void deleteTree(Node *node)
    {
        if (node == nullptr)
            return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

    Node *insert(Node *currentNode, Node *ptrNewNode)
    {
        if (currentNode == nullptr)
        {
            return ptrNewNode;
        }
        if (currentNode->key < ptrNewNode->key)
        {
            currentNode->left = insert(currentNode->left, ptrNewNode);
        }
        else if (currentNode->key >= ptrNewNode->key)
        {
            currentNode->right = insert(currentNode->right, ptrNewNode);
        }
        return currentNode;
    }

    Node *findMin(Node *node)
    {
        if (node->left == nullptr)
            return node;
        return findMin(node->left);
    }
    // TODO
    Node *deleteNode(Node *root, Node *target)
    {
    }
    // end of AVL Tree Stuff

    bool empty() const { return size() == 0; }
    size_t size() const { return m_size; }

    Ref push_last(T x)
    {
        Node *newNode = new Node(size(), x);
        if (size() == 0)
        {
            root = newNode;
        }
        else
        {
            insert(root, newNode);
        }
        m_size++;
        return Queue<T>::Ref(newNode);
    }

    // TODO
    T pop_first() { return T(); } // throw std::out_of_range if empty

    size_t position(const Ref &it) const { return 0; }

    void jump_ahead(const Ref &it, size_t positions) { return; }

    Node *root;
    size_t m_size;
};

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

#define CHECK_EX(expr, ex)                                                                                                             \
    do                                                                                                                                 \
    {                                                                                                                                  \
        try                                                                                                                            \
        {                                                                                                                              \
            (expr);                                                                                                                    \
            fail++;                                                                                                                    \
            std::cout << "Line " << __LINE__ << ": Expected " STR(expr) " to throw " #ex " but no exception was raised." << std::endl; \
        }                                                                                                                              \
        catch (const ex &)                                                                                                             \
        {                                                                                                                              \
            ok++;                                                                                                                      \
        }                                                                                                                              \
        catch (...)                                                                                                                    \
        {                                                                                                                              \
            fail++;                                                                                                                    \
            std::cout << "Line " << __LINE__ << ": Expected " STR(expr) " to throw " #ex " but got different exception." << std::endl; \
        }                                                                                                                              \
    } while (0)

////////////////// End of dark magic ////////////////////////

void test1(int &ok, int &fail)
{
    Queue<int> Q;
    CHECK(Q.empty(), true);
    CHECK(Q.size(), 0);

    constexpr int RUN = 10, TOT = 105;

    for (int i = 0; i < TOT; i++)
    {
        Q.push_last(i % RUN);
        CHECK(Q.empty(), false);
        CHECK(Q.size(), i + 1);
    }

    for (int i = 0; i < TOT; i++)
    {
        CHECK(Q.pop_first(), i % RUN);
        CHECK(Q.size(), TOT - 1 - i);
    }

    CHECK(Q.empty(), true);
}

void test2(int &ok, int &fail)
{
    Queue<int> Q;
    CHECK(Q.empty(), true);
    CHECK(Q.size(), 0);
    std::vector<decltype(Q.push_last(0))> refs;

    constexpr int RUN = 10, TOT = 105;

    for (int i = 0; i < TOT; i++)
    {
        refs.push_back(Q.push_last(i % RUN));
        CHECK(Q.size(), i + 1);
    }

    for (int i = 0; i < TOT; i++)
        CHECK(Q.position(refs[i]), i);

    Q.jump_ahead(refs[0], 15);
    Q.jump_ahead(refs[3], 0);

    CHECK(Q.size(), TOT);
    for (int i = 0; i < TOT; i++)
        CHECK(Q.position(refs[i]), i);

    Q.jump_ahead(refs[8], 100);
    Q.jump_ahead(refs[9], 100);
    Q.jump_ahead(refs[7], 1);

    static_assert(RUN == 10 && TOT >= 30);
    for (int i : {9, 8, 0, 1, 2, 3, 4, 5, 7, 6})
        CHECK(Q.pop_first(), i);

    for (int i = 0; i < TOT * 2 / 3; i++)
    {
        CHECK(Q.pop_first(), i % RUN);
        CHECK(Q.size(), TOT - 11 - i);
    }

    CHECK(Q.empty(), false);
}

template <int TOT>
void test_speed(int &ok, int &fail)
{
    Queue<int> Q;
    CHECK(Q.empty(), true);
    CHECK(Q.size(), 0);
    std::vector<decltype(Q.push_last(0))> refs;

    for (int i = 0; i < TOT; i++)
    {
        refs.push_back(Q.push_last(i));
        CHECK(Q.size(), i + 1);
    }

    for (int i = 0; i < TOT; i++)
    {
        CHECK(Q.position(refs[i]), i);
        Q.jump_ahead(refs[i], i);
    }

    for (int i = 0; i < TOT; i++)
        CHECK(Q.position(refs[i]), TOT - 1 - i);

    for (int i = 0; i < TOT; i++)
    {
        CHECK(Q.pop_first(), TOT - 1 - i);
        CHECK(Q.size(), TOT - 1 - i);
    }

    CHECK(Q.empty(), true);
}

int main()
{
    int ok = 0, fail = 0;
    if (!fail)
        test1(ok, fail);
    // if (!fail)
    //     test2(ok, fail);
    // if (!fail)
    //     test_speed<1'000>(ok, fail);

    if (!fail)
        std::cout << "Passed all " << ok << " tests!" << std::endl;
    else
        std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;
}

#endif
