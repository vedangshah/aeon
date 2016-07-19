import os
import fnmatch

def write_manifest(output_file, *filenames):
    """ Writes out a manifest file from a series of lists of filenames
    """

    with open(output_file, "w") as fid:
        for line in zip(*filenames):
            fid.write(",".join(line) + "\n")

    return True


def get_files(directory, pattern, recursive=True):
    """ Return the full path to all files in directory matching the specified
    pattern.

    pattern should be a glob style pattern (e.g. "*.wav")
    """

    matches = list()
    for root, dirnames, filenames in os.walk(directory):
        for filename in fnmatch.filter(filenames, pattern):
            matches.append(os.path.join(root, filename))
        if not recursive:
            break

    return matches
