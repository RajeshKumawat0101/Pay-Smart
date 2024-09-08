#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <algorithm>
#include <climits>
using namespace std;
struct Bank{
    string name;
    int netAmount;
    unordered_set<string> types;
};
template <typename K, typename V>
class Pair{
    private:
        K key;
        V value;
    public:
        Pair(K key, V value) : key(key), value(value) {}
        K getKey() { return key; }
        void setKey(K key) { this->key = key; }
        V getValue() { return value; }
        void setValue(V value) { this->value = value; }
};

int getMinIndex(vector<Bank>& listOfNetAmounts, int numOfBanks){
    int min = INT_MAX;
    int minIndex = -1;
    for(int i=0;i<numOfBanks;i++){
        if (listOfNetAmounts[i].netAmount == 0){
            continue;
        }
        if(listOfNetAmounts[i].netAmount<min){
            minIndex = i;
            min = listOfNetAmounts[i].netAmount;
        }
    }
    return minIndex;
}

int getSimpleMaxIndex(vector<Bank>& listOfNetAmounts, int numOfBanks){
    int max = INT_MIN;
    int maxIndex = -1;
    for(int i=0;i<numOfBanks;i++){
        if(listOfNetAmounts[i].netAmount==0){
            continue;
        }
        if(listOfNetAmounts[i].netAmount>max){
            maxIndex = i;
            max = listOfNetAmounts[i].netAmount;
        }
    }
    return maxIndex;
}

Pair<int, string> getMaxIndex(vector<Bank>& listOfNetAmounts, int numOfBanks, int minIndex, vector<Bank>& bankIndex, int maxNumTypes){
    int max = INT_MIN;
    int maxIndex = -1;
    string matchingType = "";
    for(int i=0;i<numOfBanks;i++){
        if(listOfNetAmounts[i].netAmount==0){
            continue;
        }
        if(listOfNetAmounts[i].netAmount<0){
            continue;
        }
        vector<string> v;
        auto ls = listOfNetAmounts[minIndex].types.begin();
        auto rs = listOfNetAmounts[i].types.begin();
        while(ls != listOfNetAmounts[minIndex].types.end() && rs != listOfNetAmounts[i].types.end()){
            string type1 = *ls;
            string type2 = *rs;
            if(type1 == type2){
                v.push_back(type1);
            }
            ls++;
            rs++;
        }
        if(!v.empty() && max < listOfNetAmounts[i].netAmount){
            max = listOfNetAmounts[i].netAmount;
            maxIndex = i;
            matchingType = v[0];
        }
    }
    return Pair<int, string>(maxIndex, matchingType);
}

void printTransactions(vector<vector<Pair<int, string>>>& transactions, int numOfBanks, vector<Bank>& bankIndex){
    cout<<"\nThe Transactions for minimum cash-flow are : \n";
    for(int i=0;i<numOfBanks;i++){
        for(int j=0;j<numOfBanks;j++){
            if(i == j){
                continue;
            }
            if(transactions[i][j].getKey() != 0 && transactions[j][i].getKey() != 0){
                if(transactions[i][j].getKey() == transactions[j][i].getKey()){
                    transactions[i][j].setKey(0);
                    transactions[j][i].setKey(0);
                }
                else if(transactions[i][j].getKey() > transactions[j][i].getKey()){
                    transactions[i][j].setKey(transactions[i][j].getKey() - transactions[j][i].getKey());
                    transactions[j][i].setKey(0);
                    cout<<bankIndex[i].name<<" pays Rs "<<transactions[i][j].getKey()<<" to "<<bankIndex[j].name<<" via "<<transactions[i][j].getValue()<<endl;
                }
                else{
                    transactions[j][i].setKey(transactions[j][i].getKey() - transactions[i][j].getKey());
                    transactions[i][j].setKey(0);
                    cout<<bankIndex[j].name<<" pays Rs "<<transactions[j][i].getKey()<<" to "<<bankIndex[i].name<<" via "<<transactions[j][i].getValue()<<endl;
                }
            }
            else if(transactions[i][j].getKey() != 0){
                cout << bankIndex[i].name << " pays Rs " << transactions[i][j].getKey() << " to " << bankIndex[j].name << " via " << transactions[i][j].getValue() << endl;
            }
            else if(transactions[j][i].getKey() != 0){
                cout << bankIndex[j].name << " pays Rs " << transactions[j][i].getKey() << " to " << bankIndex[i].name << " via " << transactions[j][i].getValue() << endl;
            }
            transactions[i][j].setKey(0);
            transactions[j][i].setKey(0);
        }
    }
    cout<<endl;
}

