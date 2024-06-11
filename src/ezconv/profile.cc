#include <stdio.h>
#include <algorithm>

#include "profile.h"

#include "absl/log/log.h"

/*
 * Profile::string_table_t
 */

Profile::string_table_t::string_table_t()
    : max_index_(0)
    , buffer_(NULL)
    , table_buffer_(NULL)
{
    this->table_ = new std::map<std::string, int64_t>();
    (*this->table_).insert(std::pair<std::string, int64_t>("", this->max_index_));
}

Profile::string_table_t::~string_table_t()
{
    delete this->table_;
    free(this->buffer_);
    free(this->table_buffer_);
}

int64_t
Profile::string_table_t::add_string(std::string str)
{
    std::map<std::string, int64_t>::iterator it = (*this->table_).find(str);
    if (it != (*this->table_).end()) {
        return it->second;
    }
    (*this->table_).insert(std::pair<std::string, int64_t>(str, ++this->max_index_));
    return this->max_index_;
}

int64_t
Profile::string_table_t::add_string(char *c_str)
{
    if (c_str == NULL) {
        LOG(FATAL) << "Error: [Profile::string_table_t::add_string(char* c_str)] c_str "
                      "== NULL .";
    }
    std::string str = c_str;
    return this->add_string(str);
}

std::string
Profile::string_table_t::get_string(int64_t index)
{
    if (index > this->max_index_) {
        return std::string("");
    }
    auto it = std::find_if(
        (*this->table_).begin(), (*this->table_).end(),
        [&index](const std::pair<std::string, int64_t> &p) { return p.second == index; });

    if (it == (*this->table_).end()) {
        return std::string("");
    } else {
        return it->first;
    }
}

size_t
Profile::string_table_t::get_table_size()
{
    return (size_t)(max_index_ + 1);
}

char **
Profile::string_table_t::encode()
{
    if (this->buffer_) {
        free(this->buffer_);
    }
    size_t buffer_size = (size_t)(max_index_ + 1);
    this->buffer_ = (char **)malloc(sizeof(char *) * buffer_size);

    size_t global_len = 0;
    std::map<int64_t, size_t> *offset_map = new std::map<int64_t, size_t>();
    for (auto &str : (*this->table_)) {
        size_t str_len = str.first.length() + 1;
        (*offset_map).insert(std::pair<int64_t, size_t>(str.second, str_len));
        global_len += str_len;
    }
    if (this->table_buffer_) {
        free(this->table_buffer_);
    }
    this->table_buffer_ = (char *)malloc(sizeof(char) * global_len);

    size_t cur_offset = 0;
    for (size_t i = 0; i < buffer_size; i++) {
        this->buffer_[i] = this->table_buffer_ + cur_offset;
        size_t temp_offset = cur_offset;
        cur_offset += (*offset_map)[i];
        (*offset_map)[i] = temp_offset;
    }

    for (auto &str : (*this->table_)) {
        memcpy(this->buffer_[str.second], str.first.c_str(), str.first.length() + 1);
    }

    delete offset_map;

    return this->buffer_;
}

/*
 * Profile::metric_type_t
 */

Profile::metric_type_t::metric_type_t(int64_t value_type, int64_t unit, int64_t des)
    : value_type_(value_type)
    , unit_(unit)
    , des_(des)
{
    this->buffer_ = DRCCTPROF__PROFILE__METRIC_TYPE__INIT;
}

Profile::metric_type_t::~metric_type_t()
{
}

int64_t
Profile::metric_type_t::get_des()
{
    return this->des_;
}

int64_t
Profile::metric_type_t::get_unit()
{
    return this->unit_;
}

int64_t
Profile::metric_type_t::get_value_type()
{
    return this->value_type_;
}

void
Profile::metric_type_t::update_uint(int64_t unit)
{
    this->unit_ = unit;
}

Drcctprof__Profile__MetricType *
Profile::metric_type_t::encode()
{
    this->buffer_.value_type = this->value_type_;
    this->buffer_.unit = this->unit_;
    this->buffer_.des = this->des_;
    return &(this->buffer_);
}

/*
 * Profile::metric_t
 */

Profile::metric_t::metric_t(int64_t int_value)
    : int_value_(int_value)
    , uint_value_(0)
    , str_index_(0)
{
    this->buffer_ = DRCCTPROF__PROFILE__METRIC__INIT;
}

Profile::metric_t::metric_t(uint64_t uint_value)
    : int_value_(0)
    , uint_value_(uint_value)
    , str_index_(0)
{
    this->buffer_ = DRCCTPROF__PROFILE__METRIC__INIT;
}

