#define IMGUI_DEFINE_MATH_OPERATORS
#include "immedia_vector_graphics.h"

#include "imgui_internal.h"

#include "immedia_vector_graphics.h"

namespace ImMedia {

struct LineArgs
{
    ImVec2 P1;
    ImVec2 P2;
    ImU32  Color;
    float  Thinkness;
};

struct RectArgs
{
    ImVec2 P1;
    ImVec2 P2;
    ImU32 Color;
    float Thinkness;
    float Rounding;
    ImDrawFlags Flags;
};

struct RectFilledArgs
{
    ImVec2 P1;
    ImVec2 P2;
    ImU32 Color;
    float Rounding;
    ImDrawFlags Flags;
};

struct CircleArgs
{
    ImVec2 Center;
    float  Radius;
    ImU32  Color;
    float  Thinkness;
    int    Segments;
};

struct CircleFilledArgs
{
    ImVec2 Center;
    float  Radius;
    ImU32  Color;
    int    Segments;
};

struct EllipseArgs
{
    ImVec2 Center;
    ImVec2 Radius;
    float  Rotation;
    ImU32  Color;
    float  Thinkness;
    int    Segments;
};

struct EllipseFilledArgs
{
    ImVec2 Center;
    ImVec2 Radius;
    float  Rotation;
    ImU32  Color;
    int    Segments;
};

struct BezierCubicArgs
{
    ImVec2 P1;
    ImVec2 P2;
    ImVec2 P3;
    ImVec2 P4;
    ImU32  Color;
    float  Thinkness;
    int    Segments;
};

struct BezierQuadraticArgs
{
    ImVec2 P1;
    ImVec2 P2;
    ImVec2 P3;
    ImU32  Color;
    float  Thinkness;
    int    Segments;
};

struct PolylineArgs
{
    ImVec2* Points;
    int     PointCount;
    ImU32   Color;
    float   Thinkness;
    ImDrawFlags Flags;
};

#define POLYLINE_ARGS_SIZE(POLYLINE_ARGS) sizeof(PolylineArgs) + sizeof(ImVec2) * POLYLINE_ARGS->PointCount

struct PolygonArgs
{
    bool    Convex;
    ImVec2* Points;
    int     PointCount;
    ImU32   Color;
};

#define POLYGON_ARGS_SIZE(POLYGON_ARGS) sizeof(PolygonArgs) + sizeof(ImVec2) * POLYGON_ARGS->PointCount

// The structure of VectorGraphicsElement
// | 0 - 16           | 16 - 32 |
// | args struct size | id      |
// 
// If args struct size is 0, the actual size needs to be able to be calculated from struct field.

#define ELEMENT_INFO(ID, ARGS_SIZE) ((0xFF & ID) << 16) | (uint16_t)ARGS_SIZE

#define ELEMENT_ARGS_SIZE(ELEMENT) (int)ELEMENT & 0xFF

enum class Element : int
{
    Line            = ELEMENT_INFO( 1, sizeof(LineArgs)           ),
    Rect            = ELEMENT_INFO( 2, sizeof(RectArgs)           ),
    RectFilled      = ELEMENT_INFO( 3, sizeof(RectFilledArgs)     ),
    Circle          = ELEMENT_INFO( 4, sizeof(CircleArgs)         ),
    CircleFilled    = ELEMENT_INFO( 5, sizeof(CircleFilledArgs)   ),
    Ellipse         = ELEMENT_INFO( 6, sizeof(EllipseArgs)        ),
    Ellipsefilled   = ELEMENT_INFO( 7, sizeof(EllipseFilledArgs)  ),
    BezierCubic     = ELEMENT_INFO( 8, sizeof(BezierCubicArgs)    ),
    BezierQuadratic = ELEMENT_INFO( 9, sizeof(BezierQuadraticArgs)),
    Polyline        = ELEMENT_INFO(10, 0                          ),
    Polygon         = ELEMENT_INFO(11, 0                          ),
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
    int old_size = ElementArgs.size();\
    ElementArgs.resize(sizeof(ARGS) + old_size);\
    *reinterpret_cast<ARGS*>(&ElementArgs[old_size]) =

void VectorGraphics::AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness)
{ ADD_ELEMENT(Element::Line, LineArgs) { p1, p2, col, thickness }; }

void VectorGraphics::AddRect(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness, float rounding, ImDrawFlags flags)
{ ADD_ELEMENT(Element::Rect, RectArgs) { p1, p2, col, thickness, rounding, flags }; }

void VectorGraphics::AddRectFilled(const ImVec2& p1, const ImVec2& p2, ImU32 col, float rounding, ImDrawFlags flags)
{ ADD_ELEMENT(Element::RectFilled, RectFilledArgs) { p1, p2, col, rounding, flags }; }

void VectorGraphics::AddCircle(const ImVec2& center, float radius, ImU32 col, float thickness, int num_segments)
{ ADD_ELEMENT(Element::Circle, CircleArgs) { center, radius, col, thickness, num_segments }; }

void VectorGraphics::AddCircleFilled(const ImVec2& center, float radius, ImU32 col, int num_segments)
{ ADD_ELEMENT(Element::CircleFilled, CircleFilledArgs) { center, radius, col, num_segments }; }

void VectorGraphics::AddEllipse(const ImVec2& center, const ImVec2& radius, ImU32 col, float rot, float thickness, int num_segments)
{ ADD_ELEMENT(Element::Ellipse, EllipseArgs) { center, radius, rot, col, thickness, num_segments }; }

void VectorGraphics::AddEllipseFilled(const ImVec2& center, const ImVec2& radius, ImU32 col, float rot, int num_segments)
{ ADD_ELEMENT(Element::Ellipsefilled, EllipseFilledArgs) { center, radius, rot, col, num_segments }; }

void VectorGraphics::AddBezierCubic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments)
{ ADD_ELEMENT(Element::BezierCubic, BezierCubicArgs) { p1, p2, p3, p4, col, thickness, num_segments }; }

