
#include "default_reader.h"

#define RYML_SINGLE_HDR_DEFINE_NOW
#include "rapidyaml-0.6.0.hpp"

#include "absl/log/log.h"

DefaultReader::DefaultReader(const std::string &file_location)
    : file_location_(file_location)
{
    if (readContent().ok()) {
        LOG(INFO) << "Successfully read content from file";
    } else {
        LOG(FATAL) << "Failed to read content from file";
    }
}

DefaultReader::~DefaultReader()
{
}



absl::Status
DefaultReader::readContent()
{
    std::ifstream file(file_location_);
    if (!file.is_open()) {
        return absl::NotFoundError("Failed to open file");
    }
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    ryml::Tree context_tree = ryml::parse_in_arena(ryml::to_csubstr(content));
    context_tree.resolve();

    LOG(INFO) << "Roots: " << context_tree["roots"].num_children();
    for (auto i = 0; i < context_tree["roots"].num_children(); i++) {
        auto metric = context_tree["roots"][i];
        std::string name;
        metric["name"] >> name;
        std::string desc;
        metric["description"] >> desc;
        metric_desc_map_[name] = desc;
        c4::yml::NodeRef render;
        if (metric["variants"].has_child("Sum")) {
            render = metric["variants"]["Sum"]["render"];
        } else if (metric["variants"].has_child("")) {
            render = metric["variants"][""]["render"];
        } else {
            LOG(WARNING) << "No render found";
        }
        bool has_percent = false;
        for (int j = 0; j < render.num_children(); j++) {
            if (render[j] == "percent") {
                has_percent = true;
                break;
            }
        }
        if (has_percent) {
            if (metric["variants"].has_child("Sum")) {
                c4::yml::NodeRef formula = metric["variants"]["Sum"]["formula"];
                // LOG(ERROR) << "Formula: " << formula;
                if (formula.has_child("exclusive") &&
                    formula["exclusive"]["standard"].has_child("scope")) {
                    std::string scope;
                    formula["exclusive"]["standard"]["scope"] >> scope;
                    metric_exc_type_map_[name] = scope;
                    // LOG(ERROR) << "Exclusive Scope: [" << name << "]" << scope;
                }
            }
        } else {
            LOG(WARNING) << "No percent found";
        }
    }

    return absl::OkStatus();
}
