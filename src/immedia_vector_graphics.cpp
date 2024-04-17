#define IMGUI_DEFINE_MATH_OPERATORS
#include "immedia_vector_graphics.h"

#include "imgui_internal.h"

#include "immedia_vector_graphics.h"

namespace ImMedia {

struct LineArgs
{
    float X1;
    float Y1;
    float X2;
    float Y2;
    ImU32 Color;
    float Thinkness;
};

struct RectArgs
{
    float X1;
    float Y1;
    float X2;
    float Y2;
    ImU32 Color;
    float Thinkness;
    float Rounding;
    ImDrawFlags Flags;
};

struct RectFilledArgs
{
    float X1;
    float Y1;
    float X2;
    float Y2;
    ImU32 Color;
    float Rounding;
    ImDrawFlags Flags;
};

// The structure of VectorGraphicsElement
// | 0 - 16           | 16 - 32 |
// | args struct size | id      |

#define ELEMENT_INFO(ID, ARGS) ((0xFF & ID) << 16) | (0xFF & (uint16_t)sizeof(ARGS))

#define ELEMENT_ARGS_SIZE(ELEMENT) (int)ELEMENT & 0xFF

enum class Element : int
{
    Line       = ELEMENT_INFO(1, LineArgs      ),
    Rect       = ELEMENT_INFO(2, RectArgs      ),
    RectFilled = ELEMENT_INFO(3, RectFilledArgs)
};

VectorGraphics::VectorGraphics(const ImVec2& size) :
    Size(size)
{

}

ImVec2 VectorGraphics::GetSize() const
{
    return Size;
}

#define ADD_ELEMENT(ELEMENT, ARGS) \
    Elements.push_back(static_cast<int>(ELEMENT));\
    int old_size = ElementInfo.size();\
    ElementInfo.resize(sizeof(ARGS) + old_size);\
    ARGS* info = reinterpret_cast<ARGS*>(&ElementInfo[old_size])

void VectorGraphics::AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness)
{
    ADD_ELEMENT(Element::Line, LineArgs);
    info->X1 = p1.x;
    info->Y1 = p1.y;
    info->X2 = p2.x;
    info->Y2 = p2.y;
    info->Color = col;
    info->Thinkness = thickness;
}

void VectorGraphics::AddRect(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness, float rounding, ImDrawFlags flags)
{
    ADD_ELEMENT(Element::Rect, RectArgs);
    info->X1 = p1.x;
    info->Y1 = p1.y;
    info->X2 = p2.x;
    info->Y2 = p2.y;
    info->Color = col;
    info->Thinkness = thickness;
    info->Rounding = rounding;
    info->Flags = flags;
}

void VectorGraphics::AddRectFilled(const ImVec2& p1, const ImVec2& p2, ImU32 col, float rounding, ImDrawFlags flags)
{
    ADD_ELEMENT(Element::RectFilled, RectFilledArgs);
    info->X1 = p1.x;
    info->Y1 = p1.y;
    info->X2 = p2.x;
    info->Y2 = p2.y;
    info->Color = col;
    info->Rounding = rounding;
    info->Flags = flags;
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

    const Element* element      = reinterpret_cast<const Element*>(Elements.begin());
    const Element* element_end  = reinterpret_cast<const Element*>(Elements.end());
    const uint8_t* element_info = ElementInfo.begin();

    double scale = fmin(size.x / Size.x, size.y / Size.y);
    ImVec2 offset((size.x - Size.x * scale) / 2, (size.y - Size.y * scale) / 2);

    while (element < element_end)
    {
        switch (*element)
        {
        case Element::Line:
        {
            const LineArgs* args = reinterpret_cast<const LineArgs*>(element_info);
            draw_list->AddLine(pos + offset + ImVec2(args->X1, args->Y1) * scale,
                               pos + offset + ImVec2(args->X2, args->Y2) * scale,
                               args->Color,
                               args->Thinkness == 0 ? 1 : args->Thinkness * scale);
            break;
        }
        case Element::Rect:
        {
            const RectArgs* args = reinterpret_cast<const RectArgs*>(element_info);
            draw_list->AddRect(pos + offset + ImVec2(args->X1, args->Y1) * scale,
                               pos + offset + ImVec2(args->X2, args->Y2) * scale,
                               args->Color,
                               args->Rounding * scale,
                               args->Flags,
                               args->Thinkness == 0 ? 1 : args->Thinkness * scale);
            break;
        }

        case Element::RectFilled:
        {
            const RectFilledArgs* args = reinterpret_cast<const RectFilledArgs*>(element_info);
            draw_list->AddRectFilled(pos + offset + ImVec2(args->X1, args->Y1) * scale,
                                     pos + offset + ImVec2(args->X2, args->Y2) * scale,
                                     args->Color,
                                     args->Rounding * scale,
                                     args->Flags);
            break;
        }

        default:
            break;
        }

        element_info += ELEMENT_ARGS_SIZE(*element);
        ++element;
    }
}

}
