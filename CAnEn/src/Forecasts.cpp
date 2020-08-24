/* 
 * File:   Forecasts.cpp
 * Author: Guido Cervone <cervone@psu.edu>
 *         Weiming Hu <weiming@psu.edu>
 *
 * Created on April 18, 2018, 12:40 AM
 * 
 *  "`-''-/").___..--''"`-._
 * (`6_ 6  )   `-.  (     ).`-.__.`)   WE ARE ...
 * (_Y_.)'  ._   )  `._ `. ``-..-'    PENN STATE!
 *   _ ..`--'_..-_/  /--'_.' ,' 
 * (il),-''  (li),'  ((!.-'
 *
 *        Geoinformatics and Earth Observation Laboratory (http://geolab.psu.edu)
 *        Department of Geography and Institute for CyberScience
 *        The Pennsylvania State University
 */

#include "Forecasts.h"

#include <stdexcept>

#if defined(_ENABLE_AI)
#include <torch/script.h>
#include <ATen/ATen.h>

#if defined(_OPENMP)
#include <omp.h>
#endif

#endif

using namespace std;

Forecasts::Forecasts() : Array4D(), BasicData() {
}

Forecasts::Forecasts(const Forecasts & orig) : Array4D(orig), BasicData(orig) {
    if (this != &orig) {
        flts_ = orig.flts_;
    }
}

Forecasts::Forecasts(const Parameters & parameters, const Stations & stations,
        const Times & times, const Times & flts) :
Array4D(), BasicData(parameters, stations, times), flts_(flts) {
}

Forecasts::~Forecasts() {
}

Times const &
Forecasts::getFLTs() const {
    return flts_;
}

Times &
Forecasts::getFLTs() {
    return flts_;
}

size_t
Forecasts::getFltTimeStamp(size_t index) const {
    return (flts_.left[index].second.timestamp);
}

size_t
Forecasts::getFltTimeIndex(size_t timestamp) const {
    return getFltTimeIndex(Time(timestamp));
}

size_t
Forecasts::getFltTimeIndex(Time const & time) const {
    return flts_.getIndex(time);
}

#if defined(_ENABLE_AI)
void
Forecasts::featureTransform(const string & embedding_model_path, Verbose verbose) {

    // Read PyTorch model
    if (verbose >= Verbose::Progress) cout << "Reading the torch model ..." << endl;
    torch::jit::script::Module module;

    try {
        module = torch::jit::load(embedding_model_path);
        module.eval();

    } catch (const c10::Error& e) {
        string msg = string("Failed to read the embedding model ") + embedding_model_path;
        msg += string("\n\n Original error message: ") + e.what();
        throw runtime_error(msg);
    }

    // Define the dimensions of the problem
    long int num_parameters = getParameters().size();
    long int num_stations = getStations().size();
    long int num_times = getTimes().size();
    long int num_lead_times = getFLTs().size();

    // As for now, the model only accpets features at a single station and a single lead time
    // This is dictated by the structure of the model.
    //
    const long int num_allowed_stations = 1;
    const long int num_allowed_lead_times = 1;

    long int num_samples = num_stations * num_times * num_lead_times;
    
    // Initialize an empty tensor
    std::vector<torch::jit::IValue> inputs;

    auto x = at::full({num_samples, num_parameters, num_allowed_stations, num_allowed_lead_times}, NAN, at::kFloat);
    auto normalization_flag = at::full({1}, true, at::kBool);

    // Populate this tensor with the original features from forecasts
    if (verbose >= Verbose::Progress) cout << "Populating the tensor ..." << endl;
    long int sample_i = 0;

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(4) \
shared(num_stations, num_times, num_lead_times, num_parameters, x)
#endif
    for (long int station_i = 0; station_i < num_stations; ++station_i) {
        for (long int time_i = 0; time_i < num_times; ++time_i) {
            for (long int lead_time_i = 0; lead_time_i < num_lead_times; ++lead_time_i, ++sample_i) {
                for (long int parameter_i = 0; parameter_i < num_parameters; ++parameter_i) {
                    x[sample_i][parameter_i][0][0] = getValue(parameter_i, station_i, time_i, lead_time_i);
                }
            }
        }
    }

    // Execute the model and turn its output into a tensor.
    if (verbose >= Verbose::Progress) cout << "Converting features ..." << endl;
    inputs.push_back(x);
    inputs.push_back(normalization_flag);

    at::Tensor output = module.forward(inputs).toTensor();

    // Sanity check
    if (output.size(0) != num_samples) {
        throw runtime_error("The number of samples (the first dimension length) have changed after feature transformation. This is fatal!");
    }

    // This is the number of latent features)bbbb
    long int num_latent_features = output.size(1);
    long int maximum_width = to_string(num_latent_features).length();

    // Modify forecasts with the updated features.
    // These members need to be changed:
    // - Parameters
    // - Data
    //
    if (verbose >= Verbose::Progress) cout << "Saving latent features ..." << endl;
    Parameters latent_features;

    for (long int feature_i = 0; feature_i < num_latent_features; ++feature_i) {

        // Construct the feature name
        ostringstream feature_name;
        feature_name << "latent_feature_" << setfill('0') << setw(maximum_width) << feature_i;

        // All latent features are treated as linear 
        Parameter feature(feature_name.str(), false);

        latent_features.push_back(feature);
    }

    swap(parameters_, latent_features);

    // Change Data
    if (verbose >= Verbose::Progress) cout << "Copying latent feature values ..." << endl;

    // There might be a different number of latent features compared to the original number of variables.
    // Therefore, I need to reallocate memory for forecasts.
    //
    resize(num_latent_features, num_stations, num_times, num_lead_times);
    initialize(NAN);

    sample_i = 0;
    double value = 0.0;

    for (long int station_i = 0; station_i < num_stations; ++station_i) {
        for (long int time_i = 0; time_i < num_times; ++time_i) {
            for (long int lead_time_i = 0; lead_time_i < num_lead_times; ++lead_time_i, ++sample_i) {
                for (long int feature_i = 0; feature_i < num_latent_features; ++feature_i) {
                    value = output[sample_i][feature_i].item<double>();
                    setValue(value, feature_i, station_i, time_i, lead_time_i);
                }
            }
        }
    }

    if (verbose >= Verbose::Progress) cout << "Forecast variables have been transformed to latent features!" << endl;
    return;
}
#endif

Forecasts &
Forecasts::operator=(const Forecasts & rhs) {

    if (this != &rhs) {
        BasicData::operator=(rhs);
        flts_ = rhs.flts_;
    }

    return *this;
}

void
Forecasts::print(ostream &os) const {
    os << "[Forecasts] size: [" <<
            parameters_.size() << ", " <<
            stations_.size() << ", " <<
            times_.size() << ", " <<
            flts_.size() << "]" << endl;
    BasicData::print(os);
    os << flts_;
    return;
}

ostream&
operator<<(ostream& os, Forecasts const & obj) {
    obj.print(os);
    return os;
}
