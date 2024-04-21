#define IMGUI_DEFINE_MATH_OPERATORS
#include "immedia_vector_graphics.h"

#include "imgui_internal.h"

#include "immedia_vector_graphics.h"

namespace ImMedia {

VectorGraphics::VectorGraphics(float width, float height) :
    Size(width, height)
{
    assert(width > 0);
    assert(height > 0);
}

VectorGraphics::~VectorGraphics()
{
    Element* element     = begin();
    Element* element_end = end();
    while (element < element_end)
    {
        delete element->Args;
        ++element;
    }
}

VectorGraphics::VectorGraphics(const VectorGraphics& other)
{
    operator=(other);
}

VectorGraphics& VectorGraphics::operator=(const VectorGraphics& other)
{
    Size = other.Size;
    const Element* element     = begin();
    const Element* element_end = end();
    while (element < element_end)
    {
        delete element->Args;
        ++element;
    }
    Elements.resize(0);
    Elements.reserve(other.Elements.size());
    element     = other.begin();
    element_end = other.end();
    while (element < element_end)
    {
        switch (element->GetType())
        {
#define COPY_ELEMENT(TYPE, ARG_TYPE) case TYPE: Elements.push_back(Element(TYPE, new ARG_TYPE(*element->GetArgs<ARG_TYPE>()))); break
        COPY_ELEMENT(ElementType::Line,            LineArgs           );
        COPY_ELEMENT(ElementType::Rect,            RectArgs           );
        COPY_ELEMENT(ElementType::RectFilled,      RectFilledArgs     );
        COPY_ELEMENT(ElementType::Circle,          CircleArgs         );
        COPY_ELEMENT(ElementType::CircleFilled,    CircleFilledArgs   );
        COPY_ELEMENT(ElementType::Ellipse,         EllipseArgs        );
        COPY_ELEMENT(ElementType::EllipseFilled,   EllipseFilledArgs  );
        COPY_ELEMENT(ElementType::BezierCubic,     BezierCubicArgs    );
        COPY_ELEMENT(ElementType::BezierQuadratic, BezierQuadraticArgs);
        COPY_ELEMENT(ElementType::Polyline,        PolylineArgs       );
        COPY_ELEMENT(ElementType::PolygonFilled,   PolygonFilledArgs  );
        COPY_ELEMENT(ElementType::Text,            TextArgs           );
        COPY_ELEMENT(ElementType::Texture,         TextureArgs        );
        default:
            break;
        }
        ++element;
    }
    return *this;
}

float VectorGraphics::GetWidth() const
{
    return Size.x;
}

float VectorGraphics::GetHeight() const
{
    return Size.y;
}

ImVec2 VectorGraphics::GetSize() const
{
    return Size;
}

#define ADD_ELEMENT(TYPE, ARG_TYPE) \
    Elements.push_back(Element(TYPE, nullptr));\
    Elements.back().Args = new ARG_TYPE

void VectorGraphics::AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness)
{ ADD_ELEMENT(ElementType::Line, LineArgs) { p1, p2, col, thickness }; }

void VectorGraphics::AddRect(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness, float rounding, ImDrawFlags flags)
{ ADD_ELEMENT(ElementType::Rect, RectArgs) { p1, p2, col, thickness }; }

void VectorGraphics::AddRectFilled(const ImVec2& p1, const ImVec2& p2, ImU32 col, float rounding, ImDrawFlags flags)
{ ADD_ELEMENT(ElementType::RectFilled, RectFilledArgs) { p1, p2, col, rounding, flags }; }

void VectorGraphics::AddCircle(const ImVec2& center, float radius, ImU32 col, float thickness, int num_segments)
{ ADD_ELEMENT(ElementType::Circle, CircleArgs) { center, radius, col, thickness, num_segments }; }

void VectorGraphics::AddCircleFilled(const ImVec2& center, float radius, ImU32 col, int num_segments)
{ ADD_ELEMENT(ElementType::CircleFilled, CircleFilledArgs) { center, radius, col, num_segments }; }

void VectorGraphics::AddEllipse(const ImVec2& center, const ImVec2& radius, ImU32 col, float rot, float thickness, int num_segments)
{ ADD_ELEMENT(ElementType::Ellipse, EllipseArgs) { center, radius, rot, col, thickness, num_segments }; }

void VectorGraphics::AddEllipseFilled(const ImVec2& center, const ImVec2& radius, ImU32 col, float rot, int num_segments)
{ ADD_ELEMENT(ElementType::EllipseFilled, EllipseFilledArgs) { center, radius, rot, col, num_segments }; }