void minimizeCashFlow(int numOfBanks, vector<Bank>& bankIndex, unordered_map<string, int>& indexOf, int numTransactions, vector<vector<int>>& graph, int maxNumTypes){
    vector<Bank> listOfNetAmounts(numOfBanks);
    for(int b=0;b<numOfBanks;b++){
        listOfNetAmounts[b].name = bankIndex[b].name;
        listOfNetAmounts[b].types = bankIndex[b].types;
        int amount = 0;
        for(int i=0;i<numOfBanks;i++){
            amount += graph[i][b];
        }
        for(int j=0;j<numOfBanks;j++){
            amount += (-1) * graph[b][j];
        }
        listOfNetAmounts[b].netAmount = amount;
    }
    vector<vector<Pair<int, string>>> transactions(numOfBanks, vector<Pair<int, string>>(numOfBanks, Pair<int, string>(0, "")));
    int numOfZeroNetAmounts = 0;
    for(int i = 0; i < numOfBanks; i++){
        if(listOfNetAmounts[i].netAmount == 0){
            numOfZeroNetAmounts++;
        }
    }
    while(numOfZeroNetAmounts != numOfBanks){
        int minIndex = getMinIndex(listOfNetAmounts, numOfBanks);
        Pair<int, string> maxAns = getMaxIndex(listOfNetAmounts, numOfBanks, minIndex, bankIndex, maxNumTypes);
        int maxIndex = maxAns.getKey();
        if(maxIndex == -1){
            transactions[minIndex][0].setKey(abs(listOfNetAmounts[minIndex].netAmount));
            transactions[minIndex][0].setValue(*bankIndex[minIndex].types.begin());
            int simpleMaxIndex = getSimpleMaxIndex(listOfNetAmounts, numOfBanks);
            transactions[0][simpleMaxIndex].setKey(abs(listOfNetAmounts[minIndex].netAmount));
            transactions[0][simpleMaxIndex].setValue(*bankIndex[simpleMaxIndex].types.begin());
            listOfNetAmounts[simpleMaxIndex].netAmount += listOfNetAmounts[minIndex].netAmount;
            listOfNetAmounts[minIndex].netAmount = 0;
            if(listOfNetAmounts[minIndex].netAmount == 0){
                numOfZeroNetAmounts++;
            }
            if(listOfNetAmounts[simpleMaxIndex].netAmount == 0){
                numOfZeroNetAmounts++;
            }
        }
        else{
            int transactionAmount = min(abs(listOfNetAmounts[minIndex].netAmount), listOfNetAmounts[maxIndex].netAmount);
            transactions[minIndex][maxIndex].setKey(transactionAmount);
            transactions[minIndex][maxIndex].setValue(maxAns.getValue());
            listOfNetAmounts[minIndex].netAmount += transactionAmount;
            listOfNetAmounts[maxIndex].netAmount -= transactionAmount;
            if(listOfNetAmounts[minIndex].netAmount == 0){
                numOfZeroNetAmounts++;
            }
            if(listOfNetAmounts[maxIndex].netAmount == 0){
                numOfZeroNetAmounts++;
            }
        }
    }
    printTransactions(transactions, numOfBanks, bankIndex);
}

int main(){
    cout << "\nWelcome to PAY SMART\n- by Rajesh Kumawat\n";
    cout << "\nThis program minimizes the number of transactions processed among multiple banks that make use of different modes of payment. There is one world bank with all payment modes to act as the intermediary between banks that have no common modes of payment.\n";
    cout << "\nEnter the number of banks in transactions.\n";
    int numOfBanks;
    cin >> numOfBanks;
    vector<Bank> bankIndex(numOfBanks);
    unordered_map<string, int> indexOf;
    cout << "\nEnter the details of the banks and transactions as stated below : \n";
    cout << "Bank name, number of payment modes it has, and the payment modes.\n";
    cout << "Bank name and payment modes should not contain spaces.\n";
    int maxNumTypes = 0;
    for(int i=0;i<numOfBanks;i++){
        if(i == 0){
            cout << "World Bank: ";
        }
        else{
            cout << "Bank " << i << ": ";
        }
        string name;
        cin >> name;
        bankIndex[i].name = name;
        indexOf[name] = i;
        int numTypes;
        cin >> numTypes;
        if(i == 0){
            maxNumTypes = numTypes;
        }
        unordered_set<string> types(numTypes);
        for(int j=0;j<numTypes;j++){
            string type;
            cin >> type;
            types.insert(type);
        }
        bankIndex[i].types = types;
    }
    cout << "Enter number of transactions : ";
    int numTransactions;
    cin >> numTransactions;
    vector<vector<int>> graph(numOfBanks, vector<int>(numOfBanks, 0));
    cout << "Enter the details of the transactions as stated below:\n";
    cout << "Bank name from which the transaction originates, bank name to which the transaction is made, and the amount.\n";
    for(int i = 0; i < numTransactions; i++){
        string from, to;
        int amount;
        cin >> from >> to >> amount;
        if(indexOf.find(from) != indexOf.end() && indexOf.find(to) != indexOf.end()){
            int fromIdx = indexOf[from];
            int toIdx = indexOf[to];
            graph[fromIdx][toIdx] = amount;
        }
        else{
            cout << "Invalid bank names: " << from << ", " << to << endl;
        }
    }
    minimizeCashFlow(numOfBanks, bankIndex, indexOf, numTransactions, graph, maxNumTypes);
    return 0;
}
