#include "AppFrame.h"
#include "Utility.h"
#include "LuaWrapper.h"

#include "resource.h"

#include "D3D9.H"  // for SetFog

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

// ÄÚÖÃluaÀ©Õ¹
extern "C" int luaopen_lfs(lua_State *L);
extern "C" int luaopen_cjson(lua_State* L);

using namespace std;
using namespace LuaSTGPlus;

#ifdef LSHOWFONTBASELINE
class FontBaseLineDebugHelper :
	public f2dFontRendererListener
{
private:
	f2dGraphics2D* m_pGraph2D;
	f2dGeometryRenderer* m_pGRenderer;
protected:
	fBool OnGlyphBeginDraw(fuInt Index, fCharW Character, fcyVec2& DrawPos, fcyVec2& Adv)
	{
		m_pGRenderer->SetPenColor(0, fcyColor(0xFF00FFFF));
		m_pGRenderer->SetPenColor(1, fcyColor(0xFF00FFFF));
		m_pGRenderer->SetPenColor(2, fcyColor(0xFF00FFFF));
		m_pGRenderer->SetPenColor(3, fcyColor(0xFF00FFFF));
		m_pGRenderer->SetPenSize(3.f);
		m_pGRenderer->DrawCircle(m_pGraph2D, DrawPos, 2.f, 12);
		m_pGRenderer->SetPenColor(0, fcyColor(0xFF00FF00));
		m_pGRenderer->SetPenColor(1, fcyColor(0xFF00FF00));
		m_pGRenderer->SetPenColor(2, fcyColor(0xFF00FF00));
		m_pGRenderer->SetPenColor(3, fcyColor(0xFF00FF00));
		m_pGRenderer->DrawLine(m_pGraph2D, DrawPos, DrawPos + Adv);
		return true;
	}
	void OnGlyphCalcuCoord(f2dGraphics2DVertex pVerts[]) { }
public:
	FontBaseLineDebugHelper(f2dGraphics2D* G, f2dGeometryRenderer* GR, fcyRect BaseRect)
		: m_pGraph2D(G), m_pGRenderer(GR)
	{
		m_pGRenderer->SetPenColor(0, fcyColor(0xFFFF0000));
		m_pGRenderer->SetPenColor(1, fcyColor(0xFFFF0000));
		m_pGRenderer->SetPenColor(2, fcyColor(0xFFFF0000));
		m_pGRenderer->SetPenColor(3, fcyColor(0xFFFF0000));
		m_pGRenderer->SetPenSize(2.f);
		m_pGRenderer->DrawRectangle(G, BaseRect);
	}
};
#endif

