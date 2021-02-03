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
#if defined(_OPENMP)
#include <omp.h>
#endif
#include <torch/torch.h>
#endif

using namespace std;

static const vector<string> _EMBEDDING_TYPE_DESC = {
    "1-dimensional embedding [parameters]",
    "2-dimensional embedding [parameters, lead times]",
    "2.5-dimensional embedding [general/vertical parameters, lead times]"

};

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
Forecasts::featureTransform(const string & embedding_model_path, Verbose verbose, long int flt_radius) {

    // Read PyTorch model
    if (verbose >= Verbose::Progress) cout << "Reading the embedding model ..." << endl;
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

    /*
     * Make sure the nest for loops are in the order of
     *
     * [lead times] -> [stations] -> [forecast times]
     *
     * This is because these dimensions will be flattened into a single dimension [sample] and
     * later on reconstructed. I need to absolutely make sure that I do not confuse the indices.
     */

    // Initialize a variable for the transformed output
    at::Tensor output;

    // Extract embedding type from the embedding model
    auto attr_list = module.named_attributes();
    auto list_begin = attr_list.begin();
    auto it = attr_list.begin();
    auto list_end = attr_list.end();
    int embedding_type = 0;

    for (it = list_begin; it != list_end; ++it) {
        if ((*it).name == "embedding_type") {
            embedding_type = (*it).value.toInt();
            break;
        }
    }

    // Only the not-equal operator is implemented, but not the equal operator, for torch::jit::slot_iterator_impl comparison.
    // So I have to compare two iterators in such a way.
    //
    if (it != list_end) {
        // Good! The embedding type is found.
    } else {
        if (verbose >= Verbose::Warning) cerr << "Warning: can't find the embedding type from the module. Default to " << embedding_type << endl;
    }

    if (embedding_type < 0 || embedding_type > _EMBEDDING_TYPE_DESC.size() - 1) throw runtime_error("Invalid embedding type");

    if (verbose >= Verbose::Detail) cout << "Embedding type is " << embedding_type << ": " << _EMBEDDING_TYPE_DESC[embedding_type] << endl;

    // Determine the embedding type
    if (embedding_type == 0) {
        featureTransform_1D_(output, module, num_parameters, num_stations, num_times, num_lead_times, verbose);

    } else if  (embedding_type == 1 || embedding_type == 2) {

        if (flt_radius <= 0) throw runtime_error("Embedding lead time radius must be positive");
        if (verbose >= Verbose::Detail) cout << "Lead time radius for embedding: " << flt_radius << endl;

        featureTransform_2D_(output, module, flt_radius, num_parameters, num_stations, num_times, num_lead_times, verbose);

    } else {
        throw runtime_error("Unknown embedding type from the embedding model (check attribute : embedding_type)");
    }

    // This is the number of latent features
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

    const auto torch_data = output.data_ptr<float>();

#if defined(_OPENMP)
#pragma omp parallel for schedule(static) collapse(3)
#endif
    for (long int lead_time_i = 0; lead_time_i < num_lead_times; ++lead_time_i) {
        for (long int station_i = 0; station_i < num_stations; ++station_i) {
            for (long int time_i = 0; time_i < num_times; ++time_i) {

                long int sample_i = (lead_time_i * num_stations + station_i) * num_times + time_i;

                for (long int feature_i = 0; feature_i < num_latent_features; ++feature_i) {
                    double value = torch_data[sample_i * num_latent_features + feature_i];
                    setValue(value, feature_i, station_i, time_i, lead_time_i);
                }
            }
        }
    }

    if (verbose >= Verbose::Progress) cout << "Forecast variables have been transformed to latent features!" << endl;
    return;
}

