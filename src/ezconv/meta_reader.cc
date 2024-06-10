#include <cassert>
#include <filesystem>
#include <functional>
#include <bitset>

#include "meta_reader.h"
#include "absl/log/log.h"

#define LITTLE_ENDIAN_TO_HOST(__VALUE__)                      \
    do {                                                      \
        if (!is_little_endian_) {                             \
            char *ptr = reinterpret_cast<char *>(&__VALUE__); \
            std::reverse(ptr, ptr + sizeof(__VALUE__));       \
        }                                                     \
    } while (0)

MetaReader::MetaReader(const std::string &file_location, Profile::profile_t *profile,
                       DefaultReader *default_reader)
    : profile_(profile)
    , default_reader_(default_reader)
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

MetaReader::~MetaReader()
{
    if (file_.is_open()) {
        file_.close();
    }
    for (auto metric_type : metric_types_) {
        delete metric_type;
    }
}

absl::Status
MetaReader::readContent()
{
    const std::unordered_map<std::string, int> section_offset_map = {
        { "General Properties", 0 },  { "Identifier Names", 1 },
        { "Performance Metrics", 2 }, { "Context Tree", 3 },
        { "Common String Table", 4 }, { "Load Modules", 5 },
        { "Source Files", 6 },        { "Functions", 7 },
    };

    const std::vector<std::string> section_read_order = {
        "Common String Table", "General Properties",  "Source Files",
        "Load Modules",        "Functions",           "Context Tree",
        "Identifier Names",    "Performance Metrics",
    };

    const std::unordered_map<
        std::string, std::function<void(std::fstream & f, int64_t pointer, int64_t size)>>
        section_reader_map = {
            { "General Properties",
              [this](std::fstream &f, int64_t pointer, int64_t size) {
                  LOG(INFO) << "Reading General Properties";
              } },
            { "Identifier Names",
              [this](std::fstream &f, int64_t pointer, int64_t size) {
                  LOG(INFO) << "Reading Identifier Names";
                  f.seekg(pointer);

                  int64_t id_names_pointer;
                  f.read(reinterpret_cast<char *>(&id_names_pointer),
                         sizeof(id_names_pointer));
                  LITTLE_ENDIAN_TO_HOST(id_names_pointer);

                  uint8_t id_names_size;
                  f.read(reinterpret_cast<char *>(&id_names_size), sizeof(id_names_size));

                  for (uint8_t i = 0; i < id_names_size; i++) {
                      f.seekg(id_names_pointer + i * 8);
                      int64_t id_name_pointer;
                      f.read(reinterpret_cast<char *>(&id_name_pointer),
                             sizeof(id_name_pointer));
                      LITTLE_ENDIAN_TO_HOST(id_name_pointer);

                      std::string name = readStr(f, id_name_pointer);
                      identifier_names_.push_back(name);
                  }
              } },
            { "Performance Metrics",
              [this](std::fstream &f, int64_t pointer, int64_t size) {
                  LOG(INFO) << "Reading Performance Metrics";
                  f.seekg(pointer);

                  int64_t md_arry_pointer;
                  f.read(reinterpret_cast<char *>(&md_arry_pointer),
                         sizeof(md_arry_pointer));
                  LITTLE_ENDIAN_TO_HOST(md_arry_pointer);

                  uint32_t md_arry_size;
                  f.read(reinterpret_cast<char *>(&md_arry_size), sizeof(md_arry_size));
                  LITTLE_ENDIAN_TO_HOST(md_arry_size);

                  uint8_t md_size;
                  f.read(reinterpret_cast<char *>(&md_size), sizeof(md_size));

                  uint8_t psi_size;
                  f.read(reinterpret_cast<char *>(&psi_size), sizeof(psi_size));

                  for (int i = 0; i < md_arry_size; i++) {
                      f.seekg(md_arry_pointer + i * md_size);
                      int64_t md_name_pointer;
                      f.read(reinterpret_cast<char *>(&md_name_pointer),
                             sizeof(md_name_pointer));
                      LITTLE_ENDIAN_TO_HOST(md_name_pointer);
                      std::string metric_name = readStr(f, md_name_pointer);

                      int64_t psi_pointer;
                      f.read(reinterpret_cast<char *>(&psi_pointer), sizeof(psi_pointer));
                      LITTLE_ENDIAN_TO_HOST(psi_pointer);

                      char holder[1024];
                      f.read(holder, 8 * sizeof(char));

                      uint16_t psi_arry_size;
                      f.read(reinterpret_cast<char *>(&psi_arry_size),
                             sizeof(psi_arry_size));
                      LITTLE_ENDIAN_TO_HOST(psi_arry_size);

                      for (uint16_t j = 0; j < psi_arry_size; j++) {
                          f.seekg(psi_pointer + j * psi_size);
                          int64_t ps_in_psi_pointer;
                          f.read(reinterpret_cast<char *>(&ps_in_psi_pointer),
                                 sizeof(ps_in_psi_pointer));
                          LITTLE_ENDIAN_TO_HOST(ps_in_psi_pointer);

                          uint16_t psi_metric_id;
                          f.read(reinterpret_cast<char *>(&psi_metric_id),
                                 sizeof(psi_metric_id));
                          LITTLE_ENDIAN_TO_HOST(psi_metric_id);

                          f.seekg(ps_in_psi_pointer);

                          int64_t ps_name_pointer;
                          f.read(reinterpret_cast<char *>(&ps_name_pointer),
                                 sizeof(ps_name_pointer));
                          LITTLE_ENDIAN_TO_HOST(ps_name_pointer);
                          std::string ps_name = readStr(f, ps_name_pointer);
                          if (default_reader_ != nullptr &&
                              default_reader_->getMetricExcType(metric_name) == ps_name) {

                              Profile::metric_type_t *metric_type =
                                  new Profile::metric_type_t(
                                      1, profile_->add_string(""),
                                      profile_->add_string(metric_name));
                              metric_types_.push_back(metric_type);
                              int metric_type_idx = metric_types_.size() - 1;
                              metric_type_idx_map_.insert(
                                  { psi_metric_id, metric_type_idx });
                              LOG(ERROR)
                                  << "Metric[" << metric_type_idx << "/" << psi_metric_id
                                  << "]: " << metric_name << " - " << ps_name;
                          }
                      }
                  }
              } },
            { "Context Tree",
              [this](std::fstream &f, int64_t pointer, int64_t size) {
                  LOG(INFO) << "Reading Context Tree";

                  this->virt_location_ =
                      this->profile_->add_location(this->virt_function_, 0);
                  this->empty_location_ =
                      this->profile_->add_location(this->empty_function_, 0);

                  this->root_context_ =
                      this->profile_->add_context(this->virt_location_, nullptr);

                  f.seekg(pointer);

                  int64_t entry_points_array_pointer;
                  f.read(reinterpret_cast<char *>(&entry_points_array_pointer),
                         sizeof(entry_points_array_pointer));
                  LITTLE_ENDIAN_TO_HOST(entry_points_array_pointer);

                  int16_t num_entry_points;
                  f.read(reinterpret_cast<char *>(&num_entry_points),
                         sizeof(num_entry_points));
                  LITTLE_ENDIAN_TO_HOST(num_entry_points);

                  int8_t entry_point_size;
                  f.read(reinterpret_cast<char *>(&entry_point_size),
                         sizeof(entry_point_size));

                  for (int i = 0; i < num_entry_points; i++) {
                      f.seekg(entry_points_array_pointer + i * entry_point_size);
                      int64_t children_size;
                      f.read(reinterpret_cast<char *>(&children_size),
                             sizeof(children_size));
                      LITTLE_ENDIAN_TO_HOST(children_size);

                      int64_t children_pointer;
                      f.read(reinterpret_cast<char *>(&children_pointer),
                             sizeof(children_pointer));
                      LITTLE_ENDIAN_TO_HOST(children_pointer);

                      int32_t context_id;
                      f.read(reinterpret_cast<char *>(&context_id), sizeof(context_id));
                      LITTLE_ENDIAN_TO_HOST(context_id);
                      //   LOG(ERROR) << "Context " << context_id << ":";

                      int16_t type;
                      f.read(reinterpret_cast<char *>(&type), sizeof(type));
                      LITTLE_ENDIAN_TO_HOST(type);
                      //   LOG(ERROR) << "Type: " << (int)type;

                      char holder[2];
                      f.read(holder, 2 * sizeof(char));

                      int64_t location_name_pointer;
                      f.read(reinterpret_cast<char *>(&location_name_pointer),
                             sizeof(location_name_pointer));
                      LITTLE_ENDIAN_TO_HOST(location_name_pointer);

                      Profile::function_t *func = this->profile_->add_function(
                          this->empty_source_file_,
                          this->str_idx_map_[location_name_pointer], 0);
                      Profile::location_t *location =
                          this->profile_->add_location(func, 0);
                      Profile::context_t *context =
                          this->profile_->add_context(location, this->root_context_);
                      this->context_idx_map_.insert({ context_id, context });

                      HpcToolkitContextInfo ctxt_info;
                      ctxt_info.is_entry = true;
                      ctxt_info.context_id = context_id;
                      ctxt_info.relation = 0;
                      ctxt_info.lexical_type = 0;
                      ctxt_info.function_idx = -1;
                      ctxt_info.sourcefile_idx = -1;
                      ctxt_info.sourcefile_line = 0;
                      ctxt_info.load_module_index = 0;
                      this->context_info_map_.insert({ context_id, ctxt_info });
                      this->context_belongfunction_map_.insert(
                          { context_id, virt_function_ });
                      readCCTChildren(context_id, f, children_pointer, children_size);
                  }
              } },
            { "Common String Table",
              [this](std::fstream &f, int64_t pointer, int64_t size) {
                  LOG(INFO) << "Reading Common String Table";
                  f.seekg(pointer);
                  char *buffer = new char[size];
                  f.read(buffer, size);
                  int str_offset = 0;
                  for (int i = 0; i < size; i++) {
                      if (buffer[i] == '\0') {
                          int64_t str_idx = this->profile_->add_string(
                              std::string(buffer + str_offset, i - str_offset));
                          this->str_idx_map_.insert({ pointer + str_offset, str_idx });
                          str_offset = i + 1;
                      }
                  }
                  this->empty_str_idx_ = 0;
                  this->virtual_str_idx_ = this->profile_->add_string("Virtual Node");
                  delete[] buffer;
              } },
            { "Load Modules",
              [this](std::fstream &f, int64_t pointer, int64_t size) {
                  LOG(INFO) << "Reading Load Modules";
                  f.seekg(pointer);
                  f.read(reinterpret_cast<char *>(&this->module_pointer_),
                         sizeof(this->module_pointer_));
                  LITTLE_ENDIAN_TO_HOST(this->module_pointer_);

                  f.read(reinterpret_cast<char *>(&this->module_num_),
                         sizeof(this->module_num_));
                  LITTLE_ENDIAN_TO_HOST(this->module_num_);

                  f.read(reinterpret_cast<char *>(&this->module_size_),
                         sizeof(this->module_size_));
                  LITTLE_ENDIAN_TO_HOST(this->module_size_);

                  for (int i = 0; i < this->module_num_; i++) {
                      f.seekg(this->module_pointer_ + i * this->module_size_ + 8);

                      int64_t load_module_path_pointer;
                      f.read(reinterpret_cast<char *>(&load_module_path_pointer),
                             sizeof(load_module_path_pointer));
                      LITTLE_ENDIAN_TO_HOST(load_module_path_pointer);

                      Profile::source_file_t *module = this->profile_->add_source_file(
                          this->str_idx_map_[load_module_path_pointer], 0);
                      this->module_idx_map_.insert({ i, module });
                  }
              } },
            { "Source Files",
              [this](std::fstream &f, int64_t pointer, int64_t size) {
                  LOG(INFO) << "Reading Source Files";
                  f.seekg(pointer);
                  f.read(reinterpret_cast<char *>(&this->source_file_pointer_),
                         sizeof(this->source_file_pointer_));
                  LITTLE_ENDIAN_TO_HOST(this->source_file_pointer_);

                  f.read(reinterpret_cast<char *>(&this->source_file_num_),
                         sizeof(this->source_file_num_));
                  LITTLE_ENDIAN_TO_HOST(this->source_file_num_);

                  f.read(reinterpret_cast<char *>(&this->source_file_size_),
                         sizeof(this->source_file_size_));
                  LITTLE_ENDIAN_TO_HOST(this->source_file_size_);

                  for (int i = 0; i < this->source_file_num_; i++) {
                      f.seekg(this->source_file_pointer_ + i * this->source_file_size_ +
                              8);
                      int64_t file_path_pointer;
                      f.read(reinterpret_cast<char *>(&file_path_pointer),
                             sizeof(file_path_pointer));
                      LITTLE_ENDIAN_TO_HOST(file_path_pointer);
                      Profile::source_file_t *sourcefile =
                          this->profile_->add_source_file(
                              this->str_idx_map_[file_path_pointer], 0);
                      this->sourcefile_idx_map_.insert({ i, sourcefile });
                  }
                  this->empty_source_file_ =
                      this->profile_->add_source_file(this->empty_str_idx_, 0);
              } },
            { "Functions",
              [this](std::fstream &f, int64_t pointer, int64_t size) {
                  LOG(INFO) << "Reading Functions";
                  f.seekg(pointer);
                  f.read(reinterpret_cast<char *>(&this->function_pointer_),
                         sizeof(this->function_pointer_));
                  LITTLE_ENDIAN_TO_HOST(this->function_pointer_);

                  f.read(reinterpret_cast<char *>(&this->function_num_),
                         sizeof(this->function_num_));
                  LITTLE_ENDIAN_TO_HOST(this->function_num_);

                  f.read(reinterpret_cast<char *>(&this->function_size_),
                         sizeof(this->function_size_));
                  LITTLE_ENDIAN_TO_HOST(this->function_size_);

                  for (int i = 0; i < this->function_num_; i++) {
                      f.seekg(this->function_pointer_ + i * this->function_size_);

                      int64_t function_name_pointer;
                      f.read(reinterpret_cast<char *>(&function_name_pointer),
                             sizeof(function_name_pointer));
                      LITTLE_ENDIAN_TO_HOST(function_name_pointer);

                      int64_t module_path_pointer;
                      f.read(reinterpret_cast<char *>(&module_path_pointer),
                             sizeof(module_path_pointer));
                      LITTLE_ENDIAN_TO_HOST(module_path_pointer);
                      int module_idx = (module_path_pointer - this->module_pointer_) /
                          this->module_size_;
                      Profile::source_file_t *module =
                          module_idx < 0 ? nullptr : this->module_idx_map_[module_idx];

                      int64_t module_offset;
                      f.read(reinterpret_cast<char *>(&module_offset),
                             sizeof(module_offset));
                      LITTLE_ENDIAN_TO_HOST(module_offset);

                      int64_t file_pointer;
                      f.read(reinterpret_cast<char *>(&file_pointer),
                             sizeof(file_pointer));
                      LITTLE_ENDIAN_TO_HOST(file_pointer);
                      int sourcefile_idx = (file_pointer - this->source_file_pointer_) /
                          this->source_file_size_;
                      Profile::source_file_t *source_file = sourcefile_idx < 0
                          ? nullptr
                          : this->sourcefile_idx_map_[sourcefile_idx];

                      int32_t line_number;
                      f.read(reinterpret_cast<char *>(&line_number), sizeof(line_number));
                      LITTLE_ENDIAN_TO_HOST(line_number);

                      Profile::source_file_t *real_source_file = this->empty_source_file_;
                      if (module != nullptr) {
                          real_source_file = module;
                      }
                      if (source_file != nullptr) {
                          real_source_file = source_file;
                      }

                      Profile::function_t *func = this->profile_->add_function(
                          real_source_file, this->str_idx_map_[function_name_pointer],
                          (int64_t)line_number);
                      this->function_idx_map_.insert({ i, func });
                  }
                  this->virt_function_ = this->profile_->add_function(
                      this->empty_source_file_, this->virtual_str_idx_, 0);
                  this->empty_function_ = this->profile_->add_function(
                      this->empty_source_file_, this->empty_str_idx_, 0);
              } },
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
    if (std::string(format_identifier) != "meta") {
        return absl::InternalError("Invalid format identifier");
    }

    // Next byte (u8) are the major version
    file_.read(reinterpret_cast<char *>(&major_version_), 1);

    // Next byte (u8) are the minor version
    file_.read(reinterpret_cast<char *>(&minor_version_), 1);

    LOG(INFO) << "Version: " << static_cast<int>(major_version_) << "."
              << static_cast<int>(minor_version_);

    for (int i = 0; i < section_offset_map.size(); i++) {
        int64_t size;
        file_.read(reinterpret_cast<char *>(&size), sizeof(size));
        LITTLE_ENDIAN_TO_HOST(size);
        section_size_.push_back(size);
        int64_t pointer;
        file_.read(reinterpret_cast<char *>(&pointer), sizeof(pointer));
        LITTLE_ENDIAN_TO_HOST(pointer);
        section_pointer_.push_back(pointer);
    }

    for (int i = 0; i < section_read_order.size(); i++) {
        const std::string &section_name = section_read_order[i];
        int64_t pointer = section_pointer_[section_offset_map.at(section_name)];
        int64_t size = section_size_[section_offset_map.at(section_name)];
        if (section_reader_map.at(section_name) != nullptr) {
            section_reader_map.at(section_name)(file_, pointer, size);
        }
    }

    return absl::OkStatus();
}

int
MetaReader::readCCTChildren(uint32_t pcontext_id, std::fstream &f, int64_t pointer,
                            int64_t size)
{
    if (size <= 0 || pointer <= 0) {
        return 0;
    }
    f.seekg(pointer);
    int ctxt_num = 0;
    int64_t index = 0;
    while (index < size) {
        ctxt_num++;

        HpcToolkitContextInfo ctxt_info;
        ctxt_info.is_entry = false;

        int64_t children_size;
        f.read(reinterpret_cast<char *>(&children_size), sizeof(children_size));
        LITTLE_ENDIAN_TO_HOST(children_size);
        index += sizeof(children_size);

        int64_t children_pointer;
        f.read(reinterpret_cast<char *>(&children_pointer), sizeof(children_pointer));
        LITTLE_ENDIAN_TO_HOST(children_pointer);
        index += sizeof(children_pointer);

        uint32_t context_id;
        f.read(reinterpret_cast<char *>(&context_id), sizeof(context_id));
        LITTLE_ENDIAN_TO_HOST(context_id);
        index += sizeof(context_id);
        ctxt_info.context_id = context_id;
        // LOG(ERROR) << "Context " << context_id << ":";

        uint8_t flags;
        f.read(reinterpret_cast<char *>(&flags), sizeof(flags));
        index += sizeof(flags);

        uint8_t relation;
        f.read(reinterpret_cast<char *>(&relation), sizeof(relation));
        index += sizeof(relation);
        ctxt_info.relation = relation;

        uint8_t lexical_type;
        f.read(reinterpret_cast<char *>(&lexical_type), sizeof(lexical_type));
        index += sizeof(lexical_type);
        ctxt_info.lexical_type = lexical_type;

        uint8_t num_flex_words;
        f.read(reinterpret_cast<char *>(&num_flex_words), sizeof(num_flex_words));
        index += sizeof(num_flex_words);

        char holder[8];
        f.read(holder, 8 * sizeof(char));
        index += 8 * sizeof(char);
        ctxt_info.function_idx = -1;
        ctxt_info.sourcefile_idx = -1;
        ctxt_info.sourcefile_line = 0;
        ctxt_info.load_module_index = -1;
        ctxt_info.load_module_offset = 0;
        if ((int)num_flex_words > 0) {
            char *flex = new char[num_flex_words * 8];
            f.read(flex, num_flex_words * 8 * sizeof(char));
            index += num_flex_words * 8 * sizeof(char);
            int flex_index = 0;
            if ((flags & 1) != 0) {
                if (num_flex_words - flex_index < 1) {
                    LOG(FATAL) << "Invalid flex index";
                }
                int64_t sub_flex;
                memcpy(&sub_flex, flex + flex_index * sizeof(int64_t), sizeof(sub_flex));
                LITTLE_ENDIAN_TO_HOST(sub_flex);
                flex_index++;
                ctxt_info.function_idx = sub_flex;
            }

            if ((flags & 2) != 0) {
                if (num_flex_words - flex_index < 2) {
                    LOG(FATAL) << "Invalid flex index";
                }
                int64_t sub_flex_1;
                memcpy(&sub_flex_1, flex + flex_index * sizeof(int64_t),
                       sizeof(sub_flex_1));
                LITTLE_ENDIAN_TO_HOST(sub_flex_1);
                flex_index++;
                ctxt_info.sourcefile_idx = sub_flex_1;

                uint32_t sub_flex_2;
                memcpy(&sub_flex_2, flex + flex_index * sizeof(int64_t),
                       sizeof(sub_flex_2));
                LITTLE_ENDIAN_TO_HOST(sub_flex_2);
                flex_index++;

                ctxt_info.sourcefile_line = sub_flex_2;
            }

            if ((flags & 4) != 0) {
                if (num_flex_words - flex_index < 2) {
                    LOG(FATAL) << "Invalid flex index";
                }

                int64_t sub_flex_1;
                memcpy(&sub_flex_1, flex + flex_index * sizeof(int64_t),
                       sizeof(sub_flex_1));
                LITTLE_ENDIAN_TO_HOST(sub_flex_1);
                flex_index++;

                ctxt_info.load_module_index = sub_flex_1;

                int64_t sub_flex_2;
                memcpy(&sub_flex_2, flex + flex_index * sizeof(int64_t),
                       sizeof(sub_flex_2));
                LITTLE_ENDIAN_TO_HOST(sub_flex_2);
                flex_index++;

                ctxt_info.load_module_offset = sub_flex_2;
            }
            delete[] flex;
        }

        context_info_map_.insert({ context_id, ctxt_info });
        // std::bitset<8> relation_bitset(relation);
        // LOG(ERROR) << relation_bitset.to_string() << "/" << (int)lexical_type;
        Profile::context_t *pcontext = context_idx_map_[pcontext_id];
        Profile::context_t *context = nullptr;
        if (lexical_type == 0) {
            // LOG(ERROR) << "Lexical type is 0";
            // LOG(ERROR) << "children : [" << children_pointer << "/" << children_size <<
            // "]";
            int64_t rel_func_idx =
                (ctxt_info.function_idx - this->function_pointer_) / this->function_size_;
            rel_func_idx = rel_func_idx < 0 ? 0 : rel_func_idx;
            // LOG(ERROR) << "Function index: " << rel_func_idx;
            Profile::function_t *be_calledfunc = this->function_idx_map_[rel_func_idx];
            context_belongfunction_map_.insert({ context_id, be_calledfunc });

            HpcToolkitContextInfo parent_ctxt_info = context_info_map_[pcontext_id];
            if (parent_ctxt_info.is_entry) {
                // LOG(ERROR) << "Parent is entry";
                Profile::function_t *func = this->profile_->add_function(
                    this->empty_source_file_, be_calledfunc->get_name(), 0);
                Profile::location_t *location = this->profile_->add_location(func, 0);
                context = this->profile_->add_context(location, pcontext);
            } else {
                // LOG(ERROR) << "Parent is not entry";
                Profile::source_file_t *source_file = nullptr;
                int32_t rel_line_no = 0;
                if (parent_ctxt_info.load_module_index != -1) {
                    int64_t rel_module_idx =
                        (parent_ctxt_info.load_module_index - this->module_pointer_) /
                        this->module_size_;
                    rel_module_idx = rel_module_idx < 0 ? 0 : rel_module_idx;
                    source_file = this->module_idx_map_[rel_module_idx];
                    rel_line_no = 0;
                }
                if (parent_ctxt_info.sourcefile_idx != -1) {
                    int64_t rel_sourcefile_idx =
                        (parent_ctxt_info.sourcefile_idx - this->source_file_pointer_) /
                        this->source_file_size_;
                    rel_sourcefile_idx = rel_sourcefile_idx < 0 ? 0 : rel_sourcefile_idx;
                    source_file = this->sourcefile_idx_map_[rel_sourcefile_idx];
                    rel_line_no = parent_ctxt_info.sourcefile_line;
                }
                if (source_file == nullptr) {
                    source_file = this->empty_source_file_;
                }
                Profile::function_t *func = this->profile_->add_function(
                    source_file, be_calledfunc->get_name(), 0);
                Profile::location_t *location =
                    this->profile_->add_location(func, rel_line_no);
                context = this->profile_->add_context(location, pcontext);
            }
        } else if (lexical_type == 1) {
            // LOG(ERROR) << "Lexical type is 1";
            // LOG(ERROR) << "children : [" << children_pointer << "/" << children_size <<
            // "]";
            context_belongfunction_map_.insert(
                { context_id, context_belongfunction_map_[pcontext_id] });
        } else if (lexical_type == 2) {
            // LOG(ERROR) << "Lexical type is 2";
            // LOG(ERROR) << "children : [" << children_pointer << "/" << children_size <<
            // "]";
            Profile::function_t *be_calledfunc = context_belongfunction_map_[pcontext_id];
            context_belongfunction_map_.insert({ context_id, be_calledfunc });
            if (children_pointer == 0 || children_size == 0) {
                Profile::source_file_t *be_called_sourcefile =
                    be_calledfunc->get_source_file();
                Profile::source_file_t *source_file = nullptr;
                int32_t rel_line_no = 0;
                if (ctxt_info.sourcefile_idx != -1) {
                    int64_t rel_sourcefile_idx =
                        (ctxt_info.sourcefile_idx - this->source_file_pointer_) /
                        this->source_file_size_;
                    rel_sourcefile_idx = rel_sourcefile_idx < 0 ? 0 : rel_sourcefile_idx;
                    source_file = this->sourcefile_idx_map_[rel_sourcefile_idx];
                    rel_line_no = ctxt_info.sourcefile_line;
                }
                if (source_file == nullptr) {
                    source_file = this->empty_source_file_;
                }
                if (be_called_sourcefile != source_file) {
                    LOG(ERROR) << "2 Source file mismatch";
                    LOG(ERROR) << "Be called source file: "
                               << this->profile_->get_string_table()->get_string(
                                      be_called_sourcefile->get_location_path());
                    LOG(ERROR) << "Source file: "
                               << this->profile_->get_string_table()->get_string(
                                      source_file->get_location_path());
                }
                Profile::location_t *location =
                    this->profile_->add_location(be_calledfunc, rel_line_no);
                context = this->profile_->add_context(location, pcontext);
            }
        } else if (lexical_type == 3) {
            // LOG(ERROR) << "Lexical type is 3";
            // LOG(ERROR) << "children : [" << children_pointer << "/" << children_size <<
            // "]";
            Profile::function_t *be_calledfunc = context_belongfunction_map_[pcontext_id];
            context_belongfunction_map_.insert({ context_id, be_calledfunc });
            if (children_pointer == 0 || children_size == 0) {
                HpcToolkitContextInfo parent_ctxt_info = context_info_map_[pcontext_id];
                if (parent_ctxt_info.lexical_type == 2) {
                    Profile::source_file_t *be_called_sourcefile =
                        be_calledfunc->get_source_file();
                    Profile::source_file_t *source_file = nullptr;
                    int32_t rel_line_no = 0;
                    if (parent_ctxt_info.sourcefile_idx != -1) {
                        int64_t rel_sourcefile_idx = (parent_ctxt_info.sourcefile_idx -
                                                      this->source_file_pointer_) /
                            this->source_file_size_;
                        rel_sourcefile_idx =
                            rel_sourcefile_idx < 0 ? 0 : rel_sourcefile_idx;
                        source_file = this->sourcefile_idx_map_[rel_sourcefile_idx];
                        rel_line_no = parent_ctxt_info.sourcefile_line;
                    }
                    if (source_file == nullptr) {
                        source_file = this->empty_source_file_;
                    }
                    if (be_called_sourcefile != source_file) {
                        LOG(ERROR) << "3 Source file mismatch";
                        LOG(ERROR) << "Be called source file: "
                                   << this->profile_->get_string_table()->get_string(
                                          be_called_sourcefile->get_location_path());
                        LOG(ERROR) << "Source file: "
                                   << this->profile_->get_string_table()->get_string(
                                          source_file->get_location_path());
                    }
                    Profile::location_t *location =
                        this->profile_->add_location(be_calledfunc, rel_line_no);
                    pcontext = this->profile_->add_context(location, pcontext);
                    this->context_idx_map_[parent_ctxt_info.context_id] = pcontext;
                }
                Profile::source_file_t *source_file = nullptr;
                uint64_t offset = 0;
                if (ctxt_info.load_module_index != -1) {
                    int64_t rel_module_idx =
                        (ctxt_info.load_module_index - this->module_pointer_) /
                        this->module_size_;
                    rel_module_idx = rel_module_idx < 0 ? 0 : rel_module_idx;
                    source_file = this->module_idx_map_[rel_module_idx];
                    offset = ctxt_info.load_module_offset;
                }
                if (source_file == nullptr) {
                    source_file = this->empty_source_file_;
                }

                std::ostringstream oss;
                oss << std::hex << std::setfill('0') << std::setw(16) << offset;
                std::string offset_hex_str = oss.str();

                int64_t func_name_idx =
                    this->profile_->add_string("[0x" + offset_hex_str + "]");
                Profile::function_t *function =
                    this->profile_->add_function(source_file, func_name_idx, 0);
                Profile::location_t *location = this->profile_->add_location(function, 0);
                context = this->profile_->add_context(location, pcontext);
            }
        }

        if (context != nullptr) {
            context_idx_map_.insert({ context_id, context });
            // LOG(ERROR) << "Context ID: " << context->get_id();
            // Profile::location_t *location = context->get_location();
            // Profile::function_t *func = location->get_leaf_line()->get_function();
            // Profile::source_file_t *source_file = func->get_source_file();
            // LOG(ERROR) << "Function name: "
            //            <<
            //            this->profile_->get_string_table()->get_string(func->get_name());
            // LOG(ERROR) << "Location line: " << location->get_leaf_line()->get_line();
            // LOG(ERROR) << "Source file: "
            //            << this->profile_->get_string_table()->get_string(
            //                   source_file->get_location_path());
        } else {
            context_idx_map_.insert({ context_id, pcontext });
        }

        int64_t return_address = f.tellg();
        readCCTChildren(context_id, f, children_pointer, children_size);
        f.seekg(return_address);
    }
    return ctxt_num;
}

std::string
MetaReader::readStr(std::fstream &f, int64_t pointer)
{
    uint64_t pos = f.tellg();
    f.seekg(pointer);
    std::vector<char> buffer;
    while (true) {
        char c;
        f.read(&c, 1);
        if (c == '\0') {
            break;
        }
        buffer.push_back(c);
    }
    f.seekg(pos);
    return std::string(buffer.begin(), buffer.end());
}

bool
MetaReader::hasCtxt(uint32_t context_id)
{
    return context_idx_map_.find(context_id) != context_idx_map_.end();
}

bool
MetaReader::needRecordMetric(uint16_t metric_id)
{
    return metric_type_idx_map_.find(metric_id) != metric_type_idx_map_.end();
}

uint16_t
MetaReader::getMetricTypeIdx(uint16_t metric_id)
{
    return metric_type_idx_map_[metric_id];
}

std::string
MetaReader::getMetricTypeName(uint16_t metric_id)
{
    int64_t name_idx = metric_types_[metric_type_idx_map_[metric_id]]->get_des();
    return profile_->get_string_table()->get_string(name_idx);
}

int
MetaReader::needRecordMetricNum()
{
    return metric_type_idx_map_.size();
}
