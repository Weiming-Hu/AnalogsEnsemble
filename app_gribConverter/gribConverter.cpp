/* 
 * File:   similarityCalculator.cpp
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on September 17, 2018, 5:20 PM
 */
#include "eccodes.h"
#include "AnEn.h"
#include "AnEnIO.h"
#include "colorTexts.h"
 
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

void my_codes_check(int ret, long par_id, long level,
        string par_key, string level_key, string val_key) {

    if (ret != CODES_SUCCESS)
    cout << BOLDRED << "Error occured when getting " << val_key
        << " at " << par_key << ": " << par_id << " " << level_key
        << ": " << level << RESET << endl;
    CODES_CHECK(ret, 0);
    return;

}

void getDoubles(vector<double> & vals, string file, long par_id, long level,
        string par_key = "paramId", string level_key = "level",
        string val_key = "values") {

    int ret;
    double* p_vals; 
    size_t vals_len;
    codes_index* index;
    codes_handle* h = NULL;

    // Construct query string
    string query_str(par_key);
    query_str.append(",");
    query_str.append(level_key);

    // Send query request
    index = codes_index_new_from_file(0, &file[0u], query_str.c_str(), &ret);
    my_codes_check(ret, par_id, level, par_key, level_key, val_key);

    // Select index based on par_key and level_key
    my_codes_check(codes_index_select_long(index, par_key.c_str(), par_id),
            par_id, level, par_key, level_key, val_key);
    my_codes_check(codes_index_select_long(index, level_key.c_str(), level),
            par_id, level, par_key, level_key, val_key);

    // Get data size
    h = codes_handle_new_from_index(index, &ret);
    my_codes_check(codes_get_size(h, val_key.c_str(), &vals_len),
            par_id, level, par_key, level_key, val_key);

    // Get data values
    p_vals = (double*) malloc(vals_len * sizeof(double));
    my_codes_check(codes_get_double_array(h, val_key.c_str(), p_vals, &vals_len),
            par_id, level, par_key, level_key, val_key);

    // Copy data to vector
    vals.clear();
    vals.resize(vals_len);
    copy(p_vals, p_vals + vals_len, vals.begin());

    // Housekeeping
    free(p_vals);
    my_codes_check(codes_handle_delete(h), par_id, level,
            par_key, level_key, val_key);
    codes_index_delete(index);

    return;
}

int main(int argc, char** argv) {
    vector<double> vec;
    getDoubles(vec, argv[argc-1], 260238, 475);
    copy(vec.begin(), vec.end(), ostream_iterator<double>(cout, " "));
    return 0;
}
