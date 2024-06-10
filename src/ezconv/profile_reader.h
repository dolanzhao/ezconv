#ifndef EZCONV_PROFILE_READER_H_
#define EZCONV_PROFILE_READER_H_

#include <fstream>
#include <vector>
#include <unordered_map>

#include "meta_reader.h"

#include "absl/status/status.h"

typedef struct _HpcToolkitProfileInfo {
    uint64_t hit_pointer;
    uint32_t flags;
} HpcToolkitProfileInfo;

typedef struct _HpcToolkitProfilePropertie {
    bool is_physical;
    uint64_t physical_id;
    uint32_t logical_id;
} HpcToolkitProfilePropertie;

typedef struct _HpcToolkitProfileHitMap {
    std::unordered_map<std::string, HpcToolkitProfilePropertie> profile_properties;
} HpcToolkitProfileHitMap;

class ProfileReader {

public:
    ProfileReader(const std::string &file_location, MetaReader *meta_reader);
    ~ProfileReader();

    HpcToolkitProfileHitMap
    getHitFromProfile(int index)
    {
        auto profile = profile_infos_[index];
        auto hit_pointer = profile.hit_pointer;
        if (hit_pointer != 0) {
            return hit_map_[hit_pointer];
        } else {
            profile = profile_infos_[summary_profile_idx_];
            hit_pointer = profile.hit_pointer;
            return hit_map_[hit_pointer];
        }
    }

private:
    absl::Status readContent();

    MetaReader *meta_reader_;

    bool is_little_endian_;
    std::fstream file_;
    uint8_t major_version_;
    uint8_t minor_version_;
    std::vector<uint64_t> section_size_;
    std::vector<uint64_t> section_pointer_;
    std::unordered_map<uint64_t, HpcToolkitProfileHitMap> hit_map_;

    int summary_profile_idx_;
    std::vector<HpcToolkitProfileInfo> profile_infos_;
};


#endif // EZCONV_PROFILE_READER_H