////////////////////////////////////////////////////////////////////////////////
/// VKCode F2DKeyCode ×ª»»±í
////////////////////////////////////////////////////////////////////////////////
const F2DINPUTKEYCODE VKCodeToF2DKeyCodeTable[256] =
{
	F2DINPUTKEYCODE_UNKNOWN,
	F2DINPUTKEYCODE_UNKNOWN,    // 1 VK_LBUTTON
	F2DINPUTKEYCODE_UNKNOWN,    // 2 VK_RBUTTON
	F2DINPUTKEYCODE_UNKNOWN,    // 3 VK_CANCEL
	F2DINPUTKEYCODE_UNKNOWN,    // 4 VK_MBUTTON
	F2DINPUTKEYCODE_UNKNOWN,    // 5 VK_XBUTTON1
	F2DINPUTKEYCODE_UNKNOWN,    // 6 VK_XBUTTON2
	F2DINPUTKEYCODE_UNKNOWN,    // 7
	F2DINPUTKEYCODE_BACK,       // 8 VK_BACK = Backspace
	F2DINPUTKEYCODE_TAB,        // 9 VK_TAB = Tab
	F2DINPUTKEYCODE_UNKNOWN,    // 10
	F2DINPUTKEYCODE_UNKNOWN,    // 11
	F2DINPUTKEYCODE_UNKNOWN,    // 12 VK_CLEAR = Clear
	F2DINPUTKEYCODE_RETURN,     // 13 VK_RETURN = Enter
	F2DINPUTKEYCODE_UNKNOWN,    // 14
	F2DINPUTKEYCODE_UNKNOWN,    // 15
	F2DINPUTKEYCODE_LSHIFT,     // 16 VK_SHIFT = Shift
	F2DINPUTKEYCODE_LCONTROL,   // 17 VK_CONTROL = Ctrl
	F2DINPUTKEYCODE_LMENU,      // 18 VK_MENU = Alt
	F2DINPUTKEYCODE_PAUSE,      // 19 VK_PAUSE = Pause
	F2DINPUTKEYCODE_CAPITAL,    // 20 VK_CAPITAL = Caps Lock
	F2DINPUTKEYCODE_KANA,       // 21 VK_KANA
	F2DINPUTKEYCODE_UNKNOWN,    // 22
	F2DINPUTKEYCODE_UNKNOWN,    // 23 VK_JUNJA
	F2DINPUTKEYCODE_UNKNOWN,    // 24 VK_FINAL
	F2DINPUTKEYCODE_KANJI,      // 25 VK_HANJA
	F2DINPUTKEYCODE_UNKNOWN,    // 26
	F2DINPUTKEYCODE_ESCAPE,     // 27 VK_ESCAPE = Esc
	F2DINPUTKEYCODE_CONVERT,    // 28 VK_CONVERT
	F2DINPUTKEYCODE_NOCONVERT,  // 29 VK_NONCONVERT
	F2DINPUTKEYCODE_UNKNOWN,    // 30 VK_ACCEPT
	F2DINPUTKEYCODE_UNKNOWN,    // 31 VK_MODECHANGE
	F2DINPUTKEYCODE_SPACE,      // 32 VK_SPACE = Space
	F2DINPUTKEYCODE_PRIOR,      // 33 VK_PRIOR = Page Up
	F2DINPUTKEYCODE_NEXT,       // 34 VK_NEXT = Page Down
	F2DINPUTKEYCODE_END,        // 35 VK_END = End
	F2DINPUTKEYCODE_HOME,       // 36 VK_HOME = Home
	F2DINPUTKEYCODE_LEFT,       // 37 VK_LEFT = Left Arrow
	F2DINPUTKEYCODE_UP,         // 38 VK_UP	= Up Arrow
	F2DINPUTKEYCODE_RIGHT,      // 39 VK_RIGHT = Right Arrow
	F2DINPUTKEYCODE_DOWN,       // 40 VK_DOWN = Down Arrow
	F2DINPUTKEYCODE_UNKNOWN,    // 41 VK_SELECT = Select
	F2DINPUTKEYCODE_UNKNOWN,    // 42 VK_PRINT = Print
	F2DINPUTKEYCODE_UNKNOWN,    // 43 VK_EXECUTE = Execute
	F2DINPUTKEYCODE_UNKNOWN,    // 44 VK_SNAPSHOT = Snapshot
	F2DINPUTKEYCODE_INSERT,     // 45 VK_INSERT = Insert
	F2DINPUTKEYCODE_DELETE,     // 46 VK_DELETE = Delete
	F2DINPUTKEYCODE_UNKNOWN,    // 47 VK_HELP = Help
	F2DINPUTKEYCODE_0,          // 48 0
	F2DINPUTKEYCODE_1,          // 49 1
	F2DINPUTKEYCODE_2,          // 50 2
	F2DINPUTKEYCODE_3,          // 51 3
	F2DINPUTKEYCODE_4,          // 52 4
	F2DINPUTKEYCODE_5,          // 53 5
	F2DINPUTKEYCODE_6,          // 54 6
	F2DINPUTKEYCODE_7,          // 55 7
	F2DINPUTKEYCODE_8,          // 56 8
	F2DINPUTKEYCODE_9,          // 57 9
	F2DINPUTKEYCODE_UNKNOWN,    // 58
	F2DINPUTKEYCODE_UNKNOWN,    // 59
	F2DINPUTKEYCODE_UNKNOWN,    // 60
	F2DINPUTKEYCODE_UNKNOWN,    // 61
	F2DINPUTKEYCODE_UNKNOWN,    // 62
	F2DINPUTKEYCODE_UNKNOWN,    // 63
	F2DINPUTKEYCODE_UNKNOWN,    // 64
	F2DINPUTKEYCODE_A,          // 65 A
	F2DINPUTKEYCODE_B,          // 66 B
	F2DINPUTKEYCODE_C,          // 67 C
	F2DINPUTKEYCODE_D,          // 68 D
	F2DINPUTKEYCODE_E,          // 69 E
	F2DINPUTKEYCODE_F,          // 70 F
	F2DINPUTKEYCODE_G,          // 71 G
	F2DINPUTKEYCODE_H,          // 72 H
	F2DINPUTKEYCODE_I,          // 73 I
	F2DINPUTKEYCODE_J,          // 74 J
	F2DINPUTKEYCODE_K,          // 75 K
	F2DINPUTKEYCODE_L,          // 76 L
	F2DINPUTKEYCODE_M,          // 77 M
	F2DINPUTKEYCODE_N,          // 78 N
	F2DINPUTKEYCODE_O,          // 79 O
	F2DINPUTKEYCODE_P,          // 80 P
	F2DINPUTKEYCODE_Q,          // 81 Q
	F2DINPUTKEYCODE_R,          // 82 R
	F2DINPUTKEYCODE_S,          // 83 S
	F2DINPUTKEYCODE_T,          // 84 T
	F2DINPUTKEYCODE_U,          // 85 U
	F2DINPUTKEYCODE_V,          // 86 V
	F2DINPUTKEYCODE_W,          // 87 W
	F2DINPUTKEYCODE_X,          // 88 X
	F2DINPUTKEYCODE_Y,          // 89 Y
	F2DINPUTKEYCODE_Z,          // 90 Z
	F2DINPUTKEYCODE_LWIN,       // 91 VK_LWIN
	F2DINPUTKEYCODE_RWIN,       // 92 VK_RWIN
	F2DINPUTKEYCODE_APPS,       // 93 VK_APPS
	F2DINPUTKEYCODE_UNKNOWN,    // 94
	F2DINPUTKEYCODE_SLEEP,      // 95 VK_SLEEP
	F2DINPUTKEYCODE_NUMPAD0,    // 96 VK_NUMPAD0 = Ð¡¼üÅÌ 0
	F2DINPUTKEYCODE_NUMPAD1,    // 97 VK_NUMPAD1 = Ð¡¼üÅÌ 1
	F2DINPUTKEYCODE_NUMPAD2,    // 98 VK_NUMPAD2 = Ð¡¼üÅÌ 2
	F2DINPUTKEYCODE_NUMPAD3,    // 99 VK_NUMPAD3 = Ð¡¼üÅÌ 3
	F2DINPUTKEYCODE_NUMPAD4,    // 100 VK_NUMPAD4 = Ð¡¼üÅÌ 4
	F2DINPUTKEYCODE_NUMPAD5,    // 101 VK_NUMPAD5 = Ð¡¼üÅÌ 5
	F2DINPUTKEYCODE_NUMPAD6,    // 102 VK_NUMPAD6 = Ð¡¼üÅÌ 6
	F2DINPUTKEYCODE_NUMPAD7,    // 103 VK_NUMPAD7 = Ð¡¼üÅÌ 7
	F2DINPUTKEYCODE_NUMPAD8,    // 104 VK_NUMPAD8 = Ð¡¼üÅÌ 8
	F2DINPUTKEYCODE_NUMPAD9,    // 105 VK_NUMPAD9 = Ð¡¼üÅÌ 9
	F2DINPUTKEYCODE_MULTIPLY,   // 106 VK_MULTIPLY = Ð¡¼üÅÌ *
	F2DINPUTKEYCODE_ADD,        // 107 VK_ADD = Ð¡¼üÅÌ +
	F2DINPUTKEYCODE_NUMPADENTER,// 108 VK_SEPARATOR = Ð¡¼üÅÌ Enter
	F2DINPUTKEYCODE_SUBTRACT,   // 109 VK_SUBTRACT = Ð¡¼üÅÌ -
	F2DINPUTKEYCODE_DECIMAL,    // 110 VK_DECIMAL = Ð¡¼üÅÌ .
	F2DINPUTKEYCODE_DIVIDE,     // 111 VK_DIVIDE = Ð¡¼üÅÌ /
	F2DINPUTKEYCODE_F1,         // 112 VK_F1 = F1
	F2DINPUTKEYCODE_F2,         // 113 VK_F2 = F2
	F2DINPUTKEYCODE_F3,         // 114 VK_F3 = F3
	F2DINPUTKEYCODE_F4,         // 115 VK_F4 = F4
	F2DINPUTKEYCODE_F5,         // 116 VK_F5 = F5
	F2DINPUTKEYCODE_F6,         // 117 VK_F6 = F6
	F2DINPUTKEYCODE_F7,         // 118 VK_F7 = F7
	F2DINPUTKEYCODE_F8,         // 119 VK_F8 = F8
	F2DINPUTKEYCODE_F9,         // 120 VK_F9 = F9
	F2DINPUTKEYCODE_F10,        // 121 VK_F10 = F10
	F2DINPUTKEYCODE_F11,        // 122 VK_F11 = F11
	F2DINPUTKEYCODE_F12,        // 123 VK_F12 = F12
	F2DINPUTKEYCODE_F13,        // 124 VK_F13
	F2DINPUTKEYCODE_F14,        // 125 VK_F14
	F2DINPUTKEYCODE_F15,        // 126 VK_F15
	F2DINPUTKEYCODE_UNKNOWN,    // 127 VK_F16
	F2DINPUTKEYCODE_UNKNOWN,    // 128 VK_F17
	F2DINPUTKEYCODE_UNKNOWN,    // 129 VK_F18
	F2DINPUTKEYCODE_UNKNOWN,    // 130 VK_F19
	F2DINPUTKEYCODE_UNKNOWN,    // 131 VK_F20
	F2DINPUTKEYCODE_UNKNOWN,    // 132 VK_F21
	F2DINPUTKEYCODE_UNKNOWN,    // 133 VK_F22
	F2DINPUTKEYCODE_UNKNOWN,    // 134 VK_F23
	F2DINPUTKEYCODE_UNKNOWN,    // 135 VK_F24
	F2DINPUTKEYCODE_UNKNOWN,    // 136
	F2DINPUTKEYCODE_UNKNOWN,    // 137
	F2DINPUTKEYCODE_UNKNOWN,    // 138
	F2DINPUTKEYCODE_UNKNOWN,    // 139
	F2DINPUTKEYCODE_UNKNOWN,    // 140
	F2DINPUTKEYCODE_UNKNOWN,    // 141
	F2DINPUTKEYCODE_UNKNOWN,    // 142
	F2DINPUTKEYCODE_UNKNOWN,    // 143
	F2DINPUTKEYCODE_NUMLOCK,    // 144 VK_NUMLOCK = Num Lock
	F2DINPUTKEYCODE_SCROLL,     // 145 VK_SCROLL = Scroll
	F2DINPUTKEYCODE_UNKNOWN,    // 146
	F2DINPUTKEYCODE_UNKNOWN,    // 147
	F2DINPUTKEYCODE_UNKNOWN,    // 148
	F2DINPUTKEYCODE_UNKNOWN,    // 149
	F2DINPUTKEYCODE_UNKNOWN,    // 150
	F2DINPUTKEYCODE_UNKNOWN,    // 151
	F2DINPUTKEYCODE_UNKNOWN,    // 152
	F2DINPUTKEYCODE_UNKNOWN,    // 153
	F2DINPUTKEYCODE_UNKNOWN,    // 154
	F2DINPUTKEYCODE_UNKNOWN,    // 155
	F2DINPUTKEYCODE_UNKNOWN,    // 156
	F2DINPUTKEYCODE_UNKNOWN,    // 157
	F2DINPUTKEYCODE_UNKNOWN,    // 158
	F2DINPUTKEYCODE_UNKNOWN,    // 159
	F2DINPUTKEYCODE_LSHIFT,     // 160 VK_LSHIFT
	F2DINPUTKEYCODE_RSHIFT,     // 161 VK_RSHIFT
	F2DINPUTKEYCODE_LCONTROL,   // 162 VK_LCONTROL
	F2DINPUTKEYCODE_RCONTROL,   // 163 VK_RCONTROL
	F2DINPUTKEYCODE_LMENU,      // 164 VK_LMENU
	F2DINPUTKEYCODE_RMENU,      // 165 VK_RMENU
	F2DINPUTKEYCODE_WEBBACK,    // 166 VK_BROWSER_BACK
	F2DINPUTKEYCODE_WEBFORWARD, // 167 VK_BROWSER_FORWARD
	F2DINPUTKEYCODE_WEBREFRESH, // 168 VK_BROWSER_REFRESH
	F2DINPUTKEYCODE_WEBSTOP,    // 169 VK_BROWSER_STOP
	F2DINPUTKEYCODE_WEBSEARCH,  // 170 VK_BROWSER_SEARCH
	F2DINPUTKEYCODE_WEBFAVORITES, // 171 VK_BROWSER_FAVORITES
	F2DINPUTKEYCODE_WEBHOME,    // 172 VK_BROWSER_HOME
	F2DINPUTKEYCODE_MUTE,       // 173 VK_VOLUME_MUTE
	F2DINPUTKEYCODE_VOLUMEDOWN, // 174 VK_VOLUME_DOWN
	F2DINPUTKEYCODE_VOLUMEUP,   // 175 VK_VOLUME_UP
	F2DINPUTKEYCODE_NEXTTRACK,  // 176 VK_MEDIA_NEXT_TRACK
	F2DINPUTKEYCODE_PREVTRACK,  // 177 VK_MEDIA_PREV_TRACK
	F2DINPUTKEYCODE_MEDIASTOP,  // 178 VK_MEDIA_STOP
	F2DINPUTKEYCODE_PLAYPAUSE,  // 179 VK_MEDIA_PLAY_PAUSE
	F2DINPUTKEYCODE_MAIL,       // 180 VK_LAUNCH_MAIL
	F2DINPUTKEYCODE_MEDIASELECT,// 181 VK_LAUNCH_MEDIA_SELECT
	F2DINPUTKEYCODE_UNKNOWN,    // 182 VK_LAUNCH_APP1
	F2DINPUTKEYCODE_UNKNOWN,    // 183 VK_LAUNCH_APP2
	F2DINPUTKEYCODE_UNKNOWN,    // 184
	F2DINPUTKEYCODE_UNKNOWN,    // 185
	F2DINPUTKEYCODE_SEMICOLON,  // 186 VK_OEM_1	= ; :
	F2DINPUTKEYCODE_EQUALS,     // 187 VK_OEM_PLUS = = +
	F2DINPUTKEYCODE_COMMA,      // 188 VK_OEM_COMMA = ,
	F2DINPUTKEYCODE_MINUS,      // 189 VK_OEM_MINUS = - _
	F2DINPUTKEYCODE_PERIOD,     // 190 VK_OEM_PERIOD = .
	F2DINPUTKEYCODE_SLASH,      // 191 VK_OEM_2 = / ?
	F2DINPUTKEYCODE_GRAVE,      // 192 VK_OEM_3 = ` ~
	F2DINPUTKEYCODE_UNKNOWN,    // 193
	F2DINPUTKEYCODE_UNKNOWN,    // 194
	F2DINPUTKEYCODE_UNKNOWN,    // 195
	F2DINPUTKEYCODE_UNKNOWN,    // 196
	F2DINPUTKEYCODE_UNKNOWN,    // 197
	F2DINPUTKEYCODE_UNKNOWN,    // 198
	F2DINPUTKEYCODE_UNKNOWN,    // 199
	F2DINPUTKEYCODE_UNKNOWN,    // 200
	F2DINPUTKEYCODE_UNKNOWN,    // 201
	F2DINPUTKEYCODE_UNKNOWN,    // 202
	F2DINPUTKEYCODE_UNKNOWN,    // 203
	F2DINPUTKEYCODE_UNKNOWN,    // 204
	F2DINPUTKEYCODE_UNKNOWN,    // 205
	F2DINPUTKEYCODE_UNKNOWN,    // 206
	F2DINPUTKEYCODE_UNKNOWN,    // 207
	F2DINPUTKEYCODE_UNKNOWN,    // 208
	F2DINPUTKEYCODE_UNKNOWN,    // 209
	F2DINPUTKEYCODE_UNKNOWN,    // 210
	F2DINPUTKEYCODE_UNKNOWN,    // 211
	F2DINPUTKEYCODE_UNKNOWN,    // 212
	F2DINPUTKEYCODE_UNKNOWN,    // 213
	F2DINPUTKEYCODE_UNKNOWN,    // 214
	F2DINPUTKEYCODE_UNKNOWN,    // 215
	F2DINPUTKEYCODE_UNKNOWN,    // 216
	F2DINPUTKEYCODE_UNKNOWN,    // 217
	F2DINPUTKEYCODE_UNKNOWN,    // 218
	F2DINPUTKEYCODE_LBRACKET,   // 219 VK_OEM_4 = [ {
	F2DINPUTKEYCODE_BACKSLASH,  // 220 VK_OEM_5 = \ |
	F2DINPUTKEYCODE_RBRACKET,   // 221 VK_OEM_6 = ] }
	F2DINPUTKEYCODE_APOSTROPHE, // 222 VK_OEM_7 = ' "
	F2DINPUTKEYCODE_UNKNOWN,    // 223 VK_OEM_8
	F2DINPUTKEYCODE_UNKNOWN,    // 224
	F2DINPUTKEYCODE_UNKNOWN,    // 225
	F2DINPUTKEYCODE_OEM_102,    // 226 VK_OEM_102
	F2DINPUTKEYCODE_UNKNOWN,    // 227
	F2DINPUTKEYCODE_UNKNOWN,    // 228
	F2DINPUTKEYCODE_UNKNOWN,    // 229 VK_PROCESSKEY
	F2DINPUTKEYCODE_UNKNOWN,    // 230
	F2DINPUTKEYCODE_UNKNOWN,    // 231 VK_PACKET
	F2DINPUTKEYCODE_UNKNOWN,    // 232
	F2DINPUTKEYCODE_UNKNOWN,    // 233
	F2DINPUTKEYCODE_UNKNOWN,    // 234
	F2DINPUTKEYCODE_UNKNOWN,    // 235
	F2DINPUTKEYCODE_UNKNOWN,    // 236
	F2DINPUTKEYCODE_UNKNOWN,    // 237
	F2DINPUTKEYCODE_UNKNOWN,    // 238
	F2DINPUTKEYCODE_UNKNOWN,    // 239
	F2DINPUTKEYCODE_UNKNOWN,    // 240
	F2DINPUTKEYCODE_UNKNOWN,    // 241
	F2DINPUTKEYCODE_UNKNOWN,    // 242
	F2DINPUTKEYCODE_UNKNOWN,    // 243
	F2DINPUTKEYCODE_UNKNOWN,    // 244
	F2DINPUTKEYCODE_UNKNOWN,    // 245
	F2DINPUTKEYCODE_UNKNOWN,    // 246 VK_ATTN
	F2DINPUTKEYCODE_UNKNOWN,    // 247 VK_CRSEL
	F2DINPUTKEYCODE_UNKNOWN,    // 248 VK_EXSEL
	F2DINPUTKEYCODE_UNKNOWN,    // 249 VK_EREOF
	F2DINPUTKEYCODE_UNKNOWN,    // 250 VK_PLAY
	F2DINPUTKEYCODE_UNKNOWN,    // 251 VK_ZOOM
	F2DINPUTKEYCODE_UNKNOWN,    // 252 VK_NONAME
	F2DINPUTKEYCODE_UNKNOWN,    // 253 VK_PA1	
	F2DINPUTKEYCODE_UNKNOWN,    // 254 VK_OEM_CLEAR
	F2DINPUTKEYCODE_UNKNOWN     // 255
};

