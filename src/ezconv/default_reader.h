#ifndef EZCONV_DEFAULT_H_
#define EZCONV_DEFAULT_H_

#include <fstream>

#include "absl/status/status.h"

class DefaultReader {
public:
    DefaultReader(const std::string &file_location);
    ~DefaultReader();

    std::string
    getMetricExcType(const std::string &metric_name)
    {
        return metric_exc_type_map_[metric_name];
    }

    std::string
    getMetricDesc(const std::string &metric_name)
    {
        return metric_desc_map_[metric_name];
    }

private:
    absl::Status readContent();

    std::string file_location_;

    std::unordered_map<std::string, std::string> metric_exc_type_map_;
    std::unordered_map<std::string, std::string> metric_desc_map_;
};

#endif // EZCONV_DEFAULT_H
