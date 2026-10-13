#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <TextEditor.h>
#include <chrono>
#include <string>

#include <Roblox/Update/Offsets.hpp>
#include <Roblox/Hyperion/TaskScheduler/TaskScheduler.hpp>
#include <Roblox/Shared/Shared.hpp>
#include <Roblox/Environment/Libraries/Drawing.hpp>
#include <Roblox/Environment/Libraries/Raknet.hpp>
#include <others/imgui_notify.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace RenderHook
{
    static TextEditor Editor;

    inline ID3D11Device* d3d11_device = nullptr;
    inline ID3D11DeviceContext* d3d11_device_context = nullptr;
    inline IDXGISwapChain* d3d11_swap_chain = nullptr;

    using PresentFn = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT);
    using ResizeBuffersFn = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

    ID3D11RenderTargetView* m_RenderTargetView = nullptr;

    HWND WindowHandle = nullptr;

    inline PresentFn OriginalPresent = nullptr;
    inline ResizeBuffersFn OriginalResizeBuffers = nullptr;
    inline WNDPROC OriginalWndProc = nullptr;

    bool imguiinitialized;

    bool renderinterface;

    float g_dpiScale = 1.0f;

    inline std::chrono::steady_clock::time_point g_injectTime = std::chrono::steady_clock::now();

    template<typename T>
    void SafeRelease(T*& ptr) {
        if (ptr) {
            ptr->Release();
            ptr = nullptr;
        }
    }

    void SetupCustomImGuiStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();

        style.WindowRounding = 6.0f;
        style.ChildRounding = 6.0f;
        style.FrameRounding = 4.0f;
        style.PopupRounding = 6.0f;
        style.ScrollbarRounding = 6.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 4.0f;

        style.Alpha = 1.0f;
        style.AntiAliasedLines = true;
        style.AntiAliasedFill = true;

        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

        style.WindowPadding = ImVec2(8.0f * g_dpiScale, 8.0f * g_dpiScale);
        style.FramePadding = ImVec2(4.0f * g_dpiScale, 3.0f * g_dpiScale);
        style.CellPadding = ImVec2(4.0f * g_dpiScale, 2.0f * g_dpiScale);
        style.ItemSpacing = ImVec2(8.0f * g_dpiScale, 4.0f * g_dpiScale);
        style.ItemInnerSpacing = ImVec2(4.0f * g_dpiScale, 4.0f * g_dpiScale);
        style.ScrollbarSize = 14.0f * g_dpiScale;
        style.GrabMinSize = 10.0f * g_dpiScale;
        style.WindowBorderSize = 1.0f * g_dpiScale;
        style.ChildBorderSize = 1.0f * g_dpiScale;
        style.PopupBorderSize = 1.0f * g_dpiScale;
        style.FrameBorderSize = 0.0f;
        style.TabBorderSize = 0.0f;
        style.WindowMinSize = ImVec2(32.0f * g_dpiScale, 32.0f * g_dpiScale);

        ImVec4* colors = style.Colors;
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.54f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.54f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.10f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 0.54f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 0.54f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.45f, 0.45f, 0.54f);
        colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.25f, 0.54f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.35f, 0.35f, 0.54f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.45f, 0.45f, 0.45f, 0.54f);
        colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.50f, 0.50f, 0.50f, 0.54f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.30f, 0.30f, 0.54f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.50f, 0.50f, 0.54f);
        colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 0.86f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.86f);
        colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 0.97f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.60f, 0.15f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    bool InitializeImGui()
    {
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigDockingWithShift = false;
        io.ConfigDockingTransparentPayload = true;
        io.IniFilename = nullptr;

        g_dpiScale = static_cast<float>(GetDpiForWindow(WindowHandle)) / 96.0f;
        if (g_dpiScale <= 0.0f)
            g_dpiScale = 1.0f;

        Drawing::g_dpiScale = g_dpiScale;

        SetupCustomImGuiStyle();

        if (!m_RenderTargetView)
        {
            ID3D11Texture2D* tempBackBuffer = nullptr;

            if (FAILED(d3d11_swap_chain->GetBuffer(
                0,
                __uuidof(ID3D11Texture2D),
                reinterpret_cast<void**>(&tempBackBuffer))))
            {
                return false;
            }

            HRESULT hr = d3d11_device->CreateRenderTargetView(
                tempBackBuffer,
                nullptr,
                &m_RenderTargetView
            );

            tempBackBuffer->Release();

            if (FAILED(hr))
                return false;
        }

        if (!ImGui_ImplWin32_Init(WindowHandle))
            return false;

        if (!ImGui_ImplDX11_Init(
            d3d11_device,
            d3d11_device_context))
            return false;

        ImFontConfig fontCfg;
        fontCfg.OversampleH = 2;
        fontCfg.OversampleV = 2;
        fontCfg.PixelSnapH = false;

        const float fontSize = 14.0f * g_dpiScale;

        ImFont* font = io.Fonts->AddFontFromFileTTF(
            "C:\\Windows\\Fonts\\segoeuib.ttf",
            fontSize,
            &fontCfg,
            io.Fonts->GetGlyphRangesDefault()
        );

        if (!font)
            font = io.Fonts->AddFontDefault();

        if (!font)
            return false;

        io.FontDefault = font;

        for (int i = 0; i < 4; ++i)
            Drawing::g_Fonts[i] = font;

        ImGui::MergeIconsWithLatestFont(14.0f * g_dpiScale, false);

        Editor.SetLanguageDefinition(TextEditor::Lua());
        Editor.SetText("print(\"Hello, world!\")");

        return true;
    }

    HRESULT WINAPI Present_Hook(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {

        if (!imguiinitialized) {
            bool success = InitializeImGui();
            if (success) {
                imguiinitialized = true;
            }
            else {
                return OriginalPresent(swapChain, syncInterval, flags);
            }
        }

        if (!m_RenderTargetView) {
            ID3D11Texture2D* tempBackBuffer = nullptr;
            if (SUCCEEDED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&tempBackBuffer)))) {
                d3d11_device->CreateRenderTargetView(tempBackBuffer, nullptr, &m_RenderTargetView);
                tempBackBuffer->Release();
            }
        }


        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        Drawing::RenderAll(ImGui::GetBackgroundDrawList());

        if (renderinterface) {

            static bool s_mainSizeSet = false;
            ImGui::Begin("ExBeta", nullptr);
            if (!s_mainSizeSet) {
                ImGui::SetWindowSize(ImVec2(600.0f * g_dpiScale, 355.0f * g_dpiScale));
                s_mainSizeSet = true;
            }
            {
                if (ImGui::BeginTabBar("#Tabs", ImGuiTabBarFlags_None))
                {

                    if (ImGui::BeginTabItem("Execution"))
                    {
                        if (ImGui::Button("Execute")) {
                            TaskScheduler::SendScript(Editor.GetText());
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Clear")) {
                            Editor.SetText("");
                        }

                        Editor.Render("TextEditor");
                        ImGui::Separator();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Scripts"))
                    {
                        if (ImGui::Button("Infinite Yield")) {
                            TaskScheduler::SendScript(R"""(loadstring(game:HttpGet("https://raw.githubusercontent.com/edgeiy/infiniteyield/master/source",true))())""");
                        }
                        if (ImGui::Button("UNC Test")) {
                            TaskScheduler::SendScript(R"""(loadstring(game:HttpGet("https://raw.githubusercontent.com/unified-naming-convention/NamingStandard/refs/heads/main/UNCCheckEnv.lua",true))())""");
                        }

                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Settings"))
                    {
                        if (ImGui::Button("Unload"))
                            exit(EXIT_SUCCESS);

                        ImGui::EndTabItem();
                    }

                    ImGui::EndTabBar();
                }
            }
            ImGui::End();

            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - g_injectTime).count();
            int hours = static_cast<int>(elapsed / 3600);
            int minutes = static_cast<int>((elapsed % 3600) / 60);
            int seconds = static_cast<int>(elapsed % 60);

            char timeBuf[64];
            sprintf_s(timeBuf, "%02d:%02d:%02d", hours, minutes, seconds);

            std::string workspace = Shared::workspace_folder.empty()
                ? "not set"
                : Shared::workspace_folder.string();

            static bool s_infoSizeSet = false;
            ImGui::Begin("ExBeta Info", nullptr);
            if (!s_infoSizeSet) {
                ImGui::SetWindowSize(ImVec2(360.0f * g_dpiScale, 90.0f * g_dpiScale));
                s_infoSizeSet = true;
            }
            {
                ImGui::TextUnformatted(timeBuf);
                ImGui::TextUnformatted(workspace.c_str());
            }
            ImGui::End();

            static bool s_rakViewerSizeSet = false;
            ImGui::Begin("Raknet Viewer", nullptr);
            if (!s_rakViewerSizeSet) {
                ImGui::SetWindowSize(ImVec2(720.0f * g_dpiScale, 460.0f * g_dpiScale));
                s_rakViewerSizeSet = true;
            }
            {
                static const char* priorityNames[] = { "IMMEDIATE", "HIGH", "MEDIUM", "LOW" };
                static const char* reliabilityNames[] = { "UNRELIABLE", "UNRELIABLE_SEQ", "RELIABLE", "RELIABLE_ORDERED", "RELIABLE_SEQ", "UNRELIABLE_ACK", "RELIABLE_ACK", "RELIABLE_ORDERED_ACK" };

                auto packets = Raknet::SnapshotLog();

                static uint64_t selectedSeq = 0;

                if (ImGui::Button(Raknet::capturePaused ? "Resume" : "Pause"))
                    Raknet::capturePaused = !Raknet::capturePaused;
                ImGui::SameLine();
                if (ImGui::Button("Clear"))
                    Raknet::ClearLog();
                ImGui::SameLine();
                ImGui::TextUnformatted(("Captured: " + std::to_string(packets.size())).c_str());
                ImGui::Separator();

                float availH = ImGui::GetContentRegionAvail().y;
                float listH = availH * 0.45f;

                if (ImGui::BeginChild("##rakpackets_list", ImVec2(0, listH), true))
                {
                    if (ImGui::BeginTable("##rakpackets", 6, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollY))
                    {
                        ImGui::TableSetupColumn("Id");
                        ImGui::TableSetupColumn("Item");
                        ImGui::TableSetupColumn("Priority");
                        ImGui::TableSetupColumn("Reliability");
                        ImGui::TableSetupColumn("Size");
                        ImGui::TableSetupColumn("Blocked");
                        ImGui::TableHeadersRow();

                        for (auto it = packets.rbegin(); it != packets.rend(); ++it)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);

                            char label[64];
                            sprintf_s(label, "0x%02X##%llu", it->id, it->seq);
                            if (ImGui::Selectable(label, it->seq == selectedSeq, ImGuiSelectableFlags_SpanAllColumns))
                                selectedSeq = it->seq;

                            ImGui::TableSetColumnIndex(1);
                            ImGui::TextUnformatted(Raknet::ItemTypeName(it->itemId));

                            ImGui::TableSetColumnIndex(2);
                            ImGui::TextUnformatted(priorityNames[it->priority < Raknet::NUMBER_OF_PRIORITIES ? it->priority : 0]);

                            ImGui::TableSetColumnIndex(3);
                            ImGui::TextUnformatted(reliabilityNames[it->reliability < Raknet::NUMBER_OF_RELIABILITIES ? it->reliability : 0]);

                            ImGui::TableSetColumnIndex(4);
                            std::string sizeStr = std::to_string(it->size);
                            if (it->truncated)
                                sizeStr += " (trunc)";
                            ImGui::TextUnformatted(sizeStr.c_str());

                            ImGui::TableSetColumnIndex(5);
                            ImGui::TextUnformatted(it->blocked ? "YES" : "no");
                        }

                        ImGui::EndTable();
                    }
                }
                ImGui::EndChild();

                if (selectedSeq != 0)
                {
                    for (auto it = packets.rbegin(); it != packets.rend(); ++it)
                    {
                        if (it->seq != selectedSeq)
                            continue;

                        if (ImGui::BeginChild("##rakdetails", ImVec2(0, 0), true))
                        {
                            char line[256];
                            sprintf_s(line, "Id: 0x%02X | Item: %s | Size: %u bytes (%u bits)%s",
                                it->id, Raknet::ItemTypeName(it->itemId), it->size, it->bitsUsed,
                                it->truncated ? " (trunc)" : "");
                            ImGui::TextUnformatted(line);
                            sprintf_s(line, "Priority: %s | Reliability: %s | Channel: %d | Broadcast: %s | ForceReceipt: %u | Blocked: %s",
                                priorityNames[it->priority < Raknet::NUMBER_OF_PRIORITIES ? it->priority : 0],
                                reliabilityNames[it->reliability < Raknet::NUMBER_OF_RELIABILITIES ? it->reliability : 0],
                                it->channel, it->broadcast ? "yes" : "no", it->forceReceipt,
                                it->blocked ? "YES" : "no");
                            ImGui::TextUnformatted(line);
                            if (!it->summary.empty())
                                ImGui::TextUnformatted(it->summary.c_str());
                            ImGui::Separator();

                            for (size_t off = 0; off < it->bytes.size(); off += 16)
                            {
                                char hexline[96];
                                int n = sprintf_s(hexline, sizeof(hexline), "%04X  ", (int)off);
                                for (size_t j = 0; j < 16; j++)
                                {
                                    if (off + j < it->bytes.size())
                                        n += sprintf_s(hexline + n, sizeof(hexline) - n, "%02X ", it->bytes[off + j]);
                                    else
                                        n += sprintf_s(hexline + n, sizeof(hexline) - n, "   ");
                                }
                                n += sprintf_s(hexline + n, sizeof(hexline) - n, " | ");
                                for (size_t j = 0; j < 16 && off + j < it->bytes.size(); j++)
                                {
                                    uint8_t c = it->bytes[off + j];
                                    hexline[n++] = (c >= 32 && c < 127) ? (char)c : '.';
                                }
                                hexline[n] = 0;
                                ImGui::TextUnformatted(hexline);
                            }

                            std::string ascii;
                            ascii.reserve(it->bytes.size());
                            for (uint8_t c : it->bytes)
                                ascii += (c >= 32 && c < 127) ? (char)c : '.';
                            ImGui::Separator();
                            ImGui::TextUnformatted("ASCII:");
                            ImGui::TextWrapped(ascii.c_str());
                        }
                        ImGui::EndChild();
                        break;
                    }
                }
            }
            ImGui::End();
        }


        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f));
        ImGui::RenderNotifications();
        ImGui::PopStyleVar(1); // Don't forget to Pop()
        ImGui::PopStyleColor(1);



        ImGui::Render();


        d3d11_device_context->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        return OriginalPresent(swapChain, syncInterval, flags);
    }

    HRESULT WINAPI ResizeBuffers_Hook(IDXGISwapChain* swapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
        SafeRelease(m_RenderTargetView);

        return OriginalResizeBuffers(swapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    }

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if (msg == WM_KEYDOWN) {
            if (wparam == VK_INSERT) {
                renderinterface = !renderinterface;
            }
        }

        if (renderinterface && ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
            return true;

        switch (msg) {
        case WM_MOUSEWHEEL:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_KEYUP:
        case WM_CAPTURECHANGED:
        case WM_NCACTIVATE:
        case WM_CHAR:
        case WM_NCHITTEST:
        case WM_GETICON:
        case WM_INPUT:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_APPCOMMAND:
            if (renderinterface)
                return true;
            break;
        }

        return CallWindowProc(OriginalWndProc, hwnd, msg, wparam, lparam);
    }

    bool StartHook() {
        static bool s_failureReported = false;
        auto Fail = [&](const char* msg) {
            if (!s_failureReported) {
                MessageBoxA(nullptr, msg, "ExBeta", MB_OK | MB_ICONERROR);
                s_failureReported = true;
            }
            };

        uintptr_t renderJob = TaskScheduler::GetJobByName("RenderJob");
        if (!renderJob) {
            while (true) {
                uintptr_t renderJob = TaskScheduler::GetJobByName("RenderJob");
                if (renderJob) {
                    break;
                }
                Sleep(1000);
            }
        }

        uintptr_t viewBase = *reinterpret_cast<uintptr_t*>(renderJob + Offsets::Renderer::ViewBase);
        if (!viewBase) {
            Fail("RenderHook: invalid viewBase");
            return false;
        }

        uintptr_t robloxDevice = *reinterpret_cast<uintptr_t*>(viewBase + Offsets::Renderer::Device);
        if (!robloxDevice) {
            Fail("RenderHook: invalid robloxDevice");
            return false;
        }

        d3d11_swap_chain = *reinterpret_cast<IDXGISwapChain**>(robloxDevice + Offsets::Renderer::SwapChain);
        if (!d3d11_swap_chain) {
            Fail("RenderHook: invalid swap chain");
            return false;
        }

        DXGI_SWAP_CHAIN_DESC swap_chain_desc;
        if (FAILED(d3d11_swap_chain->GetDesc(&swap_chain_desc))) {
            Fail("RenderHook: GetDesc failed");
            return false;
        }

        WindowHandle = swap_chain_desc.OutputWindow;

        if (FAILED(d3d11_swap_chain->GetDevice(__uuidof(ID3D11Device), (void**)(&d3d11_device)))) {
            Fail("RenderHook: GetDevice failed");
            return false;
        }

        d3d11_device->GetImmediateContext(&d3d11_device_context);

        static size_t vtableSize = 18;
        uintptr_t* originalVTable = *reinterpret_cast<uintptr_t**>(d3d11_swap_chain);

        uintptr_t* shadowVTable = new uintptr_t[vtableSize];
        memcpy(shadowVTable, originalVTable, sizeof(uintptr_t) * vtableSize);

        OriginalPresent = reinterpret_cast<PresentFn>(originalVTable[8]);
        shadowVTable[8] = reinterpret_cast<uintptr_t>(&Present_Hook);

        OriginalResizeBuffers = reinterpret_cast<ResizeBuffersFn>(originalVTable[13]);
        shadowVTable[13] = reinterpret_cast<uintptr_t>(&ResizeBuffers_Hook);

        *reinterpret_cast<uintptr_t**>(d3d11_swap_chain) = shadowVTable;

        OriginalWndProc = (WNDPROC)(SetWindowLongPtrW(WindowHandle, GWLP_WNDPROC, (LONG_PTR)WndProc));
        return true;
    }

    void Shutdown() {
        if (d3d11_device_context) d3d11_device_context->Release();
        if (d3d11_device) d3d11_device->Release();
    }
}