void VectorGraphics::AddBezierQuadratic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness, int num_segments)
{ ADD_ELEMENT(Element::BezierQuadratic, BezierQuadraticArgs) { p1, p2, p3, col, thickness, num_segments }; }

void VectorGraphics::AddPolyline(const ImVec2* points, int num_points, ImU32 col, float thickness, ImDrawFlags flags)
{
    Elements.push_back(static_cast<int>(Element::Polyline));
    int old_size = ElementArgs.size();
    ElementArgs.resize(sizeof(PolylineArgs) + sizeof(ImVec2) * num_points + old_size);

    PolylineArgs* args = reinterpret_cast<PolylineArgs*>(&ElementArgs[old_size]);
    uint8_t* point_buffer = &ElementArgs[old_size] + sizeof(PolylineArgs);
    memcpy(point_buffer, points, sizeof(ImVec2) * num_points);
    args->Points = reinterpret_cast<ImVec2*>(point_buffer);
    args->PointCount = num_points;
    args->Color = col;
    args->Thinkness = thickness;
    args->Flags = flags;
}

void VectorGraphics::AddPolygonFilled(const ImVec2* points, int num_points, ImU32 col, bool convex)
{
    Elements.push_back(static_cast<int>(Element::Polygon));
    int old_size = ElementArgs.size();
    ElementArgs.resize(sizeof(PolygonArgs) + sizeof(ImVec2) * num_points + old_size);

    PolygonArgs* args = reinterpret_cast<PolygonArgs*>(&ElementArgs[old_size]);
    uint8_t* point_buffer = &ElementArgs[old_size] + sizeof(PolygonArgs);
    memcpy(point_buffer, points, sizeof(ImVec2) * num_points);
    args->Convex = convex;
    args->Points = reinterpret_cast<ImVec2*>(point_buffer);
    args->PointCount = num_points;
    args->Color = col;
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

    if (Elements.empty())
        return;
    Draw(window->DrawList, bb.Min, bb.Max);
}

