#include <bits/stdc++.h>
using namespace std;

bool isBalanced(const string &s)
{
    stack<char> st;
    for (char c : s)
    {
        if (c == '(' || c == '{' || c == '[')
        {
            st.push(c);
        }
        else if (c == ')' || c == '}' || c == ']')
        {
            if (st.empty())
                return false;

            char top = st.top();
            st.pop();

            if ((c == ')' && top != '(') || (c == '}' && top != '{') || (c == ']' && top != '['))
            {
                return false;
            }
        }
    }
    return st.empty();
}

int main()
{
    string s;
    cout << "Enter string: ";
    getline(cin, s);
    if (isBalanced(s))
        cout << "Balanced\n";
    else
        cout << "Not Balanced\n";
    return 0;
}
