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

#include <iostream>
#include <chrono>

#include "gtest/gtest.h"

#include "gen_image.hpp"

using namespace std;

gen_image image_dataset;

static void CreateImageDataset() {
//    std::chrono::high_resolution_clock timer;
//    auto start = timer.now();
    image_dataset.Directory("image_data")
            .Prefix("archive-")
            .MacrobatchMaxItems(500)
            // SetSize must be a multiple of (minibatchCount*batchSize) which is 8320 currently
            .DatasetSize(1500)
            .ImageSize(128,128)
            .Create();
//    auto end = timer.now();
//    cout << "image dataset " << (chrono::duration_cast<chrono::milliseconds>(end - start)).count() << " msec" << endl;
}

static void DeleteDataset() {
    image_dataset.Delete();
}

extern "C" int main( int argc, char** argv ) {
    CreateImageDataset();

    ::testing::InitGoogleTest(&argc, argv);
    int rc = RUN_ALL_TESTS();

    DeleteDataset();

    return rc;
}
