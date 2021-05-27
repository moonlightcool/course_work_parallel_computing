#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <vector>
#include <set>
#include <map>
#include <mutex>
#include <time.h>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#pragma warning(disable: 4996)
using namespace std;
namespace fs = filesystem;
string path1 = "E:/parallel/datasets/aclImdb/test/neg";
string path2 = "E:/parallel/datasets/aclImdb/test/pos";
string path3 = "E:/parallel/datasets/aclImdb/train/neg";
string path4 = "E:/parallel/datasets/aclImdb/train/pos";
string path5 = "E:/parallel/datasets/aclImdb/train/unsup";

mutex mtx;

vector < map <string, filesystem::path>> madMap;
map<string, vector<filesystem::path>> Index;
void getMap(string s,filesystem::path mypath);
void createMaps(int N, int M, string mypath,int j);

void addtoindex(map <string, filesystem::path> small1)
{
    mtx.lock();
        if (Index.empty() == 1)
        {
            for (auto it = small1.begin(); it != small1.end(); ++it)
            {
                vector<filesystem::path> temp;
                temp.push_back(it->second);
                Index.insert(pair<string, vector<filesystem::path>>(it->first, temp));
            }
        }
        else
        {
            int k = 0;
            for (auto it = small1.begin(); it != small1.end(); ++it)
            {
                 k = 0;
                for (auto it2 = Index.begin(); it2 != Index.end(); ++it2)
                {
                   
                    if (it2->first == it->first)
                    {
                        
                        it2->second.push_back(it->second);
                        k = 1;
                        break;
  
                    }

                }
                if (k != 1)
                {
           
                    vector<filesystem::path> temp;
                    temp.push_back(it->second);
                    Index.insert(pair<string, vector<filesystem::path>>(it->first, temp));

                }

            }
          
        }
        mtx.unlock();
}

void createMaps(int N,int M, string mypath,int j)
{
    int i = N;
    int z = N;

    for (const auto& entry : fs::directory_iterator(mypath))
    {
        if (z == j)
        {
            break;
        }
        if (i == N)
        {
            ifstream file(entry.path());
            string line;
            while (getline(file, line))
            {            
                getMap(line, entry.path());
            }
        }
        i++;
        z++;
        if (i == M)
        {
            i = 0;
        }
    }
    
}

void getMap(string s,filesystem::path mypath)
{
    set <string> words;
    string token;
    string delimiter = ".";
    size_t pos = 0;


    while ((pos = s.find(delimiter)) != string::npos) {
        s.erase(pos, delimiter.length());
    }
    delimiter = ",";
    pos = 0;


    while ((pos = s.find(delimiter)) != string::npos) {
        s.erase(pos, delimiter.length());
    }

    delimiter = "-";
    pos = 0;


    while ((pos = s.find(delimiter)) != string::npos) {
        s.erase(pos, delimiter.length());
    }
    delimiter = "(";
    pos = 0;


    while ((pos = s.find(delimiter)) != string::npos) {
        s.erase(pos, delimiter.length());
    }
    delimiter = "\"";
    pos = 0;


    while ((pos = s.find(delimiter)) != string::npos) {
        s.erase(pos, delimiter.length());
    }

    delimiter = "\'";
    pos = 0;


    while ((pos = s.find(delimiter)) != string::npos) {
        s.erase(pos, delimiter.length());
    }
    delimiter = ")";
    pos = 0;


    while ((pos = s.find(delimiter)) != string::npos) {
        s.erase(pos, delimiter.length());
    }
    delimiter = " ";
    pos = 0;


    while ((pos = s.find(delimiter)) != std::string::npos) 
    {
        token = s.substr(0, pos);
        words.insert(token);
        s.erase(0, pos + delimiter.length());
    }

    map <string, filesystem::path> smallMap;
    set <string> ::iterator it;
    
    for (it = words.begin(); it != words.end(); it++)
    {
        smallMap.insert(pair<string, filesystem::path>(*it, mypath));   
    }
    addtoindex(smallMap);
}

void final()
{
    for (auto it2 = Index.begin(); it2 != Index.end(); ++it2)
    {
        set <filesystem::path> sorted;
        set <filesystem::path> ::iterator it1;
        for (int i = 0; i < it2->second.size(); i++)
        {
            sorted.insert(it2->second[i]);
        }
        while (it2->second.size() != 0)
        {
            it2->second.pop_back();
        }
        for ( it1 = sorted.begin(); it1 != sorted.end(); it1++)
        {
            it2->second.push_back(*it1);
        }
    }
    cout << "index ready" << endl;
}

int main()
{
    srand(time(NULL));
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) 
    {
        cout << "Error" << std::endl;
        exit(1);
    }

    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, SOMAXCONN);

    SOCKET newConnection;
    newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

    if (newConnection == 0)
    {
        cout << "Error #2\n";
    }
    else {
        cout << "Client Connected!\n";
        char n[100];
        char n2[100];
        recv(newConnection, n, sizeof(n), NULL);
        int m;
        if ((int)n[1] - 48==0)
        {
            m = 10;
        }
        else {
            m = (int)n[0] - 48;
        }
        double start = clock();

        if (0 == 0)
        {
            vector<thread> ths;
            for (int i = 0; i < m; ++i)
            {
                ths.push_back(thread(createMaps, i, m, path1, 250));
            }
            for (auto& th : ths)
            {
                th.join();
            }

            vector<thread> ths2;
            for (int i = 0; i < m; ++i)
            {
                ths2.push_back(thread(createMaps, i, m, path2, 250));
            }
            for (auto& th2 : ths2)
            {
                th2.join();
            }
            //vector<thread> ths3;
            //for (int i = 0; i < m; ++i)
            //{
            //    ths3.push_back(thread(createMaps, i, m, path3, 250));
            //}
            //for (auto& th3 : ths3)
            //{
             //   th3.join();
            //}
            //vector<thread> ths4;
            //for (int i = 0; i < m; ++i)
            //{
             //   ths4.push_back(thread(createMaps, i, m, path4, 250));
            //}
            //for (auto& th4 : ths4)
            //{
            //    th4.join();
            //}
            //vector<thread> ths5;
           // for (int i = 0; i < m; ++i)
           // {
            //    ths5.push_back(thread(createMaps, i, m, path5, 1000));
            //}
            //for (auto& th5 : ths5)
            //{
            //    th5.join();
            //}

        }

        double end = clock();
        //cout << (end - start) / CLK_TCK << endl;
        final();
        recv(newConnection, n2, sizeof(n2), NULL);
        string search;
        search = string(n2);
        for (auto it = Index.begin(); it != Index.end(); it++)
        {
            if (it->first == search)
            {
                for (int i = 0; i < it->second.size(); i++)
                {
                    char s[100];
                    string path_string{it->second[i].u8string() };
                    strcpy(s,path_string.c_str());
                    send(newConnection, s, sizeof(s), NULL);
                }

            }
        }
 

    }

    return 0;
}