Profile::metric_t::metric_t(int64_t int_value, uint64_t uint_value, int64_t str_index)
    : int_value_(int_value)
    , uint_value_(uint_value)
    , str_index_(str_index)
{
    this->buffer_ = DRCCTPROF__PROFILE__METRIC__INIT;
}

Profile::metric_t::~metric_t()
{
}

Drcctprof__Profile__Metric *
Profile::metric_t::encode()
{
    this->buffer_.int_value = this->int_value_;
    this->buffer_.uint_value = this->uint_value_;
    this->buffer_.str_value = this->str_index_;
    return &(this->buffer_);
}

/*
 * Profile::source_file_t
 */

Profile::source_file_t::source_file_t(uint64_t id, int64_t file_name,
                                      int64_t location_path, int64_t type)
    : id_(id)
    , file_name_(file_name)
    , location_path_(location_path)
    , type_(type)
{
    this->buffer_ = DRCCTPROF__PROFILE__SOURCE_FILE__INIT;
}

Profile::source_file_t::~source_file_t()
{
}

uint64_t
Profile::source_file_t::get_id()
{
    return this->id_;
}

int64_t
Profile::source_file_t::get_location_path()
{
    return this->location_path_;
}

Drcctprof__Profile__SourceFile *
Profile::source_file_t::encode()
{
    this->buffer_.id = this->id_;
    this->buffer_.filename = this->file_name_;
    this->buffer_.location_path = this->location_path_;
    this->buffer_.type = this->type_;
    return &(this->buffer_);
}

/*
 * Profile::function_t
 */

Profile::function_t::function_t(uint64_t id, int64_t name, int64_t system_name,
                                int64_t start_line, Profile::source_file_t *source_file)
    : id_(id)
    , name_(name)
    , system_name_(system_name)
    , start_line_(start_line)
    , source_file_(source_file)
{
    this->buffer_ = DRCCTPROF__PROFILE__FUNCTION__INIT;
}

Profile::function_t::~function_t()
{
}

uint64_t
Profile::function_t::get_id()
{
    return this->id_;
}

int64_t
Profile::function_t::get_name()
{
    return this->name_;
}

Profile::source_file_t *
Profile::function_t::get_source_file()
{
    return this->source_file_;
}

void
Profile::function_t::set_start_line(int64_t line_no)
{
    this->start_line_ = (this->start_line_ == 0 || this->start_line_ > line_no)
        ? line_no
        : this->start_line_;
}

Drcctprof__Profile__Function *
Profile::function_t::encode()
{
    this->buffer_.id = this->id_;
    this->buffer_.name = this->name_;
    this->buffer_.system_name = this->system_name_;
    this->buffer_.source_file_id = this->source_file_->get_id();
    this->buffer_.start_line = this->start_line_;
    return &(this->buffer_);
}

/*
 * Profile::line_t
 */

Profile::line_t::line_t(Profile::function_t *func, int64_t line)
    : func_(func)
    , line_(line)
{
    this->buffer_ = DRCCTPROF__PROFILE__LINE__INIT;
}

Profile::line_t::~line_t()
{
}

Profile::function_t *
Profile::line_t::get_function()
{
    return this->func_;
}

int64_t
Profile::line_t::get_line()
{
    return this->line_;
}

Drcctprof__Profile__Line *
Profile::line_t::encode()
{
    this->buffer_.function_id = this->func_->get_id();
    this->buffer_.line = this->line_;
    return &(this->buffer_);
}

/*
 * Profile::location_t
 */

Profile::location_t::location_t(uint64_t id)
    : id_(id)
{
    this->line_ = new std::vector<Profile::line_t *>();
    this->buffer_ = DRCCTPROF__PROFILE__LOCATION__INIT;
}

Profile::location_t::~location_t()
{
    std::for_each(std::begin(*this->line_), std::end(*this->line_),
                  [](Profile::line_t *l) { delete l; });
    delete this->line_;
    free(this->buffer_.line);
}

void
Profile::location_t::append_line(Profile::line_t *line)
{
    (*this->line_).push_back(line);
}

uint64_t
Profile::location_t::get_id()
{
    return this->id_;
}

Profile::line_t *
Profile::location_t::get_leaf_line()
{
    return (*this->line_)[0];
}

