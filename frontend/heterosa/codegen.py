#!/usr/bin/env python3

import sympy
import sys
import argparse
import re
import numpy as np


def delete_arg_from_arg_list(line, arg, content):
    """Delete the argument from the argument list

    Parameters
    ----------
    line: list
        codeline containing the argument list
    arg: list
        argument to be deleted
    line_id: int
        the current line id
    content: list
        the printed content before current line
    """
    line = line.strip()
    if line[-1] != ",":
        comma_pos = content[-1].find(",")
        content[-1] = content[-1][:comma_pos] + "\n"


def contains_pipeline_for(pos, lines):
    """Examine if there is any for loop with hls_pipeline annotation inside the current for loop"""
    n_l_bracket = 0
    n_r_bracket = 0
    code_len = len(lines)
    init_state = 1
    while pos < code_len and n_r_bracket <= n_l_bracket:
        if lines[pos].find("{") != -1:
            n_l_bracket += 1
        if lines[pos].find("}") != -1:
            n_r_bracket += 1
        if lines[pos].find("for") != -1:
            if init_state:
                init_state = 0
            else:
                if lines[pos + 1].find("hls_pipeline") != -1:
                    return 1
        if n_l_bracket == n_r_bracket and not init_state:
            break
        pos += 1
    return 0


def insert_xlnx_pragmas(lines):
    """Insert HLS pragmas for Xilinx program

    Replace the comments of "// hls_pipeline" and "// hls_unroll" with
    HLS pragmas
    For "// hls pipeline", find the previous for loop before hitting any "}".
    Insert "#pragma HLS PIPELINE II=1" below the for loop.
    For "// hls unroll", find the previous for loop before hitting the "simd" mark.
    Insert "#pragma HLS UNROLL" below the for loop.
    For "// hls_dependence.x", the position is the same with hls_pipeline.
    Insert "#pragma HLS DEPENDENCE variable=x inter false".

    Parameters
    ----------
    lines:
        contains the codelines of the program
    """
    # Handle hls_dependence
    handle_dep_pragma = 1

    code_len = len(lines)
    pos = 0
    while pos < code_len:
        line = lines[pos]
        if line.find("// hls_pipeline") != -1 or line.find("// hls_dependence") != -1:
            is_pipeline = 0
            is_dep = 0
            if line.find("// hls_pipeline") != -1:
                is_pipeline = 1
            else:
                is_dep = 1
            # Find if there is any other hls_pipeline/hls_dependence annotation
            # below
            n_l_bracket = 0
            n_r_bracket = 0
            next_pos = pos + 1
            find_pipeline = 0
            init_state = 1
            while next_pos < code_len and n_r_bracket <= n_l_bracket:
                if is_pipeline and lines[next_pos].find("hls_pipeline") != -1:
                    find_pipeline = 1
                    break
                if (
                    is_dep
                    and lines[next_pos].find("hls_dependence") != -1
                    and handle_dep_pragma
                ):
                    find_pipeline = 1
                    break
                if lines[next_pos].find("{") != -1:
                    n_l_bracket += 1
                    init_state = 0
                if lines[next_pos].find("}") != -1:
                    n_r_bracket += 1
                if n_l_bracket == n_r_bracket and not init_state:
                    break
                next_pos += 1
            if find_pipeline:
                pos += 1
                continue

            # Find the for loop above before hitting any "}"
            prev_pos = pos - 1
            find_for = 0
            n_l_bracket = 0
            n_r_bracket = 0
            while prev_pos >= 0:
                if lines[prev_pos].find("while") != -1:
                    break
                if lines[prev_pos].find("{") != -1:
                    n_l_bracket += 1
                if lines[prev_pos].find("}") != -1:
                    n_r_bracket += 1
                if lines[prev_pos].find("for") != -1:
                    if n_l_bracket > n_r_bracket:
                        # check if the pragma is already inserted
                        if (
                            is_pipeline
                            and lines[prev_pos + 1].find("#pragma HLS PIPELINE II=1\n")
                            == -1
                        ):
                            find_for = 1
                        if (
                            is_dep
                            and lines[prev_pos + 2].find("#pragma HLS DEPENDENCE") == -1
                            and handle_dep_pragma
                        ):
                            find_for = 1
                        # check if there is any other for loop with
                        # hls_pipeline annotation inside
                        if contains_pipeline_for(prev_pos, lines):
                            find_for = 0
                        break
                prev_pos -= 1
            if find_for == 1:
                # insert the pragma right after the for loop
                indent = lines[prev_pos].find("for")
                if line.find("hls_pipeline") != -1:
                    new_line = " " * indent + "#pragma HLS PIPELINE II=1\n"
                else:
                    line_cp = line
                    var_name = line_cp.strip().split(".")[-1]
                    new_line = (
                        " " * indent
                        + "#pragma HLS DEPENDENCE variable="
                        + var_name
                        + " inter false\n"
                    )
                lines.insert(prev_pos + 1, new_line)
                del lines[pos + 1]
        elif line.find("// hls_unroll") != -1:
            # Find the for loop above before hitting any "simd"
            prev_pos = pos - 1
            find_for = 0
            while prev_pos >= 0 and lines[prev_pos].find("simd") == -1:
                if lines[prev_pos].find("for") != -1:
                    find_for = 1
                    break
                prev_pos -= 1
            if find_for == 1:
                # insert the pragma right after the for loop
                indent = lines[prev_pos].find("for")
                new_line = " " * indent + "#pragma HLS UNROLL\n"
                lines.insert(prev_pos + 1, new_line)
                del lines[pos + 1]
        pos = pos + 1

    return lines