static inline F2DINPUTKEYCODE VKKeyToF2DKey(fuInt VKCode)
{
	return VKCodeToF2DKeyCodeTable[VKCode];
}

////////////////////////////////////////////////////////////////////////////////
/// AppFrame
////////////////////////////////////////////////////////////////////////////////
LNOINLINE AppFrame& AppFrame::GetInstance()
{
	static AppFrame s_Instance;
	return s_Instance;
}

AppFrame::AppFrame()
{
}

AppFrame::~AppFrame()
{
	if (m_iStatus != AppStatus::NotInitialized && m_iStatus != AppStatus::Destroyed)
	{
		// ÈôÃ»ÓÐÏú»Ù¿ò¼Ü£¬ÔòÖ´ÐÐÏú»Ù
		Shutdown();
	}
}

#pragma region 脚本接口
LNOINLINE void AppFrame::ShowSplashWindow(const char* imgPath)LNOEXCEPT
{
	if (m_iStatus == AppStatus::Initializing)
	{
		try
		{
			Gdiplus::Image* pImg = nullptr;

			// ÈôÓÐÍ¼Æ¬£¬Ôò¼ÓÔØ
			if (imgPath)
			{
				fcyRefPointer<fcyMemStream> tDataBuf;
				if (m_ResourceMgr.LoadFile(imgPath, tDataBuf))
					pImg = SplashWindow::LoadImageFromMemory((fcData)tDataBuf->GetInternalBuffer(), (size_t)tDataBuf->GetLength());
				
				if (!pImg)
					LERROR("ShowSplashWindow: ÎÞ·¨¼ÓÔØÍ¼Æ¬'%m'", imgPath);
			}

			// ÏÔÊ¾´°¿Ú
			m_SplashWindow.ShowSplashWindow(pImg);

			FCYSAFEDEL(pImg);
		}
		catch (const bad_alloc&)
		{
			LERROR("ShowSplashWindow: ÄÚ´æ²»×ã");
			return;
		}
		m_bSplashWindowEnabled = true;
	}
	else
		LWARNING("ShowSplashWindow: ÎÞ·¨ÔÚ´ËÊ±×°ÔØ´°¿Ú");
}

void AppFrame::SetWindowed(bool v)LNOEXCEPT
{
	if (m_iStatus == AppStatus::Initializing)
		m_OptionWindowed = v;
	else if (m_iStatus == AppStatus::Running)
		LWARNING("ÊÔÍ¼ÔÚÔËÐÐÊ±¸ü¸Ä´°¿Ú»¯Ä£Ê½");
}

void AppFrame::SetFPS(fuInt v)LNOEXCEPT
{
	if (m_iStatus == AppStatus::Initializing)
		m_OptionFPSLimit = v;
	else if (m_iStatus == AppStatus::Running)
		LWARNING("ÊÔÍ¼ÔÚÔËÐÐÊ±¸ü¸ÄFPSÏÞÖÆ");
}

void AppFrame::SetVsync(bool v)LNOEXCEPT
{
	if (m_iStatus == AppStatus::Initializing)
		m_OptionVsync = v;
	else if (m_iStatus == AppStatus::Running)
		LWARNING("ÊÔÍ¼ÔÚÔËÐÐÊ±¸ü¸Ä´¹Ö±Í¬²½Ä£Ê½");
}

void AppFrame::SetResolution(fuInt width, fuInt height)LNOEXCEPT
{
	if (m_iStatus == AppStatus::Initializing)
		m_OptionResolution.Set((float)width, (float)height);
	else if (m_iStatus == AppStatus::Running)
		LWARNING("ÊÔÍ¼ÔÚÔËÐÐÊ±¸ü¸Ä·Ö±æÂÊ");
}

void AppFrame::SetSplash(bool v)LNOEXCEPT
{
	m_OptionSplash = v;
	if (m_pMainWindow)
		m_pMainWindow->HideMouse(!m_OptionSplash);
}

LNOINLINE void AppFrame::SetTitle(const char* v)LNOEXCEPT
{
	try
	{
		m_OptionTitle = std::move(fcyStringHelper::MultiByteToWideChar(v, CP_UTF8));
		if (m_pMainWindow)
			m_pMainWindow->SetCaption(m_OptionTitle.c_str());
	}
	catch (const bad_alloc&)
	{
		LERROR("ÐÞ¸Ä´°¿Ú±êÌâÊ±ÎÞ·¨·ÖÅäÄÚ´æ");
	}
}

LNOINLINE bool AppFrame::ChangeVideoMode(int width, int height, bool windowed, bool vsync)LNOEXCEPT
{
	if (m_iStatus == AppStatus::Initialized)
	{
		// ÇÐ»»µ½ÐÂµÄÊÓÆµÑ¡Ïî
		if (FCYOK(m_pRenderDev->SetBufferSize(
			(fuInt)width,
			(fuInt)height,
			windowed,
			vsync,
			F2DAALEVEL_NONE)))
		{
			LINFO("ÊÓÆµÄ£Ê½ÇÐ»»³É¹¦ (%dx%d Vsync:%b Windowed:%b) -> (%dx%d Vsync:%b Windowed:%b)",
				(int)m_OptionResolution.x, (int)m_OptionResolution.y, m_OptionVsync, m_OptionWindowed,
				width, height, vsync, windowed);

			m_OptionResolution.Set((float)width, (float)height);
			m_OptionWindowed = windowed;
			m_OptionVsync = vsync;

			// ÇÐ»»´°¿Ú´óÐ¡
			m_pMainWindow->SetBorderType(m_OptionWindowed ? F2DWINBORDERTYPE_FIXED : F2DWINBORDERTYPE_NONE);
			m_pMainWindow->SetClientRect(
				fcyRect(10.f, 10.f, 10.f + m_OptionResolution.x, 10.f + m_OptionResolution.y)
				);
			m_pMainWindow->SetTopMost(!m_OptionWindowed);
			m_pMainWindow->MoveToCenter();
			return true;
		}
		else
		{
			// ! µ±ÏÔÊ¾¼ÓÔØ´°¿Ú¶øÆô¶¯ÓÎÏ·ºó¸ÄÎªÈ«ÆÁÊ§°ÜÊ±Ôò½«´°¿ÚÄ£Ê½ÉèÎªtrue
			if (m_bSplashWindowEnabled)
				m_OptionWindowed = true;

			// ÇÐ»»´°¿Ú´óÐ¡
			m_pMainWindow->SetBorderType(m_OptionWindowed ? F2DWINBORDERTYPE_FIXED : F2DWINBORDERTYPE_NONE);
			m_pMainWindow->SetClientRect(
				fcyRect(10.f, 10.f, 10.f + m_OptionResolution.x, 10.f + m_OptionResolution.y)
				);
			m_pMainWindow->SetTopMost(!m_OptionWindowed);
			m_pMainWindow->MoveToCenter();

			LINFO("ÊÓÆµÄ£Ê½ÇÐ»»Ê§°Ü (%dx%d Vsync:%b Windowed:%b) -> (%dx%d Vsync:%b Windowed:%b)",
				(int)m_OptionResolution.x, (int)m_OptionResolution.y, m_OptionVsync, m_OptionWindowed,
				width, height, vsync, windowed);
		}
	}
	return false;
}

LNOINLINE void AppFrame::LoadScript(const char* path)LNOEXCEPT
{
	LINFO("×°ÔØ½Å±¾'%m'", path);
	fcyRefPointer<fcyMemStream> tMemStream;
	if (!m_ResourceMgr.LoadFile(path, tMemStream))
	{
		luaL_error(L, "can't load script '%s'", path);
		return;
	}
	if (luaL_loadbuffer(L, (fcStr)tMemStream->GetInternalBuffer(), (size_t)tMemStream->GetLength(), luaL_checkstring(L, 1)))
	{
		tMemStream = nullptr;
		const char* tDetail = lua_tostring(L, -1);
		LERROR("±àÒë½Å±¾'%m'Ê§°Ü: %m", path, tDetail);
		luaL_error(L, "failed to compile '%s': %s", path, tDetail);
		return;
	}
	tMemStream = nullptr;
	lua_call(L, 0, 0);
	/*
	if (lua_pcall(L, 0, 0, 0))
	{
		const char* tDetail = lua_tostring(L, -1);
		LERROR("Ö´ÐÐ½Å±¾'%m'Ê§°Ü", path);
		luaL_error(L, "failed to execute '%s':\n\t%s", path, tDetail);
		return;
	}
	*/
}

fBool AppFrame::GetKeyState(int VKCode)LNOEXCEPT
{
	if (VKCode > 0 && VKCode < _countof(m_KeyStateMap))
	{
		if (LJOYSTICK1_MAPPING_START <= VKCode && VKCode <= LJOYSTICK1_MAPPING_END)  // joystick1Ó³ÉäÇøÓò
		{
			if (m_Joystick[0])
			{
				switch (VKCode)
				{
				case LJOYSTICK1_MAPPING_START:  // ÉÏ
					return (m_Joystick[0]->GetYPosition() < LJOYSTICK_Y_MIN);
				case LJOYSTICK1_MAPPING_START + 1:  // ÏÂ
					return (m_Joystick[0]->GetYPosition() > LJOYSTICK_Y_MAX);
				case LJOYSTICK1_MAPPING_START + 2:  // ×ó
					return (m_Joystick[0]->GetXPosition() < LJOYSTICK_X_MIN);
				case LJOYSTICK1_MAPPING_START + 3:  // ÓÒ
					return (m_Joystick[0]->GetXPosition() > LJOYSTICK_X_MAX);
				default:
					return m_Joystick[0]->IsButtonDown(VKCode - LJOYSTICK1_MAPPING_START - 4);
				}
			}	
			else
				return false;
		}
		else if (LJOYSTICK2_MAPPING_START <= VKCode && VKCode <= LJOYSTICK2_MAPPING_END)  // joystick2Ó³ÉäÇøÓò
		{
			if (m_Joystick[1])
			{
				switch (VKCode)
				{
				case LJOYSTICK2_MAPPING_START:  // ÉÏ
					return (m_Joystick[1]->GetYPosition() < LJOYSTICK_Y_MIN);
				case LJOYSTICK2_MAPPING_START + 1:  // ÏÂ
					return (m_Joystick[1]->GetYPosition() > LJOYSTICK_Y_MAX);
				case LJOYSTICK2_MAPPING_START + 2:  // ×ó
					return (m_Joystick[1]->GetXPosition() < LJOYSTICK_X_MIN);
				case LJOYSTICK2_MAPPING_START + 3:  // ÓÒ
					return (m_Joystick[1]->GetXPosition() > LJOYSTICK_X_MAX);
				default:
					return m_Joystick[1]->IsButtonDown(VKCode - LJOYSTICK2_MAPPING_START - 4);
				}
			}
			else
				return false;
		}	
		else if (m_Keyboard)
			return m_Keyboard->IsKeyDown(VKKeyToF2DKey(VKCode));
		else
			return m_KeyStateMap[VKCode];
	}
	return false;
}

