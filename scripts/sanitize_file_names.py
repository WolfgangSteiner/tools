#! /usr/bin/env python
import glob, re, os, sys
import typer


def construct_new_filename(filename, shorten_dates):
    path, filename = os.path.split(filename)

    fn = filename.replace(' - ', '_')
    fn = re.sub(r'([a-z])([A-Z])', r'\1_\2', fn)
    fn = re.sub(r'(\d{4})-(\d{2})(?:-(\d{2}))?', r'\1\2\3', fn)
    if shorten_dates:
        fn = re.sub(r'20(\d{2})(\d{2})(\d{2})?', r'\1\2\3', fn)
    fn = fn.lower() 
    fn = re.sub(r'[ -]', '_', fn)
    fn = re.sub(r'__+', '_', fn)
    fn = re.sub(r'[^a-zäöü0-9_.]', '', fn)
    return os.path.join(path, fn)

def get_path_list(directory=".", recursive=False):
    if recursive:
        return glob.glob(os.path.join(directory, "**"), recursive=True)
    else:
        return glob.glob("*")
             
def main(
    dry_run:bool = False,
    shorten_dates:bool = False,
    recursive:bool = False,
    rename_directories:bool = True,
):
    file_list = get_path_list(".", recursive=recursive)
    print(file_list)
    file_list = sorted(file_list, key=lambda p: len(p.split('/')), reverse=True)
    for filename in file_list:
        if rename_directories or os.path.isfile(filename):
            new_filename = construct_new_filename(filename, shorten_dates)
            if filename != new_filename:
                if dry_run:
                    print(f"'{filename}' -> '{new_filename}'")
                else:
                    try:
                        os.rename(filename, new_filename)
                        print(f"'{filename}' -> '{new_filename}'")
                    except Exception as e:
                        print(f"Failed to rename '{filename}': {e}")


if __name__ == "__main__":
    typer.run(main)
