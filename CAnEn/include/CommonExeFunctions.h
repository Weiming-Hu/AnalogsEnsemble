/* 
 * File:   CommonExeFunctions.h
 * Author: Weiming Hu <weiming@psu.edu>
 *
 * Created on September 19, 2018, 10:49 AM
 */

#ifndef COMMONEXEFUNCTIONS_H
#define COMMONEXEFUNCTIONS_H

#include "Levenshtein.h"

/**
 * \namespace CommonExeFunctions
 * 
 * \brief This name space contains common and convenient functions for 
 * creating an executable.
 */
namespace {
    using namespace std;

    /**
     * A template function to guess the unregistered parameters.
     * 
     * @param unregistered_keys The unregistered keys returned by 
     * boost::program_options::collect_unrecognized.
     * @param available_options A vector of string for available options.
     */
    template<class charT>
    void guess_arguments(const vector< std::basic_string<charT> > & unregistered_keys,
            const vector<string> available_options) {
        vector<int> distances;
        cerr << BOLDRED << "Found unknown parameters: " << RESET << endl;
        for (auto arg : unregistered_keys) {

            // check for deprecated options
            if (arg == "-L") {
                cout << RED << "-L is deprecated. Please use -B or -N" << RESET << endl;
                return;
            }
            if (arg == "--Luca-method") {
                cout << RED << "--Luca-method is deprecated. Please use -B or -N" << RESET << endl;
                return;
            }

            distances.clear();
            cout << arg.c_str();

            // compute the Leveshtein distances
            for (auto arg_available : available_options) {

                // the weights are suggested by Github
                distances.push_back(levenshtein(arg, arg_available, 0, 2, 1, 3));
            }

            // find the most similar ones and print them out
            cout << ": did you mean ";
            int min = *min_element(distances.begin(), distances.end());
            auto iter = distances.begin();
            while (iter != distances.end()) {
                if (*iter == min) {
                    cout << GREEN << available_options.at(iter - distances.begin()) << RESET << " ";
                }
                iter++;
            }
            cout << endl;
        }
        cout << "Please refer to the full list of parameters using -h or --help" << endl;
        return;
    }

        /**
     * An overload function for convenient printing of vector values.
     */
    template<class T>
    ostream& operator<<(ostream& os, const vector<T>& v) {
        copy(v.begin(), v.end(), ostream_iterator<T>(os, ","));
        return os;
    }

    /**
     * A function to handle exceptions.
     * 
     * @param eptr Exception.
     */
    void handle_exception(const exception_ptr & eptr) {
        // handle general exceptions
        try {
            rethrow_exception(eptr);
        } catch (const exception & e) {
            cerr << RED << e.what() << RESET << endl;
        } catch (...) {
            cerr << RED << "ERROR: caught unknown exceptions!" << RESET << endl;
        }
    }
}
#endif /* COMMONEXEFUNCTIONS_H */

