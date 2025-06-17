#pragma once

#include <string>

const char* DEFAULT_IMGUI_LAYOUT =
R"(; imgui_layout.ini par défaut
[Window][DockSpace]
Pos=0,0
Size=1280,720
Collapsed=0

[Window][Debug##Default]
Pos=60,60
Size=400,400
Collapsed=0

[Window][Scene]
Pos=8,27
Size=844,429
Collapsed=0
DockId=0x00000007,0

[Window][Hierarchy]
Pos=854,27
Size=161,429
Collapsed=0
DockId=0x00000008,0

[Window][Inspector]
Pos=1017,27
Size=255,685
Collapsed=0
DockId=0x00000006,0

[Window][Project]
Pos=724,458
Size=291,254
Collapsed=0
DockId=0x00000004,0

[Window][Game]
Pos=8,458
Size=714,254
Collapsed=0
DockId=0x00000003,0

[Window][Créer ou charger un projet]
Pos=35,36
Size=538,334
Collapsed=0

[Docking][Data]
DockSpace       ID=0x14621557 Window=0x3DA2F1DE Pos=8,27 Size=1264,685 Split=X Selected=0xE601B12F
  DockNode      ID=0x00000005 Parent=0x14621557 SizeRef=1007,685 Split=Y
    DockNode    ID=0x00000001 Parent=0x00000005 SizeRef=1264,429 Split=X Selected=0xE601B12F
      DockNode  ID=0x00000007 Parent=0x00000001 SizeRef=844,429 CentralNode=1 Selected=0xE601B12F
      DockNode  ID=0x00000008 Parent=0x00000001 SizeRef=161,429 Selected=0xBABDAE5E
    DockNode    ID=0x00000002 Parent=0x00000005 SizeRef=1264,254 Split=X Selected=0xD1EB2482
      DockNode  ID=0x00000003 Parent=0x00000002 SizeRef=897,254 Selected=0xD1EB2482
      DockNode  ID=0x00000004 Parent=0x00000002 SizeRef=365,254 Selected=0x9C21DE82
  DockNode      ID=0x00000006 Parent=0x14621557 SizeRef=255,685 Selected=0x36DC96AB
)";