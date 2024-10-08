#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import os, sys
from glob import glob
from re import sub
import argparse


def escape(s):
    s = s.decode()
    lookup = {
        "\0": "\\0",
        "\t": "\\t",
        "\n": '\\n"\n"',
        "\r": "\\r",
        "\\": "\\\\",
        '"': '\\"',
    }
    return '""\n"{}"'.format("".join([lookup[x] if x in lookup else x for x in s]))


def chew_filename(t):
    return {"func": "test_{}_fn".format(sub(r"/|\.|-", "_", t)), "desc": t}


def script_to_map(test_file):
    r = {"name": chew_filename(test_file)["func"]}
    with open(test_file, "rb") as test:
        script = test.readlines()

    # Test for import skip_if and inject it into the test as needed.
    if "import skip_if\n" in script:
        index = script.index("import skip_if\n")
        script.pop(index)
        script.insert(index, "class skip_if:\n")
        with open("../tests/skip_if.py") as skip_if:
            total_lines = 1
            for line in skip_if:
                stripped = line.strip()
                if not stripped or stripped.startswith(("#", '"""')):
                    continue
                script.insert(index + total_lines, "\t" + line)
                total_lines += 1
    r["script"] = escape(b"".join(script))

    with open(test_file + ".exp", "rb") as f:
        r["output"] = escape(f.read())

    return r


def load_profile(profile_file, test_dirs, exclude_tests):
    profile_globals = {"test_dirs": test_dirs, "exclude_tests": exclude_tests}
    exec(profile_file.read(), profile_globals)
    return profile_globals["test_dirs"], profile_globals["exclude_tests"]


test_function = (
    "void {name}(void* data) {{\n"
    "  static const char pystr[] = {script};\n"
    "  static const char exp[] = {output};\n"
    '  printf("\\n");\n'
    "  upytest_set_expected_output(exp, sizeof(exp) - 1);\n"
    "  upytest_execute_test(pystr);\n"
    '  printf("result: ");\n'
    "}}"
)

testcase_struct = "struct testcase_t {name}_tests[] = {{\n{body}\n  END_OF_TESTCASES\n}};"
testcase_member = '  {{ "{desc}", {func}, TT_ENABLED_, 0, 0 }},'

testgroup_struct = "struct testgroup_t groups[] = {{\n{body}\n  END_OF_GROUPS\n}};"
testgroup_member = '  {{ "{name}", {name}_tests }},'

## XXX: may be we could have `--without <groups>` argument...

test_dirs = set(
    (
        "basics",
        "extmod",
        "float",
        "micropython",
        "misc",
    )
)

exclude_tests = set(
    (
        # pattern matching in .exp
        "basics/bytes_compare3.py",
        "extmod/ticks_diff.py",
        "extmod/time_ms_us.py",
        # unicode char issue
        "extmod/json_loads.py",
        # doesn't output to python stdout
        "extmod/re_debug.py",
        "extmod/vfs_basic.py",
        "extmod/vfs_fat_ramdisk.py",
        "extmod/vfs_fat_fileio.py",
        "extmod/vfs_fat_fsusermount.py",
        "extmod/vfs_fat_oldproto.py",
        # rounding issues
        "float/float_divmod.py",
        # requires double precision floating point to work
        "float/float2int_doubleprec_intbig.py",
        "float/float_format_ints_doubleprec.py",
        "float/float_parse_doubleprec.py",
        # different filename in output
        "micropython/emg_exc.py",
        "micropython/heapalloc_traceback.py",
        # don't have emergency exception buffer
        "micropython/heapalloc_exc_compressed_emg_exc.py",
        # pattern matching in .exp
        "micropython/meminfo.py",
        # needs sys stdfiles
        "misc/print_exception.py",
        # settrace .exp files are too large
        "misc/sys_settrace_loop.py",
        "misc/sys_settrace_generator.py",
        "misc/sys_settrace_features.py",
        # don't have f-string
        "basics/string_fstring.py",
        "basics/string_fstring_debug.py",
    )
)

output = []
tests = []

argparser = argparse.ArgumentParser(
    description="Convert native MicroPython tests to tinytest/upytesthelper C code"
)
argparser.add_argument("--stdin", action="store_true", help="read list of tests from stdin")
argparser.add_argument("--exclude", action="append", help="exclude test by name")
argparser.add_argument(
    "--profile",
    type=argparse.FileType("rt", encoding="utf-8"),
    help="optional profile file providing test directories and exclusion list",
)
args = argparser.parse_args()

if not args.stdin:
    if args.profile:
        test_dirs, exclude_tests = load_profile(args.profile, test_dirs, exclude_tests)
    if args.exclude:
        exclude_tests = exclude_tests.union(args.exclude)
    for group in test_dirs:
        tests += [test for test in glob("{}/*.py".format(group)) if test not in exclude_tests]
else:
    for l in sys.stdin:
        tests.append(l.rstrip())

output.extend([test_function.format(**script_to_map(test)) for test in tests])
testcase_members = [testcase_member.format(**chew_filename(test)) for test in tests]
output.append(testcase_struct.format(name="", body="\n".join(testcase_members)))

testgroup_members = [testgroup_member.format(name=group) for group in [""]]

output.append(testgroup_struct.format(body="\n".join(testgroup_members)))

## XXX: may be we could have `--output <filename>` argument...
# Don't depend on what system locale is set, use utf8 encoding.
sys.stdout.buffer.write("\n\n".join(output).encode("utf8"))
