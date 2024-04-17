#ifndef IMMEDIA_VECTOR_GRAPHICS_H
#define IMMEDIA_VECTOR_GRAPHICS_H

#include <stdint.h>

#include "imgui.h"

namespace ImMedia {

class VectorGraphics
{
public:
    VectorGraphics(const ImVec2& size);
    ~VectorGraphics() {};
    
    ImVec2 GetSize() const;

    void AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 0.0f);
    void AddRect(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 0.0f, float rounding = 0.0f, ImDrawFlags flags = 0);
    void AddRectFilled(const ImVec2& p1, const ImVec2& p2, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0);


    void Show(const ImVec2& size) const;

private:
    ImVec2            Size;
    ImVector<int>     Elements;
    ImVector<uint8_t> ElementInfo;
};

}

#endif // IMMEDIA_VECTOR_GRAPHICS_H
