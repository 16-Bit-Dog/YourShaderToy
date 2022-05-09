#pragma once
#include "imgui.h"
#include <../imGUI/imgui_stdlib.h>
#include <string>
#include <functional>

namespace ImGui {

	static void GreenText(const char* s) {
		ImGui::TextColored({ 0.1f, 1.0f, 0.1f, 1.0f }, s);
	}
	static void RedText(const char* s) {
		ImGui::TextColored({ 1.0f, 0.1f, 0.1f, 1.0f }, s);
	}

	static void OpenRed() {
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.1f, 0.1f, 1.0f });
	}
	static void CloseRed() {
		ImGui::PopStyleColor();
	}
	static void OpenGreen() {
		ImGui::PushStyleColor(ImGuiCol_Text, { 0.1f, 1.0f, 0.1f, 1.0f });
	}
	static void CloseGreen() {
		ImGui::PopStyleColor();
	}

	static bool CollapsingHeaderOpenRed(const char* s) {
		OpenRed();
		if (ImGui::CollapsingHeader(s)) {
			CloseRed();
			return true;
		}
		CloseRed();
		return false;
	}
	static bool CollapsingHeaderOpenGreen(const char* s) {
		OpenGreen();
		if (ImGui::CollapsingHeader(s)) {
			CloseGreen();
			return true;
		}
		CloseGreen();
		return false;
	}

	static bool BeginMenuGreen(const char* s) {
		OpenGreen();
		if (ImGui::BeginMenu(s)) {
			CloseGreen();
			return true;
		}
		CloseGreen();
		return false;
	}
	static bool BeginMenuRed(const char* s) {
		OpenRed();
		if (ImGui::BeginMenu(s)) {
			CloseRed();
			return true;
		}
		CloseRed();
		return false;
	}

	static void InputTextMultilineQuick(std::string Tag, std::string* S, ImGuiInputTextFlags* flags) {
		ImGui::InputTextMultiline(Tag.c_str(), S, ImVec2(ImGui::GetWindowWidth(), GetWindowHeight() * 0.5), *flags, NULL, nullptr);
	}
	static void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
	static bool CheckboxCallback(const std::string& s, bool* b, std::function<void()> f) {
		if (ImGui::Checkbox(s.c_str(), b)) {
			f();
			return true;
		}
		else {
			return false;
		}
	}
	static float GetContentRegionHere() {
		return ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;
	}
	static bool BeginChildBorderFixedSize(const std::string& s) { //has a max

		return ImGui::BeginChild(s.c_str(), {0,ImGui::GetWindowContentRegionMax().y/2.0f}, true);
	}

}