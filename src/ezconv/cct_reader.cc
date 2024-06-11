#include <iomanip>  

#include "cct_reader.h"
#include "meta_reader.h"
#include "profile_reader.h"

#include "absl/log/log.h"

#define LITTLE_ENDIAN_TO_HOST(__VALUE__)                      \
    do {                                                      \
        if (!is_little_endian_) {                             \
            char *ptr = reinterpret_cast<char *>(&__VALUE__); \
            std::reverse(ptr, ptr + sizeof(__VALUE__));       \
        }                                                     \
    } while (0)

CCTReader::CCTReader(const std::string &file_location, Profile::profile_t *profile,
                     MetaReader *meta_reader, ProfileReader *profile_reader)
    : profile_(profile)
    , meta_reader_(meta_reader)
    , profile_reader_(profile_reader)
{
    const int num = 1;
    is_little_endian_ = *reinterpret_cast<const char *>(&num) == 1;

    file_.open(file_location, std::ios::in | std::ios::binary);
    if (!file_.is_open()) {
        LOG(ERROR) << "Failed to open file: " << file_location;
        LOG(FATAL) << "Error code: " << strerror(errno);
        return;
    } else {
        LOG(INFO) << "Successfully opened file: " << file_location;
    }

    auto status = readContent();
    if (status.ok()) {
        LOG(INFO) << "Successfully read content from file";
    } else {
        LOG(ERROR) << status.message();
        LOG(FATAL) << "Failed to read content from file";
    }
}

CCTReader::~CCTReader()
{
    if (file_.is_open()) {
        file_.close();
    }
}

absl::Status
CCTReader::readContent()
{
    const std::unordered_map<std::string, int> section_offset_map = { { "Context Headers",
                                                                        0 } };

    const std::vector<std::string> section_read_order = { "Context Headers" };

    const std::unordered_map<
        std::string,
        std::function<void(std::fstream & f, uint64_t pointer, uint64_t size)>>
        section_reader_map = {
            { "Context Headers",
              [this](std::fstream &f, uint64_t pointer, uint64_t size) {
                  LOG(ERROR) << "Reading Context Headers";
                  f.seekg(pointer);
                  uint64_t ctxts_pointer;
                  f.read(reinterpret_cast<char *>(&ctxts_pointer), sizeof(ctxts_pointer));
                  LITTLE_ENDIAN_TO_HOST(ctxts_pointer);

                  uint32_t ctxts_nums;
                  f.read(reinterpret_cast<char *>(&ctxts_nums), sizeof(ctxts_nums));
                  LITTLE_ENDIAN_TO_HOST(ctxts_nums);

                  uint8_t ctxts_size;
                  f.read(reinterpret_cast<char *>(&ctxts_size), sizeof(ctxts_size));
                  metric_sum_value_.resize(meta_reader_->needRecordMetricNum());
                  for (auto &i : metric_sum_value_) {
                      metric_sum_value_[i] = 0.0;
                  }
                  for (uint32_t i = 1; i < ctxts_nums; i++) {
                      if (!meta_reader_->hasCtxt(i)) {
                          continue;
                      }
                      this->readSingleContextMetrics(ctxts_pointer + i * ctxts_size, i);
                  }
                  std::vector<double> metric_factor;
                  for (auto i = 0; i < metric_sum_value_.size(); i++) {
                      Profile::metric_type_t *metric_type = meta_reader_->getMetricType(i);
                      std::string metric_des = profile_->get_string_table()->get_string(
                          metric_type->get_des());
                      double factor = 1.0;
                      if (metric_des.find("(sec)") != std::string::npos) {
                          factor = 0.000001;
                      } else {
                        if (metric_sum_value_[i] > static_cast<double>(UINT64_MAX)) {
                            factor = metric_sum_value_[i] / static_cast<double>(UINT64_MAX);
                            double exponent = ceil(log10(factor));
                            factor = pow(10, exponent);
                        }
                      }
                      
                      std::ostringstream oss;
                      oss << std::setprecision(2) << std::scientific << std::uppercase
                          << factor;
                      std::string factor_str = "* " + oss.str();
                      int64_t uint_str_idx = profile_->add_string(factor_str);
                      metric_type->update_uint(uint_str_idx);
                      metric_factor.push_back(factor);
                      LOG(ERROR) << "SUM Metric " << metric_des << ": " << metric_sum_value_[i];
                  }

                  for (auto i = 0; i < metric_sum_value_.size(); i++) {
                      if (metric_sum_value_[i] == 0.0) {
                          continue;
                      }
                      Profile::metric_type_t *metric_type = meta_reader_->getMetricType(i);
                      std::string metric_des = profile_->get_string_table()->get_string(
                          metric_type->get_des());
                      size_t pos = metric_des.find("(sec)");
                      if (pos != std::string::npos) {
                        metric_des.replace(pos, 5, "(us)");
                      }
                      profile_->add_metric_type(metric_type->get_value_type(),
                                                metric_type->get_unit(), profile_->add_string(metric_des));
                  }

                  for (auto &ctxt_metric_values : ctxt_metric_values_map_) {
                      uint32_t ctxt_id = ctxt_metric_values.first;
                      Profile::context_t *ctxt = meta_reader_->getCtxt(ctxt_id);
                      std::vector<uint64_t> metric_values;
                      for (auto i = 0; i < ctxt_metric_values.second.size(); i++) {
                          if (metric_sum_value_[i] == 0.0) {
                            continue;
                          }
                          uint64_t value = static_cast<uint64_t>(
                              ctxt_metric_values.second[i] / metric_factor[i]);
                          // LOG(ERROR) << "Metric " << i << ": " << value;
                          metric_values.push_back(value);
                      }
                      profile_->add_sample(ctxt, metric_values);
                  }
              } }
        };
    // First 10 bytes should be "HPCTOOLKIT"
    char identifier[11];
    file_.read(identifier, 10);
    identifier[10] = '\0';
    if (std::string(identifier) != "HPCTOOLKIT") {
        return absl::InternalError("Invalid identifier");
    }

    // Next 4 bytes (u8) are the "Specific format identifier"
    char format_identifier[5];
    file_.read(format_identifier, 4);
    format_identifier[4] = '\0';
    if (std::string(format_identifier) != "ctxt") {
        return absl::InternalError("Invalid format identifier");
    }

    // Next byte (u8) are the major version
    file_.read(reinterpret_cast<char *>(&major_version_), 1);

    // Next byte (u8) are the minor version
    file_.read(reinterpret_cast<char *>(&minor_version_), 1);

    LOG(INFO) << "Version: " << static_cast<int>(major_version_) << "."
              << static_cast<int>(minor_version_);

    for (auto i = 0; i < section_offset_map.size(); i++) {
        uint64_t size;
        file_.read(reinterpret_cast<char *>(&size), sizeof(size));
        LITTLE_ENDIAN_TO_HOST(size);
        section_size_.push_back(size);
        uint64_t pointer;
        file_.read(reinterpret_cast<char *>(&pointer), sizeof(pointer));
        LITTLE_ENDIAN_TO_HOST(pointer);
        section_pointer_.push_back(pointer);
    }

    for (auto i = 0; i < section_read_order.size(); i++) {
        const std::string &section_name = section_read_order[i];
        uint64_t pointer = section_pointer_[section_offset_map.at(section_name)];
        uint64_t size = section_size_[section_offset_map.at(section_name)];
        if (section_reader_map.at(section_name) != nullptr) {
            section_reader_map.at(section_name)(file_, pointer, size);
        }
    }

    return absl::OkStatus();
}

