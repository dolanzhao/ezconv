#ifndef EZCONV_PROFILE_H
#define EZCONV_PROFILE_H

#include <cstdint>
#include <vector>
#include <string>
#include <map>

#include "profile.pb-c.h"

namespace Profile {

class string_table_t;
class metric_type_t;
class metric_t;
class source_file_t;
class function_t;
class line_t;
class location_t;
class context_t;
class sample_t;
class profile_t;

typedef struct _function_map_key_t {
    int64_t file_path_;
    int64_t name_;

    bool
    operator==(const struct _function_map_key_t &f) const
    {
        return file_path_ == f.file_path_ && name_ == f.name_;
    }

    bool
    operator<(const struct _function_map_key_t &f) const
    {
        return file_path_ < f.file_path_ ||
            (file_path_ == f.file_path_ && name_ < f.name_);
    }
} function_map_key_t;

typedef struct _location_map_key_t {
    uint64_t func_id_;
    int64_t line_no_;

    bool
    operator==(const struct _location_map_key_t &l) const
    {
        return func_id_ == l.func_id_ && line_no_ == l.line_no_;
    }

    bool
    operator<(const struct _location_map_key_t &l) const
    {
        return func_id_ < l.func_id_ ||
            (func_id_ == l.func_id_ && line_no_ < l.line_no_);
    }
} location_map_key_t;

class string_table_t {
public:
    string_table_t();

    ~string_table_t();

    int64_t
    add_string(std::string str);

    int64_t
    add_string(char *c_str);

    std::string
    get_string(int64_t index);

    size_t
    get_table_size();

    char **
    encode();

private:
    std::map<std::string, int64_t> *table_;
    int64_t max_index_;

    char **buffer_;
    char *table_buffer_;
};

class metric_type_t {
public:
    metric_type_t(int64_t value_type, int64_t unit, int64_t des);
    ~metric_type_t();

    int64_t
    get_des();

    int64_t
    get_unit();

    int64_t
    get_value_type();

    void
    update_uint(int64_t unit);

    Drcctprof__Profile__MetricType *
    encode();

private:
    int64_t value_type_;
    int64_t unit_;
    int64_t des_;

    Drcctprof__Profile__MetricType buffer_;
};

class metric_t {
public:
    metric_t(int64_t int_value);
    metric_t(uint64_t uint_value);
    metric_t(int64_t int_value, uint64_t uint_value, int64_t str_index);
    ~metric_t();

    Drcctprof__Profile__Metric *
    encode();

private:
    int64_t int_value_;
    uint64_t uint_value_;
    int64_t str_index_;

    Drcctprof__Profile__Metric buffer_;
};

class source_file_t {
public:
    source_file_t(uint64_t id, int64_t file_name, int64_t location_path, int64_t type);
    ~source_file_t();

    uint64_t
    get_id();

    int64_t
    get_location_path();

    Drcctprof__Profile__SourceFile *
    encode();

private:
    uint64_t id_;
    int64_t file_name_;
    int64_t location_path_;
    int64_t type_;

    Drcctprof__Profile__SourceFile buffer_;
};

class function_t {
public:
    function_t(uint64_t id, int64_t name, int64_t system_name, int64_t start_line,
               source_file_t *source_file);
    ~function_t();

    uint64_t
    get_id();

    int64_t
    get_name();

    source_file_t *
    get_source_file();

    void
    set_start_line(int64_t line_no);

    Drcctprof__Profile__Function *
    encode();

private:
    uint64_t id_;
    int64_t name_;
    int64_t system_name_;
    int64_t start_line_;
    source_file_t *source_file_;

    Drcctprof__Profile__Function buffer_;
};

class line_t {
public:
    line_t(function_t *func, int64_t line);
    ~line_t();

    function_t *
    get_function();

    int64_t
    get_line();

    Drcctprof__Profile__Line *
    encode();

private:
    function_t *func_;
    int64_t line_;

    Drcctprof__Profile__Line buffer_;
};

class location_t {
public:
    location_t(uint64_t id);
    ~location_t();
    void
    append_line(line_t *line);

    uint64_t
    get_id();

    line_t *
    get_leaf_line();

    Drcctprof__Profile__Location *
    encode();

private:
    uint64_t id_;
    std::vector<line_t *> *line_;

    Drcctprof__Profile__Location buffer_;
};

class context_t {
public:
    context_t(uint64_t id, location_t *location, context_t *parent);
    ~context_t();
    void
    add_child(context_t *child);

    uint64_t
    get_id();

    location_t *
    get_location();

    Drcctprof__Profile__Context *
    encode();

private:
    uint64_t id_;
    location_t *location_;
    context_t *parent_;
    std::vector<context_t *> *children_;

    Drcctprof__Profile__Context buffer_;
};

class sample_t {
public:
    sample_t(profile_t *profile, context_t *context);
    ~sample_t();
    void
    append_metirc(metric_t *metric);

    void
    append_metirc(int64_t value);

    void
    append_metirc(uint64_t value);

    void
    append_metirc(std::string value);

    Drcctprof__Profile__Sample *
    encode();

private:
    context_t *context_;
    std::vector<metric_t *> *metric_;

    profile_t *profile_;
    Drcctprof__Profile__Sample buffer_;
};

class profile_t {
public:
    profile_t();
    ~profile_t();

    Drcctprof__Profile__Profile *
    encode();

    void
    serialize_to_file(const char *file_name);

    int
    add_metric_type(int64_t value_type, int64_t unit, int64_t des);

    int
    add_metric_type(int64_t value_type, std::string unit, std::string des);

    string_table_t *
    get_string_table();

    int64_t
    add_string(std::string str);

    std::map<int64_t, source_file_t *> *
    get_source_file_map();

    source_file_t *
    add_source_file(int64_t location_path_idx, int64_t type);

    std::map<function_map_key_t, function_t *> *
    get_func_map();

    function_t *
    add_function(source_file_t *source_file, int64_t filename_str_idx, int64_t line_no);

    location_t *
    add_location(function_t *func, int64_t line_no);

    context_t *
    add_context(location_t *location, context_t *parent);

    sample_t *
    add_sample(context_t *ctxt, std::vector<uint64_t> &metric_values);

    metric_type_t *
    get_metric_type(int index);

    function_t *
    get_parent_ctxt_func(context_t *ctxt);

private:
    std::vector<metric_type_t *> *metric_type_list_;
    std::vector<sample_t *> *sample_list_;

    uint64_t context_max_id_;
    std::map<uint64_t, context_t *> *context_map_;

    uint64_t location_max_id_;
    std::map<location_map_key_t, location_t *> *location_map_;

    uint64_t func_max_id_;
    std::map<function_map_key_t, function_t *> *func_map_;

    std::map<int64_t, source_file_t *> *source_file_map_;

    string_table_t *string_table_;

    Drcctprof__Profile__Profile buffer_;
};
}

#endif // EZCONV_PROFILE_H
