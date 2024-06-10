#ifndef EZCONV_META_READER_H_
#define EZCONV_META_READER_H_

#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "profile.h"
#include "default_reader.h"

#include "absl/status/status.h"

typedef struct _HpcToolkitContextInfo {
    bool is_entry;
    uint32_t context_id;
    uint8_t relation;
    uint8_t lexical_type;
    int64_t function_idx;
    int64_t sourcefile_idx;
    uint32_t sourcefile_line;
    int64_t load_module_index;
    uint64_t load_module_offset;
} HpcToolkitContextInfo;

class MetaReader {
public:
    MetaReader(const std::string &file_location, Profile::profile_t *profile,
               DefaultReader *default_reader);
    ~MetaReader();

    bool
    hasCtxt(uint32_t context_id);

    bool
    needRecordMetric(uint16_t metric_id);

    uint16_t
    getMetricTypeIdx(uint16_t metric_id);

    std::string
    getMetricTypeName(uint16_t metric_id);

    int
    needRecordMetricNum();

    std::string
    getIdentifierName(int kind)
    {
        return identifier_names_[kind];
    }

    Profile::context_t *
    getCtxt(uint32_t context_id)
    {
        return context_idx_map_[context_id];
    }

    Profile::metric_type_t *
    getMetricType(uint16_t metric_id)
    {
        return metric_types_[metric_type_idx_map_[metric_id]];
    }

    Profile::metric_type_t *
    getMetricType(int idx)
    {
        return metric_types_[idx];
    }


private:
    absl::Status
    readContent();

    int
    readCCTChildren(uint32_t pcontext_id, std::fstream &f, int64_t pointer, int64_t size);

    std::string
    readStr(std::fstream &f, int64_t pointer);

    bool is_little_endian_;
    std::fstream file_;
    uint8_t major_version_;
    uint8_t minor_version_;
    std::vector<int64_t> section_size_;
    std::vector<int64_t> section_pointer_;
    std::unordered_map<int64_t, int64_t> str_idx_map_;
    std::unordered_map<int64_t, Profile::source_file_t *> sourcefile_idx_map_;
    std::unordered_map<int64_t, Profile::source_file_t *> module_idx_map_;
    std::unordered_map<int64_t, Profile::function_t *> function_idx_map_;
    std::unordered_map<uint32_t, Profile::context_t *> context_idx_map_;
    std::unordered_map<uint32_t, HpcToolkitContextInfo> context_info_map_;
    std::unordered_map<uint32_t, Profile::function_t *> context_belongfunction_map_;
    std::vector<Profile::metric_type_t* > metric_types_;
    std::unordered_map<uint16_t, int> metric_type_idx_map_;
    std::vector<std::string> identifier_names_;

    Profile::profile_t *profile_;
    DefaultReader *default_reader_;

    int64_t source_file_pointer_;
    int16_t source_file_size_;
    int32_t source_file_num_;

    int64_t module_pointer_;
    int16_t module_size_;
    int32_t module_num_;

    int64_t function_pointer_;
    int16_t function_size_;
    int32_t function_num_;

    int64_t empty_str_idx_;
    Profile::source_file_t *empty_source_file_;

    int64_t virtual_str_idx_;
    Profile::function_t *virt_function_;
    Profile::function_t *empty_function_;

    Profile::location_t *virt_location_;
    Profile::location_t *empty_location_;
    Profile::context_t *root_context_;
};

#endif // EZCONV_META_READER_H_