/*
 Copyright 2016 Nervana Systems Inc.
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include <sys/stat.h>

#include <cassert>
#include <sstream>
#include <fstream>

#include "block_loader_file.hpp"

using namespace std;
using namespace nervana;

block_loader_file::block_loader_file(shared_ptr<nervana::manifest_csv> mfst,
                                     float subset_fraction,
                                     uint block_size)
: block_loader(block_size),
  _manifest(mfst),
  _subset_fraction(subset_fraction)
{
    assert(_subset_fraction > 0.0 && _subset_fraction <= 1.0);
}

void block_loader_file::loadBlock(nervana::buffer_in_array& dest, uint block_num)
{
    // NOTE: thread safe so long as you aren't modifying the manifest
    // NOTE: dest memory must already be allocated at the correct size
    // NOTE: end_i - begin_i may not be a full block for the last
    // block_num

    // begin_i and end_i contain the indexes into the manifest file which
    // hold the requested block
    size_t begin_i = block_num * _block_size;
    size_t end_i = min((block_num + 1) * (size_t)_block_size, _manifest->objectCount());

    if (_subset_fraction != 1.0) {
        // adjust end_i in relation to begin_i.  We want to scale (end_i
        // - begin_i) by _subset_fraction.  In the case of a smaller block
        // than block_size (in the last block), we want _subset_fraction
        // of them so we need to make sure we first shorten the end_i to
        // the corrent smaller block size, and then scale that.
        end_i = begin_i + (((end_i - begin_i) * _subset_fraction));
    }

    // ensure we stay within bounds of manifest
    assert(begin_i <= _manifest->objectCount());
    assert(end_i <= _manifest->objectCount());

    // TODO: move index offset logic and bounds asserts into Manifest
    // interface to more easily support things like offset/limit queries.
    // It isn't obvious yet what the best interface for this will be.
    // Some options include:
    //  - the manifest should know about block_num and block_size itself
    //  - it should expose an at(index) method instead of begin()/end()
    //  - it should expose a getCursor(index_begin, index_end) which more
    //    closely mirrors most database query patterns (limit/offset)
    auto begin_it = _manifest->begin() + begin_i;
    auto end_it = _manifest->begin() + end_i;

    for(auto it = begin_it; it != end_it; ++it) {
        // load both object and target files into respective buffers
        //
        // NOTE: if at some point in the future, loadFile is loading
        // files from a network like s3 it may make sense to use multiple
        // threads to make loads faster.  multiple threads would only
        // slow down reads from a magnetic disk.
        auto file_list = *it;
        for (uint i = 0; i < file_list.size(); i++) {
            try {
                loadFile(dest[i], file_list[i]);
            } catch (std::exception& e) {
                dest[i]->add_exception(std::current_exception());
            }
        }
    }
}

void block_loader_file::loadFile(nervana::buffer_in* buff, const string& filename)
{
    off_t size = getFileSize(filename);
    ifstream fin(filename, ios::binary);
    buff->read(fin, size);
}

off_t block_loader_file::getFileSize(const string& filename)
{
    // ensure that filename exists and get its size

    struct stat stats;
    if (stat(filename.c_str(), &stats) == -1) {
        throw std::runtime_error("Could not find file: \"" + filename + "\"");
    }

    return stats.st_size;
}

uint block_loader_file::objectCount()
{
    if (_subset_fraction == 1.0) {
        return _manifest->objectCount();
    } else {
        uint full_block_count = int(_manifest->objectCount() / _block_size);
        uint subset_object_count = full_block_count * int(_block_size * _subset_fraction);
        uint leftover_object_count = _manifest->objectCount() - full_block_count * _block_size;
        subset_object_count += (leftover_object_count * _subset_fraction);
        return subset_object_count;
    }
}