void VectorGraphics::Draw(ImDrawList* draw_list, const ImVec2& p1, const ImVec2& p2) const
{
    const ImVec2 size = p2 - p1;
    double scale = fmin(size.x / Size.x, size.y / Size.y);
    ImVec2 offset = ImVec2((size.x - Size.x * scale) / 2, (size.y - Size.y * scale) / 2) + p1;

    const Element* element      = reinterpret_cast<const Element*>(Elements.begin());
    const Element* element_end  = reinterpret_cast<const Element*>(Elements.end());
    const uint8_t* element_args = ElementArgs.begin();

    while (element < element_end)
    {
        int args_size = ELEMENT_ARGS_SIZE(*element);

        switch (*element)
        {
        case Element::Line:
        {
            const LineArgs* args = reinterpret_cast<const LineArgs*>(element_args);
            draw_list->AddLine(offset + args->P1 * scale,
                offset + args->P2 * scale,
                args->Color,
                args->Thinkness == 0 ? 1 : args->Thinkness * scale);
            break;
        }
        case Element::Rect:
        {
            const RectArgs* args = reinterpret_cast<const RectArgs*>(element_args);
            draw_list->AddRect(offset + args->P1 * scale,
                offset + args->P2 * scale,
                args->Color,
                args->Rounding * scale,
                args->Flags,
                args->Thinkness == 0 ? 1 : args->Thinkness * scale);
            break;
        }
        case Element::RectFilled:
        {
            const RectFilledArgs* args = reinterpret_cast<const RectFilledArgs*>(element_args);
            draw_list->AddRectFilled(offset + args->P1 * scale,
                offset + args->P2 * scale,
                args->Color,
                args->Rounding * scale,
                args->Flags);
            break;
        }
        case Element::Circle:
        {
            const CircleArgs* args = reinterpret_cast<const CircleArgs*>(element_args);
            draw_list->AddCircle(offset + args->Center * scale,
                args->Radius * scale,
                args->Color,
                args->Segments,
                args->Thinkness == 0 ? 1 : args->Thinkness * scale);
            break;
        }
        case Element::CircleFilled:
        {
            const CircleFilledArgs* args = reinterpret_cast<const CircleFilledArgs*>(element_args);
            draw_list->AddCircleFilled(offset + args->Center * scale,
                args->Radius * scale,
                args->Color,
                args->Segments);
            break;
        }
        case Element::Ellipse:
        {
            const EllipseArgs* args = reinterpret_cast<const EllipseArgs*>(element_args);
            draw_list->AddEllipse(offset + args->Center * scale,
                args->Radius.x * scale,
                args->Radius.y * scale,
                args->Color,
                args->Rotation,
                args->Segments,
                args->Thinkness == 0 ? 1 : args->Thinkness * scale);
            break;
        }
        case Element::Ellipsefilled:
        {
            const EllipseFilledArgs* args = reinterpret_cast<const EllipseFilledArgs*>(element_args);
            draw_list->AddEllipseFilled(offset + args->Center * scale,
                args->Radius.x * scale,
                args->Radius.y * scale,
                args->Color,
                args->Rotation,
                args->Segments);
            break;
        }
        case Element::BezierCubic:
        {
            const BezierCubicArgs* args = reinterpret_cast<const BezierCubicArgs*>(element_args);
            draw_list->AddBezierCubic(offset + args->P1 * scale,
                offset + args->P2 * scale,
                offset + args->P3 * scale,
                offset + args->P4 * scale,
                args->Color,
                args->Thinkness == 0 ? 1 : args->Thinkness * scale,
                args->Segments);
            break;
        }
        case Element::BezierQuadratic:
        {
            const BezierQuadraticArgs* args = reinterpret_cast<const BezierQuadraticArgs*>(element_args);
            draw_list->AddBezierQuadratic(offset + args->P1 * scale,
                offset + args->P2 * scale,
                offset + args->P3 * scale,
                args->Color,
                args->Thinkness == 0 ? 1 : args->Thinkness * scale,
                args->Segments);
            break;
        }
        case Element::Polyline:
        {
            VectorGraphics* p = const_cast<VectorGraphics*>(this);
            const PolylineArgs* args = reinterpret_cast<const PolylineArgs*>(element_args);
            p->PointBuffer.resize(args->PointCount);
            for (size_t i = 0; i < args->PointCount; i++)
                p->PointBuffer[i] = offset + args->Points[i] * scale;
            draw_list->AddPolyline(PointBuffer.Data, args->PointCount,
                args->Color,
                args->Flags,
                args->Thinkness == 0 ? 1 : args->Thinkness * scale);
            args_size = POLYLINE_ARGS_SIZE(args);
            break;
        }
        case Element::Polygon:
        {
            VectorGraphics* p = const_cast<VectorGraphics*>(this);
            const PolygonArgs* args = reinterpret_cast<const PolygonArgs*>(element_args);
            p->PointBuffer.resize(args->PointCount);
            for (size_t i = 0; i < args->PointCount; i++)
                p->PointBuffer[i] = offset + args->Points[i] * scale;
            if (args->Convex)
                draw_list->AddConvexPolyFilled(PointBuffer.Data, args->PointCount, args->Color);
            else
                draw_list->AddConvexPolyFilled(PointBuffer.Data, args->PointCount, args->Color);
            args_size = POLYGON_ARGS_SIZE(args);
        }
        default:
            break;
        }

        ++element;
        element_args += args_size;
    }
}

}
