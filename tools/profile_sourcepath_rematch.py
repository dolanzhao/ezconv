import os
import profile_pb2
from google.protobuf import json_format
import json

from absl import app
from absl import flags


FLAGS = flags.FLAGS

flags.DEFINE_string('protobuf_file_path', "",
                    'The path of the protobuf file')
flags.DEFINE_string('hpctoolkit_db_folder_path', "",
                    'The path of the hpctoolkit db folder')
flags.DEFINE_string('exe_source_folder_path', "",
                    'The path of the exe source folder')


_exe_source_folder_files = []


def search_all_files(folder_path: str, store_list: list):
    search_path = os.path.join(folder_path, "src")
    for dirpath, dirnames, filenames in os.walk(search_path):
        for filename in filenames:
            full_path = os.path.join(dirpath, filename)
            # print(full_path)
            store_list.append(full_path)
    pass


def replace_with_original_path(path: str):
    for file_path in _exe_source_folder_files:
        if file_path in path:
            return file_path
    return path


def search_best_map_file(un_mapped_file: str):
    for file_path in _exe_source_folder_files:
        if file_path.endswith(un_mapped_file):
            return file_path
    return un_mapped_file


def rematch_source_file(profile: profile_pb2.Profile):
    json_profile = json.loads(json_format.MessageToJson(profile))
    for source_file in json_profile["sourceFile"]:
        if "locationPath" in source_file:
            path = json_profile["stringTable"][int(
                source_file["locationPath"])]
            if path.startswith("src/"):
                if FLAGS.exe_source_folder_path:
                    full_path = replace_with_original_path(path)
                else:
                    full_path = os.path.join(
                        FLAGS.hpctoolkit_db_folder_path, path)
            else:
                full_path = path
                if FLAGS.exe_source_folder_path:
                    full_path = search_best_map_file(full_path)

            json_profile["stringTable"][int(
                source_file["locationPath"])] = full_path

    rematched_profile = profile_pb2.Profile()
    json_format.Parse(json.dumps(json_profile), rematched_profile)
    with open(FLAGS.protobuf_file_path, 'wb') as fw:
        fw.write(rematched_profile.SerializeToString())
    pass


def main(argv):
    if not FLAGS.protobuf_file_path:
        raise ValueError(
            'You must supply the protobuf file path with --protobuf_file_path')
    if not FLAGS.hpctoolkit_db_folder_path:
        raise ValueError(
            'You must supply the hpctoolkit db folder path with --hpctoolkit_db_folder_path')
    if FLAGS.exe_source_folder_path:
        search_all_files(FLAGS.exe_source_folder_path,
                         _exe_source_folder_files)

    profile = profile_pb2.Profile()
    with open(FLAGS.protobuf_file_path, 'rb') as f:
        buf = f.read()
        profile.ParseFromString(buf)
    rematch_source_file(profile)
    pass


if __name__ == '__main__':
    app.run(main)
