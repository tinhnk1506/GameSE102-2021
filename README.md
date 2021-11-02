# mario-dx10

#INSTALL
Linker > Input > Additional Dependencies:
d3d10.lib;dxguid.lib;dinput8.lib;
Tools > NuGet Package Manager > Package Manager Console and paste this in
Install-Package Microsoft.DXSDK.D3DX -Version 9.29.952.8
SFML here (Note: By the time I was writing this, the IDE version may have changed and the binaries may not be compatible with the project anymore, and if so, follow the instructions on SFML website.)
[IMPORTANT] Configuration: Active(Debug), Platform: Win32 or x86
