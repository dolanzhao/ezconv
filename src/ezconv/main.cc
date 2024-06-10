#include <iostream>
#include <string>
#include <vector>
#include <iostream>

#include "default_reader.h"
#include "meta_reader.h"
#include "profile.h"
#include "profile_reader.h"
#include "cct_reader.h"

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/log.h"
#include "absl/log/initialize.h"

ABSL_FLAG(std::string, input, "", "Input file to read");

int
main(int argc, char *argv[])
{
    absl::ParseCommandLine(argc, argv);
    absl::InitializeLog();

    if (absl::GetFlag(FLAGS_input).empty()) {
        LOG(ERROR) << "Input file not specified";
        return 1;
    } else {
        LOG(INFO) << "Input file: " << absl::GetFlag(FLAGS_input);
    }

    Profile::profile_t profile;

    DefaultReader default_reader(absl::GetFlag(FLAGS_input) + "/metrics/default.yaml");
    MetaReader meta_reader(absl::GetFlag(FLAGS_input) + "/meta.db", &profile,
                           &default_reader);
    ProfileReader profile_reader(absl::GetFlag(FLAGS_input) + "/profile.db",
                                 &meta_reader);

    CCTReader cct_reader(absl::GetFlag(FLAGS_input) + "/cct.db", &profile, &meta_reader,
                         &profile_reader);

    std::string output = absl::GetFlag(FLAGS_input) + "/ezview.ezview";
    profile.serialize_to_file(output.c_str());

    return 0;
}