#pragma once

namespace ht {

class Widget {
public:
    Widget();
    ~Widget();

    void ToggleVisibility() { mVisibility = !mVisibility; }

protected:
    bool Begin(const char* title, int flags = 0);
    void End();

    bool mVisibility  = true;
    bool mIsFocused   = false;
    int  mWindowFlags = 0;

private:
    bool mShouldCallEnd = true;
};

}  // namespace ht