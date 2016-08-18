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

#pragma once

#include "etl_image.hpp"
#include "cap_mjpeg_decoder.hpp"

namespace nervana {
    namespace video {
        class config;
        class extractor;
        class transformer;
        class loader;
    }

    class video::config : public interface::config {
    public:
        uint32_t                              max_frame_count;
        nervana::image::config                frame;

        config(nlohmann::json js) :
        frame(js["frame"])
        {
            if(js.is_null()) {
                throw std::runtime_error("missing video config in json config");
            }

            for(auto& info : config_list) {
                info->parse(js);
            }
            verify_config("video", config_list, js);

            // channel major only
            add_shape_type({frame.channels, max_frame_count, frame.height, frame.width},
                           frame.type_string);
        }

    private:
        config() {}
        std::vector<std::shared_ptr<interface::config_info_interface>> config_list = {
            ADD_SCALAR(max_frame_count, mode::REQUIRED),
            ADD_IGNORE(frame)
        };
    };

    class video::extractor : public interface::extractor<image::decoded> {
    public:
        extractor(const video::config&) {}
        virtual ~extractor() {}

        virtual std::shared_ptr<image::decoded> extract(const char* item, int itemSize) override;

    protected:
    private:
        extractor() = delete;
    };

    // simple wrapper around image::transformer for now
    class video::transformer : public interface::transformer<image::decoded, image::params> {
    public:
        transformer(const video::config&);
        virtual ~transformer() {}
        virtual std::shared_ptr<image::decoded> transform(
                                                std::shared_ptr<image::params>,
                                                std::shared_ptr<image::decoded>) override;
    protected:
        transformer() = delete;
        image::transformer frame_transformer;
        uint32_t max_frame_count;
    };

    class video::loader : public interface::loader<image::decoded> {
    public:
        loader(const video::config& cfg) {}
        virtual ~loader() {}
        virtual void load(const std::vector<void*>&, std::shared_ptr<image::decoded>) override;

    private:
        loader() = delete;
    };
}
