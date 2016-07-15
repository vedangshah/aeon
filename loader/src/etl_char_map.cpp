#include "etl_char_map.hpp"

using namespace std;
using namespace nervana;

std::shared_ptr<char_map::decoded> char_map::extractor::extract(const char* in_array, int in_sz)
{
    auto rc = make_shared<char_map::decoded>();
    string transcript(in_array, in_sz);

    for (auto c: transcript)
    {
        auto l = _cmap.find(c);
        if (l != _cmap.end())
        {
            rc->_labels.push_back(l->second);
        }
        else
        {
            rc = nullptr;
            break;
        }
    }
    return rc;
}


void char_map::loader::load(char* out_array, std::shared_ptr<char_map::decoded> dc)
{
    memset(out_array, 0, _max_length * sizeof(uint8_t));
    uint32_t copy_length = std::min(_max_length, (uint32_t) dc->get_data().size());
    for (uint i=0; i<copy_length; i++)
    {
        out_array[i] = dc->get_data()[i];
    }
}
