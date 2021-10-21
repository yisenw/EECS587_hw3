#include <iostream>

using namespace std;

int main() {

    int critical = 0;

    #pragma omp parallel
    {
    // Code inside this region runs in parallel.
    cout << "fuck: ";
    #pragma omp critical(dataupdate)
        {
        critical++;
        cout << critical << endl;
        }
    }

    cout << "MLGB" << endl;
}
