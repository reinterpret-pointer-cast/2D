#pragma once

#include <fan/types/types.hpp>
#include <fan/window/window_input.hpp>

#include <fan/types/vector.hpp>

#ifdef fan_platform_unix

    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/Xos.h>
    #include <X11/Xatom.h>
    #include <X11/keysym.h>
    #include <X11/XKBlib.h>

    #include <GL/glxew.h>

    #include <sys/time.h>
    #include <unistd.h>

#endif


namespace fan {

	namespace sys {

#ifdef fan_platform_unix

        inline static Display* m_display;

        static Display* get_display()
        {
            return m_display;
        }

#endif

#if defined(fan_platform_windows)

		class input {
        public:

            static inline std::unordered_map<uint16_t, bool> key_down;
            static inline std::unordered_map<uint16_t, bool> reset_keys;

            static fan::vec2i get_mouse_position() {
                POINT p;
                GetCursorPos(&p);

                return fan::vec2i(p.x, p.y);
            }

            static void set_mouse_position(const fan::vec2i& position) {
                SetCursorPos(position.x, position.y);
            }

            static void send_mouse_event(uint16_t key, fan::key_state state) {

                constexpr auto get_key = [](uint16_t key, fan::key_state state) {

                    auto press = state == fan::key_state::press;

                    switch (key) {
                        case fan::mouse_left: {
                            if (press) {
                                return MOUSEEVENTF_LEFTDOWN;
                            }
                        
                            return MOUSEEVENTF_LEFTUP;
                        }

                        case fan::mouse_middle: {
                            if (press) {
                                return MOUSEEVENTF_MIDDLEDOWN;
                            }

                            return MOUSEEVENTF_MIDDLEUP;
                        }

                        case fan::mouse_right: {
                            if (press) {
                                return MOUSEEVENTF_RIGHTDOWN;
                            }

                            return MOUSEEVENTF_RIGHTUP;
                        }

                    }
                };

                INPUT input = {};

                input.type = INPUT_MOUSE;
                input.mi.dwFlags = get_key(key, state);

                SendInput(1, &input, sizeof(input));
            }

            inline static void send_keyboard_event(uint16_t key, fan::key_state state) {
                INPUT input;

                input.type = INPUT_KEYBOARD;
                input.ki.wScan = 0;
                input.ki.time = 0;
                input.ki.dwExtraInfo = 0;
                
                input.ki.wVk = fan::window_input::convert_fan_to_keys(key);

                input.ki.dwFlags = (state == fan::key_state::press ? 0 : KEYEVENTF_KEYUP);

                SendInput(1, &input, sizeof(INPUT));
            }

            // creates another thread, non blocking
            static void listen(std::function<void(uint16_t key, fan::key_state key_state, bool action, std::any data)> input_callback_) {

                input_callback = input_callback_;

                thread_loop();

            }

            static void listen(std::function<void(const fan::vec2i& position)> mouse_move_callback_) {

                mouse_move_callback = mouse_move_callback_;

                thread_loop();
            }

        private:

            static DWORD WINAPI thread_loop() {

                HINSTANCE hInstance = GetModuleHandle(NULL);

                if (!hInstance) {
                    fan::print("failed to initialize hinstance");
                    exit(1);
                }

                mouse_hook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)mouse_callback, hInstance, NULL);
                keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)keyboard_callback, hInstance, NULL);

                MSG message;

                while (GetMessage(&message, NULL, 0, 0))
                {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }

                UnhookWindowsHookEx(mouse_hook);
                UnhookWindowsHookEx(keyboard_hook);
            }


            static __declspec(dllexport) LRESULT CALLBACK keyboard_callback(int nCode, WPARAM wParam, LPARAM lParam)
            {
                KBDLLHOOKSTRUCT hooked_key = *((KBDLLHOOKSTRUCT*)lParam);

                auto key = fan::window_input::convert_keys_to_fan(hooked_key.vkCode);

                auto state = (fan::key_state)((nCode == HC_ACTION) && ((wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN)));

                if (state == fan::key_state::press && !reset_keys[key]) {
                    key_down[key] = true;
                }

                input_callback(key, state, key_down[key], 0);

                if (state == fan::key_state::release) {
                    reset_keys[key] = false;
                }
                else {
                    key_down[key] = false;
                    reset_keys[key] = true;
                }

                return CallNextHookEx(keyboard_hook, nCode, wParam, lParam);
            }
            //WM_LBUTTONDOWN
            static __declspec(dllexport) LRESULT CALLBACK mouse_callback(int nCode, WPARAM wParam, LPARAM lParam)
            {
                MSLLHOOKSTRUCT hooked_key = *((MSLLHOOKSTRUCT*)lParam);

                if (nCode != HC_ACTION) {
                    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
                }

                constexpr auto get_mouse_key = [](uint16_t key) {
                    switch (key) {

                        case WM_LBUTTONDOWN: { return fan::input::mouse_left; }
                        case WM_MBUTTONDOWN: { return fan::input::mouse_middle; }
                        case WM_RBUTTONDOWN: { return fan::input::mouse_right; }

                        case WM_LBUTTONUP: { return fan::input::mouse_left; }
                        case WM_MBUTTONUP: { return fan::input::mouse_middle; }
                        case WM_RBUTTONUP: { return fan::input::mouse_right; }

                        case WM_MOUSEWHEEL: { return fan::input::mouse_scroll_up; } // ?
                        case WM_MOUSEHWHEEL: { return fan::input::mouse_scroll_down; } // ?

                    }
                };

                auto key = get_mouse_key(wParam);

                switch (wParam) {
                    case WM_MOUSEMOVE: {    

                        if (mouse_move_callback) {

                            mouse_move_callback(fan::vec2i(hooked_key.pt.x, hooked_key.pt.y));
                        }

                        break;
                    }
                    case WM_LBUTTONDOWN: 
                    case WM_MBUTTONDOWN:
                    case WM_RBUTTONDOWN:
                    case WM_MOUSEWHEEL:
                    case WM_MOUSEHWHEEL:
                    {
                        if (!reset_keys[key]) {
                            key_down[key] = true;
                        }

                        if (input_callback) {
                            input_callback(key, fan::key_state::press, key_down[key], fan::vec2i(hooked_key.pt.x, hooked_key.pt.y));
                        }

                        key_down[key] = false;
                        reset_keys[key] = true;

                        break;
                    }
                    case WM_LBUTTONUP:
                    case WM_MBUTTONUP:
                    case WM_RBUTTONUP:
                    {
                        if (input_callback) {
                            input_callback(key, fan::key_state::release, key_down[key], fan::vec2i(hooked_key.pt.x, hooked_key.pt.y));
                        }

                        reset_keys[key] = false;

                        break;
                    }
                }

                return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
            }

            static inline HHOOK keyboard_hook;
            static inline HHOOK mouse_hook;

            static inline std::function<void(uint16_t, fan::key_state, bool action, std::any)> input_callback;
            static inline std::function<void(const fan::vec2i& position)> mouse_move_callback;

		};

#endif

	}

}