void VectorGraphics::AddBezierCubic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments)
{ ADD_ELEMENT(ElementType::BezierCubic, BezierCubicArgs) { p1, p2, p3, p4, col, thickness, num_segments }; }

void VectorGraphics::AddBezierQuadratic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness, int num_segments)
{ ADD_ELEMENT(ElementType::BezierQuadratic, BezierQuadraticArgs) { p1, p2, p3, col, thickness, num_segments }; }

void VectorGraphics::AddPolyline(const ImVec2* points, int num_points, ImU32 col, float thickness, ImDrawFlags flags)
{
    PolylineArgs* args = new PolylineArgs{ {}, col, thickness, flags };
    args->Points.resize(num_points);
    memcpy(args->Points.Data, points, sizeof(ImVec2) * num_points);
    Elements.push_back(Element(ElementType::Polyline, args));
}

void VectorGraphics::AddPolygonFilled(const ImVec2* points, int num_points, ImU32 col)
{
    PolygonFilledArgs* args = new PolygonFilledArgs{ {}, col };
    args->Points.resize(num_points);
    memcpy(args->Points.Data, points, sizeof(ImVec2) * num_points);
    Elements.push_back(Element(ElementType::PolygonFilled, args));
}

void VectorGraphics::AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end)
{
    AddText(nullptr, 0.0f, pos, col, text_begin, text_end); 
}

void VectorGraphics::AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
{
    TextArgs* args = new TextArgs{ pos, col, {}, font, font_size, wrap_width, {}, cpu_fine_clip_rect != nullptr };
    size_t text_len = text_end
        ? text_end - text_begin
        : strlen(text_begin);
    args->Text.resize(text_len);
    memcpy(args->Text.Data, text_begin, text_len);
    if (cpu_fine_clip_rect)
        args->ClipRect = *cpu_fine_clip_rect;
    Elements.push_back(Element(ElementType::Text, args));
}

void VectorGraphics::AddTexture(ImTextureID texture, const ImVec2& p1, const ImVec2& p2, const ImVec2& uv1, const ImVec2& uv2, ImU32 col)
{ ADD_ELEMENT(ElementType::Texture, TextureArgs) { texture, p1, p2, uv1, uv2, col }; }

void VectorGraphics::Show(const ImVec2& size) const
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return;

    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
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
    const double scale = fmin(size.x / Size.x, size.y / Size.y);
    const ImVec2 offset = ImVec2((size.x - Size.x * scale) / 2, (size.y - Size.y * scale) / 2) + p1;
