#!/usr/bin/env python
import os
from ingest.utils import write_manifest, get_files


def main(input_directory, manifest_file):
    """
    - Load up .wav files from subdirectories of wav48 directory
    - Convert each to pcm-16: what sample rate? option?
    - Optionally segment files
    - Load corresponding text file
    - Output to manifest file
    """

    wav_directory = os.path.join(input_directory, "wav48")
    txt_directory = os.path.join(input_directory, "txt")

    filenames = get_files(wav_directory, "*.wav", recursive=True)
    text_files = list()
    wav_files = list()

    not_existing = list()
    for fname in filenames:
        speaker_dir, wav_file = os.path.split(fname)
        speaker_dir = os.path.split(speaker_dir)[1]
        text_file = "{}.txt".format(os.path.splitext(wav_file)[0])
        text_file = os.path.join(txt_directory, speaker_dir, text_file)
        if not os.path.isfile(text_file):
            not_existing.append(fname)
            continue
            # raise IOError("Could not find transcript file {}".format(text_file))

        wav_files.append(fname)
        text_files.append(text_file)

    if len(not_existing) > 0:
        print("The following files had no transcription, so they were skipped")
        print("\n".join(not_existing))

    return write_manifest(manifest_file, wav_files, text_files)


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser("Import CSTR VCTK Corpus")
    parser.add_argument("input_directory")
    parser.add_argument("manifest_file")

    args = parser.parse_args()
    main(args.input_directory, args.manifest_file)
