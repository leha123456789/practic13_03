#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <omp.h>
#include <vector>
#include <unordered_map>
#include <sstream>
using namespace std;
static bool compare_by_frequency(const pair<string, int>& a, const pair<string, int>& b)
{
    return a.second > b.second;
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "RUSSIAN");
    string filename = argv[1];
    ifstream infile(filename);
    int n = 0;
    string line;
    while (getline(infile, line)) 
    {
        n++;
    }
    infile.clear();
    int num_threads = 4;
    int block_size = n / num_threads;
    vector<vector<string>> thread_lines(num_threads);
#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < num_threads; ++i) 
    {
        vector<string> lines;
        infile.seekg(i * block_size);
        for (int j = 0; j < block_size; ++j) 
        {
            getline(infile, line);
            lines.push_back(line);
        }
        thread_lines[i] = lines;
    }
    unordered_map<string, int> word_count;
#pragma omp parallel for num_threads(num_threads)
    for (int i = 0; i < num_threads; ++i) 
    {
        for (string line : thread_lines[i]) 
        {
            stringstream ss(line);
            string word;
            while (getline(ss, word, ' ')) 
            {
                word.erase(remove_if(word.begin(), word.end(), [](unsigned char c) { return ispunct(c); }), word.end());
#pragma omp critical
                word_count[word]++;
            }
        }
    }
    vector<pair<string, int>> word_counts;
    for (auto it : word_count) 
    {
        word_counts.push_back({ it.first, it.second });
    }
    sort(word_counts.begin(), word_counts.end(), compare_by_frequency);
    for (auto it : word_counts) 
    {
        cout << it.first << " " << it.second << endl;
    }
    infile.close();
    return 0;
}
