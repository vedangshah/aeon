import os
import fnmatch
import subprocess

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


def convert_audio(input_file, output_file, bit_depth=16,
                  encoding_type="signed", sample_frequency=16000):
    """ Converts an input audio file to output audio file using the sox command
    line utility.
    """

    r = subprocess.check_call(["sox",
                               input_file,
                               "-r", str(sample_frequency),
                               "-b", str(bit_depth),
                               "-e", encoding_type,
                               output_file])

    return r == 0
