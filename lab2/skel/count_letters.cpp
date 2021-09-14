#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

int main() {
    map<char, int> letters;
    vector<string> words;

    for (int i = 32; i < 128; i++) {
        letters[(char)i] = 0;
    }

    for (int i = 1; i <= 100; i++) {
        string filename = "files/f" + to_string(i);
        char content[10000];
        FILE *file;

        file = fopen(filename.c_str(),"r");
        while (fgets(content, sizeof(content), file) != NULL)
        
        string temp(content);
        words.push_back(string(content));
        fclose(file);
    }

    for (int i = 0; i < words.size(); i++) {
        for (int j = 0; j < words[i].size(); j++) {
            letters[words[i][j]]++;
        }
    }

    for (int i = 32; i < 128; i++) {
        cout << (char)i << ": " << letters[char(i)] << endl;
    }
}