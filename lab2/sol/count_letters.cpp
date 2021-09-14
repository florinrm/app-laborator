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
    int i, j;
    char content[10000];
    FILE *file;
    string filename;

    #pragma omp parallel for private(i) shared(letters) 
    for (i = 32; i < 128; i++) {
        letters[(char)i] = 0;
    }

    for (i = 1; i <= 100; i++) {
        filename = "files/f" + to_string(i);
        char content[10000];
        FILE *file;

        file = fopen(filename.c_str(),"r");
        while (fgets(content, sizeof(content), file) != NULL)
        
        string temp(content);
        words.push_back(string(content));
        fclose(file);
    }

    double t1, t2;
    t1 = omp_get_wtime();
    #pragma omp parallel for private(i, j) shared(letters, words) schedule(auto)
    for (i = 0; i < words.size(); i++) {
        for (j = 0; j < words[i].size(); j++) {
            #pragma omp critical
            letters[words[i][j]]++;
        }
    }
    t2 = omp_get_wtime();

    cout << t2 - t1 << endl;

    for (i = 32; i < 128; i++) {
        cout << (char)i << ": " << letters[char(i)] << endl;
    }
}