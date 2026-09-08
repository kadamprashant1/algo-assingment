#include <bits/stdc++.h>
using namespace std;

struct Node
{
    int val;
    Node* prev;
    Node* next;
};

Node *createListNode(string input)
{
    int size = input.length(), start = 0;
    while (start < size && input[start] == '0')
        start++;

    Node *head = NULL, *tail = NULL;
    size -= 1;
    while (start <= size)
    {
        Node *temp = new Node;        
        temp->val = input[size] - '0';
        temp->next = NULL;
        temp->prev = NULL;

        if (head == NULL)
            head = tail = temp;
        else
        {
            tail->next = temp;
            temp->prev = tail;          
            tail = temp;
        }
        size--;
    }
    return head;
}

void printNumber(Node *num)
{
    if (num == NULL)
    {
        cout << "No integer passed in printNumber function" << endl;
        return;
    }
    for (Node *temp = num; temp != NULL; temp = temp->next)
        cout << temp->val;
    cout << '\n';
}

Node *performSum(Node *num1, Node *num2)
{
    Node *temp1 = num1, *temp2 = num2;
    int carry = 0;
    string ans;
    while (temp1 && temp2)
    {
        int sum = temp1->val + temp2->val + carry;
        carry = sum / 10;
        ans += to_string(sum % 10);
        temp1 = temp1->next;
        temp2 = temp2->next;
    }
    while (temp1)
    {
        int sum = temp1->val + carry;
        carry = sum / 10;
        ans += to_string(sum % 10);
        temp1 = temp1->next;
    }
    while (temp2)
    {
        int sum = temp2->val + carry;
        carry = sum / 10;
        ans += to_string(sum % 10);
        temp2 = temp2->next;
    }
    if (carry)                           
        ans += to_string(carry);
    if (ans.empty())                    
        ans = "0";
    return createListNode(ans);
}

Node* performMultiplication(Node* num1, Node* num2)
{
    if (!num1 || !num2) return NULL;

    vector<int> A, B;
    for (Node* p = num1; p != NULL; p = p->next) A.push_back(p->val); 
    for (Node* p = num2; p != NULL; p = p->next) B.push_back(p->val);

    int n = A.size(), m = B.size();
    vector<int> res(n + m, 0);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            res[i + j] += A[i] * B[j];

    int carry = 0;
    for (int i = 0; i < n + m; i++)
    {
        long long current = res[i] + carry;
        res[i] = current % 10;
        carry = current / 10;
    }

    while (res.size() > 1 && res.back() == 0) 
        res.pop_back();

    Node *head = NULL, *tail = NULL;           
    for (int d : res)
    {
        Node* temp = new Node;
        temp->val = d;
        temp->next = NULL;
        temp->prev = NULL;
        if (head == NULL)
            head = tail = temp;
        else
        {
            tail->next = temp;
            temp->prev = tail;
            tail = temp;
        }
    }
    return head;
}

int main()
{
    cout << "Enter two numbers" << endl;
    string input1, input2;
    cout << "number 1 : ";
    cin >> input1;
    Node *num1 = createListNode(input1);

    cout << "number 2 : ";
    cin >> input2;
    Node *num2 = createListNode(input2);

    cout << "Sum          : ";
    printNumber(performSum(num1, num2));

    cout << "Product      : ";
    printNumber(performMultiplication(num1, num2));

    return 0;
}
