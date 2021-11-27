#!/usr/bin/env python3
import argparse
import shutil
import os
import sys
import subprocess
from . import codegen


def main():
    parser = argparse.ArgumentParser(description="==== HeteroSA ====")
    parser.add_argument("src_file")
    parser.add_argument(
        "--output-dir",
        metavar="OUTPUT_DIR",
        required=False,
        default="./heterosa.out",
        help="output directory",
    )
    parser.add_argument("--search", default=False, action="store_true")
    parser.add_argument("--quiet", default=False, action="store_true")
    args, _ = parser.parse_known_args()

    # Some default values
    output_dir = args.output_dir
    search = args.search
    src_file = args.src_file
    src_file_prefix = os.path.basename(src_file).split(".")[0]

    # Arguments for heterosa_cc
    argv = sys.argv
    argv[0] = "heterosa_cc"
    if "--search" in argv:
        argv.remove("--search")
    if "--quiet" in argv:
        argv.remove("--quiet")

    os.makedirs(f"{output_dir}/src", exist_ok=True)
    os.makedirs(f"{output_dir}/latency_est", exist_ok=True)
    os.makedirs(f"{output_dir}/resource_est", exist_ok=True)
    os.makedirs(f"{output_dir}/tuning", exist_ok=True)

    # Execute the AutoSA
    if args.quiet:
        process = subprocess.run(argv, stdout=open(os.devnull, "wb"))
    else:
        print("Command: ", " ".join(argv))
        process = subprocess.run(argv)
    if process.returncode != 0 or not os.path.exists(f"{output_dir}/src/completed"):
        print("[AutoSA] Error: Exit abnormally!")
        sys.exit(process.returncode)
    os.remove(f"{output_dir}/src/completed")

    # Generate the top module
    top_gen_code = f"{output_dir}/src/{src_file_prefix}_top_gen.cpp"
    if not args.quiet:
        print("[AutoSA] Post-processing the generated code...")
    if not os.path.exists(top_gen_code):
        raise RuntimeError(f"Top-level code generator not produced.")
    cmd = f"g++ -o {output_dir}/src/top_gen {top_gen_code} -lisl"
    os.system(cmd)
    my_env = os.environ.copy()
    cmd = f"{output_dir}/src/top_gen"
    process = subprocess.run(cmd.split(), env=my_env)

    if not search:
        # Generate the final code
        codegen.codegen_run(
            f"{output_dir}/src/top.cpp",
            f"{output_dir}/src/{src_file_prefix}_kernel_modules.cpp",
            f"{output_dir}/src/{src_file_prefix}_kernel.cpp",
            args.quiet,
        )

        # Copy the headers to the output directory
        headers = src_file.split(".")
        headers[-1] = "h"
        headers = ".".join(headers)
        if os.path.exists(headers):
            shutil.copy(headers, f"{output_dir}/src/")

        # Clean up the temp files
        os.remove(f"{output_dir}/src/top_gen")
        os.remove(f"{output_dir}/src/top.cpp")
        os.remove(f"{output_dir}/src/{src_file_prefix}_top_gen.cpp")
        os.remove(f"{output_dir}/src/{src_file_prefix}_top_gen.h")
        os.remove(f"{output_dir}/src/{src_file_prefix}_kernel_modules.cpp")


if __name__ == "__main__":
    main()
