#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

using namespace std;

void showVectorValues(string label, vector<double> &v,ofstream &file)
{
    file << label << " ";
    for (unsigned i = 0; i < v.size(); i++)
    {
        file << v[i] << " ";
    }
    file << endl;
}

void showTestVectorValues(string label, vector<double> &v,ofstream &file)
{
    file << label << " ";
    for (unsigned i = 0; i < v.size(); i++)
    {
        file << v[i] <<" : "<<round(v[i]) << " ";
    }
    file << endl;
}

double round(double f,double pres)
{
    return (double) (floor(f*(1.0f/pres) + 0.5)/(1.0f/pres));
}
