#ifndef EZCONV_CCT_READER_H_
#define EZCONV_CCT_READER_H_

#include <string>

#include "meta_reader.h"
#include "profile_reader.h"
#include "profile.h"

#include "absl/status/status.h"

class CCTReader {
public:
    CCTReader(const std::string &file_location, Profile::profile_t *profile,
              MetaReader *meta_reader, ProfileReader *profile_reader);
    ~CCTReader();

private:
    absl::Status
    readContent();

    void
    readSingleContextMetrics(uint64_t pointer, uint32_t ctxt_id);

    Profile::profile_t *profile_;
    MetaReader *meta_reader_;
    ProfileReader *profile_reader_;

    bool is_little_endian_;
    std::fstream file_;
    uint8_t major_version_;
    uint8_t minor_version_;
    std::vector<uint64_t> section_size_;
    std::vector<uint64_t> section_pointer_;

    std::vector<float64_t> metric_sum_value_;

    std::unordered_map<uint32_t, std::vector<float64_t>> ctxt_metric_values_map_;

    std::unordered_map<std::string, std::string> metric_exc_type_map_;
    std::unordered_map<std::string, std::string> metric_desc_map_;
};

#endif // EZCONV_CCT_READER_H_