LNOINLINE int AppFrame::GetLastChar(lua_State* L)LNOEXCEPT
{
	if (m_LastChar)
	{
		try
		{
			fCharW tBuf[2] = { m_LastChar, 0 };
			lua_pushstring(L,
				fcyStringHelper::WideCharToMultiByte(tBuf, CP_UTF8).c_str());
		}
		catch (const bad_alloc&)
		{
			LERROR("GetLastChar: ÄÚ´æ²»×ã");
			return 0;
		}
	}
	else
		lua_pushstring(L, "");
	return 1;
}

bool AppFrame::BeginScene()LNOEXCEPT
{
	if (!m_bRenderStarted)
	{
		LERROR("²»ÄÜÔÚRenderFuncÒÔÍâµÄµØ·½Ö´ÐÐäÖÈ¾");
		return false;
	}

	if (m_GraphType == GraphicsType::Graph2D)
	{
		if (FCYFAILED(m_Graph2D->Begin()))
		{
			LERROR("Ö´ÐÐf2dGraphics2D::BeginÊ§°Ü");
			return false;
		}
	}

	return true;
}

bool AppFrame::EndScene()LNOEXCEPT
{
	if (m_GraphType == GraphicsType::Graph2D)
	{
		if (FCYFAILED(m_Graph2D->End()))
		{
			LERROR("Ö´ÐÐf2dGraphics2D::EndÊ§°Ü");
			return false;
		}
	}

	return true;
}

void AppFrame::SetFog(float start, float end, fcyColor color)
{
	if (m_Graph2D->IsInRender())
		m_Graph2D->Flush();

	// ´Óf2dRenderDeviceÖÐÈ¡³öD3DÉè±¸
	IDirect3DDevice9* pDev = (IDirect3DDevice9*)m_pRenderDev->GetHandle();

	if (start != end)
	{
		pDev->SetRenderState(D3DRS_FOGENABLE, TRUE);
		pDev->SetRenderState(D3DRS_FOGCOLOR, color.argb);
		if (start == -1.0f)
		{
			pDev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP);
			pDev->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&end));
		}
		else if (start == -2.0f)
		{
			pDev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP2);
			pDev->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&end));
		}
		else
		{
			pDev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
			pDev->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&start));
			pDev->SetRenderState(D3DRS_FOGEND, *(DWORD *)(&end));
		}
	}
	else
		pDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
}

bool AppFrame::RenderText(ResFont* p, wchar_t* strBuf, fcyRect rect, fcyVec2 scale, ResFont::FontAlignHorizontal halign, ResFont::FontAlignVertical valign, bool bWordBreak)LNOEXCEPT
{
	if (m_GraphType != GraphicsType::Graph2D)
	{
		LERROR("RenderText: Ö»ÓÐ2DäÖÈ¾Æ÷¿ÉÒÔÖ´ÐÐ¸Ã·½·¨");
		return false;
	}

	f2dFontProvider* pFontProvider = p->GetFontProvider();

	// ×¼±¸äÖÈ¾×ÖÌå
	m_FontRenderer->SetFontProvider(pFontProvider);
	m_FontRenderer->SetScale(scale);
#ifdef LSHOWFONTBASELINE
	FontBaseLineDebugHelper tDebugger(m_Graph2D, m_GRenderer, rect);
	m_FontRenderer->SetListener(&tDebugger);
#endif

	// ÉèÖÃ»ìºÏºÍÑÕÉ«
	updateGraph2DBlendMode(p->GetBlendMode());
	m_FontRenderer->SetColor(p->GetBlendColor());

	// µÚÒ»´Î±éÀú¼ÆËãÒªäÖÈ¾¶àÉÙÐÐ
	const wchar_t* pText = strBuf;
	int iLineCount = 1;
	float fLineWidth = 0.f;
	while (*pText)
	{
		bool bNewLine = false;
		if (*pText == L'\n')
			bNewLine = true;
		else
		{
			f2dGlyphInfo tGlyphInfo;
			if (FCYOK(pFontProvider->QueryGlyph(m_Graph2D, *pText, &tGlyphInfo)))
			{
				float adv = tGlyphInfo.Advance.x * scale.x;
				if (bWordBreak && fLineWidth + adv > rect.GetWidth())  // ½Ø¶ÏÄ£Ê½
				{
					if (pText == strBuf || *(pText - 1) == L'\n')
					{
						++pText;  // ·ÀÖ¹Ò»¸ö×Ö·û¶¼²»äÖÈ¾µ¼ÖÂËÀÑ­»·
						if (*pText == L'\0')
							break;
					}
					bNewLine = true;
				}
				else
					fLineWidth += adv;
			}
		}
		if (bNewLine)
		{
			++iLineCount;
			fLineWidth = 0.f;
		}
		if (*pText != L'\0')
			++pText;
	}
	
	// ¼ÆËãÆð±ÊÎ»ÖÃ
	float fTotalLineHeight = pFontProvider->GetLineHeight() * iLineCount * scale.y;
	fcyVec2 vRenderPos;
	switch (valign)
	{
	case ResFont::FontAlignVertical::Bottom:
		vRenderPos.y = rect.b.y + fTotalLineHeight;
		break;
	case ResFont::FontAlignVertical::Middle:
		vRenderPos.y = rect.a.y - rect.GetHeight() / 2.f + fTotalLineHeight / 2.f;
		break;
	case ResFont::FontAlignVertical::Top:
	default:
		vRenderPos.y = rect.a.y;
		break;
	}
	vRenderPos.x = rect.a.x;
	vRenderPos.y -= pFontProvider->GetAscender() * scale.y;

	// ÖðÐÐäÖÈ¾ÎÄ×Ö
	wchar_t* pScanner = strBuf;
	wchar_t c = 0;
	bool bEOS = false;
	fLineWidth = 0.f;
	pText = pScanner;
	while (!bEOS)
	{
		// Ñ°ÕÒ¶Ï¾äÎ»ÖÃ£¬»»ÐÐ¡¢EOF¡¢»òÕßÐÐÒç³ö
		while (*pScanner != L'\0' && *pScanner != '\n')
		{
			f2dGlyphInfo tGlyphInfo;
			if (FCYOK(pFontProvider->QueryGlyph(m_Graph2D, *pScanner, &tGlyphInfo)))
			{
				float adv = tGlyphInfo.Advance.x * scale.x;

				// ¼ì²éµ±Ç°×Ö·ûäÖÈ¾ºó»á²»»áµ¼ÖÂÐÐÒç³ö
				if (bWordBreak && fLineWidth + adv > rect.GetWidth())
				{
					if (pScanner == pText)  // ·ÀÖ¹Ò»¸ö×Ö·û¶¼²»äÖÈ¾µ¼ÖÂËÀÑ­»·
						++pScanner;
					break;
				}
				fLineWidth += adv;
			}
			++pScanner;
		}
		
		// ÔÚ¶Ï¾äÎ»ÖÃÐ´Èë\0
		c = *pScanner;
		if (c == L'\0')
			bEOS = true;
		else
			*pScanner = L'\0';
		
		// äÖÈ¾´ÓpText~pScannerµÄÎÄ×Ö
		switch (halign)
		{
		case ResFont::FontAlignHorizontal::Right:
			m_FontRenderer->DrawTextW2(m_Graph2D, pText,
				fcyVec2(vRenderPos.x + rect.GetWidth() - fLineWidth, vRenderPos.y));
			break;
		case ResFont::FontAlignHorizontal::Center:
			m_FontRenderer->DrawTextW2(m_Graph2D, pText,
				fcyVec2(vRenderPos.x + rect.GetWidth() / 2.f - fLineWidth / 2.f, vRenderPos.y));
			break;
		case ResFont::FontAlignHorizontal::Left:
		default:
			m_FontRenderer->DrawTextW2(m_Graph2D, pText, vRenderPos);
			break;
		}

		// »Ö¸´¶Ï¾ä´¦×Ö·û
		*pScanner = c;
		fLineWidth = 0.f;
		if (c == L'\n')
			pText = ++pScanner;
		else
			pText = pScanner;
		
		// ÒÆ¶¯yÖá
		vRenderPos.y -= p->GetFontProvider()->GetLineHeight() * scale.y;
	}

#ifdef LSHOWFONTBASELINE
	m_FontRenderer->SetListener(nullptr);
#endif
	return true;
}

fcyVec2 AppFrame::CalcuTextSize(ResFont* p, const wchar_t* strBuf, fcyVec2 scale)LNOEXCEPT
{
	if (m_GraphType != GraphicsType::Graph2D)
	{
		LERROR("RenderText: Ö»ÓÐ2DäÖÈ¾Æ÷¿ÉÒÔÖ´ÐÐ¸Ã·½·¨");
		return false;
	}

	f2dFontProvider* pFontProvider = p->GetFontProvider();

	int iLineCount = 1;
	float fLineWidth = 0.f;
	float fMaxLineWidth = 0.f;
	while (*strBuf)
	{
		if (*strBuf == L'\n')
		{
			++iLineCount;
			fMaxLineWidth = max(fMaxLineWidth, fLineWidth);
			fLineWidth = 0.f;
		}
		else
		{
			f2dGlyphInfo tGlyphInfo;
			if (FCYOK(pFontProvider->QueryGlyph(m_Graph2D, *strBuf, &tGlyphInfo)))
				fLineWidth += tGlyphInfo.Advance.x * scale.x;
		}
		++strBuf;
	}
	fMaxLineWidth = max(fMaxLineWidth, fLineWidth);

	return fcyVec2(fMaxLineWidth, iLineCount * pFontProvider->GetLineHeight() * scale.y);
}

LNOINLINE bool AppFrame::RenderText(const char* name, const char* str, float x, float y, float scale, ResFont::FontAlignHorizontal halign, ResFont::FontAlignVertical valign)LNOEXCEPT
{
	fcyRefPointer<ResFont> p = m_ResourceMgr.FindSpriteFont(name);
	if (!p)
	{
		LERROR("RenderText: ÕÒ²»µ½ÎÄ×Ö×ÊÔ´'%m'", name);
		return false;
	}

	// ±àÂë×ª»»
	static std::wstring s_TempStringBuf;
	try
	{
		Utf8ToUtf16(str, s_TempStringBuf);
	}
	catch (const bad_alloc&)
	{
		LERROR("RenderText: ÄÚ´æ²»×ã");
		return false;
	}

	// ¼ÆËãäÖÈ¾Î»ÖÃ
	fcyVec2 tSize = CalcuTextSize(p, s_TempStringBuf.c_str(), fcyVec2(scale, scale));
	switch (halign)
	{
	case ResFont::FontAlignHorizontal::Right:
		x -= tSize.x;
		break;
	case ResFont::FontAlignHorizontal::Center:
		x -= tSize.x / 2.f;
		break;
	case ResFont::FontAlignHorizontal::Left:
	default:
		break;
	}
	switch (valign)
	{
	case ResFont::FontAlignVertical::Bottom:
		y += tSize.y;
		break;
	case ResFont::FontAlignVertical::Middle:
		y += tSize.y / 2.f;
		break;
	case ResFont::FontAlignVertical::Top:
	default:
		break;
	}

	return RenderText(
		p,
		const_cast<wchar_t*>(s_TempStringBuf.data()),
		fcyRect(x, y, x + tSize.x, y - tSize.y),
		fcyVec2(scale, scale),
		halign,
		valign,
		false
		);
}

