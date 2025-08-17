# gm_webp

## turn png into webp with one func

did this for better compression on webp's side

saving the files as .png works with windows photo opener thing aswell

adds global func WEBC

`OUT_WEBP_STR = WEBC( IN_PNG_STR, QUALITY_LVL )`

i hope the quality arg actually works because i think it doesn't lol

nothing else to it.

some xmpl:

```lua
local t = "webp"
if not WEBP then require(t) end

hook.Add("PostRenderVGUI", t, function()
	local c = render.Capture({
		x = 0,
		y = 0,
		w = ScrW(),
		h = ScrH(),
		format = "png", -- must stay png cuz jpeg unhandled
	})
  
	file.Write("2151.png", c)

	file.Write("2152.dat", WEBP(c, 95)) -- eugh since .webp cant be made, i think, idk.
	-- can save as .png aswell. works just as.

	hook.Remove("PostRenderVGUI", t)
end)
```

oh also you need premake5 and gmcommon (+allotherfeatures). there are like 700 repo's on github showing how to do all this install section part

also

get vcpkg and integrate if not (godlike(not perfect tho))

```
vcpkg install libpng
vcpkg install libwebp
```

also a lot of funky .dlls appear next to release - dump those into gmod root folder or 
`<rootfolder>/bin/<yourbranch>`
for example mine's 
`...\Steam\steamapps\common\GarrysMod\bin\win64\libpng16.dll`
