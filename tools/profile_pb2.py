# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: profile.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


DESCRIPTOR = _descriptor.FileDescriptor(
    name='profile.proto',
    package='drcctprof.profile',
    syntax='proto3',
    serialized_options=None,
    create_key=_descriptor._internal_create_key,
    serialized_pb=b'\n\rprofile.proto\x12\x11\x64rcctprof.profile\"\xbd\x02\n\x07Profile\x12\x32\n\x0bmetric_type\x18\x01 \x03(\x0b\x32\x1d.drcctprof.profile.MetricType\x12)\n\x06sample\x18\x02 \x03(\x0b\x32\x19.drcctprof.profile.Sample\x12+\n\x07\x63ontext\x18\x03 \x03(\x0b\x32\x1a.drcctprof.profile.Context\x12-\n\x08location\x18\x04 \x03(\x0b\x32\x1b.drcctprof.profile.Location\x12-\n\x08\x66unction\x18\x05 \x03(\x0b\x32\x1b.drcctprof.profile.Function\x12\x32\n\x0bsource_file\x18\x06 \x03(\x0b\x32\x1d.drcctprof.profile.SourceFile\x12\x14\n\x0cstring_table\x18\x07 \x03(\t\"G\n\x06Sample\x12\x12\n\ncontext_id\x18\x01 \x01(\x04\x12)\n\x06metric\x18\x02 \x03(\x0b\x32\x19.drcctprof.profile.Metric\";\n\nMetricType\x12\x12\n\nvalue_type\x18\x01 \x01(\x03\x12\x0c\n\x04unit\x18\x02 \x01(\x03\x12\x0b\n\x03\x64\x65s\x18\x03 \x01(\x03\"B\n\x06Metric\x12\x11\n\tint_value\x18\x01 \x01(\x03\x12\x12\n\nuint_value\x18\x02 \x01(\x04\x12\x11\n\tstr_value\x18\x03 \x01(\x03\"R\n\x07\x43ontext\x12\n\n\x02id\x18\x01 \x01(\x04\x12\x13\n\x0blocation_id\x18\x02 \x01(\x04\x12\x11\n\tparent_id\x18\x03 \x01(\x04\x12\x13\n\x0b\x63hildren_id\x18\x04 \x03(\x04\"=\n\x08Location\x12\n\n\x02id\x18\x01 \x01(\x04\x12%\n\x04line\x18\x02 \x03(\x0b\x32\x17.drcctprof.profile.Line\")\n\x04Line\x12\x13\n\x0b\x66unction_id\x18\x01 \x01(\x04\x12\x0c\n\x04line\x18\x02 \x01(\x03\"e\n\x08\x46unction\x12\n\n\x02id\x18\x01 \x01(\x04\x12\x0c\n\x04name\x18\x02 \x01(\x03\x12\x13\n\x0bsystem_name\x18\x03 \x01(\x03\x12\x16\n\x0esource_file_id\x18\x04 \x01(\x04\x12\x12\n\nstart_line\x18\x05 \x01(\x03\"O\n\nSourceFile\x12\n\n\x02id\x18\x01 \x01(\x04\x12\x10\n\x08\x66ilename\x18\x02 \x01(\x03\x12\x15\n\rlocation_path\x18\x03 \x01(\x03\x12\x0c\n\x04type\x18\x04 \x01(\x03\x62\x06proto3'
)