#define RESIZE_POINT(POINT) offset + POINT * scale
#define RESIZE_THINKNESS(THINKNESS) THINKNESS == 0 ? 1 : THINKNESS * scale

    VectorGraphics* p = const_cast<VectorGraphics*>(this);
    const Element* element     = begin();
    const Element* element_end = end();
    while (element < element_end)
    {
        switch (element->GetType())
        {
        case ElementType::Line:
        {
            const LineArgs* args = element->GetArgs<LineArgs>();
            draw_list->AddLine(RESIZE_POINT(args->P1),
                               RESIZE_POINT(args->P2),
                               args->Color,
                               RESIZE_THINKNESS(args->Thinkness));
            break;
        }
        case ElementType::Rect:
        {
            const RectArgs* args = element->GetArgs<RectArgs>();
            draw_list->AddRect(RESIZE_POINT(args->P1),
                               RESIZE_POINT(args->P2),
                               args->Color,
                               args->Rounding * scale,
                               args->Flags,
                               RESIZE_THINKNESS(args->Thinkness));
            break;
        }
        case ElementType::RectFilled:
        {
            const RectFilledArgs* args = element->GetArgs<RectFilledArgs>();
            draw_list->AddRectFilled(RESIZE_POINT(args->P1),
                                     RESIZE_POINT(args->P2),
                                     args->Color,
                                     args->Rounding * scale,
                                     args->Flags);
            break;
        }
        case ElementType::Circle:
        {
            const CircleArgs* args = element->GetArgs<CircleArgs>();
            draw_list->AddCircle(RESIZE_POINT(args->Center),
                                 args->Radius * scale,
                                 args->Color,
                                 args->Segments,
                                 RESIZE_THINKNESS(args->Thinkness));
            break;
        }
        case ElementType::CircleFilled:
        {
            const CircleFilledArgs* args = element->GetArgs<CircleFilledArgs>();
            draw_list->AddCircleFilled(RESIZE_POINT(args->Center),
                                       args->Radius * scale,
                                       args->Color,
                                       args->Segments);
            break;
        }
        case ElementType::Ellipse:
        {
            const EllipseArgs* args = element->GetArgs<EllipseArgs>();
            draw_list->AddEllipse(RESIZE_POINT(args->Center),
                                  args->Radius.x * scale,
                                  args->Radius.y * scale,
                                  args->Color,
                                  args->Rotation,
                                  args->Segments,
                                  RESIZE_THINKNESS(args->Thinkness));
            break;
        }
        case ElementType::EllipseFilled:
        {
            const EllipseFilledArgs* args = element->GetArgs<EllipseFilledArgs>();
            draw_list->AddEllipseFilled(RESIZE_POINT(args->Center),
                                        args->Radius.x * scale,
                                        args->Radius.y * scale,
                                        args->Color,
                                        args->Rotation,
                                        args->Segments);
            break;
        }
        case ElementType::BezierCubic:
        {
            const BezierCubicArgs* args = element->GetArgs<BezierCubicArgs>();
            draw_list->AddBezierCubic(RESIZE_POINT(args->P1),
                                      RESIZE_POINT(args->P2),
                                      RESIZE_POINT(args->P3),
                                      RESIZE_POINT(args->P4),
                                      args->Color,
                                      RESIZE_THINKNESS(args->Thinkness),
                                      args->Segments);
            break;
        }
        case ElementType::BezierQuadratic:
        {
            const BezierQuadraticArgs* args = element->GetArgs<BezierQuadraticArgs>();
            draw_list->AddBezierQuadratic(RESIZE_POINT(args->P1),
                                          RESIZE_POINT(args->P2),
                                          RESIZE_POINT(args->P3),
                                          args->Color,
                                          RESIZE_THINKNESS(args->Thinkness),
                                          args->Segments);
            break;
        }
        case ElementType::Polyline:
        {
            const PolylineArgs* args = element->GetArgs<PolylineArgs>();
            if (PointBuffer.size() < args->Points.size())
                p->PointBuffer.resize(args->Points.size());
            for (size_t i = 0; i < args->Points.size(); ++i)
                p->PointBuffer[i] = RESIZE_POINT(args->Points[i]);
            draw_list->AddPolyline(PointBuffer.Data, args->Points.size(),
                                   args->Color,
                                   args->Flags,
                                   RESIZE_THINKNESS(args->Thinkness));
            break;
        }
        case ElementType::PolygonFilled:
        {
            const PolygonFilledArgs* args = element->GetArgs<PolygonFilledArgs>();
            if (PointBuffer.size() < args->Points.size())
                p->PointBuffer.resize(args->Points.size());
            for (size_t i = 0; i < args->Points.size(); i++)
                p->PointBuffer[i] = RESIZE_POINT(args->Points[i]);
            draw_list->AddConvexPolyFilled(PointBuffer.Data, args->Points.size(), args->Color);
            break;
        }
        case ElementType::Text:
        {
            const TextArgs* args = element->GetArgs<TextArgs>();
            ImVec4 clip_rect = args->ClipRect;
            if (args->UseClip)
            {
                clip_rect.x = offset.x + clip_rect.x * scale;
                clip_rect.y = offset.y + clip_rect.y * scale;
                clip_rect.z = offset.x + clip_rect.z * scale;
                clip_rect.w = offset.y + clip_rect.w * scale;
            }
            draw_list->AddText(args->Font,
                               args->FontSize * scale,
                               RESIZE_POINT(args->Position),
                               args->Color,
                               args->Text.begin(), args->Text.end(),
                               args->WrapWidth * scale,
                               args->UseClip ? &clip_rect : nullptr);
            break;
        }
        case ElementType::Texture:
        {
            const TextureArgs* args = element->GetArgs<TextureArgs>();
            draw_list->AddImage(args->Texture,
                                RESIZE_POINT(args->P1),
                                RESIZE_POINT(args->P2),
                                args->UV1,
                                args->UV2,
                                args->Color);
            break;
        }
        default:
            break;
        }
        ++element;
    }
}

VectorGraphics::Element* VectorGraphics::begin() { return Elements.begin(); }
VectorGraphics::Element* VectorGraphics::end()   { return Elements.end(); }
const VectorGraphics::Element* VectorGraphics::begin() const { return Elements.begin(); }
const VectorGraphics::Element* VectorGraphics::end() const   { return Elements.end(); }

bool VGButton(const VectorGraphics& vg, const ImVec2& size)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;
    const ImVec2 padding = ImGui::GetStyle().FramePadding;
    bool pressed = ImGui::Button("##", size);
    vg.Draw(window->DrawList, ImGui::GetItemRectMin() + padding, ImGui::GetItemRectMax() - padding);
    return pressed;
}

}
