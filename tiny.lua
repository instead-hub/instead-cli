-- some stubs for tiny-instead
-- fake game.gui
-- stat, menu
-- fake audio
-- fake input

if API == 'stead3' then
	require 'tiny3'
	local instead = std '@instead'
	instead.restart = instead_restart
	instead.menu = instead_menu

	std.mod_start(function()
		local mp = std.ref '@metaparser'
		if mp then
			mp.msg.CUTSCENE_MORE = '^'..mp.msg.CUTSCENE_HELP
		end
	end)
else
	require 'tiny2'
end