Drcctprof__Profile__Location *
Profile::location_t::encode()
{
    this->buffer_.id = this->id_;
    this->buffer_.n_line = (*this->line_).size();
    if (this->buffer_.line) {
        free(this->buffer_.line);
    }
    this->buffer_.line = (Drcctprof__Profile__Line **)malloc(
        this->buffer_.n_line * sizeof(Drcctprof__Profile__Line *));
    for (size_t i = 0; i < this->buffer_.n_line; i++) {
        this->buffer_.line[i] = (*this->line_)[i]->encode();
    }
    return &(this->buffer_);
}

/*
 * Profile::context_t
 */

Profile::context_t::context_t(uint64_t id, Profile::location_t *location,
                              Profile::context_t *parent)
    : id_(id)
    , location_(location)
    , parent_(parent)
{
    this->children_ = new std::vector<Profile::context_t *>();
    this->buffer_ = DRCCTPROF__PROFILE__CONTEXT__INIT;
    if (this->parent_) {
        this->parent_->add_child(this);
    }
}

Profile::context_t::~context_t()
{
    delete this->children_;
    free(this->buffer_.children_id);
}

uint64_t
Profile::context_t::get_id()
{
    return this->id_;
}

Profile::location_t *
Profile::context_t::get_location()
{
    return this->location_;
}

void
Profile::context_t::add_child(Profile::context_t *child)
{
    (*this->children_).push_back(child);
}

Drcctprof__Profile__Context *
Profile::context_t::encode()
{
    this->buffer_.id = this->id_;
    this->buffer_.location_id = this->location_->get_id();
    if (this->parent_) {
        this->buffer_.parent_id = this->parent_->get_id();
    } else {
        this->buffer_.parent_id = 0;
    }

    this->buffer_.n_children_id = (*this->children_).size();
    if (this->buffer_.children_id) {
        free(this->buffer_.children_id);
    }
    this->buffer_.children_id =
        (uint64_t *)malloc(this->buffer_.n_children_id * sizeof(uint64_t));
    for (size_t i = 0; i < this->buffer_.n_children_id; i++) {
        this->buffer_.children_id[i] = (*this->children_)[i]->get_id();
    }

    return &(this->buffer_);
}

/*
 * Profile::sample_t
 */

Profile::sample_t::sample_t(profile_t *profile, Profile::context_t *context)
    : context_(context)
{
    this->metric_ = new std::vector<Profile::metric_t *>();
    this->buffer_ = DRCCTPROF__PROFILE__SAMPLE__INIT;
}

Profile::sample_t::~sample_t()
{
    std::for_each(std::begin(*this->metric_), std::end(*this->metric_),
                  [](Profile::metric_t *m) { delete m; });
    delete this->metric_;
    free(this->buffer_.metric);
}

void
Profile::sample_t::append_metirc(Profile::metric_t *metric)
{
    (*this->metric_).push_back(metric);
}

void
Profile::sample_t::append_metirc(int64_t value)
{
    (*this->metric_).push_back(new Profile::metric_t(value));
}

void
Profile::sample_t::append_metirc(uint64_t value)
{
    (*this->metric_).push_back(new Profile::metric_t(value));
}

void
Profile::sample_t::append_metirc(std::string value)
{
    (*this->metric_)
        .push_back(new Profile::metric_t(0, 0, this->profile_->add_string(value)));
}

Drcctprof__Profile__Sample *
Profile::sample_t::encode()
{
    this->buffer_.context_id = this->context_->get_id();
    this->buffer_.n_metric = this->metric_->size();
    if (this->buffer_.metric) {
        free(this->buffer_.metric);
    }
    this->buffer_.metric = (Drcctprof__Profile__Metric **)malloc(
        this->buffer_.n_metric * sizeof(Drcctprof__Profile__Metric *));
    for (size_t i = 0; i < this->buffer_.n_metric; i++) {
        this->buffer_.metric[i] = (*this->metric_)[i]->encode();
    }
    return &(this->buffer_);
}

/*
 * Profile::profile_t
 */

Profile::profile_t::profile_t()
    : context_max_id_(0)
    , location_max_id_(0)
    , func_max_id_(0)
{
    this->string_table_ = new Profile::string_table_t();
    this->context_map_ = new std::map<uint64_t, Profile::context_t *>();
    this->location_map_ =
        new std::map<Profile::location_map_key_t, Profile::location_t *>();
    this->func_map_ = new std::map<Profile::function_map_key_t, Profile::function_t *>();
    this->source_file_map_ = new std::map<int64_t, Profile::source_file_t *>();

    this->sample_list_ = new std::vector<Profile::sample_t *>();
    this->metric_type_list_ = new std::vector<Profile::metric_type_t *>();

    this->buffer_ = DRCCTPROF__PROFILE__PROFILE__INIT;
}

