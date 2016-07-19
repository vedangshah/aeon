#!/usr/bin/env python
import os
import subprocess
from ingest.utils import write_manifest, get_files


def convert_mp3_to_wav(mp3_file, wav_file):
    """ Converts mp3 files to wav files using the sox command line utility.
    """

    r = subprocess.check_call(["sox", mp3_file, "-b", "16",
                               "-e", "signed", wav_file])

    return r == 0


def get_all_transcripts(sentence_csv_file):
    """ Extracts all of the transcripts from the sentence details csv file
    """

    transcripts = dict()
    with open(sentence_csv_file, "r") as fid:
        for line in fid:
            details = line.split("\t")
            transcripts[details[0]] = details[2]

    return transcripts


def main(mp3_dir, wav_dir, transcript_dir, sentence_details_file,
         manifest_file):

    if not os.path.isdir(mp3_dir):
        raise IOError("Directory of mp3 files does not exist! {}".format(mp3_dir))

    if not os.path.isfile(sentence_details_file):
        raise IOError("Sentence details file does not exist! {}".format(sentence_details_file))

    if not os.path.exists(wav_dir):
        os.makedirs(wav_dir)

    if not os.path.exists(transcript_dir):
        os.makedirs(transcript_dir)

    transcripts = get_all_transcripts(sentence_details_file)
    mp3_files = get_files(mp3_dir, "*.mp3")

    failed = list()
    wav_files = list()
    txt_files = list()
    for mp3_file in mp3_files:

        fname = os.path.splitext(os.path.basename(mp3_file))[0]
        try:
            tscript = transcripts[fname]
        except KeyError:
            print("Could not find transcript for {}".format(mp3_file))
            continue

        wav_file = os.path.join(wav_dir, "{}.wav".format(fname))
        txt_file = os.path.join(transcript_dir, "{}.txt".format(fname))

        success = convert_mp3_to_wav(mp3_file, wav_file)
        if success is False:
            failed.append(mp3_file)
            continue

        with open(txt_file, "w") as fid:
            fid.write(tscript)

        wav_files.append(wav_file)
        txt_files.append(txt_file)

    if len(failed) > 0:
        print("The following files failed to convert from mp3 to wav")
        print("\n".join(failed))

    return write_manifest(manifest_file, wav_files, txt_files)


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser("Import tatoeba sentences")
    parser.add_argument("mp3_directory")
    parser.add_argument("sentence_details_file")
    parser.add_argument("wav_directory")
    parser.add_argument("transcript_directory")
