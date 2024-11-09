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
    Queue() : root(nullptr) {};
    ~Queue()
    {
        deleteTree(root);
    }

    struct Node
    {
        Node *left;
        Node *right;
        Node *parent;
        size_t size;
        size_t height;
        T value;
        Node(T value) : left(nullptr), right(nullptr), parent(nullptr), size(1), height(1), value(value) {};
    };

    void deleteTree(Node *node)
    {
        if (node == nullptr)
            return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

    struct Ref
    {
        Node *ptrNode;
        Ref(Node *newNode) : ptrNode(newNode) {};
    };

    // AVL array

    // helper
    size_t getSize(Node *node) const
    {
        return node ? node->size : 0;
    }

    void updateNode(Node *node)
    {
        if (node)
        {
            node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
            node->size = 1 + getSize(node->left) + getSize(node->right);
        }
    }

    // main
    Node *insert(Node *curr, size_t index, Node *newNode)
    {
        if (curr == nullptr)
        {
            return newNode;
        }
        size_t leftSize = getSize(curr->left);
        if (index <= leftSize)
        {
            curr->left = insert(curr->left, index, newNode);
            curr->left->parent = curr;
        }
        else
        {
            curr->right = insert(curr->right, index - leftSize - 1, newNode);
            curr->right->parent = curr;
        }
        updateNode(curr);
        return curr;
    }

    Node *findMin(Node *node)
    {
        if (node->left == nullptr)
            return node;
        return findMin(node->left);
    }

    Node *erase(Node *curr, size_t index)
    {
        if (curr == nullptr)
        {
            throw std::out_of_range("Index out of range");
        }

        size_t leftSize = getSize(curr->left);
        if (index < leftSize)
        {
            curr->left = erase(curr->left, index);
        }
        else if (index > leftSize)
        {
            curr->right = erase(curr->right, index - leftSize - 1);
        }
        else
        {
            if (!curr->left || !curr->right)
            {
                Node *temp = curr->left ? curr->left : curr->right;
                if (temp)
                    temp->parent = curr->parent;
                delete curr;
                return temp;
            }
            else
            {
                Node *temp = findMin(curr->right);
                curr->value = temp->value;
                curr->right = erase(curr->right, 0);
            }
        }
        updateNode(curr);
        return curr;
    }

    void printInorder(Node *curr) const
    {
        if (curr)
        {
            printInorder(curr->left);
            std::cout << "val: " << curr->value << "  size: " << curr->size << std::endl;
            printInorder(curr->right);
        }
    }

    // QUEUE
    bool empty() const { return !root; }
    size_t size() const { return empty() ? 0 : root->size; }

    Ref push_last(T x)
    {
        Node *newNode = new Node(x);
        if (root == nullptr)
        {
            root = newNode;
        }
        else
        {
            root = insert(root, root->size, newNode);
        }
        return Queue<T>::Ref(newNode);
    }

    T pop_first()
    {
        T value = findMin(root)->value;
        root = erase(root, 0);
        return value;
    }

    size_t position(const Ref &it) const
    {
        Node *current = it.ptrNode;
        size_t pos = getSize(current->left);
        while (current->parent)
        {
            if (current == current->parent->right)
            {
                pos += getSize(current->parent->left) + 1;
            }
            else
            {
                break;
            }
            current = current->parent;
        }
        return pos;
    }

    void jump_ahead(const Ref &it, size_t positions) { return; }

    Node *root;
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
    std::cout << "==============================" << std::endl;
    Q.printInorder(Q.root);
    std::cout << "TOTAL SIZE: " << Q.root->size << std::endl;
    std::cout << "==============================" << std::endl;
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
