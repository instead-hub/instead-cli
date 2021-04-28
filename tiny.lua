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
else
	require 'tiny2'
end
