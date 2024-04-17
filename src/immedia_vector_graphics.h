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

    void AddRect      (const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 0.0f, float rounding = 0.0f, ImDrawFlags flags = 0);
    void AddRectFilled(const ImVec2& p1, const ImVec2& p2, ImU32 col,                         float rounding = 0.0f, ImDrawFlags flags = 0);

    void AddCircle       (const ImVec2& center, float radius, ImU32 col, float thickness = 0.0f, int num_segments = 0);
    void AddCircleFilled (const ImVec2& center, float radius, ImU32 col,                         int num_segments = 0);

    void AddEllipse      (const ImVec2& center, const ImVec2& radius, ImU32 col, float thickness = 0.0f, float rot = 0.0f, int num_segments = 0);
    void AddEllipseFilled(const ImVec2& center, const ImVec2& radius, ImU32 col,                         float rot = 0.0f, int num_segments = 0);

    void AddPolyline(const ImVec2* points, int num_points, ImU32 col, float thickness = 0.0f, ImDrawFlags flags = 0);

    void Show(const ImVec2& size) const;

private:
    ImVec2            Size;
    ImVector<int>     Elements;
    ImVector<uint8_t> ElementArgs;

    ImVector<ImVec2>  PointBuffer;
};

}

#endif // IMMEDIA_VECTOR_GRAPHICS_H