Profile::profile_t::~profile_t()
{
    std::for_each(std::begin(*this->metric_type_list_),
                  std::end(*this->metric_type_list_),
                  [](Profile::metric_type_t *mt) { delete mt; });
    delete this->metric_type_list_;
    free(this->buffer_.metric_type);

    std::for_each(std::begin(*this->sample_list_), std::end(*this->sample_list_),
                  [](Profile::sample_t *sm) { delete sm; });
    delete this->sample_list_;
    free(this->buffer_.sample);

    for (auto &c : (*this->context_map_)) {
        delete c.second;
    }
    delete this->context_map_;
    free(this->buffer_.context);

    for (auto &l : (*this->location_map_)) {
        delete l.second;
    }
    delete this->location_map_;
    free(this->buffer_.location);

    for (auto &f : (*this->func_map_)) {
        delete f.second;
    }
    delete this->func_map_;
    free(this->buffer_.function);

    for (auto &sf : (*this->source_file_map_)) {
        delete sf.second;
    }
    delete this->source_file_map_;
    free(this->buffer_.source_file);

    delete this->string_table_;
}

Drcctprof__Profile__Profile *
Profile::profile_t::encode()
{
    this->buffer_.n_metric_type = (*this->metric_type_list_).size();
    this->buffer_.metric_type = (Drcctprof__Profile__MetricType **)malloc(
        sizeof(Drcctprof__Profile__MetricType *) * this->buffer_.n_metric_type);
    for (size_t i = 0; i < this->buffer_.n_metric_type; i++) {
        this->buffer_.metric_type[i] = (*this->metric_type_list_)[i]->encode();
    }

    this->buffer_.n_sample = (*this->sample_list_).size();
    this->buffer_.sample = (Drcctprof__Profile__Sample **)malloc(
        sizeof(Drcctprof__Profile__Sample *) * this->buffer_.n_sample);
    for (size_t i = 0; i < this->buffer_.n_sample; i++) {
        this->buffer_.sample[i] = (*this->sample_list_)[i]->encode();
    }

    this->buffer_.n_context = (*this->context_map_).size();
    this->buffer_.context = (Drcctprof__Profile__Context **)malloc(
        sizeof(Drcctprof__Profile__Context *) * this->buffer_.n_context);
    uint64_t idx = 0;
    for (auto &c : (*this->context_map_)) {
        this->buffer_.context[idx] = c.second->encode();
        idx++;
    }

    this->buffer_.n_location = (*this->location_map_).size();
    this->buffer_.location = (Drcctprof__Profile__Location **)malloc(
        sizeof(Drcctprof__Profile__Location *) * this->buffer_.n_location);
    idx = 0;
    for (auto &l : (*this->location_map_)) {
        this->buffer_.location[idx] = l.second->encode();
        idx++;
    }

    this->buffer_.n_function = (*this->func_map_).size();
    this->buffer_.function = (Drcctprof__Profile__Function **)malloc(
        sizeof(Drcctprof__Profile__Function *) * this->buffer_.n_function);
    idx = 0;
    for (auto &f : (*this->func_map_)) {
        this->buffer_.function[idx] = f.second->encode();
        idx++;
    }

    this->buffer_.n_source_file = (*this->source_file_map_).size();
    this->buffer_.source_file = (Drcctprof__Profile__SourceFile **)malloc(
        sizeof(Drcctprof__Profile__SourceFile *) * this->buffer_.n_source_file);
    idx = 0;
    for (auto &sf : (*this->source_file_map_)) {
        this->buffer_.source_file[idx] = sf.second->encode();
        idx++;
    }

    this->buffer_.n_string_table = this->string_table_->get_table_size();
    this->buffer_.string_table = this->string_table_->encode();
    return &(this->buffer_);
}

void
Profile::profile_t::serialize_to_file(const char *file_name)
{
    this->encode();
    size_t len = drcctprof__profile__profile__get_packed_size(&(this->buffer_));
    uint8_t *buf = (uint8_t *)malloc(len);
    drcctprof__profile__profile__pack(&(this->buffer_), buf);
    FILE *p_file;
    p_file = fopen(file_name, "wb");
    fwrite(buf, len, 1, p_file);
    fclose(p_file);
    free(buf);
}

int
Profile::profile_t::add_metric_type(int64_t value_type, int64_t unit, int64_t des)
{
    Profile::metric_type_t *metric_type =
        new Profile::metric_type_t(value_type, unit, des);
    (*this->metric_type_list_).push_back(metric_type);
    return (*this->metric_type_list_).size() - 1;
}

