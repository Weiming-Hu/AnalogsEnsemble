/* 
 * File:   functions_netcdf.h
 * Author: Weiming Hu
 *
 * Created on January 27, 2019, 5:21 PM
 */

#ifndef FUNCTIONS_NETCDF_H
#define FUNCTIONS_NETCDF_H

#include <netcdf>

void deallocate_memory(void* &p,
        const netCDF::NcType::ncType & vartype, int verbose) {
    
    using namespace netCDF;
    using namespace std;

    if (!p) {
        switch (vartype) {
            case NcType::nc_CHAR:
                delete [] (char*) p;
                break;
            case NcType::nc_BYTE:
            case NcType::nc_SHORT:
            case NcType::nc_INT:
                delete [] (int*) p;
                break;
            case NcType::nc_FLOAT:
                delete [] (float*) p;
                break;
            case NcType::nc_DOUBLE:
                delete [] (double*) p;
                break;
            case NcType::nc_UBYTE:
            case NcType::nc_USHORT:
            case NcType::nc_UINT:
            case NcType::nc_INT64:
            case NcType::nc_UINT64:
                delete [] (unsigned *) p;
                break;
            default:
                if (verbose >= 1) cout << "Failed in deleting the pointer." << endl;
                throw runtime_error("Unknown NcType");
        }
    }
    
    return;
}

void deallocate_memory(void* & p, const nc_type & vartype, int verbose) {

    using namespace netCDF;
    using namespace std;

    if (!p) {
        switch (vartype) {
            case NC_CHAR:
                delete [] (char*) p;
                break;
            case NC_BYTE:
            case NC_SHORT:
            case NC_INT:
                delete [] (int*) p;
                break;
            case NC_FLOAT:
                delete [] (float*) p;
                break;
            case NC_DOUBLE:
                delete [] (double*) p;
                break;
            case NC_UBYTE:
            case NC_USHORT:
            case NC_UINT:
            case NC_INT64:
            case NC_UINT64:
                delete [] (unsigned *) p;
                break;
            default:
                if (verbose >= 1) cout << "Failed in deleting the pointer." << endl;
                throw runtime_error("Unknown NcType");
        }
    }
    
    return;
}

void allocate_memory(void* &p,
        const netCDF::NcType::ncType & vartype, size_t len, int verbose) {

    using namespace netCDF;
    using namespace std;
    
    if (p) {
        throw runtime_error("Please pass a null pointer.");
    }

    try {
        switch (vartype) {
            case NcType::nc_CHAR:
                p = new char[len]();
                break;
            case NcType::nc_BYTE:
            case NcType::nc_SHORT:
            case NcType::nc_INT:
                p = new int[len]();
                break;
            case NcType::nc_FLOAT:
                p = new float[len]();
                break;
            case NcType::nc_DOUBLE:
                p = new double[len]();
                break;
            case NcType::nc_UBYTE:
            case NcType::nc_USHORT:
            case NcType::nc_UINT:
            case NcType::nc_INT64:
            case NcType::nc_UINT64:
                p = new unsigned int[len]();
                break;
            default:
                if (verbose >= 1) cout << "Error: Variable type not supported!" << endl;
                throw runtime_error("Type not supported.");
        }
    } catch (bad_alloc & e) {
        if (verbose >= 1) cout <<
                "Error: Insufficient memory when reading variable!" << endl;
        throw;
    } catch (...) {
        throw;
    }
    
    return;
}

void allocate_memory(void* & p, const nc_type & vartype, int len, int verbose) {
    
    using namespace netCDF;
    using namespace std;
    
    if (p) {
        throw runtime_error("Please pass a null pointer.");
    }
    
    try {
        switch (vartype) {
            case NC_CHAR:
                p = new char[len]();
                break;
            case NC_BYTE:
            case NC_SHORT:
            case NC_INT:
                p = new int[len]();
                break;
            case NC_FLOAT:
                p = new float[len]();
                break;
            case NC_DOUBLE:
                p = new double[len]();
                break;
            case NC_UBYTE:
            case NC_USHORT:
            case NC_UINT:
            case NC_INT64:
            case NC_UINT64:
                p = new unsigned int[len]();
                break;
            default:
                if (verbose >= 1) cout << "A rank failed in allocating memory." << endl;
                throw runtime_error("Unknown variable type");
        }

    } catch (bad_alloc & e) {
        if (verbose >= 1) cout <<
                "Error: Insufficient memory when reading variable!" << endl;
        throw;
    } catch (...) {
        throw;
    }
    
    return;
}


#endif /* FUNCTIONS_NETCDF_H */

