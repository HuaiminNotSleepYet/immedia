#ifndef IMMEDIA_VECTOR_GRAPHICS_H
#define IMMEDIA_VECTOR_GRAPHICS_H

#include "imgui.h"

namespace ImMedia {

class VectorGraphics
{
public:
    VectorGraphics(const ImVec2& size);
    ~VectorGraphics();

    VectorGraphics(const VectorGraphics&);
    VectorGraphics& operator=(const VectorGraphics&);
    VectorGraphics(VectorGraphics&&) = default;
    VectorGraphics& operator=(VectorGraphics&&) = default;

    ImVec2 GetSize() const;

    void AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 0.0f);
    void AddRect(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 0.0f, float rounding = 0.0f, ImDrawFlags flags = 0);
    void AddRectFilled(const ImVec2& p1, const ImVec2& p2, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0);
    void AddCircle(const ImVec2& center, float radius, ImU32 col, float thickness = 0.0f, int num_segments = 0);
    void AddCircleFilled(const ImVec2& center, float radius, ImU32 col, int num_segments = 0);
    void AddEllipse(const ImVec2& center, const ImVec2& radius, ImU32 col, float thickness = 0.0f, float rot = 0.0f, int num_segments = 0);
    void AddEllipseFilled(const ImVec2& center, const ImVec2& radius, ImU32 col, float rot = 0.0f, int num_segments = 0);
    void AddBezierCubic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness = 0.0f, int num_segments = 0);
    void AddBezierQuadratic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness = 0.0f, int num_segments = 0);
    void AddPolyline(const ImVec2* points, int num_points, ImU32 col, float thickness = 0.0f, ImDrawFlags flags = 0);
    void AddPolygonFilled(const ImVec2* points, int num_points, ImU32 col);
    void AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = nullptr);
    void AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = nullptr, float wrap_width = 0.0f, const ImVec4* cpu_fine_clip_rect = nullptr);
    void AddTexture(ImTextureID texture, const ImVec2& p1, const ImVec2& p2, const ImVec2& uv1 = { 0, 0 }, const ImVec2& uv2 = { 1, 1 }, ImU32 col = IM_COL32_WHITE);

    void Show(const ImVec2& size) const;
    void Draw(ImDrawList* draw_list, const ImVec2& p1, const ImVec2& p2) const;

public:
    struct LineArgs            { ImVec2 P1; ImVec2 P2; ImU32 Color; float Thinkness; };
    struct RectArgs            { ImVec2 P1; ImVec2 P2; ImU32 Color; float Thinkness; float Rounding; ImDrawFlags Flags; };
    struct RectFilledArgs      { ImVec2 P1; ImVec2 P2; ImU32 Color; float Rounding; ImDrawFlags Flags; };
    struct CircleArgs          { ImVec2 Center; float Radius; ImU32 Color; float Thinkness; int Segments; };
    struct CircleFilledArgs    { ImVec2 Center; float  Radius; ImU32  Color; int    Segments; };
    struct EllipseArgs         { ImVec2 Center; ImVec2 Radius; float Rotation; ImU32 Color; float Thinkness; int Segments; };
    struct EllipseFilledArgs   { ImVec2 Center; ImVec2 Radius; float Rotation; ImU32 Color; int Segments; };
    struct BezierCubicArgs     { ImVec2 P1; ImVec2 P2; ImVec2 P3; ImVec2 P4; ImU32 Color; float Thinkness; int Segments; };
    struct BezierQuadraticArgs { ImVec2 P1; ImVec2 P2; ImVec2 P3; ImU32 Color; float Thinkness; int Segments; };
    struct PolylineArgs        { ImVector<ImVec2> Points; ImU32 Color; float Thinkness; ImDrawFlags Flags; };
    struct PolygonFilledArgs   { ImVector<ImVec2> Points; ImU32 Color; };
    struct TextArgs            { ImVec2 Position; ImU32 Color; ImVector<char> Text; const ImFont* Font; float FontSize; float WrapWidth; ImVec4 ClipRect; bool UseClip; };
    struct TextureArgs         { ImTextureID Texture; ImVec2 P1; ImVec2 P2; ImVec2 UV1; ImVec2 UV2; ImU32 Color; };

    enum ElementType
    {
        Line,
        Rect,
        RectFilled,
        Circle,
        CircleFilled,
        Ellipse,
        EllipseFilled,
        BezierCubic,
        BezierQuadratic,
        Polyline,
        PolygonFilled,
        Texture,
        Text
    };

    class Element
    {
        friend VectorGraphics;

    public:
        ElementType GetType() const { return Type; }

        template<typename T>       T* GetArgs()       { return reinterpret_cast<T*>(Args); }
        template<typename T> const T* GetArgs() const { return reinterpret_cast<T*>(Args); }

    private:
        ElementType Type;
        void*       Args;

        Element(ElementType type, void* args) : Type(type), Args(args) {};
    };

    Element* begin();
    Element* end();
    const Element* begin() const;
    const Element* end() const;

private:
    ImVec2            Size;
    ImVector<Element> Elements;

    ImVector<ImVec2>  PointBuffer;
};

bool VGButton(const VectorGraphics& vg, const ImVec2& size);

}

#endif // IMMEDIA_VECTOR_GRAPHICS_H