def float_to_int(matchobj):
    str_expr = matchobj.group(0)
    if float(str_expr) == int(float(str_expr)):
        return str(int(float(str_expr)))
    else:
        return str_expr


def index_simplify(matchobj):
    str_expr = matchobj.group(0)
    if str_expr == "[arb]" or str_expr == "[!arb]" or str_expr == "[index[n]":
        return str_expr
    if "++" in str_expr:
        return str_expr
    expr = sympy.sympify(str_expr[1 : len(str_expr) - 1])
    """
    This will sometimes cause bugs due to the different semantics in C
    E.g., x = 9, (x+3)/4 != x/4+3/4.
    We could use cxxcode, but it will generate floating expressions which are
    expensive on FPGA.
    At present, we check if there is floor or ceil in the expression.
    If so, we abort and use the original expression. Otherwise, we replace it
    with the simplified one.
    """
    expr = sympy.simplify(expr)
    new_str_expr = sympy.printing.ccode(expr)

    if "floor" in new_str_expr or "ceil" in new_str_expr or ".0" in new_str_expr:
        return str_expr
    else:
        return "[" + new_str_expr + "]"


def mod_simplify(matchobj):
    str_expr = matchobj.group(0)
    str_expr = str_expr[1 : len(str_expr) - 3]
    expr = sympy.sympify(str_expr)
    expr = sympy.simplify(expr)
    str_expr = str(expr)

    return "(" + str_expr + ") %"


def simplify_expressions(lines):
    """Simplify the index expressions in the program

    Use Sympy to simplify all the array index expressions in the program.

    Parameters
    ----------
    lines:
        contains the codelines of the program
    """
    code_len = len(lines)
    # Simplify array index expressions
    for pos in range(code_len):
        line = lines[pos]
        line = re.sub(r"\[(.+?)\]", index_simplify, line)
        lines[pos] = line

    # Simplify mod expressions
    for pos in range(code_len):
        line = lines[pos]
        line = re.sub(r"\((.+?)\) %", mod_simplify, line)
        lines[pos] = line

    return lines