_PROFILE = _descriptor.Descriptor(
    name='Profile',
    full_name='drcctprof.profile.Profile',
    filename=None,
    file=DESCRIPTOR,
    containing_type=None,
    create_key=_descriptor._internal_create_key,
    fields=[
        _descriptor.FieldDescriptor(
            name='metric_type', full_name='drcctprof.profile.Profile.metric_type', index=0,
            number=1, type=11, cpp_type=10, label=3,
            has_default_value=False, default_value=[],
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='sample', full_name='drcctprof.profile.Profile.sample', index=1,
            number=2, type=11, cpp_type=10, label=3,
            has_default_value=False, default_value=[],
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='context', full_name='drcctprof.profile.Profile.context', index=2,
            number=3, type=11, cpp_type=10, label=3,
            has_default_value=False, default_value=[],
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='location', full_name='drcctprof.profile.Profile.location', index=3,
            number=4, type=11, cpp_type=10, label=3,
            has_default_value=False, default_value=[],
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='function', full_name='drcctprof.profile.Profile.function', index=4,
            number=5, type=11, cpp_type=10, label=3,
            has_default_value=False, default_value=[],
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='source_file', full_name='drcctprof.profile.Profile.source_file', index=5,
            number=6, type=11, cpp_type=10, label=3,
            has_default_value=False, default_value=[],
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='string_table', full_name='drcctprof.profile.Profile.string_table', index=6,
            number=7, type=9, cpp_type=9, label=3,
            has_default_value=False, default_value=[],
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    ],
    extensions=[
    ],
    nested_types=[],
    enum_types=[
    ],
    serialized_options=None,
    is_extendable=False,
    syntax='proto3',
    extension_ranges=[],
    oneofs=[
    ],
    serialized_start=37,
    serialized_end=354,
)


_SAMPLE = _descriptor.Descriptor(
    name='Sample',
    full_name='drcctprof.profile.Sample',
    filename=None,
    file=DESCRIPTOR,
    containing_type=None,
    create_key=_descriptor._internal_create_key,
    fields=[
        _descriptor.FieldDescriptor(
            name='context_id', full_name='drcctprof.profile.Sample.context_id', index=0,
            number=1, type=4, cpp_type=4, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='metric', full_name='drcctprof.profile.Sample.metric', index=1,
            number=2, type=11, cpp_type=10, label=3,
            has_default_value=False, default_value=[],
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    ],
    extensions=[
    ],
    nested_types=[],
    enum_types=[
    ],
    serialized_options=None,
    is_extendable=False,
    syntax='proto3',
    extension_ranges=[],
    oneofs=[
    ],
    serialized_start=356,
    serialized_end=427,
)


_METRICTYPE = _descriptor.Descriptor(
    name='MetricType',
    full_name='drcctprof.profile.MetricType',
    filename=None,
    file=DESCRIPTOR,
    containing_type=None,
    create_key=_descriptor._internal_create_key,
    fields=[
        _descriptor.FieldDescriptor(
            name='value_type', full_name='drcctprof.profile.MetricType.value_type', index=0,
            number=1, type=3, cpp_type=2, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='unit', full_name='drcctprof.profile.MetricType.unit', index=1,
            number=2, type=3, cpp_type=2, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='des', full_name='drcctprof.profile.MetricType.des', index=2,
            number=3, type=3, cpp_type=2, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    ],
    extensions=[
    ],
    nested_types=[],
    enum_types=[
    ],
    serialized_options=None,
    is_extendable=False,
    syntax='proto3',
    extension_ranges=[],
    oneofs=[
    ],
    serialized_start=429,
    serialized_end=488,
)


_METRIC = _descriptor.Descriptor(
    name='Metric',
    full_name='drcctprof.profile.Metric',
    filename=None,
    file=DESCRIPTOR,
    containing_type=None,
    create_key=_descriptor._internal_create_key,
    fields=[
        _descriptor.FieldDescriptor(
            name='int_value', full_name='drcctprof.profile.Metric.int_value', index=0,
            number=1, type=3, cpp_type=2, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='uint_value', full_name='drcctprof.profile.Metric.uint_value', index=1,
            number=2, type=4, cpp_type=4, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='str_value', full_name='drcctprof.profile.Metric.str_value', index=2,
            number=3, type=3, cpp_type=2, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    ],
    extensions=[
    ],
    nested_types=[],
    enum_types=[
    ],
    serialized_options=None,
    is_extendable=False,
    syntax='proto3',
    extension_ranges=[],
    oneofs=[
    ],
    serialized_start=490,
    serialized_end=556,
)


_CONTEXT = _descriptor.Descriptor(
    name='Context',
    full_name='drcctprof.profile.Context',
    filename=None,
    file=DESCRIPTOR,
    containing_type=None,
    create_key=_descriptor._internal_create_key,
    fields=[
        _descriptor.FieldDescriptor(
            name='id', full_name='drcctprof.profile.Context.id', index=0,
            number=1, type=4, cpp_type=4, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='location_id', full_name='drcctprof.profile.Context.location_id', index=1,
            number=2, type=4, cpp_type=4, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='parent_id', full_name='drcctprof.profile.Context.parent_id', index=2,
            number=3, type=4, cpp_type=4, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='children_id', full_name='drcctprof.profile.Context.children_id', index=3,
            number=4, type=4, cpp_type=4, label=3,
            has_default_value=False, default_value=[],
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    ],
    extensions=[
    ],
    nested_types=[],
    enum_types=[
    ],
    serialized_options=None,
    is_extendable=False,
    syntax='proto3',
    extension_ranges=[],
    oneofs=[
    ],
    serialized_start=558,
    serialized_end=640,
)


_LOCATION = _descriptor.Descriptor(
    name='Location',
    full_name='drcctprof.profile.Location',
    filename=None,
    file=DESCRIPTOR,
    containing_type=None,
    create_key=_descriptor._internal_create_key,
    fields=[
        _descriptor.FieldDescriptor(
            name='id', full_name='drcctprof.profile.Location.id', index=0,
            number=1, type=4, cpp_type=4, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='line', full_name='drcctprof.profile.Location.line', index=1,
            number=2, type=11, cpp_type=10, label=3,
            has_default_value=False, default_value=[],
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    ],
    extensions=[
    ],
    nested_types=[],
    enum_types=[
    ],
    serialized_options=None,
    is_extendable=False,
    syntax='proto3',
    extension_ranges=[],
    oneofs=[
    ],
    serialized_start=642,
    serialized_end=703,
)


_LINE = _descriptor.Descriptor(
    name='Line',
    full_name='drcctprof.profile.Line',
    filename=None,
    file=DESCRIPTOR,
    containing_type=None,
    create_key=_descriptor._internal_create_key,
    fields=[
        _descriptor.FieldDescriptor(
            name='function_id', full_name='drcctprof.profile.Line.function_id', index=0,
            number=1, type=4, cpp_type=4, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='line', full_name='drcctprof.profile.Line.line', index=1,
            number=2, type=3, cpp_type=2, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    ],
    extensions=[
    ],
    nested_types=[],
    enum_types=[
    ],
    serialized_options=None,
    is_extendable=False,
    syntax='proto3',
    extension_ranges=[],
    oneofs=[
    ],
    serialized_start=705,
    serialized_end=746,
)


_FUNCTION = _descriptor.Descriptor(
    name='Function',
    full_name='drcctprof.profile.Function',
    filename=None,
    file=DESCRIPTOR,
    containing_type=None,
    create_key=_descriptor._internal_create_key,
    fields=[
        _descriptor.FieldDescriptor(
            name='id', full_name='drcctprof.profile.Function.id', index=0,
            number=1, type=4, cpp_type=4, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='name', full_name='drcctprof.profile.Function.name', index=1,
            number=2, type=3, cpp_type=2, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='system_name', full_name='drcctprof.profile.Function.system_name', index=2,
            number=3, type=3, cpp_type=2, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='source_file_id', full_name='drcctprof.profile.Function.source_file_id', index=3,
            number=4, type=4, cpp_type=4, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='start_line', full_name='drcctprof.profile.Function.start_line', index=4,
            number=5, type=3, cpp_type=2, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    ],
    extensions=[
    ],
    nested_types=[],
    enum_types=[
    ],
    serialized_options=None,
    is_extendable=False,
    syntax='proto3',
    extension_ranges=[],
    oneofs=[
    ],
    serialized_start=748,
    serialized_end=849,
)


_SOURCEFILE = _descriptor.Descriptor(
    name='SourceFile',
    full_name='drcctprof.profile.SourceFile',
    filename=None,
    file=DESCRIPTOR,
    containing_type=None,
    create_key=_descriptor._internal_create_key,
    fields=[
        _descriptor.FieldDescriptor(
            name='id', full_name='drcctprof.profile.SourceFile.id', index=0,
            number=1, type=4, cpp_type=4, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='filename', full_name='drcctprof.profile.SourceFile.filename', index=1,
            number=2, type=3, cpp_type=2, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='location_path', full_name='drcctprof.profile.SourceFile.location_path', index=2,
            number=3, type=3, cpp_type=2, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
        _descriptor.FieldDescriptor(
            name='type', full_name='drcctprof.profile.SourceFile.type', index=3,
            number=4, type=3, cpp_type=2, label=1,
            has_default_value=False, default_value=0,
            message_type=None, enum_type=None, containing_type=None,
            is_extension=False, extension_scope=None,
            serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    ],
    extensions=[
    ],
    nested_types=[],
    enum_types=[
    ],
    serialized_options=None,
    is_extendable=False,
    syntax='proto3',
    extension_ranges=[],
    oneofs=[
    ],
    serialized_start=851,
    serialized_end=930,
)

_PROFILE.fields_by_name['metric_type'].message_type = _METRICTYPE
_PROFILE.fields_by_name['sample'].message_type = _SAMPLE
_PROFILE.fields_by_name['context'].message_type = _CONTEXT
_PROFILE.fields_by_name['location'].message_type = _LOCATION
_PROFILE.fields_by_name['function'].message_type = _FUNCTION
_PROFILE.fields_by_name['source_file'].message_type = _SOURCEFILE
_SAMPLE.fields_by_name['metric'].message_type = _METRIC
_LOCATION.fields_by_name['line'].message_type = _LINE
DESCRIPTOR.message_types_by_name['Profile'] = _PROFILE
DESCRIPTOR.message_types_by_name['Sample'] = _SAMPLE
DESCRIPTOR.message_types_by_name['MetricType'] = _METRICTYPE
DESCRIPTOR.message_types_by_name['Metric'] = _METRIC
DESCRIPTOR.message_types_by_name['Context'] = _CONTEXT
DESCRIPTOR.message_types_by_name['Location'] = _LOCATION
DESCRIPTOR.message_types_by_name['Line'] = _LINE
DESCRIPTOR.message_types_by_name['Function'] = _FUNCTION
DESCRIPTOR.message_types_by_name['SourceFile'] = _SOURCEFILE
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

Profile = _reflection.GeneratedProtocolMessageType('Profile', (_message.Message,), {
    'DESCRIPTOR': _PROFILE,
    '__module__': 'profile_pb2'
    # @@protoc_insertion_point(class_scope:drcctprof.profile.Profile)
})
_sym_db.RegisterMessage(Profile)

Sample = _reflection.GeneratedProtocolMessageType('Sample', (_message.Message,), {
    'DESCRIPTOR': _SAMPLE,
    '__module__': 'profile_pb2'
    # @@protoc_insertion_point(class_scope:drcctprof.profile.Sample)
})
_sym_db.RegisterMessage(Sample)

MetricType = _reflection.GeneratedProtocolMessageType('MetricType', (_message.Message,), {
    'DESCRIPTOR': _METRICTYPE,
    '__module__': 'profile_pb2'
    # @@protoc_insertion_point(class_scope:drcctprof.profile.MetricType)
})
_sym_db.RegisterMessage(MetricType)

Metric = _reflection.GeneratedProtocolMessageType('Metric', (_message.Message,), {
    'DESCRIPTOR': _METRIC,
    '__module__': 'profile_pb2'
    # @@protoc_insertion_point(class_scope:drcctprof.profile.Metric)
})
_sym_db.RegisterMessage(Metric)

Context = _reflection.GeneratedProtocolMessageType('Context', (_message.Message,), {
    'DESCRIPTOR': _CONTEXT,
    '__module__': 'profile_pb2'
    # @@protoc_insertion_point(class_scope:drcctprof.profile.Context)
})
_sym_db.RegisterMessage(Context)

Location = _reflection.GeneratedProtocolMessageType('Location', (_message.Message,), {
    'DESCRIPTOR': _LOCATION,
    '__module__': 'profile_pb2'
    # @@protoc_insertion_point(class_scope:drcctprof.profile.Location)
})
_sym_db.RegisterMessage(Location)

Line = _reflection.GeneratedProtocolMessageType('Line', (_message.Message,), {
    'DESCRIPTOR': _LINE,
    '__module__': 'profile_pb2'
    # @@protoc_insertion_point(class_scope:drcctprof.profile.Line)
})
_sym_db.RegisterMessage(Line)

Function = _reflection.GeneratedProtocolMessageType('Function', (_message.Message,), {
    'DESCRIPTOR': _FUNCTION,
    '__module__': 'profile_pb2'
    # @@protoc_insertion_point(class_scope:drcctprof.profile.Function)
})
_sym_db.RegisterMessage(Function)

SourceFile = _reflection.GeneratedProtocolMessageType('SourceFile', (_message.Message,), {
    'DESCRIPTOR': _SOURCEFILE,
    '__module__': 'profile_pb2'
    # @@protoc_insertion_point(class_scope:drcctprof.profile.SourceFile)
})
_sym_db.RegisterMessage(SourceFile)


# @@protoc_insertion_point(module_scope)
