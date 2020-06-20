#include<iostream>
#include<cstring>
#include<string>
#include<vector>
#include<map>
#include<set>
#include<fstream>
using namespace std;
struct Data{
    int year;
    int month;
    int day;
    int min;
    int ID;
    string from;
    string to;
    set<string> content;

};
vector<Data> database;
map<int, Data *> ID;
map<string, int> mon = {
    {"January", 1},
    {"February", 2},
    {"March", 3},
    {"April", 4},
    {"May", 5},
    {"June", 6},
    {"July", 7},
    {"August", 8},
    {"September", 9},
    {"October", 10},
    {"November", 11},
    {"December", 12}
};
int main(){
    int i , j, k;
    string line;
    string a = "../final/test_data/mail";
    int nowsize = 0;
    char *ptr;

    for(int h = 1; h < 10001; h++){
        nowsize;
        database.resize(nowsize + 1);

        ifstream file;
        file.open(a + to_string(h));
        //From
        getline(file, line);
        database[nowsize].from = &line[6];        
        
        //Date
        getline(file, line);
            //day
        database[nowsize].day = 0;
        for(i = 6; i < line.length(); i++){
            if(line[i] == ' '){
                i++;
                break;
            }
            database[nowsize].day  = database[nowsize].day * 10 + line[i] - '0';
        }
            //month
        ptr = &line[i];
        for(; i < line.length(); i++){
            if(line[i] == ' '){
                line[i] = '\0';
                i++;
                break;
            }
        }
        database[nowsize].month = mon[ptr];
            //year
        database[nowsize].year = 0;
        for(; i < line.length(); i++){
            if(line[i] == ' '){
                i++;
                break;
            }
            database[nowsize].year = database[nowsize].year * 10 + line[i] - '0';
        }
            //at
        i += 3;
            //time
        database[nowsize].min  = ((line[i] - '0') * 10 + (line[i + 1] - '0') )* 60 + (line[i + 3] - '0') * 10 + (line[i + 4] - '0');

        
        //Message-ID
        getline(file, line);
        database[nowsize].ID = 0;
        for(i = 12; i < line.length(); i++){
            database[nowsize].ID = database[nowsize].ID * 10 + line[i] - '0';
        }
        //Subject
        getline(file, line);
        ptr = &line[6];
        for(i = 6; i < line.length(); i++){
            if((line[i] <= 'z' && line[i] >= 'a') || (line[i] <= '9' && line[i] >= '0')){
                //great!
                continue;
            }
            if(line[i] <= 'Z' && line[i] >= 'A'){//'a' = 97, 'A' = 65
                line[i] += 32;
            }
            else{
                line[i] = '\0';
                database[nowsize].content.insert(ptr);
                ptr = &line[i + 1];
            }
        }
        if(ptr != &line[line.length()]){
            database[nowsize].content.insert(ptr);
        }
        // for(auto iter = database[database.size()-1].content.begin(); iter != database[database.size()-1].content.end(); ++iter){
        //     cout << *iter << endl;
        // }
        //To
        getline(file, line);
        database[nowsize].to = &line[4];
        //content
        getline(file, line);
        while(getline(file, line)){
            ptr = &line[0];
            for(i = 0; i < line.length(); i++){
                if((line[i] <= 'z' && line[i] >= 'a') || (line[i] <= '9' && line[i] >= '0')){
                    //great!
                    continue;
                }
                if(line[i] <= 'Z' && line[i] >= 'A'){//'a' = 97, 'A' = 65
                    line[i] += 32;
                }
                else{
                    line[i] = '\0';
                    if(ptr != &line[i])
                        database[nowsize].content.insert(ptr);
                    ptr = &line[i + 1];
                    
                }
            }
            if(ptr != &line[line.length()]){
                database[nowsize].content.insert(ptr);
            }
        }
        nowsize++;
    }
    // for(i = 0; i < database.size(); i++){
    //     cout << "from: " << database[i].from << endl;
    //     cout << "to:   " << database[i].to << endl;
    //     cout << "year: " << database[i].year << endl;
    //     cout << "month:" << database[i].month << endl;
    //     cout << "min:  " << database[i].min << endl;
    //     cout << "ID:   " << database[i].ID << endl;

    // }
    // for(int i = 0; i < database.size(); i++){
    //     for(auto iter = database[i].content.begin(); iter != database[i].content.end(); ++iter){
    //         cout << *iter << endl;
    //     }
    //     cout << "*******\n";
    // }
}