def lift_split_buffers(lines):
    """Lift the split buffers in the program

    For each module, if we find any split buffers with the name "buf_data_split",
    we will lift them out of the for loops and put them in the variable declaration
    section at the beginning of the module.

    Parameters
    ----------
    lines:
        contains the codelines of the program
    """
    code_len = len(lines)
    for pos in range(code_len):
        line = lines[pos]
        if line.find("variable=buf_data_split") != -1:
            # Search for the variable declaration section
            decl_pos = -1
            prev_pos = pos - 1
            while prev_pos >= 0:
                prev_line = lines[prev_pos]
                if prev_line.find("Variable Declaration") != -1:
                    decl_pos = prev_pos
                    break
                prev_pos -= 1
            # Move the two code lines at [pos - 1] and [pos] to [decl_pos] and
            # [decl_pos + 1]
            indent = lines[decl_pos].find("/*")
            line1 = " " * indent + lines[pos - 1].lstrip()
            line2 = " " * indent + lines[pos].lstrip()
            del lines[pos - 1]
            del lines[pos - 1]
            lines.insert(decl_pos, line1)
            lines.insert(decl_pos + 1, line2)

    return lines


def build_dummy_module_def(group_name, fifo_type, module_in, PE_ids):
    """Build the definition of the dummy module

    Parameters
    ----------
    group_name: str
    fifo_type: str
    module_in: int
    PE_ids: list
    """
    dir_str = "out" if module_in == 0 else "in"
    index_str = ["idx", "idy", "idz"]
    fifo_name = f"fifo_{group_name}_{dir_str}"

    lines = []
    lines.append("/* Module Definition */\n")
    lines.append(f"void {group_name}_PE_dummy_{dir_str}(")
    for pos in range(len(PE_ids)):
        lines.append(f"int {index_str[pos]}, ")
    lines.append(f"hls::stream<{fifo_type}> &{fifo_name}){{\n")
    if module_in == 0:
        lines.append(f"  if (!{fifo_name}.full())\n")
        lines.append(f"    {fifo_name}.write(0);\n")
    else:
        lines.append(f"  {fifo_type} fifo_data = {fifo_name}.read();\n")
    lines.append(f"}}\n")
    lines.append(f"/* Module Definition */\n")

    return lines


def build_dummy_module_call(group_name, fifo_name, module_in, PE_ids):
    """Build the call of the dummy module

    Parameters
    ----------
    group_name: str
    fifo_name: str
    module_in: int
    PE_ids: list
    """
    dir_str = "out" if module_in == 0 else "in"

    lines = []
    lines.append("\n")
    lines.append("  /* Module Call */\n")
    lines.append(f"  {group_name}_PE_dummy_{dir_str}(\n")
    for id in PE_ids:
        lines.append(f"    /* module id */ {id},\n")
    lines.append(f"    /* fifo */ {fifo_name}\n")
    lines.append(f"  );\n")
    lines.append(f"  /* Module Call */\n")

    return lines


