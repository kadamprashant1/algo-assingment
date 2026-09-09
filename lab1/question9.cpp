// Question 9: three-way search tree.

#include <bits/stdc++.h>
using namespace std;

struct Node
{
    int key1;
    int key2;
    bool hasSecondKey;
    Node *left;
    Node *middle;
    Node *right;

    explicit Node(int key) : key1(key), key2(0), hasSecondKey(false), left(nullptr), middle(nullptr), right(nullptr) {}
};

bool search(const Node *root, int key)
{
    if (root == nullptr)
    {
        return false;
    }
    if (key == root->key1 || (root->hasSecondKey && key == root->key2))
    {
        return true;
    }
    if (key < root->key1)
    {
        return search(root->left, key);
    }
    if (!root->hasSecondKey || key < root->key2)
    {
        return search(root->middle, key);
    }
    return search(root->right, key);
}

void insert(Node *&root, int key)
{
    if (root == nullptr)
    {
        root = new Node(key);
        return;
    }
    if (key == root->key1 || (root->hasSecondKey && key == root->key2))
    {
        return;
    }

    if (!root->hasSecondKey)
    {
        if (key < root->key1)
        {
            root->key2 = root->key1;
            root->key1 = key;
        }
        else
        {
            root->key2 = key;
        }
        root->hasSecondKey = true;
        return;
    }

    if (key < root->key1)
    {
        insert(root->left, key);
    }
    else if (key < root->key2)
    {
        insert(root->middle, key);
    }
    else
    {
        insert(root->right, key);
    }
}

void printInOrder(const Node *root)
{
    if (root == nullptr)
    {
        return;
    }
    printInOrder(root->left);
    cout << root->key1 << ' ';
    printInOrder(root->middle);
    if (root->hasSecondKey)
    {
        cout << root->key2 << ' ';
        printInOrder(root->right);
    }
}

void deleteTree(Node *root)
{
    if (root == nullptr)
    {
        return;
    }
    deleteTree(root->left);
    deleteTree(root->middle);
    deleteTree(root->right);
    delete root;
}

int main()
{
    Node *root = nullptr;
    int count;
    cout << "Enter number of keys: ";
    cin >> count;
    if (count < 0)
    {
        return 1;
    }
    cout << "Enter keys: ";
    for (int index = 0; index < count; ++index)
    {
        int key;
        cin >> key;
        insert(root, key);
    }
    cout << "Keys in sorted order: ";
    printInOrder(root);
    cout << "\nEnter key to search: ";
    int key;
    cin >> key;
    cout << (search(root, key) ? "Key found\n" : "Key not found\n");
    cout << "Enter key to insert: ";
    cin >> key;
    insert(root, key);
    cout << "Keys in sorted order after insertion: ";
    printInOrder(root);
    deleteTree(root);
    return 0;
}
