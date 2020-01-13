class AnEnIO {
public:
    errorType writeForecasts(const Forecasts & forecasts) const;
    errorType writeObservations(const Observations & observations) const;
    errorType writeFLTs(const anenTime::FLTs & flts, bool unlimited) const;
    errorType writeParameters(const anenPar::Parameters & parameters, bool unlimited) const;
    errorType writeStations(const anenSta::Stations & stations, bool unlimited, const std::string & dim_name_prefix = "", const std::string & var_name_prefix = "") const; 
    errorType writeTimes(const anenTime::Times & times, bool unlimited, const std::string & dim_name = "num_times", const std::string & var_name = "Times") const;
    errorType writeSimilarityMatrices( const SimilarityMatrices & sims, const anenPar::Parameters & parameters, const anenSta::Stations & test_stations, const anenTime::Times & test_times, const anenTime::FLTs & flts, const anenSta::Stations & search_stations, const anenTime::Times & search_times) const; 
    errorType writeAnalogs( const Analogs & analogs, const anenSta::Stations & test_stations, const anenTime::Times & test_times, const anenTime::FLTs & flts, const anenSta::Stations & search_stations, const anenTime::Times & search_times) const;
    errorType writeStandardDeviation( const StandardDeviation & sds, const anenPar::Parameters & parameters, const anenSta::Stations & stations, const anenTime::FLTs & flts) const;

    static errorType combineParameters( const std::vector<std::string> & in_files, const std::string & file_type, anenPar::Parameters & parameters, int verbose, const std::vector<size_t> & start = {}, const std::vector<size_t> & count = {}, bool allow_duplicates = false);
    static errorType combineStations( const std::vector<std::string> & in_files, const std::string & file_type, anenSta::Stations & stations, int verbose, const std::string & dim_name_prefix = "", const std::string & var_name_prefix = "", const std::vector<size_t> & start = {}, const std::vector<size_t> & count = {}, bool allow_duplicates = false);
    static errorType combineTimes( const std::vector<std::string> & in_files, const std::string & file_type, anenTime::Times & times, int verbose, const std::string & var_name = "Times", const std::vector<size_t> & start = {}, const std::vector<size_t> & count = {}, bool allow_duplicates = false);
    static errorType combineFLTs( const std::vector<std::string> & in_files, const std::string & file_type, anenTime::FLTs & flts, int verbose, const std::vector<size_t> & start = {}, const std::vector<size_t> & count = {}, bool allow_duplicates = false);
    static errorType combineForecastsArray( const std::vector<std::string> & in_files, Forecasts_array & forecasts, size_t along, int verbose = 2, const std::vector<size_t> & starts = {}, const std::vector<size_t> & counts = {});
    static errorType combineObservationsArray( const std::vector<std::string> & in_files, Observations_array & observations, size_t along, int verbose = 2, const std::vector<size_t> & starts = {}, const std::vector<size_t> & counts = {});
    static errorType combineStandardDeviation(const std::vector<std::string> & in_files, StandardDeviation & sds, anenPar::Parameters & parameters, anenSta::Stations & stations, anenTime::FLTs & flts, size_t along, int verbose = 2);
    static errorType combineSimilarityMatrices(const std::vector<std::string> & in_files, SimilarityMatrices & sims, anenSta::Stations & stations, anenTime::Times & times, anenTime::FLTs & flts, anenSta::Stations & search_stations, anenTime::Times & search_times, size_t along, int verbose = 2, const std::vector<size_t> & starts = {}, const std::vector<size_t> & counts = {});
    static errorType combineAnalogs(const std::vector<std::string> & in_files, Analogs & analogs, anenSta::Stations & stations, anenTime::Times & times, anenTime::FLTs & flts, anenSta::Stations & member_stations, anenTime::Times & member_times, size_t along, int verbose = 2, const std::vector<size_t> & starts = {}, const std::vector<size_t> & counts = {});

protected:

    errorType writeSimilarityMatricesOnly_(const SimilarityMatrices & sims) const;
    errorType writeAnalogsOnly_(const Analogs & analogs) const;
    errorType writeStandardDeviationOnly_(const StandardDeviation & sds) const;

    template<typename T>
    static errorType compareNewDimensions( const std::vector<size_t> & same_dimensions, T * new_shape, T * expected_shape, int verbose);
};
