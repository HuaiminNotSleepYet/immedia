#define IMGUI_DEFINE_MATH_OPERATORS
#include "immedia_vector_graphics.h"

#include "imgui_internal.h"

#include "immedia_vector_graphics.h"

namespace ImMedia {

#define ELEMENT_INFO(NUM, FLOAT_COUNT, COLOR_COUNT, FLAGS_COUNT) \
    ((0xFF & NUM        ) << 20)\
  | ((0xFF & FLOAT_COUNT) <<  3)\
  | ((0b11 & COLOR_COUNT) <<  1)\
  | ((0b01 & FLAGS_COUNT) <<  0)

#define ELEMENT_FLOAT_ARG_COUNT(ELEMENT) ((int)ELEMENT >> 3) & 0xFF
#define ELEMENT_COLOR_ARG_COUNT(ELEMENT) ((int)ELEMENT >> 1) & 0b11
#define ELEMENT_FLAGS_ARG_COUNT(ELEMENT) ((int)ELEMENT >> 0) & 0b01

enum class Element : int
{
                          // | ID | float count | color count | flags count |
    Line       = ELEMENT_INFO(  1,            5,            1,            0 ),
    Rect       = ELEMENT_INFO(  2,            6,            1,            1 ),
    RectFilled = ELEMENT_INFO(  3,            5,            1,            1 )
};

VectorGraphics::VectorGraphics(const ImVec2& size) :
    Size(size)
{

}

ImVec2 VectorGraphics::GetSize() const
{
    return Size;
}

void VectorGraphics::AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness)
{
    Elements.push_back((int)Element::Line);
    ElementFloatArgs.push_back(p1.x);
    ElementFloatArgs.push_back(p1.y);
    ElementFloatArgs.push_back(p2.x);
    ElementFloatArgs.push_back(p2.y);
    ElementColorArgs.push_back(col);
    ElementFloatArgs.push_back(thickness);
}

void VectorGraphics::AddRect(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness, float rounding, ImDrawFlags flags)
{
    Elements.push_back((int)Element::Rect);
    ElementFloatArgs.push_back(p1.x);
    ElementFloatArgs.push_back(p1.y);
    ElementFloatArgs.push_back(p2.x);
    ElementFloatArgs.push_back(p2.y);
    ElementColorArgs.push_back(col);
    ElementFloatArgs.push_back(thickness);
    ElementFloatArgs.push_back(rounding);
    ElementFlagsArgs.push_back(flags);
}

void VectorGraphics::AddRectFilled(const ImVec2& p1, const ImVec2& p2, ImU32 col, float rounding, ImDrawFlags flags)
{
    Elements.push_back((int)Element::RectFilled);
    ElementFloatArgs.push_back(p1.x);
    ElementFloatArgs.push_back(p1.y);
    ElementFloatArgs.push_back(p2.x);
    ElementFloatArgs.push_back(p2.y);
    ElementColorArgs.push_back(col);
    ElementFloatArgs.push_back(rounding);
    ElementFlagsArgs.push_back(flags);
}

void VectorGraphics::Show(const ImVec2& size) const
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImVec2 pos = window->DC.CursorPos;
    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0))
        return;

    ImGuiContext* ctx = ImGui::GetCurrentContext();
    ImDrawList* draw_list = window->DrawList;

    const Element* element     = reinterpret_cast<const Element*>(Elements.begin());
    const Element* element_end = reinterpret_cast<const Element*>(Elements.end());
    const float*   float_args  = ElementFloatArgs.begin();
    const ImU32*   color_args  = ElementColorArgs.begin();
    const int*     flags_args  = ElementFlagsArgs.begin();

    float x0 = 0;
    float y0 = 0;
    float x1 = 0;
    float y1 = 0;
    ImU32 color = IM_COL32_WHITE;
    float rounding = 0;
    float thinkness = 0;
    int   flags = 0;

    double scale = fmin(size.x / Size.x, size.y / Size.y);
    ImVec2 offset((size.x - Size.x * scale) / 2, (size.y - Size.y * scale) / 2);

    while (element < element_end)
    {
        switch (*element)
        {
        case Element::Line:
            x0 = float_args[0];
            y0 = float_args[1];
            x1 = float_args[2];
            y1 = float_args[3];
            color = color_args[0];
            thinkness = float_args[4];
            draw_list->AddLine(pos + offset + ImVec2(x0, y0) * scale,
                               pos + offset + ImVec2(x1, y1) * scale,
                               color,
                               thinkness == 0 ? 1 : thinkness * scale);
            break;

        case Element::Rect:
            x0 = float_args[0];
            y0 = float_args[1];
            x1 = float_args[2];
            y1 = float_args[3];
            color = color_args[0];
            rounding = float_args[4];
            thinkness = float_args[5];
            flags = flags_args[0];
            draw_list->AddRect(pos + offset + ImVec2(x0, y0) * scale,
                               pos + offset + ImVec2(x1, y1) * scale,
                               color,
                               rounding * scale,
                               flags,
                               thinkness == 0 ? 1 :thinkness * scale);
            break;

        case Element::RectFilled:
            x0 = float_args[0];
            y0 = float_args[1];
            x1 = float_args[2];
            y1 = float_args[3];
            color = color_args[0];
            rounding = float_args[4];
            flags = flags_args[0];
            draw_list->AddRectFilled(pos + offset + ImVec2(x0, y0) * scale,
                                     pos + offset + ImVec2(x1, y1) * scale,
                                     color,
                                     rounding * scale,
                                     flags);

        default:
            break;
        }

        float_args += ELEMENT_FLOAT_ARG_COUNT(*element);
        color_args += ELEMENT_COLOR_ARG_COUNT(*element);
        flags_args += ELEMENT_FLAGS_ARG_COUNT(*element);
        ++element;
    }
}

}