void
Forecasts::featureTransform_1D_(at::Tensor & output, torch::jit::script::Module & module,
        long int num_parameters, long int num_stations, long int num_times, long int num_lead_times, Verbose verbose) {

    // For embedding 0, 1-dimensional embedding with parameters only, the model only accpets features at a 
    // single station and a single lead time. This is dictated by the structure of the Torch model.
    //
    const long int num_allowed_stations = 1;
    const long int num_allowed_lead_times = 1;

    long int num_samples = num_stations * num_times * num_lead_times;
    
    // Initialize an empty tensor
    std::vector<torch::jit::IValue> inputs;

    // Populate this tensor with the original features from forecasts
    if (verbose >= Verbose::Progress) cout << "Populating the tensor with 1-dimensional embeddings (parameters only) ..." << endl;
    vector<float> torch_data(num_samples * num_parameters * num_allowed_stations * num_allowed_lead_times);

#if defined(_OPENMP)
#pragma omp parallel for default(none) schedule(static) collapse(3) \
shared(num_stations, num_times, num_lead_times, num_parameters, torch_data)
#endif
    for (long int lead_time_i = 0; lead_time_i < num_lead_times; ++lead_time_i) {
        for (long int station_i = 0; station_i < num_stations; ++station_i) {
            for (long int time_i = 0; time_i < num_times; ++time_i) {
                for (long int parameter_i = 0; parameter_i < num_parameters; ++parameter_i) {
                    long int pos = ((lead_time_i * num_stations + station_i) * num_times + time_i) * num_parameters + parameter_i;
                    torch_data[pos] = getValue(parameter_i, station_i, time_i, lead_time_i);
                }
            }
        }
    }

    auto x = torch::from_blob(torch_data.data(), {num_samples, num_parameters, num_allowed_stations, num_allowed_lead_times}, torch::TensorOptions());
    auto add_cpp_routines = at::full({1}, true, at::kBool);

    // Execute the model
    if (verbose >= Verbose::Progress) cout << "Converting features ..." << endl;
    inputs.push_back(x);
    inputs.push_back(add_cpp_routines);

        // Disabling gradient calculation for the current thread
    torch::NoGradGuard no_grad;
    output = module.forward(inputs).toTensor();
    return;
}

void
Forecasts::featureTransform_2D_(at::Tensor & output, torch::jit::script::Module & module, long int flt_radius,
        long int num_parameters, long int num_stations, long int num_times, long int num_lead_times, Verbose verbose) {

    // For embedding 1, 2-dimensional embedding with parameters and lead times, the model accepts features 
    // at a single station but continuous lead times. This is dictated by the structure of the Torch model.
    //
    const long int num_allowed_stations = 1;

    // Populate this tensor with the original features from forecasts
    if (verbose >= Verbose::Progress) cout << "Populating the tensor with 2-dimensional embeddings (parameters with lead times) ..." << endl;

    vector<at::Tensor> tensor_outputs(num_lead_times);
    size_t counter = 0;

#if defined(_OPENMP)
#pragma omp parallel for schedule(static)
#endif
    for (long int lead_time_i = 0; lead_time_i < num_lead_times; ++lead_time_i) {

        long int num_samples = num_stations * num_times;

        // Calculate the window size
        long int flt_left = lead_time_i - flt_radius;
        long int flt_right = lead_time_i + flt_radius + 1;

        if (flt_left < 0) flt_left = 0;
        if (flt_right >= num_lead_times) flt_right = num_lead_times;

        long int window_size = flt_right - flt_left;

        // Flatten the array data into a long vector
        vector<float> torch_data(num_samples * num_parameters * num_allowed_stations * window_size);
        long int pos = 0;

        for (long int station_i = 0; station_i < num_stations; ++station_i) {
            for (long int time_i = 0; time_i < num_times; ++time_i) {
                for (long int parameter_i = 0; parameter_i < num_parameters; ++parameter_i) {
                    for (long int window_i = 0; window_i < window_size; ++window_i, ++pos) {
                        torch_data[pos] = getValue(parameter_i, station_i, time_i, flt_left + window_i);
                    }
                }
            }
        }

        auto x = torch::from_blob(torch_data.data(), {num_samples, num_parameters, num_allowed_stations, window_size}, torch::TensorOptions());
        auto add_cpp_routines = at::full({1}, true, at::kBool);

        // Initialize an empty vector
        std::vector<torch::jit::IValue> inputs;

        inputs.push_back(x);
        inputs.push_back(add_cpp_routines);

        // Disabling gradient calculation for the current thread
        torch::NoGradGuard no_grad;

        // Execute the model
        tensor_outputs[lead_time_i] = module.forward(inputs).toTensor();

#pragma omp critical
        if (verbose >= Verbose::Detail) {
            counter++;
            cout << '\r' << "Features transformed for " << counter << "/" << num_lead_times << " lead times";
            cout.flush();
        }
    }

    if (verbose >= Verbose::Detail) cout << '\r' << "Features transformed for all " << num_lead_times << " lead times"<< endl;

    output = at::cat(tensor_outputs, 0);
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
