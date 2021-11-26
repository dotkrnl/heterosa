#!/usr/bin/env python3
import sys
import subprocess
import os
import time


def exec_sys_cmd(cmd):
    p = subprocess.Popen(cmd, shell=True)
    ret = p.wait()
    return ret


def main():
    # Some default values
    output_dir = "./heterosa.out"
    src_file_prefix = "kernel"
    xilinx_host = "opencl"
    search = False

    # Parse and update the arguments
    n_arg = len(sys.argv)
    argv = sys.argv
    argv[0] = "heterosa_cc"
    search_idx = -1
    for i in range(n_arg):
        arg = argv[i]
        if "--output-dir" in arg:
            output_dir = arg.split("=")[-1]
        if "--search" in arg:
            search = True
            search_idx = i
    if n_arg > 1:
        src_file = argv[1]
        src_file_prefix = os.path.basename(src_file).split(".")[0]
    if search:
        del argv[search_idx]

    os.makedirs(output_dir + "/src", exist_ok=True)
    os.makedirs(output_dir + "/latency_est", exist_ok=True)
    os.makedirs(output_dir + "/resource_est", exist_ok=True)
    os.makedirs(output_dir + "/tuning", exist_ok=True)

    # Execute the AutoSA
    print("Executing AutoSA... Command: ", " ".join(argv))
    process = subprocess.run(argv)
    if process.returncode != 0 or not os.path.exists(output_dir + "/src/completed"):
        print("[AutoSA] Error: Exit abnormally!")
        sys.exit(process.returncode)
    exec_sys_cmd(f"rm {output_dir}/src/completed")

    # Generate the top module
    print("[AutoSA] Post-processing the generated code...")
    if not os.path.exists(f"{output_dir}/src/{src_file_prefix}_top_gen.cpp"):
        raise RuntimeError(
            f"{output_dir}/src/{src_file_prefix}_top_gen.cpp not exists."
        )
    cmd = (
        "g++ -o "
        + output_dir
        + "/src/top_gen "
        + output_dir
        + "/src/"
        + src_file_prefix
        + "_top_gen.cpp "
        + "-lisl"
    )
    exec_sys_cmd(cmd)
    my_env = os.environ.copy()
    cwd = os.getcwd()
    cmd = output_dir + "/src/top_gen"
    process = subprocess.run(cmd.split(), env=my_env)

    if not search:
        # Generate the final code
        cmd = (
            "heterosa_codegen -c "
            + output_dir
            + "/src/top.cpp -d "
            + output_dir
            + "/src/"
            + src_file_prefix
            + "_kernel_modules.cpp -o "
            + output_dir
            + "/src/"
            + src_file_prefix
            + "_kernel.cpp"
        )
        exec_sys_cmd(cmd)

        # Copy the input code to the output directory
        exec_sys_cmd(f"cp {argv[1]} {output_dir}/src/")
        headers = src_file.split(".")
        headers[-1] = "h"
        headers = ".".join(headers)
        if os.path.exists(headers):
            exec_sys_cmd(f"cp {headers} {output_dir}/src/")

        # Clean up the temp files
        exec_sys_cmd(f"rm {output_dir}/src/{src_file_prefix}_kernel.h")
        exec_sys_cmd(f"rm {output_dir}/src/top_gen")
        exec_sys_cmd(f"rm {output_dir}/src/top.cpp")
        exec_sys_cmd(f"rm {output_dir}/src/{src_file_prefix}_top_gen.cpp")
        exec_sys_cmd(f"rm {output_dir}/src/{src_file_prefix}_top_gen.h")
        exec_sys_cmd(f"rm {output_dir}/src/{src_file_prefix}_kernel_modules.cpp")


if __name__ == "__main__":
    main()
