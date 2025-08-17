newoption({
	trigger = "gmcommon",
	description = "Sets the path to the garrysmod_common (https://github.com/danielga/garrysmod_common) directory",
	value = "path to garrysmod_common directory"
})

local gmcommon = assert(_OPTIONS.gmcommon or os.getenv("GARRYSMOD_COMMON"),
	"you didn't provide a path to your garrysmod_common (https://github.com/danielga/garrysmod_common) directory")
include(path.join(gmcommon, "generator.v3.lua"))

CreateWorkspace({name = "webp"})
  CreateProject({serverside = false, source_path = "src"})
		IncludeLuaShared()
		IncludeHelpersExtended() -- uses this repo path
		IncludeDetouring() -- uses this repo detouring submodule
		IncludeScanning() -- uses this repo scanning submodule

		IncludeSDKCommon()
		IncludeSDKTier0()
		IncludeSDKTier1()
		IncludeSDKTier2()
		IncludeSDKTier3()
		IncludeSDKMathlib()
		IncludeSDKRaytrace()
		IncludeSteamAPI()
    