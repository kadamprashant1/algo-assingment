// Question 8: random binary trees and alternating sums at leaf nodes.

#include <bits/stdc++.h>
#include <random>
using namespace std;

struct Node
{
    int key;
    Node *left = NULL;
    Node *right = NULL;
};

Node *buildRandomTree(int nodeCount)
{
    if (nodeCount == 0)
    {
        return nullptr;
    }

    uniform_int_distribution<int> split(0, nodeCount - 1);
    int leftCount = rand() % nodeCount;
    int rightCount = nodeCount - 1 - leftCount;

    Node *root = new Node();
    root->key = rand() % 100;
    root->left = buildRandomTree(leftCount);
    root->right = buildRandomTree(rightCount);
    return root;
}

void printLeafAlternatingSums(const Node *root, int alternatingSum, int depth)
{
    if (root == nullptr)
    {
        return;
    }
    int currentSum = alternatingSum + (depth % 2 == 0 ? root->key : -root->key);
    if (root->left == nullptr && root->right == nullptr)
    {
        cout << currentSum << ' ';
        return;
    }
    printLeafAlternatingSums(root->left, currentSum, depth + 1);
    printLeafAlternatingSums(root->right, currentSum, depth + 1);
}

void printNodeInOrder(const Node *root)
{
    if (root == nullptr)
    {
        return;
    }
    printNodeInOrder(root->left);
    cout << root->key << ' ';
    printNodeInOrder(root->right);
}

void deleteTree(Node *root)
{
    if (root == nullptr)
    {
        return;
    }
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

int main()
{
    int nodeCount;
    cout << "Enter number of nodes: ";
    cin >> nodeCount;
    if (nodeCount < 0)
    {
        cout << "Number of nodes cannot be negative.\n";
        return 1;
    }
    Node *root = buildRandomTree(nodeCount);
    cout << "In-order traversal :\n";
    printNodeInOrder(root);
    cout << '\n';
    cout << "Alternating sums at leaf nodes :\n";
    printLeafAlternatingSums(root, 0, 0);
    deleteTree(root);
    return 0;
}