def insert_dummy_modules(def_lines, call_lines):
    """Insert the missing dummy modules

    Collect the FIFO information of PEs (fifo_name, fifo_type).
    Delete those FIFOs that are connected to other modules.
    Insert dummy modules for the rest of FIFOs.

    Parameters
    ----------
    def_lines: list
        Contains the codelines of the module definitions
    call_lines: list
        Contains the codelines of the module calls
    """
    PE_fifos = []
    for line in def_lines:
        if line.find("void PE_wrapper") != -1:
            # Parse the argument list
            m = re.search(r"\((.+?)\)", line)
            args = m.group(1).strip().split(",")
            for arg in args:
                if arg.find("fifo") != -1:
                    m = re.search(r"stream<(.+?)>", arg)
                    fifo_type = m.group(1)
                    fifo_name = arg.split("&")[-1]
                    PE_fifos.append({"type": fifo_type, "name": fifo_name})
    # Collect all used fifos
    used_fifos = {}
    kernel_start = 0
    for line in call_lines:
        if line.find("void kernel0") != -1:
            kernel_start = 1
        if kernel_start:
            if line.find("* fifo *") != -1:
                fifo = line.strip().split("*")[2][2:]
                if fifo[-1] == ",":
                    fifo = fifo[:-1]
                # Only process PE level fifos
                if fifo.find("PE") == -1:
                    continue
                if fifo not in used_fifos:
                    used_fifos[fifo] = -1
                else:
                    del used_fifos[fifo]
    # Locate the fifo position
    inside_module = False
    inside_PE = False
    fifo_pos = 0
    PE_call_start = -1
    PE_call_end = -1
    line_id = 0
    for line in call_lines:
        if line.find("Module Call") != -1:
            inside_module = not inside_module
            if inside_PE:
                PE_call_end = line_id
            inside_PE = False
        if inside_module:
            if line.find("PE_wrapper") != -1:
                inside_PE = True
                fifo_pos = 0
                if PE_call_start == -1:
                    PE_call_start = line_id - 1
            if inside_PE:
                if line.find("fifo") != -1:
                    for used_fifo in used_fifos:
                        if line.find(used_fifo) != -1:
                            used_fifos[used_fifo] = fifo_pos
                    fifo_pos += 1
        line_id += 1
    # Insert the dummy module definitions
    offset_line = 0
    for used_fifo in used_fifos:
        fifo_info = PE_fifos[used_fifos[used_fifo]]
        # Extract the module direction
        if fifo_info["name"].endswith("in"):
            module_in = 0
        else:
            module_in = 1
        # Extract the group name
        if fifo_info["name"].endswith("in"):
            group_name = fifo_info["name"][5:-3]
        else:
            group_name = fifo_info["name"][5:-4]
        # Extract the PE ids
        PE_ids = used_fifo[len(f"fifo_{group_name}_PE_") :].split("_")

        # Build the dummy module definition
        module_def = build_dummy_module_def(
            group_name, fifo_info["type"], module_in, PE_ids
        )
        def_lines += module_def
        def_lines.append("\n")

        # Build the dummy module call
        module_call = build_dummy_module_call(
            group_name, used_fifo, module_in, PE_ids
        )  # TODO
        if module_in == 0:
            for i in range(len(module_call)):
                call_lines.insert(PE_call_start - 1 + i, module_call[i])
            offset_line += len(module_call)
        else:
            for i in range(len(module_call)):
                call_lines.insert(PE_call_end + 1 + offset_line + i, module_call[i])

    return def_lines, call_lines


def codegen_run(
    kernel_call, kernel_def, kernel="heterosa.out/src/kernel_kernel.cpp", quiet=False
):
    """Generate the kernel file for Xilinx platform

    We will copy the content of kernel definitions before the kernel calls.

    Parameters
    ----------
    kernel_call:
        file containing kernel calls
    kernel_def:
        file containing kernel definitions
    kernel:
        output kernel file
    """

    # Load kernel definition file
    lines = []
    with open(kernel_def, "r") as f:
        lines = f.readlines()
    call_lines = []
    with open(kernel_call, "r") as f:
        call_lines = f.readlines()

    # Simplify the expressions
    lines = simplify_expressions(lines)

    # Insert the HLS pragmas
    lines = insert_xlnx_pragmas(lines)

    # Lift the split_buffers
    lines = lift_split_buffers(lines)

    kernel = str(kernel)
    if not quiet:
        print("Please find the generated file: " + kernel)

    with open(kernel, "w") as f:
        f.write("\n")
        f.writelines(lines)
        f.writelines(call_lines)


def main():
    parser = argparse.ArgumentParser(description="==== AutoSA CodeGen ====")
    parser.add_argument(
        "-c",
        "--kernel-call",
        metavar="KERNEL_CALL",
        required=True,
        help="kernel function call",
    )
    parser.add_argument(
        "-d",
        "--kernel-def",
        metavar="KERNEL_DEF",
        required=True,
        help="kernel function definition",
    )
    parser.add_argument(
        "-o", "--output", metavar="OUTPUT", required=False, help="output kernel file"
    )

    args = parser.parse_args()

    codegen_run(args.kernel_call, args.kernel_def, args.output)


if __name__ == "__main__":
    main()