LNOINLINE bool AppFrame::RenderTTF(const char* name, const char* str, float left, float right, float bottom, float top, float scale, int format, fcyColor c)LNOEXCEPT
{
	fcyRefPointer<ResFont> p = m_ResourceMgr.FindTTFFont(name);
	if (!p)
	{
		LERROR("RenderTTF: ÕÒ²»µ½ÎÄ×Ö×ÊÔ´'%m'", name);
		return false;
	}

	// ±àÂë×ª»»
	static std::wstring s_TempStringBuf;
	try
	{
		Utf8ToUtf16(str, s_TempStringBuf);
	}
	catch (const bad_alloc&)
	{
		LERROR("RenderTTF: ÄÚ´æ²»×ã");
		return false;
	}

	p->SetBlendColor(c);

	bool bWordBreak = false;
	ResFont::FontAlignHorizontal halign = ResFont::FontAlignHorizontal::Left;
	ResFont::FontAlignVertical valign = ResFont::FontAlignVertical::Top;

	if ((format & DT_CENTER) == DT_CENTER)
		halign = ResFont::FontAlignHorizontal::Center;
	else if ((format & DT_RIGHT) == DT_RIGHT)
		halign = ResFont::FontAlignHorizontal::Right;

	if ((format & DT_VCENTER) == DT_VCENTER)
		valign = ResFont::FontAlignVertical::Middle;
	else if ((format & DT_BOTTOM) == DT_BOTTOM)
		valign = ResFont::FontAlignVertical::Bottom;

	if ((format & DT_WORDBREAK) == DT_WORDBREAK)
		bWordBreak = true;

	return RenderText(
		p,
		const_cast<wchar_t*>(s_TempStringBuf.data()),
		fcyRect(left, top, right, bottom),
		fcyVec2(scale, scale) * 0.5f,  // Ëõ·ÅÏµÊý=0.5
		halign,
		valign,
		bWordBreak
		);
}

LNOINLINE void AppFrame::SnapShot(const char* path)LNOEXCEPT
{
	LASSERT(m_pRenderDev);

	try
	{
		fcyRefPointer<fcyFileStream> tOutputFile;
		tOutputFile.DirectSet(new fcyFileStream(fcyStringHelper::MultiByteToWideChar(path, CP_UTF8).c_str(), true));
		tOutputFile->SetLength(0);

		if (FCYFAILED(m_pRenderDev->SaveScreen(tOutputFile)))
			LERROR("Snapshot: ±£´æ½ØÍ¼µ½'%m'Ê§°Ü", path);
	}
	catch (const bad_alloc&)
	{
		LERROR("Snapshot: ÄÚ´æ²»×ã");
	}
	catch (const fcyException& e)
	{
		LERROR("Snapshot: ±£´æ½ØÍ¼Ê§°Ü (Òì³£ÐÅÏ¢'%m' Ô´'%m')", e.GetDesc(), e.GetSrc());
	}
}

bool AppFrame::CheckRenderTargetInUse(fcyRefPointer<f2dTexture2D> rt)LNOEXCEPT
{
	if (!rt || !rt->IsRenderTarget() || m_stRenderTargetStack.empty())
		return false;

	return rt == m_stRenderTargetStack.back();
}

bool AppFrame::CheckRenderTargetInUse(ResTexture* rt)LNOEXCEPT
{
	if (!rt || !rt->IsRenderTarget() || m_stRenderTargetStack.empty())
		return false;

	return rt->GetTexture() == m_stRenderTargetStack.back();
}

bool AppFrame::PushRenderTarget(fcyRefPointer<f2dTexture2D> rt)LNOEXCEPT
{
	fcyRect orgVP = m_pRenderDev->GetViewport();
	if (FCYFAILED(m_pRenderDev->SetRenderTarget(rt)))
	{
		LERROR("PushRenderTarget: ÄÚ²¿´íÎó (f2dRenderDevice::SetRenderTarget failed.)");
		return false;
	}
	m_pRenderDev->SetViewport(orgVP);

	try
	{
		m_stRenderTargetStack.push_back(rt);
	}
	catch (const std::bad_alloc&)
	{
		LERROR("PushRenderTarget: ÄÚ´æ²»×ã");
		if (m_stRenderTargetStack.empty())
			m_pRenderDev->SetRenderTarget(nullptr);
		else
			m_pRenderDev->SetRenderTarget(m_stRenderTargetStack.back());
		m_pRenderDev->SetViewport(orgVP);
		return false;
	}

	return true;
}

LNOINLINE bool AppFrame::PushRenderTarget(ResTexture* rt)LNOEXCEPT
{
	if (!rt || !rt->IsRenderTarget())
	{
		assert(false);  // Õâ²»¸Ã·¢Éú
		return false;
	}

	if (!m_bRenderStarted)
	{
		LERROR("PushRenderTarget: ·Ç·¨µ÷ÓÃ");
		return false;
	}

	return PushRenderTarget(rt->GetTexture());
}

LNOINLINE bool AppFrame::PopRenderTarget()LNOEXCEPT
{
	if (!m_bRenderStarted)
	{
		LERROR("PopRenderTarget: ·Ç·¨µ÷ÓÃ");
		return false;
	}

	if (m_stRenderTargetStack.empty())
	{
		LERROR("PopRenderTarget: RenderTargetÕ»Îª¿Õ");
		return false;
	}

	fcyRect orgVP = m_pRenderDev->GetViewport();

	m_stRenderTargetStack.pop_back();
	if (m_stRenderTargetStack.empty())
		m_pRenderDev->SetRenderTarget(nullptr);
	else
		m_pRenderDev->SetRenderTarget(m_stRenderTargetStack.back());
	
	m_pRenderDev->SetViewport(orgVP);
	return true;
}

bool AppFrame::PostEffect(fcyRefPointer<f2dTexture2D> rt, ResFX* shader, BlendMode blend)LNOEXCEPT
{
	f2dEffectTechnique* pTechnique = shader->GetEffect()->GetTechnique(0U);

	if (!pTechnique)
	{
		LERROR("PostEffect: ÎÞÐ§µÄShaderÊý¾Ý");
		return false;
	}

	// ÎÆÀíÊ¹ÓÃ¼ì²é
	if (CheckRenderTargetInUse(rt))
	{
		LERROR("PostEffect: ÎÞ·¨ÔÚÒ»¸öRenderTargetÕýÔÚÊ¹ÓÃÊ±×÷Îªºó´¦ÀíÊäÈë");
		return false;
	}

	// ÖÕÖ¹äÖÈ¾¹ý³Ì
	bool bRestartRenderPeriod = false;
	switch (m_GraphType)
	{
	case GraphicsType::Graph2D:
		if (m_Graph2D->IsInRender())
		{
			bRestartRenderPeriod = true;
			m_Graph2D->End();
		}
		break;
	case GraphicsType::Graph3D:
		if (m_Graph3D->IsInRender())
		{
			bRestartRenderPeriod = true;
			m_Graph3D->End();
		}
		break;
	}

	// ¸üÐÂäÖÈ¾×´Ì¬
	updateGraph3DBlendMode(blend);

	// ¹Ø±Õfog
	IDirect3DDevice9* pDev = (IDirect3DDevice9*)m_pRenderDev->GetHandle();
	DWORD iFogEnabled = FALSE;
	pDev->GetRenderState(D3DRS_FOGENABLE, &iFogEnabled);
	if (iFogEnabled == TRUE)
		pDev->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// ÉèÖÃeffect
	shader->SetPostEffectTexture(rt);
	shader->SetViewport(m_pRenderDev->GetViewport());
	shader->SetScreenSize(fcyVec2((float)m_pRenderDev->GetBufferWidth(), (float)m_pRenderDev->GetBufferHeight()));
	m_Graph3D->SetEffect(shader->GetEffect());
	if (FCYFAILED(m_Graph3D->Begin()))
	{
		// £¡ Òì³£ÍË³ö²»¿É»Ö¸´äÖÈ¾¹ý³Ì
		LERROR("PostEffect: ÄÚ²¿´íÎó (f2dGraphics3D::Begin failed)");
		return false;
	}
	// Ö´ÐÐËùÓÐµÄpass
	for (fuInt i = 0; i < pTechnique->GetPassCount(); ++i)
	{
		m_Graph3D->BeginPass(i);
		m_Graph3D->RenderPostEffect();
		m_Graph3D->EndPass();
	}
	m_Graph3D->End();
	shader->SetPostEffectTexture(NULL);

	// ¼ì²éÊÇ·ñ¿ªÆôÁËÎí
	if (iFogEnabled == TRUE)
		pDev->SetRenderState(D3DRS_FOGENABLE, TRUE);

	// ÖØÆôäÖÈ¾¹ý³Ì
	if (bRestartRenderPeriod)
	{
		switch (m_GraphType)
		{
		case GraphicsType::Graph2D:
			m_Graph2D->Begin();
			break;
		case GraphicsType::Graph3D:
			m_Graph3D->Begin();
			break;
		}
	}

	return true;
}

LNOINLINE bool AppFrame::PostEffect(ResTexture* rt, ResFX* shader, BlendMode blend)LNOEXCEPT
{
	if (!m_bRenderStarted)
	{
		LERROR("PostEffect: ·Ç·¨µ÷ÓÃ");
		return false;
	}

	return PostEffect(rt->GetTexture(), shader, blend);
}

LNOINLINE bool AppFrame::PostEffectCapture()LNOEXCEPT
{
	if (!m_bRenderStarted || m_bPostEffectCaptureStarted)
	{
		LERROR("PostEffectCapture: ·Ç·¨µ÷ÓÃ (RenderStarted=%d,PostEffectCaptureStarted=%d)", m_bRenderStarted, m_bPostEffectCaptureStarted);
		return false;
	}

	PushRenderTarget(m_PostEffectBuffer);
	m_pRenderDev->ClearColor();
	m_bPostEffectCaptureStarted = true;
	return true;
}

LNOINLINE bool AppFrame::PostEffectApply(ResFX* shader, BlendMode blend)LNOEXCEPT
{
	if (!m_bRenderStarted || !m_bPostEffectCaptureStarted)
	{
		LERROR("PostEffectApply: ·Ç·¨µ÷ÓÃ (RenderStarted=%d,PostEffectCaptureStarted=%d)", m_bRenderStarted, m_bPostEffectCaptureStarted);
		return false;
	}
	
	if (m_stRenderTargetStack.empty() || m_stRenderTargetStack.back() != m_PostEffectBuffer)
	{
		LERROR("PostEffectApply: ·Ç·¨µ÷ÓÃ£¬RenderTargetÕ»¿Õ»òÎ´Æ¥Åä");
		return false;
	}

	if (!PopRenderTarget())
	{
		LERROR("PostEffectApply: PopRenderTargetÊ§°Ü");
		return false;
	}

	m_bPostEffectCaptureStarted = false;
	return PostEffect(m_PostEffectBuffer, shader, blend);
}