int
Profile::profile_t::add_metric_type(int64_t value_type, std::string unit, std::string des)
{
    Profile::metric_type_t *metric_type = new Profile::metric_type_t(
        value_type, this->add_string(unit), this->add_string(des));
    (*this->metric_type_list_).push_back(metric_type);
    return (*this->metric_type_list_).size() - 1;
}

Profile::string_table_t *
Profile::profile_t::get_string_table()
{
    return this->string_table_;
}

int64_t
Profile::profile_t::add_string(std::string str)
{
    return this->string_table_->add_string(str);
}

std::map<int64_t, Profile::source_file_t *> *
Profile::profile_t::get_source_file_map()
{
    return this->source_file_map_;
}

Profile::source_file_t *
Profile::profile_t::add_source_file(int64_t location_path_idx, int64_t type)
{
    std::map<int64_t, Profile::source_file_t *>::iterator it =
        (*this->source_file_map_).find(location_path_idx);
    if (it != (*this->source_file_map_).end()) {
        return it->second;
    }
    Profile::source_file_t *source_file = new Profile::source_file_t(
        location_path_idx, location_path_idx, location_path_idx, type);
    (*this->source_file_map_)
        .insert(
            std::pair<int64_t, Profile::source_file_t *>(location_path_idx, source_file));
    return source_file;
}

std::map<Profile::function_map_key_t, Profile::function_t *> *
Profile::profile_t::get_func_map()
{
    return this->func_map_;
}

Profile::function_t *
Profile::profile_t::add_function(Profile::source_file_t *source_file,
                                 int64_t functionname_str_idx, int64_t line_no)
{
    int64_t sourcefile_str_idx = source_file->get_location_path();
    Profile::function_map_key_t key = { sourcefile_str_idx, functionname_str_idx };
    std::map<Profile::function_map_key_t, Profile::function_t *>::iterator it =
        (*this->func_map_).find(key);
    if (it != (*this->func_map_).end()) {
        it->second->set_start_line(line_no);
        return it->second;
    }
    Profile::function_t *func = new Profile::function_t(this->func_max_id_++, key.name_,
                                                        key.name_, line_no, source_file);
    (*this->func_map_)
        .insert(std::pair<Profile::function_map_key_t, Profile::function_t *>(key, func));
    return func;
}

Profile::location_t *
Profile::profile_t::add_location(Profile::function_t *func, int64_t line_no)
{
    Profile::location_map_key_t key = { func->get_id(), line_no };
    std::map<Profile::location_map_key_t, Profile::location_t *>::iterator it =
        (*this->location_map_).find(key);
    if (it != (*this->location_map_).end()) {
        return it->second;
    }
    Profile::location_t *location = new Profile::location_t(this->location_max_id_++);
    location->append_line(new Profile::line_t(func, line_no));
    (*this->location_map_)
        .insert(
            std::pair<Profile::location_map_key_t, Profile::location_t *>(key, location));
    return location;
}

Profile::context_t *
Profile::profile_t::add_context(Profile::location_t *location, Profile::context_t *parent)
{
    Profile::context_t *context =
        new Profile::context_t(this->context_max_id_++, location, parent);
    (*this->context_map_)
        .insert(std::pair<uint64_t, Profile::context_t *>(context->get_id(), context));
    if (parent) {
        parent->add_child(context);
    }
    // LOG(ERROR) << this->get_string_table()->get_string(
    //                   location->get_leaf_line()->get_function()->get_name())
    //            << ":L" << location->get_leaf_line()->get_line()
    //            << " "<< this->get_string_table()->get_string(location->get_leaf_line()
    //                                                        ->get_function()
    //                                                        ->get_source_file()
    //                                                        ->get_location_path());
    return context;
}

Profile::metric_type_t *
Profile::profile_t::get_metric_type(int index)
{
    return (*this->metric_type_list_)[index];
}

Profile::function_t *
Profile::profile_t::get_parent_ctxt_func(Profile::context_t *ctxt)
{
    return ctxt->get_location()->get_leaf_line()->get_function();
}

Profile::sample_t *
Profile::profile_t::add_sample(Profile::context_t *ctxt,
                               std::vector<uint64_t> &metric_values)
{
    Profile::sample_t *sample = new Profile::sample_t(this, ctxt);
    for (auto &metric_value : metric_values) {
        sample->append_metirc(metric_value);
    }
    (*this->sample_list_).push_back(sample);
    return sample;
}
