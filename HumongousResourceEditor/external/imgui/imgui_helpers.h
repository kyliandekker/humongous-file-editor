#ifndef IMGUI_HELPERS_H
#define IMGUI_HELPERS_H

#include <imgui/imgui.h>

#include <stdint.h>
#include <string>
#include <map>

namespace ImGui
{
    ImVec2 operator+(ImVec2 const &a, ImVec2 const &b);
    ImVec2 operator-(ImVec2 const &a, ImVec2 const &b);

    void ShowTooltipOnHover(
        char const *tooltip);

    bool DropDown(
        char const *label,
        unsigned char &value,
        char const *const names[],
        unsigned int nameCount,
        char const *tooltip);

    void TextBox(
        char const *label,
        ImVec2 const &size);

    bool CheckboxButton(const char *label, bool *p_value, const ImVec2 &size_arg = ImVec2(0, 0));
    bool InvisButton(const char *label, const ImVec2 &size_arg = ImVec2(0, 0));

	bool RangeSliderFloat(const char *label, float *v1, float *v2, float v_min, float v_max, const char *display_format = "(%.3f, %.3f)", float power = 1.0f);

    void UvMeter(char const* label, ImVec2 const& size, double* value, float v_min, float v_max, int steps = 10, float* stack = nullptr, int* count = nullptr, float background = 0.f, std::map<float, float> segment = {});
    void UvMeter(char const* label, ImVec2 const& size, float* value, float v_min, float v_max, int steps = 10, float* stack = nullptr, int* count = nullptr, float background = 0.f, std::map<float, float> segment = {});
    void UvMeter(ImDrawList* draw_list, char const* label, ImVec2 const& size, float* value, float v_min, float v_max, int steps = 10, float* stack = nullptr, int* count = nullptr, float background = 0.f, std::map<float, float> segment = {});

#endif // IMGUI_HELPERS_H
}