#pragma endregion

#pragma region 框架函数
static int StackTraceback(lua_State *L)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");  // errmsg t
	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");  // errmsg t f
	if (!lua_isfunction(L, -1) && !lua_iscfunction(L, -1))
	{
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);  // errmsg t f errmsg
	lua_pushinteger(L, 2);  // errmsg t f errmsg 2
	if (0 != lua_pcall(L, 2, 1, 0))  // errmsg t 
	{
		LWARNING("Ö´ÐÐstacktraceÊ±·¢Éú´íÎó¡£(%m)", lua_tostring(L, -1));
		lua_pop(L, 2);
	}	
	return 1;
}

bool AppFrame::Init()LNOEXCEPT
{
	LASSERT(m_iStatus == AppStatus::NotInitialized);

	LINFO("¿ªÊ¼³õÊ¼»¯ °æ±¾: %s", LVERSION);
	m_iStatus = AppStatus::Initializing;

	Scope tSplashWindowExit([this]() {
		m_SplashWindow.HideSplashWindow();
	});

	//////////////////////////////////////// Lua³õÊ¼»¯²¿·Ö
	LINFO("¿ªÊ¼³õÊ¼»¯LuaÐéÄâ»ú °æ±¾: %m", LVERSION_LUA);
	L = lua_open();
	if (!L)
	{
		LERROR("ÎÞ·¨³õÊ¼»¯LuaÐéÄâ»ú");
		return false;
	}
	if (0 != luaJIT_setmode(L, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON))
		LWARNING("ÎÞ·¨Æô¶¯JITÄ£Ê½");

	lua_gc(L, LUA_GCSTOP, 0);  // ³õÊ¼»¯Ê±¹Ø±ÕGC

	luaL_openlibs(L);  // ÄÚ½¨¿â
	luaopen_lfs(L);  // ÎÄ¼þÏµÍ³¿â
	luaopen_cjson(L);  // CJSON¿â
	ColorWrapper::Register(L);  // ÑÕÉ«¶ÔÏó
	RandomizerWrapper::Register(L);  // Ëæ»úÊý·¢ÉúÆ÷
	BentLaserDataWrapper::Register(L);  // ÇúÏß¼¤¹â
	BuiltInFunctionWrapper::Register(L);  // ÄÚ½¨º¯Êý¿â

	lua_gc(L, LUA_GCRESTART, -1);  // ÖØÆôGC

	// Îª¶ÔÏó³Ø·ÖÅä¿Õ¼ä
	LINFO("³õÊ¼»¯¶ÔÏó³Ø ÉÏÏÞ=%u", LGOBJ_MAXCNT);
	try
	{
		m_GameObjectPool = make_unique<GameObjectPool>(L);
	}
	catch (const bad_alloc&)
	{
		LERROR("ÎÞ·¨Îª¶ÔÏó³Ø·ÖÅä×ã¹»ÄÚ´æ");
		return false;
	}

	// ÉèÖÃÃüÁîÐÐ²ÎÊý
	regex tDebuggerPattern("\\/debugger:(\\d+)");
	lua_getglobal(L, "lstg");  // t
	lua_newtable(L);  // t t
	for (int i = 0, c = 1; i < __argc; ++i)
	{
		cmatch tMatch;
		if (regex_match(__argv[i], tMatch, tDebuggerPattern))
		{
#if (defined LDEVVERSION) || (defined LDEBUG)
			// ´´½¨µ÷ÊÔÆ÷
			if (!m_DebuggerClient)
			{
				fuShort tPort = atoi(tMatch[1].first);
				
				try
				{
					m_DebuggerClient = make_unique<RemoteDebuggerClient>(tPort);
					LINFO("µ÷ÊÔÆ÷ÒÑ´´½¨£¬ÓÚ¶Ë¿Ú£º%d", (fuInt)tPort);
				}
				catch (const fcyException& e)
				{
					LERROR("´´½¨µ÷ÊÔÆ÷Ê§°Ü (ÏêÏ¸ÐÅÏ¢: %m)", e.GetDesc());
				}
			}
			else
				LWARNING("ÃüÁîÐÐ²ÎÊýÖÐ´øÓÐ¶à¸ö/debuggerÏî£¬ºöÂÔ¡£");
#endif
			// ²»½«debuggerÏî´«ÈëÓÃ»§ÃüÁîÐÐ²ÎÊýÖÐ
			continue;
		}
		lua_pushinteger(L, c++);  // t t i
		lua_pushstring(L, __argv[i]);  // t t i s
		lua_settable(L, -3);  // t t
	}
	lua_setfield(L, -2, "args");  // t
	lua_pop(L, 1);

	//////////////////////////////////////// ×°ÔØ³õÊ¼»¯½Å±¾
	LINFO("×°ÔØ³õÊ¼»¯½Å±¾'%s'", LLAUNCH_SCRIPT);
	fcyRefPointer<fcyMemStream> tMemStream;
	if (!m_ResourceMgr.LoadFile(LLAUNCH_SCRIPT, tMemStream))
		return false;
	if (!SafeCallScript((fcStr)tMemStream->GetInternalBuffer(), (size_t)tMemStream->GetLength(), "launch"))
		return false;
	
	//////////////////////////////////////// ³õÊ¼»¯fancy2dÒýÇæ
	LINFO("³õÊ¼»¯fancy2d °æ±¾ %d.%d (·Ö±æÂÊ: %dx%d ´¹Ö±Í¬²½: %b ´°¿Ú»¯: %b)",
		(F2DVERSION & 0xFFFF0000) >> 16, F2DVERSION & 0x0000FFFF,
		(int)m_OptionResolution.x, (int)m_OptionResolution.y, m_OptionVsync, m_OptionWindowed);
	struct : public f2dInitialErrListener
	{
		void OnErr(fuInt TimeTick, fcStr Src, fcStr Desc)
		{
			LERROR("³õÊ¼»¯fancy2dÊ§°Ü (Òì³£ÐÅÏ¢'%m' Ô´'%m')", Desc, Src);
		}
	} tErrListener;

	if (FCYFAILED(CreateF2DEngineAndInit(
		F2DVERSION,
		fcyRect(0.f, 0.f, m_OptionResolution.x, m_OptionResolution.y),
		m_OptionTitle.c_str(),
		m_bSplashWindowEnabled ? true : m_OptionWindowed,
		m_OptionVsync,
		F2DAALEVEL_NONE,
		this,
		&m_pEngine,
		&tErrListener
		)))
	{
		return false;
	}

	// »ñÈ¡×é¼þ
	m_pMainWindow = m_pEngine->GetMainWindow();
	m_pRenderer = m_pEngine->GetRenderer();
	m_pRenderDev = m_pRenderer->GetDevice();
	m_pSoundSys = m_pEngine->GetSoundSys();
	m_pInputSys = m_pEngine->GetInputSys();

	// ´òÓ¡Éè±¸ÐÅÏ¢
	f2dCPUInfo stCPUInfo = { 0 };
	m_pEngine->GetCPUInfo(stCPUInfo);
	LINFO("CPU %m %m / GPU %m", stCPUInfo.CPUBrandString, stCPUInfo.CPUString, m_pRenderDev->GetDeviceName());

	// ´´½¨äÖÈ¾Æ÷
	if (FCYFAILED(m_pRenderDev->CreateGraphics2D(1024, 2048, &m_Graph2D)))
	{
		LERROR("ÎÞ·¨´´½¨äÖÈ¾Æ÷ (fcyRenderDevice::CreateGraphics2D failed)");
		return false;
	}
	m_Graph2DLastBlendMode = BlendMode::AddAlpha;
	m_Graph2DBlendState = m_Graph2D->GetBlendState();
	m_Graph2DColorBlendState = m_Graph2D->GetColorBlendType();
	m_bRenderStarted = false;

	// ´´½¨ÎÄ×ÖäÖÈ¾Æ÷
	if (FCYFAILED(m_pRenderer->CreateFontRenderer(nullptr, &m_FontRenderer)))
	{
		LERROR("ÎÞ·¨´´½¨×ÖÌåäÖÈ¾Æ÷ (fcyRenderer::CreateFontRenderer failed)");
		return false;
	}
	m_FontRenderer->SetZ(0.5f);

	// ´´½¨Í¼ÔªäÖÈ¾Æ÷
	if (FCYFAILED(m_pRenderer->CreateGeometryRenderer(&m_GRenderer)))
	{
		LERROR("ÎÞ·¨´´½¨Í¼ÔªäÖÈ¾Æ÷ (fcyRenderer::CreateGeometryRenderer failed)");
		return false;
	}

	// ´´½¨3DäÖÈ¾Æ÷
	if (FCYFAILED(m_pRenderDev->CreateGraphics3D(nullptr, &m_Graph3D)))
	{
		LERROR("ÎÞ·¨´´½¨3DäÖÈ¾Æ÷ (fcyRenderDevice::CreateGraphics3D failed)");
		return false;
	}
	m_Graph3DLastBlendMode = BlendMode::AddAlpha;
	m_Graph3DBlendState = m_Graph3D->GetBlendState();
	
	// ´´½¨PostEffect»º³å
	if (FCYFAILED(m_pRenderDev->CreateRenderTarget(
		m_pRenderDev->GetBufferWidth(),
		m_pRenderDev->GetBufferHeight(),
		true,
		&m_PostEffectBuffer)))
	{
		LERROR("ÎÞ·¨´´½¨POSTEFFECT»º³åÇø (fcyRenderDevice::CreateRenderTarget failed)");
		return false;
	}

	// ´´½¨¼üÅÌÊäÈë
	m_pInputSys->CreateKeyboard(-1, false, &m_Keyboard);
	if (!m_Keyboard)
		LWARNING("ÎÞ·¨´´½¨¼üÅÌÉè±¸£¬½«Ê¹ÓÃ´°¿ÚÏûÏ¢×÷ÎªÊäÈëÔ´ (f2dInputSys::CreateKeyboard failed.)");
	
	// ´´½¨ÊÖ±úÊäÈë
	fuInt iJoyStickCount = ::min(2U, m_pInputSys->GetDeviceCount(F2DINPUTDEVTYPE_GAMECTRL));
	for (fuInt i = 0; i < iJoyStickCount; ++i)
	{
		LINFO("Õì²âµ½ÊÖ±ú Éè±¸Ãû£º%s ²úÆ·Ãû£º%s", m_pInputSys->GetDeviceName(F2DINPUTDEVTYPE_GAMECTRL, i), 
			m_pInputSys->GetDeviceProductName(F2DINPUTDEVTYPE_GAMECTRL, i));
		if (FCYFAILED(m_pInputSys->CreateJoystick(i, false, &m_Joystick[i])))
			LWARNING("ÎÞ·¨×°ÔØÊÖ±ú£¬ºöÂÔ¡£");
	}

	// luastg²»Ê¹ÓÃZBuffer£¬½«Æä¹Ø±Õ¡£
	m_pRenderDev->SetZBufferEnable(false);
	
	// ÉèÖÃ´°¿ÚÍ¼±ê
	HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
	SendMessage((HWND)m_pMainWindow->GetHandle(), WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)hIcon);
	SendMessage((HWND)m_pMainWindow->GetHandle(), WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIcon);
	DestroyIcon(hIcon);

	// ÈôÃ»ÓÐÔØÈë´°¿Ú£¬ÔòÏÔÊ¾ÓÎÏ·´°¿Ú
	if (!m_bSplashWindowEnabled)
	{
		// ÏÔÊ¾´°¿Ú
		m_pMainWindow->MoveToCenter();
		m_pMainWindow->SetVisiable(true);
	}

	m_LastChar = 0;
	m_LastKey = 0;
	::memset(m_KeyStateMap, 0, sizeof(m_KeyStateMap));
	::memset(m_MouseState, 0, sizeof(m_MouseState));

	//////////////////////////////////////// ×°ÔØºËÐÄ½Å±¾²¢Ö´ÐÐGameInit
	LINFO("×°ÔØºËÐÄ½Å±¾'%s'", LCORE_SCRIPT);
	if (!m_ResourceMgr.LoadFile(LCORE_SCRIPT, tMemStream))
		return false;
	if (!SafeCallScript((fcStr)tMemStream->GetInternalBuffer(), (size_t)tMemStream->GetLength(), "core.lua"))
		return false;
	if (!SafeCallGlobalFunction(LFUNC_GAMEINIT))
		return false;

	m_iStatus = AppStatus::Initialized;
	LINFO("³õÊ¼»¯³É¹¦Íê³É");
	return true;
}

