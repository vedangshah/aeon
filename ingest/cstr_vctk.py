#!/usr/bin/env python
import os
from ingest.utils import write_manifest, get_files, convert_audio


def main(input_directory, output_directory=None, manifest_file=None):
    """ Convert all .wav files from input_directory to 16-bit signed .wav files sampled at 16 kHz and store them in output_diretory. Write a manifest file to manifest_file referring to each .wav file and its paired transcript.
    """

    # Set up directories
    wav_directory = os.path.join(input_directory, "wav48")
    txt_directory = os.path.join(input_directory, "txt")

    if output_directory is None:
        output_directory = os.path.join(input_directory, "ingested")
    if manifest_file is None:
        manifest_file = os.path.join(input_directory, "manifest.csv")

    # Create output directories
    for ss in os.listdir(wav_directory):
        out_dir = os.path.join(output_directory, ss)
        os.makedirs(out_dir)

    # Get full path to all wav files
    filenames = get_files(wav_directory, "*.wav", recursive=True)

    wav_files = list()
    text_files = list()
    for fname in filenames:
        # Set up filename for output .wav
        speaker_dir, wav_file = os.path.split(fname)
        speaker_dir = os.path.split(speaker_dir)[1]
        output_file = os.path.join(output_directory, speaker_dir, wav_file)

        # Find exists transcript .txt file
        text_file = "{}.txt".format(os.path.splitext(wav_file)[0])
        text_file = os.path.join(txt_directory, speaker_dir, text_file)
        if not os.path.isfile(text_file):
            print("Could not find transcript for {}".format(fname))
            continue

        # Convert 48k wav file to 16k wav
        success = convert_audio(fname, output_file)
        if success is False:
            print("Audio conversion failed for {}".format(fname))
            continue

        # Add to output lists to be written to manifest
        wav_files.append(output_file)
        text_files.append(text_file)

    return write_manifest(manifest_file, wav_files, text_files)


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("manifest_file",
                        help="Output file that specifies the filename for each"
                        " input .wav and output .txt")
    parser.add_argument("input_directory",
                        help="Directory containing the CSTR VCTK corpus wav"
                        " files and transcript files. It should have a"
                        " subdirectory called wav48 filled with .wavs and a"
                        " subdirectory called txt filled with .txts.")

    args = parser.parse_args()
    main(args.input_directory, args.manifest_file)