void
CCTReader::readSingleContextMetrics(uint64_t pointer, uint32_t ctxt_id)
{
    file_.seekg(pointer);

    uint64_t nzval_nums;
    file_.read(reinterpret_cast<char *>(&nzval_nums), sizeof(nzval_nums));
    LITTLE_ENDIAN_TO_HOST(nzval_nums);

    uint64_t nzval_pointer;
    file_.read(reinterpret_cast<char *>(&nzval_pointer), sizeof(nzval_pointer));
    LITTLE_ENDIAN_TO_HOST(nzval_pointer);

    uint16_t nmetrics;
    file_.read(reinterpret_cast<char *>(&nmetrics), sizeof(nmetrics));
    LITTLE_ENDIAN_TO_HOST(nmetrics);

    char holder[6];
    file_.read(holder, 6);

    uint64_t metrics_pointer;
    file_.read(reinterpret_cast<char *>(&metrics_pointer), sizeof(metrics_pointer));
    LITTLE_ENDIAN_TO_HOST(metrics_pointer);

    typedef struct {
        uint64_t metric_id;
        uint64_t metric_start_idx;
    } metric_t;
    std::vector<metric_t> metrics;
    file_.seekg(metrics_pointer);
    for (uint16_t i = 0; i < nmetrics; i++) {
        uint16_t metric_id;
        file_.read(reinterpret_cast<char *>(&metric_id), sizeof(metric_id));
        LITTLE_ENDIAN_TO_HOST(metric_id);

        uint64_t metric_start_idx;
        file_.read(reinterpret_cast<char *>(&metric_start_idx), sizeof(metric_start_idx));
        LITTLE_ENDIAN_TO_HOST(metric_start_idx);
        if (meta_reader_->needRecordMetric(metric_id)) {
            metrics.push_back({ metric_id, metric_start_idx });
        }
    }
    if (metrics.empty()) {
        return;
    }
    // LOG(ERROR) << "Context " << ctxt_id << ": [" << nzval_nums << "/" << metrics.size()
    //            << "]";

    ctxt_metric_values_map_.insert(
        { ctxt_id, std::vector<double>(meta_reader_->needRecordMetricNum(), 0) });
    for (auto metric : metrics) {
        // LOG(ERROR) << "Recorded Metric " << metric.metric_id << " at " <<
        // metric.metric_start_idx;
        file_.seekg(nzval_pointer +
                    metric.metric_start_idx * (sizeof(uint64_t) + sizeof(uint32_t)));

        uint32_t profile_idx;
        file_.read(reinterpret_cast<char *>(&profile_idx), sizeof(profile_idx));
        LITTLE_ENDIAN_TO_HOST(profile_idx);

        double value;
        file_.read(reinterpret_cast<char *>(&value), sizeof(value));
        LITTLE_ENDIAN_TO_HOST(value);
        // LOG(ERROR) << "Metric " << meta_reader_->getMetricTypeName(metric.metric_id)
        //            << ": " << value;
        ctxt_metric_values_map_[ctxt_id]
                               [meta_reader_->getMetricTypeIdx(metric.metric_id)] = value;
        metric_sum_value_[meta_reader_->getMetricTypeIdx(metric.metric_id)] += value;
    }
}