void AppFrame::Shutdown()LNOEXCEPT
{
	m_GameObjectPool = nullptr;
	LINFO("ÒÑÇå¿Õ¶ÔÏó³Ø");

	m_ResourceMgr.ClearAllResource();
	LINFO("ÒÑÇå¿ÕËùÓÐ×ÊÔ´");

	m_Joystick[0] = m_Joystick[1] = nullptr;
	m_Keyboard = nullptr;
	m_PostEffectBuffer = nullptr;
	m_Graph3D = nullptr;
	m_GRenderer = nullptr;
	m_FontRenderer = nullptr;
	m_Graph2D = nullptr;
	m_pInputSys = nullptr;
	m_pSoundSys = nullptr;
	m_pRenderDev = nullptr;
	m_pRenderer = nullptr;
	m_pMainWindow = nullptr;
	m_pEngine = nullptr;
	LINFO("ÒÑÐ¶ÔØfancy2d");

	if (L)
	{
		lua_close(L);
		L = nullptr;
		LINFO("ÒÑÐ¶ÔØLuaÐéÄâ»ú");
	}
	m_ResourceMgr.UnloadAllPack();
	LINFO("ÒÑÐ¶ÔØËùÓÐ×ÊÔ´°ü");

	m_iStatus = AppStatus::Destroyed;
	LINFO("¿ò¼ÜÏú»Ù");
}

void AppFrame::Run()LNOEXCEPT
{
	LASSERT(m_iStatus == AppStatus::Initialized);
	LINFO("¿ªÊ¼Ö´ÐÐÓÎÏ·Ñ­»·");

	m_fFPS = 0.f;
#if (defined LDEVVERSION) || (defined LDEBUG)
	m_UpdateTimer = 0.f;
	m_RenderTimer = 0.f;
	m_PerformanceUpdateTimer = 0.f;
	m_PerformanceUpdateCounter = 0.f;
	m_FPSTotal = 0.f;
	m_ObjectTotal = 0.f;
	m_UpdateTimerTotal = 0.f;
	m_RenderTimerTotal = 0.f;
#endif

	if (m_bSplashWindowEnabled)  // ÏÔÊ¾¹ýÔØÈë´°¿Ú
	{
		// ÏÔÊ¾´°¿Ú
		m_pMainWindow->MoveToCenter();
		m_pMainWindow->SetVisiable(true);

		// ¸Ä±äÏÔÊ¾Ä£Ê½µ½È«ÆÁ
		if (!m_OptionWindowed)
			ChangeVideoMode((int)m_OptionResolution.x, (int)m_OptionResolution.y, m_OptionWindowed, m_OptionVsync);
	}
	m_bSplashWindowEnabled = false;

	// ´°¿ÚÇ°ÒÆ¡¢ÏÔÊ¾¡¢Òþ²ØÊó±êÖ¸Õë
	SetActiveWindow((HWND)m_pMainWindow->GetHandle());  // È»²¢ÂÑ
	// SetForegroundWindow((HWND)m_pMainWindow->GetHandle());
	// BringWindowToTop((HWND)m_pMainWindow->GetHandle());
	m_pMainWindow->SetHideIME(true);
	m_pMainWindow->HideMouse(!m_OptionSplash);

	// Æô¶¯ÓÎÏ·Ñ­»·
	m_pEngine->Run(F2DENGTHREADMODE_MULTITHREAD, m_OptionFPSLimit);

	LINFO("ÍË³öÓÎÏ·Ñ­»·");
}

bool AppFrame::SafeCallScript(const char* source, size_t len, const char* desc)LNOEXCEPT
{
	lua_pushcfunction(L, StackTraceback);

	if (0 != luaL_loadbuffer(L, source, len, desc))
	{
		try
		{
			wstring tErrorInfo = StringFormat(
				L"½Å±¾'%m'±àÒëÊ§°Ü: %m",
				desc,
				lua_tostring(L, -1)
			);

			LERROR("½Å±¾´íÎó£º%s", tErrorInfo.c_str());
			MessageBox(
				m_pMainWindow ? (HWND)m_pMainWindow->GetHandle() : 0,
				tErrorInfo.c_str(),
				L"LuaSTGPlus½Å±¾´íÎó",
				MB_ICONERROR | MB_OK
			);
		}
		catch (const bad_alloc&)
		{
			LERROR("³¢ÊÔÐ´³ö½Å±¾´íÎóÊ±·¢ÉúÄÚ´æ²»×ã´íÎó");
		}
		
		lua_pop(L, 2);
		return false;
	}

	if (0 != lua_pcall(L, 0, 0, lua_gettop(L) - 1))
	{
		try
		{
			wstring tErrorInfo = StringFormat(
				L"½Å±¾'%m'ÖÐ²úÉúÎ´´¦ÀíµÄÔËÐÐÊ±´íÎó:\n\t%m",
				desc,
				lua_tostring(L, -1)
			);

			LERROR("½Å±¾´íÎó£º%s", tErrorInfo.c_str());
			MessageBox(
				m_pMainWindow ? (HWND)m_pMainWindow->GetHandle() : 0,
				tErrorInfo.c_str(),
				L"LuaSTGPlus½Å±¾´íÎó",
				MB_ICONERROR | MB_OK
			);
		}
		catch (const bad_alloc&)
		{
			LERROR("³¢ÊÔÐ´³ö½Å±¾´íÎóÊ±·¢ÉúÄÚ´æ²»×ã´íÎó");
		}

		lua_pop(L, 2);
		return false;
	}

	lua_pop(L, 1);
	return true;
}

bool AppFrame::SafeCallGlobalFunction(const char* name, int retc)LNOEXCEPT
{
	lua_pushcfunction(L, StackTraceback);  // ... c
	int tStacktraceIndex = lua_gettop(L);

	lua_getglobal(L, name);  // ... c f
	if (0 != lua_pcall(L, 0, retc, tStacktraceIndex))
	{
		try
		{
			wstring tErrorInfo = StringFormat(
				L"Ö´ÐÐº¯Êý'%m'Ê±²úÉúÎ´´¦ÀíµÄÔËÐÐÊ±´íÎó:\n\t%m",
				name,
				lua_tostring(L, -1)
			);

			LERROR("½Å±¾´íÎó£º%s", tErrorInfo.c_str());
			MessageBox(
				m_pMainWindow ? (HWND)m_pMainWindow->GetHandle() : 0,
				tErrorInfo.c_str(),
				L"LuaSTGPlus½Å±¾´íÎó",
				MB_ICONERROR | MB_OK
			);
		}
		catch (const bad_alloc&)
		{
			LERROR("³¢ÊÔÐ´³ö½Å±¾´íÎóÊ±·¢ÉúÄÚ´æ²»×ã´íÎó");
		}

		lua_pop(L, 2);
		return false;
	}

	lua_remove(L, tStacktraceIndex);
	return true;
}
#pragma endregion

