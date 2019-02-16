#include <mpi.h>
#include <iostream>
#include <string>

#include "AnEnIO.h"

using namespace std;

int main (int argc, char** argv) {
    
    MPI_Init(&argc, &argv);
    string file_path;

    if (argc == 2) {
        file_path = string(argv[argc - 1]);
    } else {
        cout << "Usage: Only one parameter is accepted as the file path." << endl;
        return 1;
    }

    int verbose = 6;

    AnEnIO io("Read", file_path, "Forecasts", verbose);

    cout << "************** Read forecasts ******************" << endl;
    Forecasts_array forecasts;
    io.readForecasts(forecasts);

    MPI_Finalize();
    return (0);
}
