#include "profile_reader.h"

#include "absl/log/log.h"

#define LITTLE_ENDIAN_TO_HOST(__VALUE__)                      \
    do {                                                      \
        if (!is_little_endian_) {                             \
            char *ptr = reinterpret_cast<char *>(&__VALUE__); \
            std::reverse(ptr, ptr + sizeof(__VALUE__));       \
        }                                                     \
    } while (0)

ProfileReader::ProfileReader(const std::string &file_location, MetaReader *meta_reader)
    : meta_reader_(meta_reader)
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

ProfileReader::~ProfileReader()
{
    if (file_.is_open()) {
        file_.close();
    }
}

absl::Status
ProfileReader::readContent()
{
    const std::unordered_map<std::string, int> section_offset_map = {
        { "Profiles Information", 0 }, { "Hierarchical Identifier Tuples", 1 }
    };

    const std::vector<std::string> section_read_order = {
        "Hierarchical Identifier Tuples", "Profiles Information"
    };

    const std::unordered_map<
        std::string,
        std::function<void(std::fstream & f, uint64_t pointer, uint64_t size)>>
        section_reader_map = {
            { "Profiles Information",
              [this](std::fstream &f, uint64_t pointer, uint64_t size) {
                  LOG(INFO) << "Reading Profiles Information";
                  f.seekg(pointer);
                  uint64_t profiles_pointer;
                  f.read(reinterpret_cast<char *>(&profiles_pointer),
                         sizeof(profiles_pointer));
                  LITTLE_ENDIAN_TO_HOST(profiles_pointer);

                  uint32_t num_profiles;
                  f.read(reinterpret_cast<char *>(&num_profiles), sizeof(num_profiles));
                  LITTLE_ENDIAN_TO_HOST(num_profiles);

                  uint8_t profile_size;
                  f.read(reinterpret_cast<char *>(&profile_size), sizeof(profile_size));

                  for (uint32_t i = 0; i < num_profiles; i++) {
                      f.seekg(profiles_pointer + i * profile_size);

                      char holder[1024];
                      f.read(holder, 0x20);

                      uint64_t hit_pointer;
                      f.read(reinterpret_cast<char *>(&hit_pointer), sizeof(hit_pointer));
                      LITTLE_ENDIAN_TO_HOST(hit_pointer);

                      uint32_t flags;
                      f.read(reinterpret_cast<char *>(&flags), sizeof(flags));
                      LITTLE_ENDIAN_TO_HOST(flags);
                      profile_infos_.push_back({ hit_pointer, flags });
                      if (hit_pointer == 0) {
                          summary_profile_idx_ = i;
                      }
                  }
              } },
            { "Hierarchical Identifier Tuples",
              [this](std::fstream &f, uint64_t pointer, uint64_t size) {
                  LOG(INFO) << "Reading Hierarchical Identifier Tuples";
                  f.seekg(pointer);

                  while (f.tellg() < pointer + size) {
                      uint64_t hit_pointer = f.tellg();

                      uint16_t num_tuples;
                      f.read(reinterpret_cast<char *>(&num_tuples), 2);
                      LITTLE_ENDIAN_TO_HOST(num_tuples);

                      char holder[6];
                      f.read(holder, 6);

                      std::unordered_map<std::string, HpcToolkitProfilePropertie>
                          profile_properties;
                      for (uint16_t i = 0; i < num_tuples; i++) {
                          uint8_t kind;
                          f.read(reinterpret_cast<char *>(&kind), 1);

                          f.read(holder, 1);

                          uint16_t flags;
                          f.read(reinterpret_cast<char *>(&flags), 2);
                          LITTLE_ENDIAN_TO_HOST(flags);

                          uint32_t logical_id;
                          f.read(reinterpret_cast<char *>(&logical_id), 4);
                          LITTLE_ENDIAN_TO_HOST(logical_id);

                          uint64_t physical_id;
                          f.read(reinterpret_cast<char *>(&physical_id), 8);
                          LITTLE_ENDIAN_TO_HOST(physical_id);

                          std::string identifier_name =
                              meta_reader_->getIdentifierName((int)kind);
                          profile_properties[identifier_name] = {
                              (flags & 0x1 ? true : false), physical_id, logical_id
                          };
                      }
                      hit_map_[hit_pointer] = { profile_properties };
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
    if (std::string(format_identifier) != "prof") {
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
