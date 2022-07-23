#pragma once

#include "widget.hpp"

namespace ht {

class Preview : public Widget {
public:
    void Update(const RenderTarget& renderTarget);

    bool IsHovered() const { return mIsHovered; }

private:
    bool mIsHovered{false};
};

}  // namespace ht