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

    // helper funcs

    void printInorder(Node *curr) const
    {
        if (curr)
        {
            printInorder(curr->left);
            std::cout << curr << " | " << "v: " << curr->value << "  size: " << curr->size << " h: " << curr->height << "\n"
                      << " l: " << curr->left << " r: " << curr->right << " p: " << curr->parent << std::endl;
            // std::cout << curr << " i: " << position(Ref(curr)) << " v: " << curr->value << std::endl;
            // std::cout << curr << " v: " << curr->value << " s: " << curr->size << std::endl;
            printInorder(curr->right);
        }
    }

    bool checkBalance(Node *curr) const
    {
        if (!curr)
        {
            return true;
        }
        int delta = getDelta(curr);
        if (delta < -1 || 1 < delta)
        {
            return false;
        }
        return checkBalance(curr->left) && checkBalance(curr->right);
    }

    size_t getSize(Node *node) const
    {
        return node ? node->size : 0;
    }

    size_t getHeight(Node *node) const
    {
        return node ? node->height : 0;
    }

    void updateNode(Node *node)
    {
        if (node)
        {
            node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
            node->size = 1 + getSize(node->left) + getSize(node->right);
        }
    }

    int getDelta(Node *node) const
    {
        return node ? getHeight(node->right) - getHeight(node->left) : 0;
    }

    // rotations
    Node *rotateRight(Node *y)
    {
        Node *x = y->left;
        Node *C = x->right;
        x->right = y;
        y->left = C;
        if (C)
        {
            C->parent = y;
        }
        x->parent = y->parent;
        y->parent = x;
        updateNode(y);
        updateNode(x);
        return x;
    }

    Node *rotateLeft(Node *x)
    {
        Node *y = x->right;
        Node *B = y->left;

        y->left = x;
        x->right = B;

        if (B)
        {
            B->parent = x;
        }
        y->parent = x->parent;
        x->parent = y;

        updateNode(x);
        updateNode(y);
        return y;
    }

    Node *balance(Node *node)
    {
        int delta = getDelta(node);
        if (delta < -1)
        {
            if (getDelta(node->left) > 0)
            {
                node->left = rotateLeft(node->left);
            }
            return rotateRight(node);
        }

        if (delta > 1)
        {
            if (getDelta(node->right) < 0)
            {
                node->right = rotateRight(node->right);
            }
            return rotateLeft(node);
        }
        return node;
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
        return balance(curr);
    }

    Node *findMin(Node *node)
    {
        if (node->left == nullptr)
            return node;
        return findMin(node->left);
    }

    Node *extractIndex(Node *curr, size_t index, Node *&extractedNode)
    {
        if (curr == nullptr)
        {
            throw std::out_of_range("Index out of range");
        }

        size_t leftSize = getSize(curr->left);
        if (index < leftSize)
        {
            curr->left = extractIndex(curr->left, index, extractedNode);
        }
        else if (index > leftSize)
        {
            curr->right = extractIndex(curr->right, index - leftSize - 1, extractedNode);
        }
        else
        {
            if (!curr->left || !curr->right)
            {
                Node *successor = curr->left ? curr->left : curr->right;
                if (successor)
                {
                    successor->parent = curr->parent;
                }
                curr->left = curr->right = curr->parent = nullptr;
                extractedNode = curr;
                return successor;
            }
            else
            {
                Node *successor = nullptr;
                curr->right = extractIndex(curr->right, 0, successor);

                successor->right = curr->right;
                if (successor->right)
                {
                    successor->right->parent = successor;
                }
                successor->left = curr->left;
                if (successor->left)
                {
                    successor->left->parent = successor;
                }
                successor->parent = curr->parent;
                curr->left = curr->right = curr->parent = nullptr;
                extractedNode = curr;
                updateNode(successor);
                return balance(successor);
            }
        }
        updateNode(curr);
        return balance(curr);
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
        Node *extracted = nullptr;
        root = extractIndex(root, 0, extracted);
        delete extracted;
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
            current = current->parent;
        }
        return pos;
    }

    void jump_ahead(const Ref &it, size_t positions)
    {
        size_t index = position(it);
        Node *extracted = nullptr;
        root = extractIndex(root, index, extracted);
        updateNode(extracted);
        size_t newIndex = 0;
        if (index > positions)
        {
            newIndex = index - positions;
        }
        root = insert(root, newIndex, extracted);
        return;
    }

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