#pragma region 游戏循环
fBool AppFrame::OnUpdate(fDouble ElapsedTime, f2dFPSController* pFPSController, f2dMsgPump* pMsgPump)
{
#if (defined LDEVVERSION) || (defined LDEBUG)
	TimerScope tProfileScope(m_UpdateTimer);
#endif

	m_fFPS = (float)pFPSController->GetFPS();

	m_LastChar = 0;
	m_LastKey = 0;

	// ´¦ÀíÏûÏ¢
	f2dMsg tMsg;
	while (FCYOK(pMsgPump->GetMsg(&tMsg)))
	{
		switch (tMsg.Type)
		{
		case F2DMSG_WINDOW_ONCLOSE:
			return false;  // ¹Ø±Õ´°¿ÚÊ±½áÊøÑ­»·
		case F2DMSG_WINDOW_ONGETFOCUS:
			if (!SafeCallGlobalFunction(LFUNC_GAINFOCUS))
				return false;
		case F2DMSG_WINDOW_ONLOSTFOCUS:
			if (!SafeCallGlobalFunction(LFUNC_LOSEFOCUS))
				return false;
		case F2DMSG_WINDOW_ONCHARINPUT:
			m_LastChar = (fCharW)tMsg.Param1;
			break;
		case F2DMSG_WINDOW_ONKEYDOWN:
			// ctrl+enterÈ«ÆÁ
			if (tMsg.Param1 == VK_RETURN && !m_KeyStateMap[VK_RETURN] && m_KeyStateMap[VK_CONTROL])  // ·ÀÖ¹·´¸´´¥·¢
				ChangeVideoMode((int)m_OptionResolution.x, (int)m_OptionResolution.y, !m_OptionWindowed, m_OptionVsync);

			if (0 < tMsg.Param1 && tMsg.Param1 < _countof(m_KeyStateMap) &&
				!(LJOYSTICK1_MAPPING_START <= tMsg.Param1 && tMsg.Param1 <= LJOYSTICK1_MAPPING_END) &&  // joystick1Ó³ÉäÇøÓò
				!(LJOYSTICK2_MAPPING_START <= tMsg.Param1 && tMsg.Param1 <= LJOYSTICK2_MAPPING_END))  // joystick2Ó³ÉäÇøÓò
			{
				m_LastKey = (fInt)tMsg.Param1;
				m_KeyStateMap[tMsg.Param1] = true;
			}	

#if (defined LDEVVERSION) || (defined LDEBUG)
			if (tMsg.Param1 == VK_F8)
				m_bShowCollider = !m_bShowCollider;
#endif
			break;
		case F2DMSG_WINDOW_ONKEYUP:
			if (m_LastKey == tMsg.Param1)
				m_LastKey = 0;
			if (0 < tMsg.Param1 && tMsg.Param1 < _countof(m_KeyStateMap) &&
				!(LJOYSTICK1_MAPPING_START <= tMsg.Param1 && tMsg.Param1 <= LJOYSTICK1_MAPPING_END) &&  // joystick1Ó³ÉäÇøÓò
				!(LJOYSTICK2_MAPPING_START <= tMsg.Param1 && tMsg.Param1 <= LJOYSTICK2_MAPPING_END))  // joystick2Ó³ÉäÇøÓò
			{
				m_KeyStateMap[tMsg.Param1] = false;
			}
			break;
		case F2DMSG_WINDOW_ONMOUSELDOWN:
			m_MouseState[0] = true;
			break;
		case F2DMSG_WINDOW_ONMOUSELUP:
			m_MouseState[0] = false;
			break;
		case F2DMSG_WINDOW_ONMOUSEMDOWN:
			m_MouseState[1] = true;
			break;
		case F2DMSG_WINDOW_ONMOUSEMUP:
			m_MouseState[1] = false;
			break;
		case F2DMSG_WINDOW_ONMOUSERDOWN:
			m_MouseState[2] = true;
			break;
		case F2DMSG_WINDOW_ONMOUSERUP:
			m_MouseState[2] = false;
			break;
		case F2DMSG_WINDOW_ONMOUSEMOVE:
			m_MousePosition.x = (float)static_cast<fInt>(tMsg.Param1);
			m_MousePosition.y = m_OptionResolution.y - (float)static_cast<fInt>(tMsg.Param2);  // ! Ç±ÔÚ´óÐ¡²»Æ¥ÅäÎÊÌâ
			break;
		case F2DMSG_JOYSTICK_ONXPOSCHANGE:
			do
			{
				double tValue = *(double*)&tMsg.Param1;
				f2dInputJoystick* pJoystick = (f2dInputJoystick*)tMsg.Param2;

				if (tValue < LJOYSTICK_X_MIN)  // ×ó¼ü´¥·¢
				{
					fInt tVKCode;
					if (pJoystick == m_Joystick[0])
						tVKCode = LJOYSTICK1_MAPPING_START + 2;
					else if (pJoystick == m_Joystick[1])
						tVKCode = LJOYSTICK2_MAPPING_START + 2;
					else
						break;
					if (!m_KeyStateMap[tVKCode])
					{
						m_LastKey = tVKCode;
						m_KeyStateMap[tVKCode] = true;
					}
				}
				else
				{
					fInt tVKCode;
					if (pJoystick == m_Joystick[0])
						tVKCode = LJOYSTICK1_MAPPING_START + 2;
					else if (pJoystick == m_Joystick[1])
						tVKCode = LJOYSTICK2_MAPPING_START + 2;
					else
						break;
					if (m_LastKey == tVKCode)
						m_LastKey = 0;
					m_KeyStateMap[tVKCode] = false;
				}

				if (tValue > LJOYSTICK_X_MAX)  // ÓÒ¼ü´¥·¢
				{
					fInt tVKCode;
					if (pJoystick == m_Joystick[0])
						tVKCode = LJOYSTICK1_MAPPING_START + 3;
					else if (pJoystick == m_Joystick[1])
						tVKCode = LJOYSTICK2_MAPPING_START + 3;
					else
						break;
					if (!m_KeyStateMap[tVKCode])
					{
						m_LastKey = tVKCode;
						m_KeyStateMap[tVKCode] = true;
					}
				}
				else
				{
					fInt tVKCode;
					if (pJoystick == m_Joystick[0])
						tVKCode = LJOYSTICK1_MAPPING_START + 3;
					else if (pJoystick == m_Joystick[1])
						tVKCode = LJOYSTICK2_MAPPING_START + 3;
					else
						break;
					if (m_LastKey == tVKCode)
						m_LastKey = 0;
					m_KeyStateMap[tVKCode] = false;
				}
			} while (false);
			break;
		case F2DMSG_JOYSTICK_ONYPOSCHANGE:
			do
			{
				double tValue = *(double*)&tMsg.Param1;
				f2dInputJoystick* pJoystick = (f2dInputJoystick*)tMsg.Param2;

				if (tValue < LJOYSTICK_Y_MIN)  // ÉÏ¼ü´¥·¢
				{
					fInt tVKCode;
					if (pJoystick == m_Joystick[0])
						tVKCode = LJOYSTICK1_MAPPING_START;
					else if (pJoystick == m_Joystick[1])
						tVKCode = LJOYSTICK2_MAPPING_START;
					else
						break;
					if (!m_KeyStateMap[tVKCode])
					{
						m_LastKey = tVKCode;
						m_KeyStateMap[tVKCode] = true;
					}
				}
				else
				{
					fInt tVKCode;
					if (pJoystick == m_Joystick[0])
						tVKCode = LJOYSTICK1_MAPPING_START;
					else if (pJoystick == m_Joystick[1])
						tVKCode = LJOYSTICK2_MAPPING_START;
					else
						break;
					if (m_LastKey == tVKCode)
						m_LastKey = 0;
					m_KeyStateMap[tVKCode] = false;
				}

				if (tValue > LJOYSTICK_Y_MAX)  // ÏÂ¼ü´¥·¢
				{
					fInt tVKCode;
					if (pJoystick == m_Joystick[0])
						tVKCode = LJOYSTICK1_MAPPING_START + 1;
					else if (pJoystick == m_Joystick[1])
						tVKCode = LJOYSTICK2_MAPPING_START + 1;
					else
						break;
					if (!m_KeyStateMap[tVKCode])
					{
						m_LastKey = tVKCode;
						m_KeyStateMap[tVKCode] = true;
					}
				}
				else
				{
					fInt tVKCode;
					if (pJoystick == m_Joystick[0])
						tVKCode = LJOYSTICK1_MAPPING_START + 1;
					else if (pJoystick == m_Joystick[1])
						tVKCode = LJOYSTICK2_MAPPING_START + 1;
					else
						break;
					if (m_LastKey == tVKCode)
						m_LastKey = 0;
					m_KeyStateMap[tVKCode] = false;
				}
			} while (false);
			break;
		case F2DMSG_JOYSTICK_ONBUTTONUP:
			do
			{
				f2dInputJoystick* pJoystick = (f2dInputJoystick*)tMsg.Param2;
				if (tMsg.Param1 < LJOYSTICK1_MAPPING_END - LJOYSTICK1_MAPPING_START + 1 - 4)
				{
					if (pJoystick == m_Joystick[0])
					{
						fInt tLastKey = (fInt)tMsg.Param1 + LJOYSTICK1_MAPPING_START + 4;
						if (m_LastKey == tLastKey)
							m_LastKey = 0;
						m_KeyStateMap[tLastKey] = false;
					}	
					else if (pJoystick == m_Joystick[1])
					{
						fInt tLastKey = (fInt)tMsg.Param1 + LJOYSTICK2_MAPPING_START + 4;
						if (m_LastKey == tLastKey)
							m_LastKey = 0;
						m_KeyStateMap[tLastKey] = false;
					}
				}
			} while (false);
			break;
		case F2DMSG_JOYSTICK_ONBUTTONDOWN:
			do
			{
				f2dInputJoystick* pJoystick = (f2dInputJoystick*)tMsg.Param2;
				if (tMsg.Param1 < LJOYSTICK1_MAPPING_END - LJOYSTICK1_MAPPING_START + 1 - 4)
				{
					if (pJoystick == m_Joystick[0])
					{
						m_LastKey = (fInt)tMsg.Param1 + LJOYSTICK1_MAPPING_START + 4;
						m_KeyStateMap[m_LastKey] = true;
					}
					else if (pJoystick == m_Joystick[1])
					{
						m_LastKey = (fInt)tMsg.Param1 + LJOYSTICK2_MAPPING_START + 4;
						m_KeyStateMap[m_LastKey] = true;
					}
				}
			} while (false);
			break;
		default:
			break;
		}
	}

	// Ö´ÐÐÖ¡º¯Êý
	if (!SafeCallGlobalFunction(LFUNC_FRAME, 1))
		return false;
	bool tAbort = lua_toboolean(L, -1) == 0 ? false : true;
	lua_pop(L, 1);

#if (defined LDEVVERSION) || (defined LDEBUG)
	// Ë¢ÐÂÐÔÄÜ¼ÆÊýÆ÷
	m_PerformanceUpdateTimer += static_cast<float>(ElapsedTime);
	m_PerformanceUpdateCounter += 1.f;
	m_FPSTotal += static_cast<float>(m_fFPS);
	m_ObjectTotal += (float)m_GameObjectPool->GetObjectCount();
	m_UpdateTimerTotal += m_UpdateTimer;
	m_RenderTimerTotal += m_RenderTimer;
	if (m_PerformanceUpdateTimer > LPERFORMANCEUPDATETIMER)
	{
		// ·¢ËÍÐÔÄÜÍ³¼ÆÐÅÏ¢
		if (m_DebuggerClient)
		{
			m_DebuggerClient->SendPerformanceCounter(
				m_FPSTotal / m_PerformanceUpdateCounter,
				m_ObjectTotal / m_PerformanceUpdateCounter,
				m_UpdateTimerTotal / m_PerformanceUpdateCounter,
				m_RenderTimerTotal / m_PerformanceUpdateCounter
				);
		}
			
		m_PerformanceUpdateTimer = 0.f;
		m_PerformanceUpdateCounter = 0.f;
		m_FPSTotal = 0.f;
		m_ObjectTotal = 0.f;
		m_UpdateTimerTotal = 0.f;
		m_RenderTimerTotal = 0.f;
	}
#endif

	return !tAbort;
}

fBool AppFrame::OnRender(fDouble ElapsedTime, f2dFPSController* pFPSController)
{
#if (defined LDEVVERSION) || (defined LDEBUG)
	TimerScope tProfileScope(m_RenderTimer);
#endif

	m_pRenderDev->Clear();

	// Ö´ÐÐäÖÈ¾º¯Êý
	m_bRenderStarted = true;
	m_bPostEffectCaptureStarted = false;
	if (!SafeCallGlobalFunction(LFUNC_RENDER, 0))
		m_pEngine->Abort();
	if (!m_stRenderTargetStack.empty())
	{
		LWARNING("OnRender: äÖÈ¾½áÊøÊ±Ã»ÓÐÍÆ³öËùÓÐµÄRenderTarget.");
		while (!m_stRenderTargetStack.empty())
			PopRenderTarget();
	}
	m_bRenderStarted = false;

#if (defined LDEVVERSION) || (defined LDEBUG)
	if (m_bShowCollider)
	{
		m_pRenderDev->ClearZBuffer();

		f2dBlendState stState = m_Graph2D->GetBlendState();
		f2dBlendState stStateClone = stState;
		stStateClone.DestBlend = F2DBLENDFACTOR_INVSRCALPHA;
		stStateClone.BlendOp = F2DBLENDOPERATOR_ADD;
		m_Graph2D->SetBlendState(stStateClone);

		m_Graph2D->Begin();
		m_GameObjectPool->DrawGroupCollider(m_Graph2D, m_GRenderer, 1, fcyColor(150, 163, 73, 164));  // GROUP_ENEMY_BULLET
		m_GameObjectPool->DrawGroupCollider(m_Graph2D, m_GRenderer, 2, fcyColor(150, 163, 73, 164));  // GROUP_ENEMY
		m_GameObjectPool->DrawGroupCollider(m_Graph2D, m_GRenderer, 5, fcyColor(150, 163, 73, 20));  // GROUP_INDES
		m_GameObjectPool->DrawGroupCollider(m_Graph2D, m_GRenderer, 4, fcyColor(100, 175, 15, 20));  // GROUP_PLAYER
		m_Graph2D->End();

		m_Graph2D->SetBlendState(stState);
	}
#endif
	return true;
}
#pragma endregion
