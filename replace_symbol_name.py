import os
import re
import argparse
import shutil

def replace_symbol_in_file(filepath, old_name, new_name):
    with open(filepath, "r", encoding="utf-8", errors="ignore") as f:
        content = f.read()

    # thay thế toàn bộ tên function (match word boundary)
    pattern = r'\b' + re.escape(old_name) + r'\b'
    new_content = re.sub(pattern, new_name, content)

    if new_content != content:
        # backup file
        shutil.copy(filepath, filepath + ".bak")

        with open(filepath, "w", encoding="utf-8") as f:
            f.write(new_content)

        print(f"[UPDATED] {filepath}")


def walk_and_replace(root, old_name, new_name, exts):
    for curpath, dirs, files in os.walk(root):
        for file in files:
            if any(file.endswith(ext) for ext in exts):
                replace_symbol_in_file(os.path.join(curpath, file), old_name, new_name)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Rename function symbol in folder tree")
    parser.add_argument("root", help="Root directory to scan")
    parser.add_argument("old", help="Old function name")
    parser.add_argument("new", help="New function name")
    parser.add_argument(
        "--ext",
        nargs="+",
        default=[".c", ".h", ".cpp", ".hpp", ".py"],
        help="File extensions to scan"
    )

    args = parser.parse_args()

    walk_and_replace(args.root, args.old, args.new, args.ext)