void mytest(int &ok, int &fail)
{
    Queue<int> Q;
    CHECK(Q.empty(), true);
    CHECK(Q.size(), 0);
    std::vector<decltype(Q.push_last(0))> refs;

    constexpr int RUN = 10, TOT = 6;

    for (int i = 0; i < TOT; i++)
    {
        refs.push_back(Q.push_last(i % RUN));
        CHECK(Q.size(), i + 1);
    }

    // for (const auto &ref : refs)
    // {
    //     std::cout << ref.ptrNode << " v: " << ref.ptrNode->value << std::endl;
    // }

    // std::cout << "============BEFORE================" << std::endl;
    // Q.printInorder(Q.root);
    // std::cout << "TOTAL SIZE: " << Q.root->size << " h: " << Q.root->height << std::endl;
    // std::cout << "==============================" << std::endl;

    // Q.jump_ahead(refs[0], 15);

    Q.jump_ahead(refs[3], 0);
    // std::cout << "============AFTER================" << std::endl;
    // Q.printInorder(Q.root);
    // std::cout << "TOTAL SIZE: " << Q.root->size << " h: " << Q.root->height << std::endl;
    // std::cout << "==============================" << std::endl;

    CHECK(Q.size(), TOT);
    for (int i = 0; i < TOT; i++)
        CHECK(Q.position(refs[i]), i);
}

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
        CHECK(Q.checkBalance(Q.root), true);
    }
    for (int i = 0; i < TOT; i++)
    {
        CHECK(Q.pop_first(), i % RUN);
        CHECK(Q.size(), TOT - 1 - i);
        CHECK(Q.checkBalance(Q.root), true);
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

    // for (const auto &ref : refs)
    // {
    //     std::cout << ref.ptrNode << " v: " << ref.ptrNode->value << std::endl;
    // }

    for (int i = 0; i < TOT; i++)
        CHECK(Q.position(refs[i]), i);

    Q.jump_ahead(refs[0], 15);
    Q.jump_ahead(refs[3], 0);

    CHECK(Q.checkBalance(Q.root), true);

    CHECK(Q.size(), TOT);
    for (int i = 0; i < TOT; i++)
        CHECK(Q.position(refs[i]), i);

    Q.jump_ahead(refs[8], 100);
    Q.jump_ahead(refs[9], 100);
    Q.jump_ahead(refs[7], 1);

    CHECK(Q.checkBalance(Q.root), true);

    static_assert(RUN == 10 && TOT >= 30);
    for (int i : {9, 8, 0, 1, 2, 3, 4, 5, 7, 6})
        CHECK(Q.pop_first(), i);

    // std::cout << "============BEFORE================" << std::endl;
    // Q.printInorder(Q.root);
    // std::cout << "TOTAL SIZE: " << Q.root->size << " h: " << Q.root->height << std::endl;
    // std::cout << "==============================" << std::endl;
    CHECK(Q.checkBalance(Q.root), true);

    for (int i = 0; i < TOT * 2 / 3; i++)
    {
        CHECK(Q.pop_first(), i % RUN);
        CHECK(Q.size(), TOT - 11 - i);
    }
    CHECK(Q.checkBalance(Q.root), true);
    // std::cout << "============AFTER================" << std::endl;
    // Q.printInorder(Q.root);
    // std::cout << "TOTAL SIZE: " << Q.root->size << " h: " << Q.root->height << std::endl;
    // std::cout << "==============================" << std::endl;

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
        mytest(ok, fail);
    if (!fail)
        test1(ok, fail);
    if (!fail)
        test2(ok, fail);
    if (!fail)
        test_speed<1'000>(ok, fail);

    if (!fail)
        std::cout << "Passed all " << ok << " tests!" << std::endl;
    else
        std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;
}

#endif
