#include<bits/stdc++.h>
using namespace std;

struct Node{
    int val;
    Node* prev;
    Node* next;
    Node(int v) : val(v), prev(NULL), next(NULL) {};
};

Node *createCircularList(int arr[], int n){
    Node *head = NULL, *tail = NULL;
    for (int i = 0; i < n; i++)
    {
        Node *temp = new Node(arr[i]);
        if (head == NULL)
            head = tail = temp;
        else
        {
            tail->next = temp;
            temp->prev = tail;
            tail = temp;
        }
    }
    tail->next = head;
    head->prev = tail;
    return head;
}

Node * applyoperation(Node *head, int k){
    if (!head || k <= 0) return head;

    vector<int> arr;
    Node* temp = head;
    do
    {
        arr.push_back(temp->val);
        temp = temp->next;
    }while (temp != head);

    int n = arr.size();
    vector<int> result(n);
    for (int i = 0; i < n; i++){
        int max_val = arr[i];
        for (int j = 1; j < k && (i + j) < n; j++){
            max_val = max(max_val, arr[i + j]);
        }
        result[i] = max_val;
    }

    Node *new_head = NULL, *new_tail = NULL;
    for (int val : result){
        Node* temp = new Node(val);
        if (new_head == NULL)  new_head = new_tail = temp;
        else
        {
            new_tail->next = temp;
            temp->prev = new_tail;
            new_tail = temp;
        }
    }
    new_tail->next = new_head;
    new_head->prev = new_tail;

    return new_head;
}


void printCircularList(Node* head){
    if (!head) return;
    Node* temp = head;
    do
    {
        cout << temp->val << " ";
        temp = temp->next;
    } while (temp != head);
    cout << endl;
}


int main(){
    cout<<"enter size of list : ";
    int n;
    cin>>n;
    cout<<""<<endl;
    int arr[n];
    cout<<"enter elements of list : ";
    for(int i=0;i<n;i++){
        cin>>arr[i];
    }
    cout<<""<<endl;

    cout<<"enter k : ";
    int k;
    cin>>k;
    cout<<""<<endl;
    Node *head = createCircularList(arr,n);
    head = applyoperation(head,k);
    printCircularList(head);
    return 0;
}